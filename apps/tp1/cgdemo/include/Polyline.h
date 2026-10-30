// OVERVIEW: Polyline.h
// ========
// Class definition for polylines.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#ifndef __Polyline_h
#define __Polyline_h

#include "cg/include/geometry/Bounds3.h"
#include <cassert>

namespace cg
{ // begin namespace cg
using index_t = unsigned;

/// @brief PolylineLimits: Represents the limits of transformations of a polyline
template<IsReal R>
struct PolylineLimits
{
  // --- Number of Points ---
  static constexpr size_t MIN_NP = 3; // min n of points
  static constexpr size_t MAX_NP = 40; // max n of points
  
  // --- Rotation Transform ---
  static constexpr R MIN_THETAP = 0; // min theta_p
  static constexpr R MAX_THETAP = 360; // max theta_p
  
  // --- Scale Transform ---
  static constexpr R MIN_SCALEP = .1; // min scale_p
  static constexpr R MAX_SCALEP = 5; // max scale_p

  // --- Extrusion ---
  static constexpr R MIN_EP = .1; // min extrusion_p
  static constexpr R MAX_EP = .9; // max extrusion_p

}; // PolylineLimits

/// @brief
// Polyline: Class that represents the discretization of a given curve.
template<IsReal R>
class Polyline
{
public:
  using type = Polyline<R>;
  using value_type = R;
  using quat = Quaternion<R>;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using bounds = Bounds3<R>;
  using limits = PolylineLimits<R>;

  /// @brief Default constructor
  constexpr Polyline() = default;

  auto& length() const
  {
    return _np;
  }

  auto& points() const
  {
    return _points;
  }

  auto& bounds() const
  {
    return _bounds;
  }

  auto width() const
  {
    return static_cast<size_t>(abs(_bounds[0][0] - _bounds[1][0]));
  }

  auto height() const
  {
    return static_cast<size_t>(abs(_bounds[0][1] - _bounds[1][1]));
  }

  virtual void set() = 0;

private:
  index_t _np{};
  R _ep;
  R _tp;
  R _sp;
  vec3 _points[2 * limits::MaxNp];
  bounds _bounds{};

}; // Polyline

template<IsReal R> using poly = Polyline<R>;

} // end namespace cg

#endif // __Polyline_h
