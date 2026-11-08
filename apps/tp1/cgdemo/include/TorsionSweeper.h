// OVERVIEW: TorsionSweeper.h
// ========
// Class definition for translational sweepers with torsion.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __TorsionSweeper_h
#define __TorsionSweeper_h

#include "PolylineSweeper.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct TorsionSweeperLimits: public PolylineSweeperLimits<R>
{
  // --- Length ---
  static constexpr R MIN_LV = 1; // min lv
  static constexpr R MAX_LV = 50; // max lv

  // --- Subdivisions ---
  static constexpr int MIN_NSV = 1; // min nsv
  static constexpr int MAX_NSV = 50; // max nsv

  // --- Scale (begin and end) ---
  static constexpr R MIN_SCALEV = .1; // min sb and se
  static constexpr R MAX_SCALEV = 5; // max sb and se

  // --- Torsion (1 = 360 degrees) ---
  static constexpr R MIN_RV = -2; // min rv
  static constexpr R MAX_RV = 2; // max rv

  // ow (-bx..bx) and oh (-by..by) checked  in sweep()
  // 

}; // TorsionSweeperLimits

template<IsReal R>
struct TorsionSweeperAttrs: public PolylineSweeperAttrs<R>
{
  R lv{10}; // length
  R ow{0}; // horizontal offset
  R oh{0}; // vertical offset
  int nsv{10}; // subdivisions
  R sb{1}; // initial scale
  R se{1}; // final scale
  R rv{0}; // torsion
  
}; // TorsionSweeperAttrs


template<IsReal R>
class TorsionSweeper: public PolylineSweeper<R>
{
public:
  using type = TorsionSweeper<R>;
  using value_type = R;
  using polyline_type = Polyline<R>;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;
  using limits = TorsionSweeperLimits<R>;
  using attrs_type = TorsionSweeperAttrs<R>;

  explicit TorsionSweeper(const attrs_type& attr = {})
  {
    set(attr);
  }

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

    a.lv = std::clamp(a.lv, limits::MIN_LV, limits::MAX_LV);
    a.nsv = std::clamp(a.nsv, limits::MIN_NSV, limits::MAX_NSV);
    a.sb = std::clamp(a.sb, limits::MIN_SCALEV, limits::MAX_SCALEV);
    a.se = std::clamp(a.se, limits::MIN_SCALEV, limits::MAX_SCALEV);
    a.rv = std::clamp(a.rv, limits::MIN_RV, limits::MAX_RV);

    _attr = a;
  }

  [[nodiscard]] TriangleMesh* sweep(const polyline_type& polyline) const override;

private:
  attrs_type _attr;

}; // TorsionSweeper

//TODO: Resolver "WINDING"
template<IsReal R>
TriangleMesh* TorsionSweeper<R>::sweep(const Polyline<R>& poly) const
{
  const auto& points = poly.points();
  const auto bx = poly.width();
  const auto by = poly.height();
  const size_t L = poly.length();
  const bool closed = poly.isClosed();
  const bool poly_extr = poly.isExtruded();

  const size_t rings = poly_extr ? 2 : 1; // rings per segment
  const size_t np = L / rings; // points per ring
  const bool capped = this->hasCaps(poly);

  // horizontal/vertical displacement (checked)
  const auto ow = std::clamp<R>(_attr.ow, -bx, bx);
  const auto oh = std::clamp<R>(_attr.oh, -by, by);

  // steps
  const size_t nsv = static_cast<size_t>(_attr.nsv);
  const size_t steps = nsv + 1;
  const R n = static_cast<R>(nsv);
  const R lv_step = static_cast<R>(_attr.lv) / n;
  const R sv_step = static_cast<R>(_attr.se - _attr.sb) / n;
  const R sb = static_cast<R>(_attr.sb);
  const R t_step = static_cast<R>(_attr.rv) * 360 / n; // degrees

  // n vertices
  const size_t nv = L * steps + (capped ? 2 * L : 0);

  // n triangles
  const size_t nt = nsv * rings * (closed ? np : np - 1) * 2
    + (capped ? 2 * (L - 2) : 0);

  // data initialization
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.triangleCount = nt;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new TriangleMesh::Triangle[nt];

  // setting vertices: scale -> translation -> rotation about z
  for(size_t k = 0; k < steps; ++k)
  {
    auto sv = sb + k * sv_step;

    auto T = vec3{ow, oh, k * lv_step};
    auto rotz = mat3{quat::eulerAngles(vec3{0, 0, k * t_step})};

    for(size_t i = 0; i < L; ++i)
      data.vertices[k * L + i] = rotz * (sv * points[i] + T);
  }

  // setting triangles
  const size_t totalSteps = rings * nsv;
  size_t t{};
  for(size_t i = 0, curSegInit = 0, upSegInit = L, curSegEnd = np - 1, upSegEnd = L + curSegEnd;
    i < totalSteps;
    ++i, curSegInit += np, upSegInit += np, curSegEnd += np, upSegEnd += np)
  {
    for(size_t j = 0; j < np - 1; ++j)
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
    const size_t p0 = steps * L, pn = p0 + L, lastSeg = (steps - 1) * L;
    for(size_t i = 0; i < L; ++i)
    {
      vec3 pi{data.vertices[i].x, data.vertices[i].y, data.vertices[i].z};
      vec3 pni{data.vertices[lastSeg + i].x, data.vertices[lastSeg + i].y, data.vertices[lastSeg + i].z};
      data.vertices[p0 + i] = pi;
      data.vertices[pn + i] = pni;
    }

    for(size_t i = 1; i < L - 1; ++i)
    {
      auto p1 = p0 + i;
      auto p2 = p0 + i + 1;
      auto pn_1 = pn + i;
      auto pn_2 = pn + i + 1;
      setTriangle<R>(&data.triangles[t++], p0, p2, p1);
      setTriangle<R>(&data.triangles[t++], pn, pn_1, pn_2);
    }
  }

  auto* mesh = new TriangleMesh{std::move(data)};
  mesh->computeNormals();
  return mesh;
}

} // end namespace cg

#endif // __TorsionSweeper_h