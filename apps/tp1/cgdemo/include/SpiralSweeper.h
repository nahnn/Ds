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
  auto de = (_attr.we - poly.width()) / 2;
  auto te = static_cast<R>(cg::math::toRadians(_attr.re * 360));
  auto steps = math::max<size_t>(floor(_attr.re + .5), 1) * _attr.nes + 1;
  auto deltaHeight = _attr.re * _attr.dh;
  vec3 dH{0, deltaHeight, 0};
  auto deltaWidth = _attr.re * _attr.dw;
  vec3 dW{de + deltaWidth, 0, 0};
  vec3 t{dW + dH};
  auto deltaAngle = te / (steps - 1);
  auto rotMat = mat3{quat{vec3{0, deltaAngle, 0}}};

  // computing triangles
  int nv = poly.length() * steps;
  int nt{(2 * nv - 2) * steps};
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.triangleCount = nt;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new vec3[nt];
  vec3* vertCache = new vec3[nv]; // se pa que deletar no fim do método

  const auto& points = poly.points();
  auto np = poly.isExtruded() ? poly.length() / 2 : poly.length();
  setVertexArray<R>(data.vertices, points, np);
  setVertexArray<R>(vertCache, points, np);

  // computing steps
  for(index_t i = 0; i < steps; ++i) // current line 
    for(index_t j = 0; j < np; ++j) // points
    {
      vec3 pi{rotMat * vertCache[i * np + j]}; // new point rotated
      pi = pi + i * t;
      vertCache[i * np + j] = data.vertices[i * np + j] = pi;
      setTriangles<R>(data.triangles, i, i + 1, np);  
    }
  
  // TODO: open / closed and capped / uncapped (conectar os vértices da primeira e última linha e / ou fechar)
  delete[] vertCache;
  TriangleMesh* tri = new TriangleMesh{std::move(data)};
  tri->computeNormals();
  return tri;
}

} // end namespace cg

#endif // __SpiralSweeper_h