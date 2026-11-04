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
  bool capped = hasCaps(poly);
  size_t length = poly.length();
  auto np = extruded ? length / 2 : length; // points per segment

  auto de = (_attr.we - poly.width()) / 2;
  auto te = static_cast<R>(cg::math::toRadians(_attr.re * 360));
  auto nre = math::max<size_t>(floor(_attr.re + .5), 1) * _attr.nes;
  auto steps = nre + 1;
  auto deltaHeight = _attr.re * _attr.dh;
  auto deltaWidth = _attr.re * _attr.dw;
  auto deltaAngle = te / nre;

  // computing triangles
  int nv = length * (steps + 1);
  int nt{capped ? nv + 2 * (length - 2) : closed ? nv : nv - 2};
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.triangleCount = nt;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new TriangleMesh::Triangle[nt];

  const auto& points = poly.points();
  setVertexArray<R>(data.vertices, points, length);

  // computing vertices
  R dw = deltaWidth / nre, dh = deltaHeight / nre;
  for(index_t k = 0; k < steps; ++k)
  {
    auto roty = mat3{quat{vec3{0, k * deltaAngle, 0}}};
    vec3 w{de + k * dw, 0, 0};
    vec3 h{0, k * dh, 0};
    for(index_t j = 0; j < length; ++j)
      data.vertices[k*length + j] = roty * (points[j] + w) + h; // transl em x deve vir primeiro que rot e transl em y
  }

  // computing triangles
  for(index_t i = 0, upSeg = 1, curSegInit = 0, upSegInit = length;
    i < steps;
    ++i, ++upSeg, curSegInit = i * length, upSegInit = upSeg * length)
  {
    index_t lastSegPoint = curSegInit + length - 1;
    for(index_t j = 0; j < length; ++j)
    {
      index_t curPoint = curSegInit + j;
      if (!closed && curPoint == lastSegPoint) continue;
      auto p0 = curPoint;
      auto p1 = p0 + 1;
      auto p2 = upSegInit + j;
      auto p3 = p2 + 1;
      setTriangle<R>(&data.triangles[curPoint], p0, p1, p2);
      setTriangle<R>(&data.triangles[curPoint + 1], p1, p3, p2);
    }
  }
  
  // capped
  if (capped)
  { 
    // add mais np verts?
    auto p0 = 0;
    auto pn = steps * length;
    auto ntc = length - 2; // number of triangles per cap
    for (index_t i = 1; i < length - 1; ++i)
    {
      auto p1 = p0 + i;
      auto p2 = p0 + i + 1;
      auto pn_1 = pn + i;
      auto pn_2 = pn + i + 1;
      setTriangle<R>(&data.triangles[nt - 2 * ntc + i - 1], p0, p1, p2);
      setTriangle<R>(&data.triangles[nt - ntc + i - 1], pn, pn_2, pn_1);
    }
  }

  auto* mesh = new TriangleMesh{std::move(data)};
  mesh->computeNormals();
  return mesh;
}

} // end namespace cg

#endif // __SpiralSweeper_h