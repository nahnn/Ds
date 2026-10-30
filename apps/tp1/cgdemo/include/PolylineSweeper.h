// OVERVIEW: PolylineSweeper.h
// ========
// Class definition for Polylines Swap.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 27/10/2026

#ifndef __PolylineSweeper_h
#define __PolylineSweeper_h

#include "include/Polyline.h"
#include "cg/include/geometry/MeshSweeper.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
class PolylineSweeper : public MeshSweeper
{
public:
   virtual TriangleMesh* build(const Polyline<R>& poly) = 0;
private:
}; // PolylineSweeper


} // end namespace cg

#endif // __PolylineSweeper_h