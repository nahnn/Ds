// OVERVIEW: Polyline.h
// ========
// Class definition for circular arcs (polylines).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __CircularArc_h
#define __CircularArc_h

#include "Polyline.h"
#include "cg/include/geometry/Bounds3.h"
#include <cassert>

namespace cg
{ // begin namespace cg

template<IsReal R>
struct CircularArcLimits: public PolylineLimits
{
    // --- Line Segments ---
    static constexpr size_t MIN_NC = 2; // min n of segments
    static constexpr size_t MAX_NC = 40; // max n of segments

    // --- Arc Angle ---
    static constexpr R MIN_THETAC_EXCLUSIVE = 0; // min theta_c (exclusive)
    static constexpr R MAX_THETAC = 360; // max theta_c

}; // CircularArc Limits


template<IsReal R>
class CircularArc: public Polyline<R>
{
public:

private:


}; // CircularArc

}


#endif // __CircularArc_h