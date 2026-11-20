// OVERVIEW: raytracer_test.cpp
// ========
// Unit tests for RayTracer.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "tests/check.h"
#include "tests/scene.h"
#include "graph/CameraProxy.h"
#include "RayTracer.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace cg;
using namespace cg::test;

namespace
{ // begin namespace

/////////////////////////////////////////////////////////////////////
//
// MemoryImage: image in main memory (no OpenGL)
// ===========
class MemoryImage: public Image
{
public:
  MemoryImage(int w, int h):
    Image{w, h},
    _pixels(w * h)
  {
    // do nothing
  }

  const Pixel& operator ()(int x, int y) const
  {
    return _pixels[y * _W + x];
  }

  void draw(int, int) const override
  {
    // do nothing
  }

private:
  std::vector<Pixel> _pixels;

  void setSubImage(int x, int y, int w, int h, const Pixel* p) override
  {
    for (int j = 0; j < h; ++j)
      for (int i = 0; i < w; ++i)
        _pixels[(y + j) * _W + x + i] = *p++;
  }

  void getSubImage(int x, int y, int w, int h, Pixel* p) const override
  {
    for (int j = 0; j < h; ++j)
      for (int i = 0; i < w; ++i)
        *p++ = _pixels[(y + j) * _W + x + i];
  }

}; // MemoryImage

// metallic sphere in front of the camera, blue background
constexpr auto sceneText = R"(
material "Mirror" { diffuse rgb(1,1,1) specular rgb(1,1,1) metallic 1 roughness 0.05 }
scene
{
  environment { background rgb(0,0,1) }
  object "Camera" { transform { position <0,0,10> } camera { angle 60 } }
  object "Light" { transform { position <0,5,10> } light { point } }
  object "Sphere" { transform { scale vec3(2) } mesh "Sphere" material "Mirror" }
})";

constexpr auto size = 8;

struct Render
{
  MemoryImage image{size, size};
  uint64_t rays{};
};

// renders the scene and gets the number of rays printed by renderImage
Render
render(graph::Scene& scene, float minWeight)
{
  Render r;
  auto camera = find<graph::CameraProxy>(scene, "Camera")->camera();
  RayTracer rt{scene, *camera};
  std::ostringstream out;

  // restores std::cout even if renderImage throws
  struct Redirect
  {
    std::streambuf* old;
    ~Redirect() { std::cout.rdbuf(old); }
  } redirect{std::cout.rdbuf(out.rdbuf())};

  rt.setMinWeight(minWeight);
  rt.renderImage(r.image);

  auto s = out.str();
  auto i = s.find("Number of rays: ");

  if (i != s.npos)
    r.rays = std::stoull(s.substr(i + 16));
  return r;
}

bool
isBackground(const Pixel& p)
{
  return p.r == 0 && p.g == 0 && p.b == 255;
}

void
testHit()
{
  auto reader = read(sceneText);
  auto r = render(*reader->scene(), RayTracer::defaultMinWeight);

  CHECK(!isBackground(r.image(size / 2, size / 2)));
  CHECK(isBackground(r.image(0, 0)));
}

void
testMinWeight()
{
  auto reader = read(sceneText);
  auto all = render(*reader->scene(), RayTracer::minMinWeight);
  auto none = render(*reader->scene(), 1);

  // a weight of 1 cuts every reflection ray
  CHECK(all.rays > 0 && none.rays > 0);
  CHECK(none.rays < all.rays);
}

void
testOnlyPBR()
{
  auto reader = read(sceneText);
  auto& scene = *reader->scene();
  auto proxy = find<graph::PrimitiveProxy>(scene, "Sphere");

  proxy->mapper()->primitive()->setMaterial(new Material{Color::red});

  bool thrown{};

  try
  {
    render(scene, RayTracer::defaultMinWeight);
  }
  catch (const std::runtime_error&)
  {
    thrown = true;
  }
  CHECK(thrown);
}

} // end namespace

int
main()
{
  initAssets();
  testHit();
  testMinWeight();
  testOnlyPBR();
  std::printf("\nraytracer_test: %d failure(s)\n", failures);
  return failures != 0;
}
