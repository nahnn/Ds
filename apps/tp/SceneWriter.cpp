//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023, 2026 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneWriter.cpp
// ========
// Source file for scene writer.
//
// Author: Paulo Pagliosa
// Updated By: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "graph/CameraProxy.h"
#include "graph/LightProxy.h"
#include "graph/PrimitiveProxy.h"
#include "graphics/Assets.h"
#include "SceneWriter.h"
#include "include/pbr/PBR.h"
#include "include/sweep/SweepMeshProxy.h"
#include <map>

namespace cg::util
{ // begin namespace cg::util


/////////////////////////////////////////////////////////////////////
//
// SceneWriter implementation
// ===========
void
SceneWriter::writeName(const char* label, const char* name)
{
  beginLine();
  write(label);
  if (name && *name)
    write(" \"%s\"", name);
  endLine();
}

inline void
SceneWriter::writeEnvironment(const Scene& scene)
{
  beginBlock("environment");
  writeColor("ambient", scene.ambientLight);
  writeColor("background", scene.backgroundColor);
  endBlock();
}

namespace
{ // begin namespace

template <typename V>
inline bool
writeIfNotNull(SceneWriter& w, const char* label, const V& v)
{
  return v.x != 0 || v.y != 0 || v.z != 0 ?
    w.writeVec3(label, v), true : false;
}

template <typename V>
inline bool
writeScale(SceneWriter& w, const V& s)
{
  return s.x != 1 || s.y != 1 || s.z != 1 ?
    w.writeVec3("scale", s), true : false;
}

void
writeCamera(SceneWriter& w, const CameraProxy& proxy)
{
  w.beginBlock("camera");

  auto camera = proxy.camera();

  if (camera->projectionType() == Camera::Perspective)
  {
    w.writeLine("perspective");
    w.writeFloat("angle", camera->viewAngle());
  }
  else
  {
    w.writeLine("parallel");
    w.writeFloat("height", camera->height());
  }
  w.writeFloat("aspect", camera->aspectRatio());

  float F, B;

  camera->clippingPlanes(F, B);
  w.writeLine("depth vec2(%g,%g)", F, B);
  w.endBlock();
}

void
writeLight(SceneWriter& w, const LightProxy& proxy)
{
  w.beginBlock("light");

  auto light = proxy.light();
  auto lt = light->type();

  if (lt == Light::Type::Directional)
    w.writeLine("directional");
  else
  {
    if (lt == Light::Type::Spot)
    {
      w.writeLine("spot");
      w.writeFloat("angle", light->spotAngle());
    }
    else
      w.writeLine("point");
    w.writeFloat("range", light->range());
    w.writeLine("falloff %d", light->falloff);
  }
  w.writeColor("color", light->color);
  w.endBlock();
}

void
writePrimitive(SceneWriter& w, const TriangleMeshProxy& proxy)
{
  w.writeLine("mesh \"%s\" material \"%s\"",
    proxy.meshName(),
    proxy.mapper()->primitive()->material()->name());
}

//support for sweep meshes (cannot derive from TriangleMeshProxy)
//the mesh is not written: only the attributes that regenerate it
using GenType = PolylineGeneratorProxy<float>::GenType;
using PolylineWriteFunction = void (*)(SceneWriter&, const PolylineAttrs<float>&);

void
writePolylineBase(SceneWriter& w, const PolylineAttrs<float>& a)
{
  w.writeFloat("scale", a.sp);
  w.writeFloat("rotation", a.tp);
  if (a.isExtruded)
    w.writeFloat("extrusion", a.ep);
}

void
writeRegularPolygon(SceneWriter& w, const PolylineAttrs<float>& attrs)
{
  auto& a = static_cast<const RegularPolygonAttrs<float>&>(attrs);

  w.writeLine("regularPolygon");
  w.writeLine("points %d", a.np);
  writePolylineBase(w, a);
}

void
writeCircularArc(SceneWriter& w, const PolylineAttrs<float>& attrs)
{
  auto& a = static_cast<const CircularArcAttrs<float>&>(attrs);

  w.writeLine("circularArc");
  w.writeFloat("angle", a.tc);
  w.writeLine("segments %d", a.nc);
  if (a.isOpen)
    w.writeLine("open");
  writePolylineBase(w, a);
}

// generator type (enum) -> function that writes its attributes
const std::map<GenType, PolylineWriteFunction>&
polylineWriters()
{
  static const std::map<GenType, PolylineWriteFunction> writers
  {
    {GenType::RegularPolygon, writeRegularPolygon},
    {GenType::CircularArc, writeCircularArc}
  };

  return writers;
}

// common header: <keyword> "name" material "m" { polyline { ... }
template <typename P>
void
beginSweep(SceneWriter& w, const char* keyword, const P& proxy)
{
  w.writeLine("%s \"%s\" material \"%s\"",
    keyword,
    proxy.meshName(),
    proxy.mapper()->primitive()->material()->name());
  w.beginBlock();

  auto& gen = proxy.polyGen();

  w.beginBlock("polyline");
  polylineWriters().at(gen.activeGenType())(w, gen.activeGen().attributes());
  w.endBlock();
}

void
writeSpiral(SceneWriter& w, const SpiralProxy<float>& proxy)
{
  beginSweep(w, "spiral", proxy);

  auto& a = proxy.attr;

  if (a.capped)
    w.writeLine("capped");
  w.writeFloat("width", a.we);
  w.writeFloat("revolutions", a.re);
  w.writeLine("subdivisions %d", a.nes);
  w.writeFloat("heightIncrement", a.dh);
  w.writeFloat("widthIncrement", a.dw);
  w.endBlock();
}

void
writeTorsion(SceneWriter& w, const TorsionProxy<float>& proxy)
{
  beginSweep(w, "torsion", proxy);

  auto& a = proxy.attr;

  if (a.capped)
    w.writeLine("capped");
  w.writeFloat("length", a.lv);
  w.writeVec2("offset", vec2f{a.ow, a.oh});
  w.writeLine("subdivisions %d", a.nsv);
  w.writeVec2("scale", vec2f{a.sb, a.se});
  w.writeFloat("rotation", a.rv);
  w.endBlock();
}

} // end namespace

inline void
SceneWriter::writeTransform(const Transform& t)
{
  auto w = false;

  beginBlock("transform");
  w |= writeIfNotNull(*this, "position", t.position());
  w |= writeIfNotNull(*this, "rotation", t.eulerAngles());
  w |= writeScale(*this, t.localScale());
  if (!w)
    writeLine("// identity");
  endBlock();
}

inline void
SceneWriter::writeComponent(const Component* c)
{
  if (auto t = asTransform(c))
    writeTransform(*t);
  else if (auto function = _writeFunctionMap.find(*c))
    function(*this, *c);
}

void
SceneWriter::writeSceneObject(const SceneObject* object)
{
  beginBlock("object", object->name());
  for (auto component : object->components())
    writeComponent(component);
  for (auto& child : object->children())
    writeSceneObject(&child);
  endBlock();
}

inline void
SceneWriter::writeMaterial(const Material& m)
{
  auto pbr = dynamic_cast<const PBR*>(&m);
  Reference<PBR> converted;

  //Conversion to PBR
  if (!pbr)
    pbr = (converted = PBR::toPBR(m)).get();

  beginBlock("material", m.name());
  writeColor("diffuse", pbr->attrs.od);
  writeColor("specular", pbr->attrs.os);
  writeFloat("metallic", pbr->attrs.m);
  writeFloat("roughness", pbr->attrs.r);
  endBlock();
}

void
SceneWriter::writeWorld(const Scene&)
{
  // do nothing
}

void
SceneWriter::write(const Scene& scene)
{
  for (const auto& [name, m] : Assets::materials())
    if (m->referenceCount() > 1)
      writeMaterial(*m);
  beginBlock("scene", scene.name());
  writeEnvironment(scene);
  writeWorld(scene);
  for (auto& object : scene.root()->children())
    writeSceneObject(&object);
  endBlock();
}

SceneWriter::WriteFunctionMap::WriteFunctionMap()
{
  SceneWriter::registerWriteFunction(writeCamera);
  SceneWriter::registerWriteFunction(writeLight);
  SceneWriter::registerWriteFunction(writePrimitive);

  //support for sweeps (the map uses the exact typeid: one function per concrete proxy)
  SceneWriter::registerWriteFunction(writeSpiral);
  SceneWriter::registerWriteFunction(writeTorsion);
}

} // end namespace cg::util
