// OVERVIEW: USceneEditor.h
// ========
// Class definition for USceneEditor. U stands for universal. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __USceneEditor_h
#define __USceneEditor_h

#include "graphics/SceneEditor.h"
#include "PBR.h"
#include "Shaders.h"
#include <vector>

namespace cg
{ // begin namespace cg
    
class USceneEditor : public SceneEditor
{
public:
  USceneEditor(SceneBase& scene):
    SceneEditor{scene}
  {
    // setting vs, gs and fs
    auto cp = GLSL::Program::current();

    _pbr.setShader(GL_VERTEX_SHADER, vertexShader);
    _pbr.setShader(GL_GEOMETRY_SHADER, geometryShader);
    _pbr.setShader(GL_FRAGMENT_SHADER, PBR::fragmentShader()).use();
    initUniformLocations();
    GLSL::Program::setUniform(_lineWidthLoc, 0.5f);
    GLSL::Program::setCurrent(cp);
  }

  void setMaterial(const PBR&);

protected:
  void beginRender() override;
  void renderLights() override;
  void renderActors() override;

private:
  struct LightLoc { GLint type, color, position, direction, falloff, range, angle; };

  GLSL::Program _pbr{"PBR"};
  GLint _mvMatrixLoc, _normalMatrixLoc, _mvpMatrixLoc, _viewportMatrixLoc, _projectionTypeLoc;
  GLint _lightCountLoc;
  LightLoc _lightLocs[maxLights];
  GLint _OdLoc, _OsLoc, _rLoc, _mLoc;
  GLint _lineWidthLoc, _lineColorLoc, _hiddenLinesLoc, _useTextureLoc, _useVertexColorsLoc;
  std::vector<std::pair<Primitive*, PBR*>> _pbrQueue; // TODO: método potencialmente caro (reformular)

  void initUniformLocations();
  void drawPBR(const Primitive&, const PBR&);
  void restoreBaseProgram() { GLSL::Program::setCurrent(&program()); }

}; // USceneEditor

} // end namespace cg

#endif // __USceneEditor_h