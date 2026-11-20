// OVERVIEW: reader_test.cpp
// ========
// Unit tests for SceneReader.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "tests/check.h"
#include "reader/SceneReader.h"
#include "graph/CameraProxy.h"
#include "graph/LightProxy.h"
#include "graphics/GLGraphics3.h"
#include <cstring>
#include <filesystem>
#include <fstream>

using namespace cg;

namespace
{ // begin namespace

// reads a scene from a temporary file; returns the reader (scene and materials)
Reference<util::SceneReader>
read(const char* text)
{
  auto path = std::filesystem::temp_directory_path() / "tp_reader_test.scn";

  std::ofstream{path} << text;

  Reference<util::SceneReader> reader = new util::SceneReader;

  reader->setInput(path.string().c_str());
  reader->execute();
  std::filesystem::remove(path);
  return reader;
}

bool
fails(const char* text)
{
  try
  {
    read(text);
  }
  catch (const std::exception& e)
  {
    std::printf("  expected error: %s\n", e.what());
    return true;
  }
  return false;
}

// component of type C of the top level object named name
template <typename C>
C*
find(graph::Scene& scene, const char* name)
{
  for (auto&& o : scene.root()->children())
    if (!std::strcmp(o.name(), name))
      for (auto&& c : o.components())
        if (auto p = dynamic_cast<C*>(&*c))
          return p;
  return nullptr;
}

PBR*
material(graph::Scene& scene, const char* name)
{
  auto proxy = find<graph::PrimitiveProxy>(scene, name);
  return proxy ? dynamic_cast<PBR*>(proxy->mapper()->primitive()->material()) : nullptr;
}

void
testMaterials()
{
  auto r = read(R"(
material "Metal" { diffuse rgb(1,0,0) specular rgb(1,1,0) metallic 0.8 roughness 0.2 }
material "Phong" { diffuse rgb(0,0,1) specular rgb(0.5,0.5,0.5) shine 50 }
scene
{
  object "A" { mesh "Sphere" material "Metal" }
  object "B" { mesh "Sphere" material "Phong" }
  object "C" { mesh "Sphere" }
})");
  auto& scene = *r->scene();
  auto a = material(scene, "A");
  auto b = material(scene, "B");
  auto c = material(scene, "C");

  CHECK(a && a->attrs.m == .8f && a->attrs.r == .2f);
  // Phong converted by PBR::toPBR
  CHECK(b && b->attrs.m == .5f && b->attrs.r < 1);
  CHECK(c && std::strcmp(c->name(), "Default PBR") == 0);
}

void
testLightsAndCamera()
{
  auto r = read(R"(
scene
{
  object "Cam" { camera { parallel height 7 depth <1,50> } }
  object "Dir" { light { directional } }
  object "Spot" { light { spot color rgb(1,0,0) angle 30 range 10 falloff 2 } }
})");
  auto& scene = *r->scene();
  auto cam = find<graph::CameraProxy>(scene, "Cam");
  auto dir = find<graph::LightProxy>(scene, "Dir");
  auto spot = find<graph::LightProxy>(scene, "Spot");

  CHECK(cam && cam->camera()->projectionType() == Camera::Parallel);
  CHECK(cam && cam->camera()->height() == 7);
  CHECK(dir && dir->light()->type() == Light::Type::Directional);
  CHECK(spot && spot->light()->type() == Light::Type::Spot);
  CHECK(spot && spot->light()->spotAngle() == 30);
  CHECK(spot && spot->light()->range() == 10);
  CHECK(spot && spot->light()->falloff == Light::Falloff::Quadratic);
}

void
testSweeps()
{
  auto r = read(R"(
scene
{
  object "S"
  {
    spiral "S"
    {
      polyline { regularPolygon points 4 scale 0.5 }
      capped revolutions 2 subdivisions 8
    }
  }
  object "T"
  {
    torsion "T"
    {
      polyline { circularArc angle 90 segments 6 open }
      length 5 scale <1, 0.5> rotation 1
    }
  }
})");
  auto& scene = *r->scene();
  auto s = find<graph::SpiralProxy<float>>(scene, "S");
  auto t = find<graph::TorsionProxy<float>>(scene, "T");

  CHECK(s && s->attr.capped && s->attr.re == 2 && s->attr.nes == 8);
  CHECK(s && s->polyline().length() == 4 && s->polyline().isClosed());
  CHECK(t && t->attr.lv == 5 && t->attr.se == .5f && t->attr.rv == 1);
  CHECK(t && t->polyline().length() == 7 && !t->polyline().isClosed());
  CHECK(t && t->mapper()->mesh()->triangleCount() > 0);
}

void
testErrors()
{
  CHECK(fails("material \"M\" { roughness 0.01 } scene { }"));
  CHECK(fails("material \"M\" { metallic 2 } scene { }"));
  CHECK(fails("scene { object { mesh \"Sphere\" material \"None\" } }"));
  CHECK(fails("scene { object { mesh \"None\" } }"));
  CHECK(fails("scene { object { spiral \"S\" { capped } } }"));
}

} // end namespace

int
main()
{
  // default meshes, as in MainWindow::beginInitialize
  Assets::initialize();
  Assets::meshes()["Sphere"] = GLGraphics3::sphere();
  testMaterials();
  testLightsAndCamera();
  testSweeps();
  testErrors();
  std::printf("reader_test: %d failure(s)\n", failures);
  return failures != 0;
}
