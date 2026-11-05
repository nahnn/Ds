// OVERVIEW: Polyline.h
// ========
// Class definition for polylines.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __Polyline_h
#define __Polyline_h

#include "geometry/Bounds3.h"
#include "math/Matrix3x3.h"
#include <algorithm>
#include <cassert>
#include <cstddef>

namespace cg
{ // begin namespace cg

using index_t = unsigned;


/// @brief PolylineLimits: Represents the limits of transformations of a polyline
template<IsReal R>
struct PolylineLimits
{
  // --- Number of Points ---
  static constexpr int MIN_NP = 3; // min n of points
  static constexpr int MAX_NP = 40; // max n of points

  // --- Scale Transform ---
  static constexpr R MIN_SCALEP = .1; // min scale_p
  static constexpr R MAX_SCALEP = 5; // max scale_p

  // --- Rotation Transform ---
  static constexpr R MIN_THETAP = 0; // min theta_p
  static constexpr R MAX_THETAP = 360; // max theta_p

  // --- Extrusion ---
  static constexpr R MIN_EP = .1; // min extrusion_p
  static constexpr R MAX_EP = .9; // max extrusion_p

}; // PolylineLimits

/// @brief PolylineAttrs: Attributes common to all polyline generators
template<IsReal R>
struct PolylineAttrs
{
  int np{3}; // number of points
  R sp{1.}; //scale
  R tp{0.}; //rotation (theta)
  R ep{0.}; //extrusion


  virtual ~PolylineAttrs() = default; //for dynamic_cast
  bool operator ==(const PolylineAttrs&) const = default;

}; // PolylineAttrs


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
  using bounds_type = Bounds3<R>;
  using limits = PolylineLimits<R>;

  //CircularArc -> 41 * 2
  static constexpr size_t MAX_LENGTH = 82;

  /// @brief Default constructor
  constexpr Polyline() = default;

  /// @brief Number of points currently stored
  size_t length() const
  {
    return _length;
  }

  void setLength(size_t length)
  {
    assert(length <= MAX_LENGTH);
    _length = length;
  }

  const auto& points() const
  {
    return _points;
  }

  /// @brief Writable access; grows the length when writing past the end
  auto& operator [](size_t i)
  {
    assert(i < MAX_LENGTH);
    if (i >= _length)
      _length = i + 1;
    return _points[i];
  }

  const auto& operator [](size_t i) const
  {
    assert(i < _length);
    return _points[i];
  }

  bool isClosed() const
  {
    return _closed;
  }

  void setClosed(bool closed)
  {
    _closed = closed;
  }

  bool isExtruded() const
  {
    return _extruded;
  }

  void setExtruded(bool extruded)
  {
    _extruded = extruded;
  }

  const bounds_type& bounds() const
  {
    return _bounds;
  }

  void computeBounds()
  {
    _bounds.setEmpty();
    for (index_t i = 0; i < _length; ++i)
      _bounds.extend(_points[i]);
  }

  R width() const
  {
    return math::abs(_bounds[0][0] - _bounds[1][0]);
  }

  R height() const
  {
    return math::abs(_bounds[0][1] - _bounds[1][1]);
  }

private:
  size_t _length{};
  bool _closed{};
  bool _extruded{};
  vec3 _points[MAX_LENGTH]{};
  bounds_type _bounds{};

}; // Polyline

template<IsReal R> using poly = Polyline<R>;


/// @brief PolylineGenerator: Represents a base PolylineGenerator
template<IsReal R>
class PolylineGenerator
{
public:
  using type = PolylineGenerator<R>;
  using value_type = R;
  using vec3 = Vector3<R>;
  using limits = PolylineLimits<R>;
  using attrs_type = PolylineAttrs<R>;

  virtual ~PolylineGenerator() = default;

  //return attributes
  virtual const PolylineAttrs<R>& attributes() const = 0;

  //Generates the polyline using the especif implemented method
  virtual Polyline<R> generate() const = 0;

  virtual void set(const attrs_type& attr) = 0;

protected:
  static void clampSet(PolylineAttrs<R>& attr)
  {
    attr.np = std::clamp(attr.np, limits::MIN_NP, limits::MAX_NP);
    attr.sp = std::clamp(attr.sp, limits::MIN_SCALEP, limits::MAX_SCALEP);
    attr.tp = std::clamp(attr.tp, limits::MIN_THETAP, limits::MAX_THETAP);
    if (attr.ep != 0)
      attr.ep = std::clamp(attr.ep, limits::MIN_EP, limits::MAX_EP);
  }

  static void transform(Polyline<R>& polyline, const PolylineAttrs<R>& attr);

}; // PolylineGenerator

template<IsReal R>
void PolylineGenerator<R>::transform(Polyline<R>& polyline, const PolylineAttrs<R>& attr)
{
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;

  if (attr.ep != 0)
  {
    polyline.setExtruded(true);

    const index_t n = static_cast<index_t>(polyline.length());
    const R f = 1 - attr.ep;
    for(index_t i = 0; i < n; ++i)
      polyline[n + i] = f * polyline[i];
  }

  const index_t length = static_cast<index_t>(polyline.length());

  if (attr.tp != 0)
  {
    auto rotTp = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(attr.tp))}}};

    for (index_t i = 0; i < length; ++i)
      polyline[i] = rotTp * polyline[i];
  }

  if (attr.sp != 1)
    for (index_t i = 0; i < length; ++i)
      polyline[i] = attr.sp * polyline[i];
} 

} // end namespace cg

#endif // __Polyline_h