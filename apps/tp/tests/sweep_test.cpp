// OVERVIEW: sweep_test.cpp
// ========
// Unit tests for polyline generators and sweepers.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "tests/check.h"
#include "include/sweep/PolylineGeneratorProxy.h"
#include "include/sweep/SpiralSweeper.h"
#include "include/sweep/TorsionSweeper.h"
#include <memory>

using namespace cg;

namespace
{ // begin namespace

constexpr auto eps = 1e-4f;

Polyline<float>
polygon(int np, bool extruded = false)
{
  RegularPolygonAttrs<float> a;

  a.np = np;
  a.isExtruded = extruded;
  a.ep = .5f;
  return RegularPolygon<float>{a}.generate();
}

Polyline<float>
arc(float tc, int nc, bool open)
{
  CircularArcAttrs<float> a;

  a.tc = tc;
  a.nc = nc;
  a.isOpen = open;
  return CircularArc<float>{a}.generate();
}

void
testRegularPolygon()
{
  auto p = polygon(5);

  CHECK(p.length() == 5);
  CHECK(p.isClosed());
  CHECK(!p.isExtruded());
  for (size_t i = 0; i < p.length(); ++i)
    CHECK_NEAR(p[i].length(), 1, eps);

  RegularPolygonAttrs<float> a;

  a.sp = 2;
  a.tp = 90;

  auto q = RegularPolygon<float>{a}.generate();

  // the first point (0, 1) rotated by 90 degrees and scaled by 2
  CHECK_NEAR(q[0].x, -2, eps);
  CHECK_NEAR(q[0].y, 0, eps);
}

void
testPolylineLimits()
{
  using limits = PolylineLimits<float>;

  CHECK(polygon(1).length() == limits::MIN_NP);
  CHECK(polygon(100).length() == limits::MAX_NP);

  RegularPolygonAttrs<float> a;

  a.sp = 100;
  a.ep = 2;
  RegularPolygon<float> g{a};
  CHECK(g.attributes().sp == limits::MAX_SCALEP);
  CHECK(g.attributes().ep == limits::MAX_EP);
}

void
testCircularArc()
{
  auto open = arc(90, 4, true);

  CHECK(open.length() == 5);
  CHECK(!open.isClosed());
  CHECK(arc(90, 4, false).isClosed());

  // full circle: polygon with nc points, always closed
  auto full = arc(360, 6, true);

  CHECK(full.length() == 6);
  CHECK(full.isClosed());

  CircularArcAttrs<float> a;

  a.nc = 100;
  CHECK(CircularArc<float>{a}.attributes().nc == CircularArcLimits<float>::MAX_NC);
}

void
testExtrusion()
{
  auto p = polygon(4, true);

  CHECK(p.isExtruded());
  CHECK(p.length() == 8);
  // inner ring scaled by 1 - ep
  CHECK_NEAR(p[4].length(), .5f, eps);
}

bool
unitNormals(const TriangleMesh& m)
{
  for (int i = 0; i < m.vertexCount(); ++i)
    if (std::fabs(m.data().vertexNormals[i].length() - 1) > 1e-3f)
      return false;
  return true;
}

template <typename S>
std::unique_ptr<TriangleMesh>
sweep(const Polyline<float>& p, bool capped)
{
  typename S::attrs_type a;

  a.capped = capped;
  return std::unique_ptr<TriangleMesh>{S{a}.sweep(p)};
}

void
testSpiralSweeper()
{
  // defaults: re = 1, nes = 16 -> 17 rings of 3 points, 16 * 3 quads
  auto m = sweep<SpiralSweeper<float>>(polygon(3), false);

  CHECK(m->vertexCount() == 51);
  CHECK(m->triangleCount() == 96);
  CHECK(unitNormals(*m));

  auto c = sweep<SpiralSweeper<float>>(polygon(3), true);

  CHECK(c->vertexCount() == 51 + 6);
  CHECK(c->triangleCount() == 96 + 2);
}

void
testTorsionSweeper()
{
  // defaults: nsv = 10 -> 11 rings of 3 points, 10 * 3 quads
  auto m = sweep<TorsionSweeper<float>>(polygon(3), false);

  CHECK(m->vertexCount() == 33);
  CHECK(m->triangleCount() == 60);
  CHECK(unitNormals(*m));

  auto c = sweep<TorsionSweeper<float>>(polygon(3), true);

  CHECK(c->vertexCount() == 33 + 6);
  CHECK(c->triangleCount() == 60 + 2);

  TorsionSweeperAttrs<float> a;

  a.nsv = 0;
  a.rv = 10;
  TorsionSweeper<float> s{a};
  CHECK(s.attributes().nsv == TorsionSweeperLimits<float>::MIN_NSV);
  CHECK(s.attributes().rv == TorsionSweeperLimits<float>::MAX_RV);
}

void
testCaps()
{
  // caps only for closed and non-extruded polylines
  auto open = arc(90, 4, true);
  auto extruded = polygon(4, true);

  CHECK(sweep<TorsionSweeper<float>>(open, true)->triangleCount() ==
    sweep<TorsionSweeper<float>>(open, false)->triangleCount());
  CHECK(sweep<TorsionSweeper<float>>(extruded, true)->triangleCount() ==
    sweep<TorsionSweeper<float>>(extruded, false)->triangleCount());
}

void
testGeneratorProxyCopy()
{
  using Proxy = PolylineGeneratorProxy<float>;

  Proxy a;
  CircularArcAttrs<float> arcAttrs;

  arcAttrs.tc = 120;
  arcAttrs.nc = 7;
  a.setType(Proxy::GenType::CircularArc);
  a.activeGen().set(arcAttrs);
  a.setDirty();
  a.build();

  Proxy b;

  b = a;
  CHECK(b.activeGenType() == Proxy::GenType::CircularArc);
  CHECK(b.polyline().length() == 8);
  CHECK(!b.isDirty());

  auto& copied = static_cast<const CircularArcAttrs<float>&>(b.activeGen().attributes());

  CHECK(copied.tc == 120 && copied.nc == 7);

  // the copy is independent
  b.setType(Proxy::GenType::RegularPolygon);
  CHECK(a.activeGenType() == Proxy::GenType::CircularArc);
}

} // end namespace

int
main()
{
  testRegularPolygon();
  testPolylineLimits();
  testCircularArc();
  testExtrusion();
  testSpiralSweeper();
  testTorsionSweeper();
  testCaps();
  testGeneratorProxyCopy();
  std::printf("sweep_test: %d failure(s)\n", failures);
  return failures != 0;
}
