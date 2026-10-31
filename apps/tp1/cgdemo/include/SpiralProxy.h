// OVERVIEW: SpiralProxy.h
// ========
// Class definition for Spiral Swap Proxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __SpiralProxy_h
#define __SpiralProxy_h

#include "cg/include/graph/PrimitiveProxy.h"

namespace cg
{ // begin namespace cg

class SpiralProxy : public PrimitiveProxy
{
private:
  Reference<TriangleMeshMapper> _mapper;
};

} // end namespace cg

#endif // __SpiralProxy_h