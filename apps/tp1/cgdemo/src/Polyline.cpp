// OVERVIEW: Polyline.cpp
// ========
// Implementation for polylines.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#include "include/RegularPolygon.h"
#include "include/CircularArc.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
void RegularPolygon<R>::set()
{
  vec3 pi;
  vec3 pi_1{0, 1, 0};
  _points[0] = pi_1;

  auto rotMat = mat3{quat{vec3{0, 0, 2 * (cg::math::pi<R>) /_np}}};

  for(index_t i = 1u; i < _np; ++i)
  {
    pi = rotMat * pi_1;
    _points[i] = pi;
    pi_1 = pi;
  }

  // Transformations
  if (_ep != 0)
  {
    assert(_ep >= limits::MIN_EP && _ep <= limits::MAX_EP);
    _np *= 2;
    pi_1 = vec3{0, 1 - _ep, 0}; // Pi+np

    for(index_t i = _np / 2; i < _np; ++i)
    {
      pi = rotMat * pi_1;
      _points[i] = pi;
      pi_1 = pi;
    }
  }

  if (_tp != 0)
  {
    assert(_tp >= limits::MIN_THETAP && _tp <= limits::MAX_THETAP);

    auto rotTp = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_tp))}}};

    for (index_t i = 0; i < _np; ++i)
      _points[i] = rotTp * _points[i];
  }

  if (sp != 0)
  {
    assert(sp >= limits::MIN_SCALEP && _sp <= limits::MAX_SCALEP);

    for (index_t i = 0; i < _np; ++i)
      _points[i] = _sp * _points[i];
  }
  
  // bounds
  for(index_t i = 0; i < _np; ++i)
    _bounds.extend(_points[i]);
}

template<IsReal R>
void CircularArc<R>::set()
{
  if (_tc == 360)
  {
    auto rotMat = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_tc / _np))}}};

    vec3 pi{static_cast<R>(cos(cg::math::toRadians(90 - _tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _tc / 2))), 0};

    for (index_t i = 0; i < _np; i++)
    {
      _points[i] = pi;
      pi = rotMat * pi;
    }
  }
  else
  {
    auto rotMat = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_tc / _nc))}}};
    mat3 rotMatInv{rotMat[0], rotMat[1], rotMat[2]};
    rotMatInv.invert();

    vec3 pi{-static_cast<R>(cos(cg::math::toRadians(90 - _tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _tc / 2))), 0};
      
    vec3 pi_1{static_cast<R>(cos(cg::math::toRadians(90 - _tc / 2))), 
      static_cast<R>(sin(cg::math::toRadians(90 - _tc / 2))), 0};

    index_t i{}, last{_np - 1};
    for(; i < _np || i < last; i += 2)
    {
      _points[i] = pi_1;
      _points[last - i] = pi;
      pi_1 = rotMat * pi_1; // anti clock-wise
      pi = rotMatInv * pi; // clock-wise
    }

    if (i == last)
      _points[static_cast<index_t>(ceil(i/2))] = vec3{0, 1, 0};
  }

  // Transformations
  if (_ep != 0)
  {
    assert(_ep >= limits::MIN_EP && _ep <= limits::MAX_EP);
    pi_1 = vec3{_points[0][0] - _ep, _points[0][1] - _ep, 0}; // Pi+np
    pi = vec3{_points[last][0] + _ep, _points[last][1] + _ep, 0};
    _np *= 2;

    index_t j{_np / 2};
    for(; j < _np || j < last; j += 2)
    {
      _points[j] = pi_1;
      _points[last - j] = pi;
      pi_1 = rotMat * pi_1; // anti clock-wise
      pi = rotMatInv * pi; // clock-wise
    }

    if (j == last)
      _points[static_cast<index_t>(ceil(j/2))] = vec3{0, 1 - _ep, 0};
  }

  if (_tp != 0)
  {
    assert(_tp >= limits::MIN_THETAP && _tp <= limits::MAX_THETAP);

    auto rotTp = mat3{quat{vec3{0, 0, static_cast<R>(cg::math::toRadians(_tp))}}};

    for (index_t i = 0; i < _np; ++i)
      _points[i] = rotTp * _points[i];
  }

  if (_sp != 0)
  {
    assert(_sp >= limits::MIN_SCALEP && _sp <= limits::MAX_SCALEP);

    for (index_t i = 0; i < _np; ++i)
      _points[i] = _sp * _points[i];
  }
  
  // bounds
  for(index_t i = 0; i < _np; ++i)
    _bounds.extend(_points[i]);
}

} // end namespace cg
    