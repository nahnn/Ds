// OVERVIEW: Polyline.cpp
// ========
// Implementation for polylines.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#include "include/Polyline.h"

namespace cg
{ // begin namespace cg
using precision = PolylineUtils::precision;

void Polyline<precision>::set(const float& ep, const float& tp, const float& sp)
{
    vec3 pi;
    vec3 pi_1{0, 1, 0};
    _points[0] = pi_1;

    auto rotMat = mat3{quat{vec3{0, 0, 2 * (cg::math::pi<value_type>) /_np}}};

    for(index_t i = 1u; i < _np; ++i)
    {
      pi = rotMat * pi_1;
      _points[i] = pi;
      pi_1 = pi;
    }

    if (ep != 0)
    {
      assert(ep >= limits::MinEp && ep <= limits::MaxEp);
      _np *= 2;
      pi_1 = vec3{0, 1 - ep, 0}; // Pi+np

      for(index_t i = _np; i < _np; ++i)
      {
        pi = rotMat * pi_1;
        _points[i] = pi;
        pi_1 = pi;
      }
    }

    if (tp != 0)
    {
      assert(tp >= limits::MinTp && tp <= limits::MaxTp);

      auto rotTp = mat3{quat{vec3{0, 0, cg::math::toRadians(tp)}}};

      for (index_t i = 0; i < _np; ++i)
        _points[i] = rotTp * _points[i];
    }

    if (sp != 0)
    {
      assert(sp >= limits::MinSp && sp <= limits::MaxSp);

      for (index_t i = 0; i < _np; ++i)
        _points[i] = sp * _points[i];
    }
}

} // end namespace cg 
    