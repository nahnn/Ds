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
  void set(const attrs_type& attr)
  {
    auto a = attr;

    //clamp set base attributes
    this->clampSet(a);

    a.tc = std::clamp(a.tc, limits::MIN_THETAC_EXCLUSIVE + R(0.1), limits::MAX_THETAC);
    a.nc = std::clamp(a.nc, limits::MIN_NC, limits::MAX_NC);

    _attr = a;
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

} // end namespace cg

#endif // __CircularArc_h