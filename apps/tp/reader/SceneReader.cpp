//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2025 Paulo Pagliosa.                        |
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
// OVERVIEW: SceneReader.cpp
// ========
// Source file for scene reader.
//
// Author: Paulo Pagliosa
// Last revision: 01/11/2025

#include "SceneReader.h"

namespace cg::util
{ // begin namespace cg::util


/////////////////////////////////////////////////////////////////////
//
// SceneReader implementation
// ===========
void
SceneReader::execute()
{
  Assets::initialize();
  Reader::execute();
}

graph::Scene*
SceneReader::makeScene(const char* name) const
{
  return  graph::Scene::New(name);
}

parser::Reader::Parser*
SceneReader::makeParser()
{
  return new Parser{*this};
}


/////////////////////////////////////////////////////////////////////
//
// SceneReader::Parser implementation
// ===================
DEFINE_KEYWORD_TABLE(SceneReader::Parser, parser::Reader::Parser)
  KEYWORD("ambient", _AMBIENT, 0)
  KEYWORD("angle", _ANGLE, 0)
  KEYWORD("aspect", _ASPECT, 0)
  KEYWORD("background", _BACKGROUND, 0)
  KEYWORD("camera", _COMPONENT, _CAMERA)
  KEYWORD("color", _COLOR, 0)
  KEYWORD("depth", _DEPTH, 0)
  KEYWORD("diffuse", _DIFFUSE, 0)
  KEYWORD("directional", _DIRECTIONAL, 0)
  KEYWORD("environment", _ENVIRONMENT, 0)
  KEYWORD("falloff", _FALLOFF, 0)
  KEYWORD("height", _HEIGHT, 0)
  KEYWORD("ior", _IOR, 0)
  KEYWORD("light", _COMPONENT, _LIGHT)
  KEYWORD("material", _MATERIAL, 0)
  KEYWORD("mesh", _COMPONENT, _MESH)
  KEYWORD("metallic", _METALLIC, 0)
  KEYWORD("object", _OBJECT, 0)
  KEYWORD("parallel", _PARALLEL, 0)
  KEYWORD("perspective", _PERSPECTIVE, 0)
  KEYWORD("point", _POINT, 0)
  KEYWORD("position", _POSITION, 0)
  KEYWORD("range", _RANGE, 0)
  KEYWORD("rotation", _ROTATION, 0)
  KEYWORD("roughness", _ROUGHNESS, 0)
  KEYWORD("scale", _SCALE, 0)
  KEYWORD("scene", _SCENE, 0)
  KEYWORD("shine", _SHINE, 0)
  KEYWORD("specular", _SPECULAR, 0)
  KEYWORD("spot", _SPOT, 0)
  KEYWORD("transform", _TRANSFORM, 0)
  KEYWORD("transparency", _TRANSPARENCY, 0)

