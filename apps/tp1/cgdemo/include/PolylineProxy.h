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

template<IsReal R>
class PolylineProxy: public Component
{
public:
    const Polyline& getPolyline() const { return _polyline; }

    virtual void build() = 0;
protected:
    Polyline<R> _polyline;
};

//TODO: Maybe move to other file

template<IsReal R>
class RegularPolygonProxy: public PolylineProxy<R>
{
public:
    RegularPolygonAttrs<R> attr;

    void build() override{
        _generator.set(attr);
        _polyline = _generator.generate();
    }

private:
    RegularPolygon<R>& _generator{};

}; //RegularPolygonProxy

template<IsReal R>
class CircularArcProxy: public PolylineProxy<R>
{
public:
    CircularArcAttrs<R> attr;

    void build() override{
        _generator.set(attr);
        _polyline = _generator.generate();
    }

private:
    CircularArc<R>& _generator{};

}; // CircularArcProxy

} // end namespace cg::graph

#endif // __PolylineProxy_h