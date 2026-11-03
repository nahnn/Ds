// OVERVIEW: CircularArc.h
// ========
// Class definition for circular arcs (polylines).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __CircularArc_h
#define __CircularArc_h

#include "Polyline.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct CircularArcLimits: public PolylineLimits<R>
{
  // --- Line Segments ---
  static constexpr int MIN_NC = 2; // min n of segments
  static constexpr int MAX_NC = 40; // max n of segments

  // --- Arc Angle (degrees) ---
  static constexpr R MIN_THETAC_EXCLUSIVE = 0; // min theta_c (exclusive)
  static constexpr R MAX_THETAC = 360; // max theta_c

}; // CircularArcLimits

template<IsReal R>
struct CircularArcAttrs: public PolylineAttrs<R>
{
  R tc{30}; // arc angle (degrees)
  int nc{4}; // number of segments
  bool isOpen{false}; // ignored if tc == 360 (polygon, always closed)

  bool operator ==(const CircularArcAttrs&) const = default;
}; // CircularArcAttrs


template<IsReal R>
class CircularArc: public PolylineGenerator<R>
{
public:
  using type = CircularArc<R>;
  using value_type = R;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;
  using limits = CircularArcLimits<R>;
  using attrs_type = CircularArcAttrs<R>;

  explicit CircularArc(const attrs_type& attr = {})
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

    //specific attrs
    if (const auto* specific_attr = dynamic_cast<const attrs_type*>(&attr))
    {
      _attr.tc = std::clamp(specific_attr->tc, limits::MIN_THETAC_EXCLUSIVE + R(0.1), limits::MAX_THETAC);
      _attr.nc = std::clamp(specific_attr->nc, limits::MIN_NC, limits::MAX_NC);
      _attr.isOpen = specific_attr->isOpen;
    }
  }

  /// If theta_c = 360 the arc is a polygon with nc points (always closed)
  bool isFullCircle() const
  {
    return _attr.tc >= limits::MAX_THETAC;
  }

  /// Number of points of the profile (before extrusion)
  size_t npoints() const
  {
    return isFullCircle() ? _attr.nc : _attr.nc + 1;
  }

  Polyline<R> generate() const override;

private:
  attrs_type _attr;

}; // CircularArc


//TODO: Alterar método para utilizar interface da polilinha, já que não deriva mais
template<IsReal R>
Polyline<R> CircularArc<R>::generate() const
{
  Polyline<R> polyline;
  auto length = polyline.setLength(npoints()).length();
  auto rotMat = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_attr.tc / length))}}};
  mat3 rotMatInv{rotMat[0], rotMat[1], rotMat[2]};
  rotMatInv.invert();

  if (isFullCircle())
  {
    vec3 pi{static_cast<R>(cos(cg::math::toRadians(90 - _attr.tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _attr.tc / 2))), 0};

    for (index_t i = 0; i < length; i++)
    {
      polyline[i] = pi;
      pi = rotMat * pi;
    }
  }
  else
  {
    vec3 pi{-static_cast<R>(cos(cg::math::toRadians(90 - _attr.tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _attr.tc / 2))), 0};
      
    vec3 pi_1{static_cast<R>(cos(cg::math::toRadians(90 - _attr.tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _attr.tc / 2))), 0};

    index_t i{}, last{length - 1};
    for(; i < length || i < last; i += 2)
    {
      polyline[i] = pi_1;
      polyline[last - i] = pi;
      pi_1 = rotMat * pi_1; // anti clock-wise
      pi = rotMatInv * pi; // clock-wise
    }

    if (i == last)
      polyline[static_cast<index_t>(ceil(i/2))] = vec3{0, 1, 0};
  }

  // Transformations
  if (_attr.ep != 0)
  {
    assert(_attr.ep >= limits::MIN_EP && _attr.ep <= limits::MAX_EP);
    index_t last{length - 1};
    vec3 pi_1 = vec3{polyline[0][0] - _attr.ep, polyline[0][1] - _attr.ep, 0}; // Pi+np
    vec3 pi = vec3{polyline[last][0] + _attr.ep, polyline[last][1] + _attr.ep, 0};
    length = polyline.setLength(2 * length).length();

    index_t j{length / 2};
    last = length - 1; 
    for(; j < length || j < last; j += 2)
    {
      polyline[j] = pi_1;
      polyline[last - j] = pi;
      pi_1 = rotMat * pi_1; // anti clock-wise
      pi = rotMatInv * pi; // clock-wise
    }

    if (j == last)
      polyline[static_cast<index_t>(ceil(j/2))] = vec3{0, 1 - _attr.ep, 0};
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

} // end namespace cg

#endif // __CircularArc_h