  // sweep keywords (sweeps are primitives, like mesh)
  KEYWORD("spiral", _COMPONENT, _SWEEP_SPIRAL)
  KEYWORD("torsion", _COMPONENT, _SWEEP_TORSION)
  KEYWORD("capped", _SWEEP_CAPPED, 0)
  KEYWORD("polyline", _SWEEP_POLYLINE, 0)
  KEYWORD("subdivisions", _SWEEP_SUBDIVISIONS, 0)
  KEYWORD("circularArc", _POLY_CIRCULAR_ARC, 0)
  KEYWORD("extrusion", _POLY_EXTRUSION, 0)
  KEYWORD("points", _POLY_POINTS, 0)
  KEYWORD("regularPolygon", _POLY_REGULAR_POLYGON, 0)
  KEYWORD("open", _ARC_OPEN, 0)
  KEYWORD("segments", _ARC_SEGMENTS, 0)
  KEYWORD("heightIncrement", _SPIRAL_HEIGHT_INCREMENT, 0)
  KEYWORD("revolutions", _SPIRAL_REVOLUTIONS, 0)
  KEYWORD("width", _SPIRAL_WIDTH, 0)
  KEYWORD("widthIncrement", _SPIRAL_WIDTH_INCREMENT, 0)
  KEYWORD("offset", _TORSION_OFFSET, 0)
  END_KEYWORD_TABLE;

DEFINE_ERROR_MESSAGE_TABLE(SceneReader::Parser, parser::Reader::Parser)
  ERROR_MESSAGE(MATERIAL_ALREADY_DEFINED,
    "Material '%s' already defined")
  ERROR_MESSAGE(COMPONENT_ALREADY_DEFINED,
    "Component '%s' already defined")
  ERROR_MESSAGE(INVALID_VALUE_FOR,
    "Invalid value for '%s'")
  ERROR_MESSAGE(EMPTY_MESH_NAME,
    "Empty mesh name")
  ERROR_MESSAGE(COULD_NOT_FIND_MESH,
    "Could not find mesh '%s'")
  ERROR_MESSAGE(COULD_NOT_FIND_MATERIAL,
    "Could not find material '%s'")
  ERROR_MESSAGE(COULD_NOT_PARSE_COMPONENT,
    "Could not parse component '%s'")
  ERROR_MESSAGE(POLYLINE_EXPECTED,
    "Polyline block expected")
  ERROR_MESSAGE(POLYLINE_TYPE_EXPECTED,
    "Polyline type ('regularPolygon' or 'circularArc') expected")
END_ERROR_MESSAGE_TABLE;

inline void
SceneReader::Parser::preamble()
{
  for (;;)
    switch (_token)
    {
      case _INCLUDE:
        include();
        break;
      case _DEFINE:
        declaration();
        break;
      case _MATERIAL:
        parseMaterial();
        break;
      default:
        return;
    }
}

void
SceneReader::Parser::start() try
{
  preamble();
  if (_token == _SCENE)
    if (_scene != nullptr)
      error(MULTIPLE_SCENE_DEFINITION);
    else
      parseScene();
  if (_token != _EOF)
    if (_token < 256)
      error(UNEXPECTED_CHAR, _token);
    else
      error(SYNTAX);
}
catch (const std::exception&)
{
  _reader->_scene = nullptr;
  throw;
}

void
SceneReader::Parser::declaration()
{
  // _DEFINE
  advance();

  auto name = matchName();

  if (_token == '=')
    advance();
  define(name, initializer());
}

inline void
SceneReader::Parser::parseMaterialProperties(Material& material, PBRSpec& pbr_spec)
{
  for (;;)
    switch (_token)
    {
      //legacy Phong
      case _AMBIENT:
        advance();
        material.ambient = matchColor();
        break;
      case _DIFFUSE:
        advance();
        material.diffuse = matchColor();
        break;
      case _SPOT:
        advance();
        material.spot = matchColor();
        break;
      case _SHINE:
        advance();
        if (auto shine = matchFloat(); shine <= 0)
          error(INVALID_VALUE_FOR, "shine");
        else
          material.shine = shine;
        break;
      case _SPECULAR:
        advance();
        material.specular = matchColor();;
        break;
      case _TRANSPARENCY:
        advance();
        material.transparency = matchColor();;
        break;
      case _IOR:
        advance();
        if (auto ior = matchFloat(); ior <= 0)
          error(INVALID_VALUE_FOR, "ior");
        else
          material.ior = ior;
        break;

      //PBR
      case _METALLIC:
        advance();
        if (auto v = matchFloat(); v < 0 || v > 1)
          error(INVALID_VALUE_FOR, "metallic");
        else
          pbr_spec.metallic = v, pbr_spec.explicitPBR = true;
        break;
      case _ROUGHNESS:
        advance();
        if (auto v = matchFloat(); v < .05f || v > 1)
          error(INVALID_VALUE_FOR, "roughness");
        else
          pbr_spec.roughness = v, pbr_spec.explicitPBR = true;
      break;
      
      default:
        return;
    }
}

void
SceneReader::Parser::parseMaterial()
{
  // _MATERIAL
  advance();

  auto name = matchString();

  if (_reader->materials.contains(name))
    error(MATERIAL_ALREADY_DEFINED, name.c_str());
  match('{');

  Reference<Material> material;
  Reference<Material> legacy_mat = new Material{Color::black};
  PBRSpec pbr_spec;

  parseMaterialProperties(*legacy_mat, pbr_spec);
  matchEndOfBlock();

  if(pbr_spec.explicitPBR)
    material = new PBR{
      legacy_mat->diffuse,
      legacy_mat->specular,
      pbr_spec.metallic,
      pbr_spec.roughness
    };
  else{
    material = PBR::toPBR(*legacy_mat);
    //TODO: maybe remove Warning
    printf("Warning: material '%s' uses legacy Phong properties; converted to PBR\n",
      name.c_str());
  }
      
  material->setName(name.c_str());
  _reader->materials.emplace(name, material);
}

inline void
SceneReader::Parser::parseSceneEnvironment()
{
  // _ENVIRONMENT
  advance();
  match('{');
  for (;;)
    if (_token == _AMBIENT)
    {
      advance();
      _scene->ambientLight = matchColor();
    }
    else if (_token == _BACKGROUND)
    {
      advance();
      _scene->backgroundColor = matchColor();
    }
    else
      break;
  matchEndOfBlock();
}

void
SceneReader::Parser::parseWorld()
{
  // do nothing
}

void
SceneReader::Parser::parseScene()
{
  // _SCENE
  advance();

  auto sceneName = matchOptionalString();
  auto scene = _reader->makeScene(sceneName.c_str());

  setScene(*scene);
  match('{');
  beginBlock();
  while (_token == _DEFINE)
    declaration();
  if (_token == _ENVIRONMENT)
    parseSceneEnvironment();
  parseWorld();
  parseObjectBlock(*scene->root());
  _reader->_scene = scene;
}

void
SceneReader::Parser::parseObjectBlock(graph::SceneObject& object)
{
  for (;;)
    if (_token == _OBJECT)
      parseObject(object);
    else if (_token == _DEFINE)
      declaration();
    else
      break;
  matchEndOfBlock();
  endBlock();
}

inline void
SceneReader::Parser::parseTransform(graph::SceneObject& object)
{
  // _TRANSFORM
  advance();
  match('{');
  for (auto t = object.transform();;)
    switch (_token)
    {
      case _POSITION:
        advance();
        t->setLocalPosition(matchVec3());
        break;
      case _ROTATION:
        advance();
        t->setLocalEulerAngles(matchVec3());
        break;
      case _SCALE:
        advance();
        if (auto s = matchVec3(); s.x > 0 && s.y > 0 && s.z > 0)
          t->setLocalScale(s);
        else
          error(INVALID_VALUE_FOR, "scale");
        break;
      default:
        matchEndOfBlock();
        return;
    }
}

inline void
SceneReader::Parser::parseChildObjectBlock(graph::SceneObject& object)
{
  while (_token == _DEFINE)
    declaration();
  if (_token == _TRANSFORM)
    parseTransform(object);
  while (_token == _COMPONENT)
    parseComponent(object);
  parseObjectBlock(object);
}

void
SceneReader::Parser::parseObject(graph::SceneObject& object)
{
  // _OBJECT
  advance();

  auto childName = matchOptionalString();
  auto child = createEmptyObject();

  if (!childName.empty())
    child->setName(childName.c_str());
  child->setParent(&object);
  if (_token == _COMPONENT)
    parseComponent(*child);
  else
  {
    match('{');
    beginBlock();
    parseChildObjectBlock(*child);
  }
}

inline Reference<graph::CameraProxy>
SceneReader::Parser::matchCamera()
{
  // _COMPONENT (_CAMERA)
  advance();
  match('{');

  Reference<graph::CameraProxy> proxy{graph::CameraProxy::New()};

  for (auto camera = proxy->camera();;)
  {
    if (_token == _PERSPECTIVE)
      advance();
    else if (_token == _PARALLEL)
    {
      advance();
      camera->setProjectionType(Camera::Parallel);
    }
    switch (_token)
    {
      case _ANGLE:
        advance();
        if (auto angle = matchFloat(); angle > 0)
          camera->setViewAngle(angle);
        else
          error(INVALID_VALUE_FOR, "angle");
        break;
      case _ASPECT:
        advance();
        if (auto aspect = matchFloat(); aspect > 0)
          camera->setAspectRatio(aspect);
        else
          error(INVALID_VALUE_FOR, "aspect");
        break;
      case _DEPTH:
        advance();
        if (auto z = matchVec2(); z.x > 0 && z.y > z.x)
          camera->setClippingPlanes(z.x, z.y);
        else
          error(INVALID_VALUE_FOR, "depth");
        break;
      case _HEIGHT:
        advance();
        if (auto height = matchFloat(); height > 0)
          camera->setHeight(height);
        else
          error(INVALID_VALUE_FOR, "height");
        break;
      default:
        matchEndOfBlock();
        graph::CameraProxy::setCurrent(camera);
        return proxy;
    }
  }
}

inline Reference<graph::LightProxy>
SceneReader::Parser::matchLight()
{
  // _COMPONENT (_LIGHT)
  advance();
  match('{');

  Reference<graph::LightProxy> proxy{graph::LightProxy::New()};

  for (auto light = proxy->light();;)
  {
    if (_token == _POINT)
    {
      advance();
      light->setType(Light::Type::Point);
    }
    else if (_token == _DIRECTIONAL)
    {
      advance();
      light->setType(Light::Type::Directional);
    }
    else if (_token == _SPOT)
    {
      advance();
      light->setType(Light::Type::Spot);
    }
    switch (_token)
    {
      case _COLOR:
        advance();
        light->color = matchColor();
        break;
      case _RANGE:
        advance();
        if (auto range = matchFloat(); range >= 0)
          light->setRange(range);
        else
          error(INVALID_VALUE_FOR, "range");
        break;
      case _ANGLE:
        advance();
        if (auto angle = matchFloat(); angle > 0)
          light->setSpotAngle(angle);
        else
          error(INVALID_VALUE_FOR, "angle");
        break;
      case _FALLOFF:
        advance();
        light->falloff = (Light::Falloff)matchIndex(0, 2);
        break;
      default:
        matchEndOfBlock();
        return proxy;
    }
  }
}

void
SceneReader::Parser::parsePrimitiveMaterial(Primitive& primitive)
{
  if (_token == _MATERIAL)
  {
    advance();

    auto name = matchString();

    if (auto material = _reader->findMaterial(name); material == nullptr)
      error(COULD_NOT_FIND_MATERIAL, name.c_str());
    else
      primitive.setMaterial(material);
  }
}

Material*
SceneReader::Parser::defaultPBR()
{
  const parser::String name{"Default PBR"};

  if (auto m = _reader->findMaterial(name))
    return m;

  auto pbr = new PBR{Color{.8f, .8f, .8f}, Color::white};

  pbr->setName("%s", name.c_str());
  _reader->materials.emplace(name, pbr);
  return pbr;
}

SceneReader::Parser::PrimitiveType
SceneReader::Parser::primitiveType(int type)
{
  switch (type)
  {
    case _MESH:
      return PRIMITIVE_BASE;
    case _SWEEP_SPIRAL:
    case _SWEEP_TORSION:
      return PRIMITIVE_SWEEP;
    default:
      return PRIMITIVE_NONE;
  }
}

// PRIMITIVE_BASE: mesh loaded from the assets
inline Reference<graph::PrimitiveProxy>
SceneReader::Parser::matchMesh(const parser::String& name)
{
  Reference<graph::PrimitiveProxy> proxy;

  if (auto mesh = Assets::loadMesh(name); nullptr == mesh)
    error(COULD_NOT_FIND_MESH, name.c_str());
  else
  {
    proxy = makePrimitive(*mesh, name);

    auto primitive = proxy->mapper()->primitive();

    primitive->setMaterial(defaultPBR());
    parsePrimitiveMaterial(*primitive);
  }
  return proxy;
}

// PRIMITIVE_SWEEP: mesh generated by a sweep
// common items of all polylines; returns true if one was consumed
bool
SceneReader::Parser::parsePolylineItem(PolylineAttrs<float>& a)
{
  switch (_token)
  {
    case _SCALE:
      advance();
      a.sp = matchFloat();
      return true;
    case _ROTATION:
      advance();
      a.tp = matchFloat();
      return true;
    case _POLY_EXTRUSION:
      advance();
      a.ep = matchFloat();
      a.isExtruded = true;
      return true;
    default:
      return false;
  }
}

void
SceneReader::Parser::parseRegularPolygon(RegularPolygonAttrs<float>& a)
{
  // _POLY_REGULAR_POLYGON
  advance();
  for (;;)
    if (_token == _POLY_POINTS)
    {
      advance();
      a.np = matchInteger();
    }
    else if (!parsePolylineItem(a))
      return;
}

void
SceneReader::Parser::parseCircularArc(CircularArcAttrs<float>& a)
{
  // _POLY_CIRCULAR_ARC
  advance();
  for (;;)
    switch (_token)
    {
      case _ANGLE:
        advance();
        a.tc = matchFloat();
        break;
      case _ARC_SEGMENTS:
        advance();
        a.nc = matchInteger();
        break;
      case _ARC_OPEN:
        advance();
        a.isOpen = true;
        break;
      default:
        if (!parsePolylineItem(a))
          return;
    }
}

void
SceneReader::Parser::parsePolyline(graph::SweepMeshProxy<float>& proxy)
{
  if (_token != _SWEEP_POLYLINE)
    error(POLYLINE_EXPECTED);
  advance();
  match('{');

  auto& gen = proxy.polyGen();

  if (_token == _POLY_REGULAR_POLYGON)
  {
    RegularPolygonAttrs<float> a;

    parseRegularPolygon(a);
    gen.setType(GenType::RegularPolygon);
    gen.activeGen().set(a);
  }
  else if (_token == _POLY_CIRCULAR_ARC)
  {
    CircularArcAttrs<float> a;

    parseCircularArc(a);
    gen.setType(GenType::CircularArc);
    // CircularArc::set gets tc, nc and isOpen by dynamic_cast
    gen.activeGen().set(a);
  }
  else
    error(POLYLINE_TYPE_EXPECTED);
  // an item of another polyline type (e.g. 'points' in an arc) stops here
  matchEndOfBlock();
  proxy.setPolylineDirty();
}

void
SceneReader::Parser::parseSweeperItems(graph::SpiralProxy<float>& proxy)
{
  auto& a = proxy.attr;

  for (;;)
    switch (_token)
    {
      case _SWEEP_CAPPED:
        advance();
        a.capped = true;
        break;
      case _SPIRAL_WIDTH:
        advance();
        a.we = matchFloat();
        break;
      case _SPIRAL_REVOLUTIONS:
        advance();
        a.re = matchFloat();
        break;
      case _SWEEP_SUBDIVISIONS:
        advance();
        a.nes = matchInteger();
        break;
      case _SPIRAL_HEIGHT_INCREMENT:
        advance();
        a.dh = matchFloat();
        break;
      case _SPIRAL_WIDTH_INCREMENT:
        advance();
        a.dw = matchFloat();
        break;
      default:
        return;
    }
}

void
SceneReader::Parser::parseSweeperItems(graph::TorsionProxy<float>& proxy)
{
  auto& a = proxy.attr;

  for (;;)
    switch (_token)
    {
      case _SWEEP_CAPPED:
        advance();
        a.capped = true;
        break;
      case _LENGTH: // token of the base reader (function length)
        advance();
        a.lv = matchFloat();
        break;
      case _TORSION_OFFSET:
      {
        advance();

        auto v = matchVec2();

        a.ow = v.x;
        a.oh = v.y;
        break;
      }
      case _SWEEP_SUBDIVISIONS:
        advance();
        a.nsv = matchInteger();
        break;
      case _SCALE:
      {
        advance();

        auto v = matchVec2();

        a.sb = v.x;
        a.se = v.y;
        break;
      }
      case _ROTATION:
        advance();
        a.rv = matchFloat();
        break;
      default:
        return;
    }
}

template <typename P>
Reference<graph::PrimitiveProxy>
SceneReader::Parser::parseSweep(const parser::String& name)
{
  Reference<P> proxy{P::New(name.c_str())};
  auto primitive = proxy->mapper()->primitive();

  primitive->setMaterial(defaultPBR());
  parsePrimitiveMaterial(*primitive);
  match('{');
  parsePolyline(*proxy);
  parseSweeperItems(*proxy);
  matchEndOfBlock();
  // regenerates the polyline and the mesh (sweeper attrs are clamped here)
  proxy->build();
  return Reference<graph::PrimitiveProxy>{proxy.get()};
}

Reference<graph::PrimitiveProxy>
SceneReader::Parser::matchSweep(int type, const parser::String& name)
{
  return type == _SWEEP_SPIRAL ?
    parseSweep<graph::SpiralProxy<float>>(name) :
    parseSweep<graph::TorsionProxy<float>>(name);
}

// single entry for primitives: the primitive type selects the rules
inline Reference<graph::PrimitiveProxy>
SceneReader::Parser::matchPrimitive(int type)
{
  auto primitive = primitiveType(type);

  if (primitive == PRIMITIVE_NONE)
    return {};

  auto name = matchString();

  if (name.empty())
    error(EMPTY_MESH_NAME);
  if (primitive == PRIMITIVE_SWEEP)
    return matchSweep(type, name);
  return matchMesh(name);
}

Reference<graph::Component>
SceneReader::Parser::parseComponent(int type, graph::SceneObject&)
{
  return (graph::Component*)matchPrimitive(type);
}

void
SceneReader::Parser::parseComponent(graph::SceneObject& object)
{
  Reference<graph::Component> component;
  auto type = (int)(size_t)_tokenValue.object;

  if (type == _CAMERA)
    component = matchCamera();
  else if (type == _LIGHT)
    component = matchLight();
  else
  {
    auto string = _lexeme;

    // _COMPONENT (...)
    advance();
    if ((component = parseComponent(type, object)) == nullptr)
      error(COULD_NOT_PARSE_COMPONENT, string.c_str());
  }

  auto typeName = component->typeName();

  if (object.addComponent<graph::Component>(component) == nullptr)
    error(COMPONENT_ALREADY_DEFINED, typeName);
}

} // end namespace cg::util
