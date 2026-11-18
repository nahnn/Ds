// OVERVIEW: USceneEditor
// ========
// Class implementation for USceneEditor. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 17/11/2026

#include "include/USceneEditor.h"

namespace cg
{ // begin namespace cg

void
USceneEditor::initUniformLocations()
{
  // getting the location of those variables on glsl program
  _mvMatrixLoc = _pbr.uniformLocation("mvMatrix");
  _normalMatrixLoc = _pbr.uniformLocation("normalMatrix");
  _mvpMatrixLoc = _pbr.uniformLocation("mvpMatrix");
  _viewportMatrixLoc = _pbr.uniformLocation("viewportMatrix");
  _projectionTypeLoc = _pbr.uniformLocation("projectionType");
  _lightCountLoc = _pbr.uniformLocation("lightCount");
  for (int i = 0; i < maxLights; ++i)
  {
    auto loc = [&](const char* field)
    {
      char name[32];
      snprintf(name, sizeof name, "lights[%d].%s", i, field);
      return _pbr.uniformLocation(name);
    };
    _lightLocs[i] = {loc("type"), loc("color"), loc("position"),
      loc("direction"), loc("falloff"), loc("range"), loc("angle")};
  }
  _OdLoc = _pbr.uniformLocation("material.Od");
  _OsLoc = _pbr.uniformLocation("material.Os");
  _rLoc = _pbr.uniformLocation("material.r");
  _mLoc = _pbr.uniformLocation("material.m");
  _lineWidthLoc = _pbr.uniformLocation("line.width");
  _lineColorLoc = _pbr.uniformLocation("line.color");
  _hiddenLinesLoc = _pbr.uniformLocation("hiddenLinesFlag");
  _useTextureLoc = _pbr.uniformLocation("useTexture");
  _useVertexColorsLoc = _pbr.uniformLocation("useVertexColors");
}

void
USceneEditor::beginRender()
{
  SceneEditor::beginRender();

  // setting viewport dimentions
  GLint v[4];
  glGetIntegerv(GL_VIEWPORT, v);
  float w = v[2] / 2.0f, h = v[3] / 2.0f;
  mat4f vp;
  vp[0].set(w, 0, 0, 0);
  vp[1].set(0, h, 0, 0);
  vp[2].set(0, 0, 1, 0);
  vp[3].set(v[0] + w, v[1] + h, 0, 0);

  // putting the program in use and setting current program uniforms
  _pbr.use();
  GLSL::Program::setUniformMat4(_viewportMatrixLoc, vp);
  GLSL::Program::setUniform(_projectionTypeLoc, (int)camera()->projectionType());
  GLSL::Program::setUniformVec4(_lineColorLoc, edgeColor);
  restoreBaseProgram();
}

void
USceneEditor::renderLights()
{
  SceneEditor::renderLights();

  // putting program in use
  auto cam = camera();
  int n = 0;

  _pbr.use();
  for (auto light : _scene->lights())
  {
    if (!light->isTurnedOn())
      continue;

    // setting lights uniforms to the current in use program
    auto& loc = _lightLocs[n];
    bool inCam = light->flags.isSet(Light::Camera);

    GLSL::Program::setUniform(loc.type, (int)light->type());
    GLSL::Program::setUniformVec4(loc.color, light->color);
    GLSL::Program::setUniformVec3(loc.position,
      inCam ? light->position() : cam->worldToCamera(light->position()));
    GLSL::Program::setUniformVec3(loc.direction,
      inCam ? light->direction() : cam->worldToCameraVector(light->direction()));
    GLSL::Program::setUniform(loc.falloff, (int)light->falloff);
    GLSL::Program::setUniform(loc.range, light->range());
    GLSL::Program::setUniform(loc.angle, light->spotAngle());
    if (++n == maxLights)
      break;
  }

  // registers a single light if none was in the scene
  if (n == 0)
  {
    GLSL::Program::setUniform(_lightLocs[0].type, 1);
    GLSL::Program::setUniformVec4(_lightLocs[0].color, vec4f{1, 1, 1, 0});
    GLSL::Program::setUniformVec3(_lightLocs[0].position, vec3f{0});
    GLSL::Program::setUniform(_lightLocs[0].range, 0.0f);
    n = 1;
  }
  GLSL::Program::setUniform(_lightCountLoc, n);
  restoreBaseProgram();
}

void
USceneEditor::renderActors()
{
  _pbrQueue.clear();

  // First pass: rendering 
  for (auto actor : _scene->actors())
  {
    if (!actor->isVisible())
      continue;

    auto mapper = actor->mapper();

    mapper->update();

    auto prim = mapper->primitive();

    if (auto pbr = prim ? dynamic_cast<PBR*>(prim->material()) : nullptr)
    {
      _pbrQueue.emplace_back(prim, pbr);
      continue;
    }

    if (!mapper->render(*this))
    {
      if (prim)
        drawMesh(*prim);
    }
    else if (flags.isSet(DrawBounds))
    {
      setLineColor(boundsColor);
      drawBounds(mapper->bounds());
    }
  }

  if (_pbrQueue.empty())
    return;

  // second pass: every primitive which was able to have its mat to be converted in PBR 
  _pbr.use();
  for (auto [prim, pbr] : _pbrQueue)
    drawPBR(*prim, *pbr);
  restoreBaseProgram();
  if (flags.isSet(DrawBounds))
    for (auto [prim, pbr] : _pbrQueue)
    {
      setLineColor(boundsColor);
      drawBounds(prim->bounds());
    }
}

void 
USceneEditor::drawPBR(const Primitive& p, const PBR& material)
{
  auto mesh = p.tesselate();
  if (!mesh) return;

  auto m = glMesh(mesh);
  if (!m) return;
  
  m->bind();

  auto cam = camera();
  auto mv = cam->worldToCameraMatrix() * p.localToWorldMatrix();

  GLSL::Program::setUniformMat4(_mvMatrixLoc, mv);
  GLSL::Program::setUniformMat4(_mvpMatrixLoc, cam->projectionMatrix() * mv);
  GLSL::Program::setUniformMat3(_normalMatrixLoc, mat3f{cam->worldToCameraMatrix()} * p.normalMatrix());

  setMaterial(material);
  GLSL::Program::setUniform(_hiddenLinesLoc, (int)(renderMode() == ShadedWithEdges));
  GLSL::Program::setUniform(_useTextureLoc, 0);
  GLSL::Program::setUniform(_useVertexColorsLoc, (int)useVertexColors());
  glDrawElements(GL_TRIANGLES, mesh->triangleCount() * 3, GL_UNSIGNED_INT, nullptr);
}

void
USceneEditor::setMaterial(const PBR& mat)
{
  const auto& a = mat.attributes();

  GLSL::Program::setUniformVec4(_OdLoc, a.od);
  GLSL::Program::setUniformVec4(_OsLoc, a.os);
  GLSL::Program::setUniform(_rLoc, a.r);
  GLSL::Program::setUniform(_mLoc, a.m);
}

} // end namespace cg