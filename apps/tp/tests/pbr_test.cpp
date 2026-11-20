// OVERVIEW: pbr_test.cpp
// ========
// Unit tests for the PBR material.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "tests/check.h"
#include "include/pbr/PBR.h"

using namespace cg;

namespace
{ // begin namespace

constexpr auto eps = 1e-4f;

bool
nearColor(const Color& a, const Color& b)
{
  return std::fabs(a.r - b.r) <= eps &&
    std::fabs(a.g - b.g) <= eps &&
    std::fabs(a.b - b.b) <= eps;
}

bool
isValid(const Color& c)
{
  return std::isfinite(c.r) && std::isfinite(c.g) && std::isfinite(c.b) &&
    c.r >= 0 && c.g >= 0 && c.b >= 0;
}

void
testAttributes()
{
  const Color gold{1.f, .8f, .3f};
  PBRAttributes dielectric{Color::red, gold, 0, .5f};
  PBRAttributes metal{Color::red, gold, 1, .5f};

  CHECK(nearColor(dielectric.f0(), PBRAttributes::minOs));
  CHECK(nearColor(metal.f0(), gold));
  CHECK(nearColor(dielectric.diffuseColor(), Color::red));
  CHECK(nearColor(metal.diffuseColor(), Color::black));
}

void
testToPBR()
{
  Material phong{Color::black};

  phong.ambient = Color::red;
  phong.diffuse = Color::black;
  phong.specular = Color::black;
  phong.spot = Color::green;
  phong.shine = 1000;
  phong.setName("phong");

  Reference<PBR> p = PBR::toPBR(phong);

  // black diffuse uses ambient; black specular uses spot
  CHECK(nearColor(p->attrs.od, Color::red));
  CHECK(nearColor(p->attrs.os, Color::green));
  CHECK(p->attrs.m == 0);
  CHECK_NEAR(p->attrs.r, powf(2.f / 1002, .25f), eps);
  CHECK(std::string{p->name()} == "phong");

  phong.shine = 1e7f;
  p = PBR::toPBR(phong);
  CHECK(p->attrs.r == .05f); // roughness clamped to the minimum

  phong.shine = .001f;
  phong.specular = Color::white;
  p = PBR::toPBR(phong);
  CHECK(p->attrs.r <= 1);
  CHECK(p->attrs.m == 1);
}

void
testSpecularReflectionColor()
{
  for (auto m : {0.f, 1.f})
    for (auto r : {.05f, .5f, 1.f})
    {
      PBR p{Color::white, Color{.9f, .6f, .2f}, m, r};
      auto front = p.specularReflectionColor(1);
      auto grazing = p.specularReflectionColor(0);

      CHECK(isValid(front) && isValid(grazing));
      // never black (rough dielectrics reflect ~0.016 at normal incidence)
      CHECK(front.r > .01f && front.g > .01f && front.b > .01f);
      // Fresnel: stronger reflection at grazing angles
      CHECK(grazing.r + grazing.g + grazing.b >= front.r + front.g + front.b);
    }
}

void
testShade()
{
  PBR p{Color::white, Color::white, .5f, .05f};
  const vec3f N{0, 0, 1};
  const auto lc = Color::white;

  // light and viewer along the normal
  auto c = p.shade(N, vec3f{0, 0, -1}, N, -1, 1, lc);

  CHECK(isValid(c));

  // degenerate grazing angles: no NaN/Inf
  const vec3f V{1, 0, -1e-6f};
  const vec3f L{-1, 0, 1e-6f};

  CHECK(isValid(p.shade(N, V.versor(), L.versor(), -1e-6f, 1e-6f, lc)));
  CHECK(isValid(p.shade(N, vec3f{0, 0, -1}, L.versor(), -1, 1e-6f, lc)));

  // the specular peak is brighter than an oblique light
  auto oblique = p.shade(N, vec3f{0, 0, -1}, vec3f{1, 0, 1}.versor(), -1, .7071f, lc);

  CHECK(c.r > oblique.r);
}

} // end namespace

int
main()
{
  testAttributes();
  testToPBR();
  testSpecularReflectionColor();
  testShade();
  std::printf("pbr_test: %d failure(s)\n", failures);
  return failures != 0;
}
