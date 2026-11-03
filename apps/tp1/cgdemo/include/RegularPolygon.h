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
  
}; // RegularPolygonLimits

template<IsReal R>
struct RegularPolygonAttrs: public PolylineAttrs<R>
{
  //no specific attributes
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
  void set(const PolylineAttrs<R>& attr) override
  {
    //object slicing (inverse) sets base attrs
    static_cast<PolylineAttrs<R>&>(_attr) = attr;

    this->clampSet(_attr);
  }

  Polyline<R> generate() const override;

private:
  attrs_type _attr;

}; // RegularPolygon


//TODO: Alterar método para utilizar interface da polilinha, já que não deriva mais
template<IsReal R>
Polyline<R> RegularPolygon<R>::generate() const
{
  Polyline<R> polyline;
  polyline.setLength(_attr.np);
  auto length = polyline.length();

  vec3 pi;
  vec3 pi_1{0, 1, 0};
  polyline[0] = pi_1;

  auto rotMat = mat3{quat{vec3{0, 0, static_cast<R>(2 * (cg::math::pi<R>) / length)}}};

  for(index_t i = 1u; i < length; ++i)
  {
    pi = rotMat * pi_1;
    polyline[i] = pi;
    pi_1 = pi;
  }

  // Transformations
  if (_attr.ep != 0)
  {
    assert(_attr.ep >= limits::MIN_EP && _attr.ep <= limits::MAX_EP);
    polyline.setExtruded(true);
    polyline.setLength(2 * length);
    length = polyline.length();
    pi_1 = vec3{0, 1 - _attr.ep, 0}; // Pi+np

    for(index_t i = length / 2; i < length; ++i)
    {
      pi = rotMat * pi_1;
      polyline[i] = pi;
      pi_1 = pi;
    }
  }

  if (_attr.tp != 0)
  {
    assert(_attr.tp >= limits::MIN_THETAP && _attr.tp <= limits::MAX_THETAP);

    auto rotTp = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_attr.tp))}}};

    for (index_t i = 0; i < length; ++i)
      polyline[i] = rotTp * polyline[i];
  }

  if (_attr.sp != 0)
  {
    assert(_attr.sp >= limits::MIN_SCALEP && _attr.sp <= limits::MAX_SCALEP);

    for (index_t i = 0; i < length; ++i)
      polyline[i] = _attr.sp * polyline[i];
  }
  
  // bounds
  polyline.computeBounds();

  return polyline;
}

} //end namespace cg

#endif // __RegularPolygon_h