// OVERVIEW: Polyline.h
// ========
// Class definition for regular polygons (polylines).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __RegularPolygon_h
#define __RegularPolygon_h

#include "Polyline.h"
#include "cg/include/geometry/Bounds3.h"
#include <cassert>

namespace cg
{ // begin namespace cg

template<IsReal R>
struct RegularPolygonLimits: public PolylineLimits
{
    // --- Number of Points ---
    static constexpr size_t MIN_NP = 3; // min n of points
    static constexpr size_t MAX_NP = 40; // max n of points

    // --- Rotation Transform ---
    static constexpr R MIN_THETAP = 0; // min theta_p
    static constexpr R MAX_THETAP = 360; // max theta_p

}; //RegularPolygonLimits


template<IsReal R>
class RegularPolygon: public Polyline<R>
{
public:
    //TODO
private:
    //TODO

}; // RegularPolygon

}




#endif // __RegularPolygon_h