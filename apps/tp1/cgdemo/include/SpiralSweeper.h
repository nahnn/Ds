// OVERVIEW: SpiralSweeper.h
// ========
// Class definition for spiral sweepers.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __SpiralSweeper_h
#define __SpiralSweeper_h

#include "PolylineSweeper.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct SpiralSweeperLimits: public PolylineSweeperLimits<R>
{
  // --- Initial Width ---
  //real min is 2 * bx (depends on the polyline), checked in sweep()
  static constexpr R MIN_WE_ABS = 0; // min we (abs)
  static constexpr R MAX_WE_ABS = 100; // max we (abs)

  // --- Revolutions ---
  static constexpr R MIN_RE = .1; // min re
  static constexpr R MAX_RE = 20; // max re

  // --- Subdivisions per Revolution ---
  static constexpr int MIN_NES = 3; // min nes
  static constexpr int MAX_NES = 40; // max nes

  // --- Height Increment per Revolution ---
  static constexpr R MIN_DELTAH = 0; // min dh
  static constexpr R MAX_DELTAH = 10; // max dh

  // --- Width Increment per Revolution ---
  static constexpr R MIN_DELTAW = 0; // min dw
  static constexpr R MAX_DELTAW = 10; // max dw

}; // SpiralSweeperLimits

template<IsReal R>
struct SpiralSweeperAttrs: public PolylineSweeperAttrs<R>
{
  R we{4}; // initial width
  R re{1}; // number of revolutions
  int nes{16}; // subdivisions per revolution
  R dh{2}; // height increment per revolution
  R dw{2}; // width increment per revolution
}; // SpiralSweeperAttrs


template<IsReal R>
class SpiralSweeper: public PolylineSweeper<R>
{
public:
  using type = SpiralSweeper<R>;
  using value_type = R;
  using polyline_type = Polyline<R>;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;
  using limits = SpiralSweeperLimits<R>;
  using attrs_type = SpiralSweeperAttrs<R>;

  explicit SpiralSweeper(const attrs_type& attr = {})
  {
    set(attr);
  }

  /// @brief Covariant: the specific attributes, instead of the common ones
  const attrs_type& attributes() const override
  {
    return _attr;
  }

  /// sets the attributes
  void set(const attrs_type& attr)
  {
    auto a = attr;

    //clamp set base attributes
    this->clampSet(a);

    a.we = std::clamp(a.we, limits::MIN_WE_ABS, limits::MAX_WE_ABS);
    a.re = std::clamp(a.re, limits::MIN_RE, limits::MAX_RE);
    a.nes = std::clamp(a.nes, limits::MIN_NES, limits::MAX_NES);
    a.dh = std::clamp(a.dh, limits::MIN_DELTAH, limits::MAX_DELTAH);
    a.dw = std::clamp(a.dw, limits::MIN_DELTAW, limits::MAX_DELTAW);

    _attr = a;
  }

  [[nodiscard]] TriangleMesh* sweep(const polyline_type& polyline) const override;

private:
  attrs_type _attr;

}; // SpiralSweeper

template<IsReal R>
TriangleMesh* SpiralSweeper<R>::sweep(const Polyline<R>& poly) const
{
  // computing process parameters
  bool extruded = poly.isExtruded();
  bool closed = poly.isClosed();
  bool capped = this->hasCaps(poly);
  const auto L = static_cast<index_t>(poly.length()); // number of points per segment
  const index_t rings = extruded ? 2 : 1; // rings per segment
  const index_t np = L / rings; // points per ring  
  const auto& points = poly.points();

  const R bx = poly.width();
  const R we = math::max<R>(_attr.we, 2*bx);
  const auto de = (we - bx) / 2;
  const auto te = static_cast<R>(_attr.re * 360);
  const index_t nre = math::max<index_t>(floor(_attr.re + R(.5)), 1) * static_cast<index_t>(_attr.nes);
  const index_t steps = nre + 1;
  const auto deltaHeight = _attr.re * _attr.dh;
  const auto deltaWidth = _attr.re * _attr.dw;
  const R dw = deltaWidth / nre, dh = deltaHeight / nre;
  const auto deltaAngle = te / nre;

  // initializing triangle mesh data
  const index_t nv = L * steps + (capped ? 2 * L : 0);
  const index_t nt = nre * rings * (closed ? np : np - 1) * 2 + (capped ? 2 * (L - 2) : 0);
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.triangleCount = nt;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new TriangleMesh::Triangle[nt];

  // computing vertices
  for(index_t k = 0; k < steps; ++k)
  {
    auto roty = mat3{quat::eulerAngles(vec3{0, k * deltaAngle, 0})};
    vec3 w{de + k * dw, 0, 0};
    vec3 h{0, k * dh, 0};
    for(index_t j = 0; j < L; ++j)
      data.vertices[k*L + j] = roty * (points[j] + w) + h;
  }

  // computing triangles
  const index_t totalSteps = rings * nre;
  index_t t{};
  for(index_t i = 0, curSegInit = 0, upSegInit = L, curSegEnd = np - 1, upSegEnd = L + curSegEnd; 
    i < totalSteps; 
    ++i, curSegInit += np, upSegInit += np, curSegEnd += np, upSegEnd += np)
  {
    for(index_t j = 0; j < np - 1; ++j)
    {
      auto p0 = curSegInit + j;
      auto p1 = p0 + 1;
      auto p3 = upSegInit + j;
      auto p2 = p3 + 1;
      setTriangle<R>(&data.triangles[t++], p0, p1, p2);
      setTriangle<R>(&data.triangles[t++], p0, p2, p3);
    }

    if (closed)
    {
      auto p0 = curSegEnd;
      auto p1 = curSegInit;
      auto p2 = upSegInit;
      auto p3 = upSegEnd;
      setTriangle<R>(&data.triangles[t++], p0, p1, p2);
      setTriangle<R>(&data.triangles[t++], p0, p2, p3);
    }
  }
  
  // capped
  if (capped)
  {
    const index_t p0 = steps * L, pn = p0 + L, lastSeg = (steps - 1) * L;
    for(index_t i = 0; i < L; ++i)
    {
      vec3 pi{data.vertices[i].x, data.vertices[i].y, data.vertices[i].z};
      vec3 pni{data.vertices[lastSeg + i].x, data.vertices[lastSeg + i].y, data.vertices[lastSeg + i].z};
      data.vertices[p0 + i] = pi;
      data.vertices[pn + i] = pni;
    }

    for (index_t i = 1; i < L - 1; ++i)
    {
      auto p1 = p0 + i;
      auto p2 = p0 + i + 1;
      auto pn_1 = pn + i;
      auto pn_2 = pn + i + 1;
      setTriangle<R>(&data.triangles[t++], p0, p1, p2);
      setTriangle<R>(&data.triangles[t++], pn, pn_2, pn_1);
    }
  }

  auto* mesh = new TriangleMesh{std::move(data)};
  mesh->computeNormals();
  return mesh;
}

} // end namespace cg

#endif // __SpiralSweeper_h
