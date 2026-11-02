// OVERVIEW: RegularPolygon.h
// ========
// Class definition for regular polygons (polylines).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __RegularPolygon_h
#define __RegularPolygon_h

#include "Polyline.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct RegularPolygonLimits: public PolylineLimits<R>
{
  // --- Number of Points ---
  static constexpr int MIN_NP = 3; // min n of points
  static constexpr int MAX_NP = 40; // max n of points
}; // RegularPolygonLimits

template<IsReal R>
struct RegularPolygonAttrs: public PolylineAttrs<R>
{

  bool operator ==(const RegularPolygonAttrs&) const = default;
  
}; // RegularPolygonAttrs


template<IsReal R>
class RegularPolygon: public PolylineGenerator<R>
{
public:
  using type = RegularPolygon<R>;
  using value_type = R;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;
  using limits = RegularPolygonLimits<R>;
  using attrs_type = RegularPolygonAttrs<R>;

  explicit RegularPolygon(const attrs_type& attr = {})
  {
    set(attr);
  }

  const attrs_type& attributes() const override
  {
    return _attr;
  }

  /// sets the attributes
  void set(const attrs_type& attr) override
  {
    auto a = attr;

    //clamp set base attributes
    this->clampSet(a);

    a.np = std::clamp(a.np, limits::MIN_NP, limits::MAX_NP);

    _attr = a;
  }

  Polyline<R> generate() const override;

private:
  attrs_type _attr;

}; // RegularPolygon

} //end namespace cg

#endif // __RegularPolygon_h