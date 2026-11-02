// OVERVIEW: PolylineProxy.h
// ========
// Class definition for Polyline Proxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __PolylineProxy_h
#define __PolylineProxy_h

#include "Polyline.h"
#include "RegularPolygon.h"
#include "CircularArc.h"
#include "include/PolylineProxy.h"
#include "cg/include/graph/Component.h"

namespace cg::graph
{ // begin namespace cg::graph

// TODO: add concepts
template<IsReal R, template<typename> class PolylineAttrsType, template<typename> class PolylineGeneratorType>
class PolylineProxy: public Component
{
public:
  using type = PolylineProxy<R, PolylineAttrsType, PolylineGeneratorType>;
  using polyline_type = Polyline<R>;
  using attrs_type = PolylineAttrsType<R>;
  using gen_type = PolylineGeneratorType<R>;
  using value_type = R;

  attrs_type attr;

  const Polyline& getPolyline() const { return _polyline; }

  virtual void build() 
  {
    _generator.set(dynamic_cast<attrs_type>(attr));
    _polyline = dynamic_cast<gen_type>(_generator).generate(); 
  }

protected:
  Polyline<R> _polyline;
  gen_type& _generator;

};

} // end namespace cg::graph

#endif // __PolylineProxy_h
