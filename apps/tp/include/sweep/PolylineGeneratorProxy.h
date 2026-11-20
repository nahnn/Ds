// OVERVIEW: PolylineGeneratorProxy.h
// ========
// Class definition for PolylineGeneratorProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#ifndef __PolylineGeneratorProxy_h
#define __PolylineGeneratorProxy_h

#include "RegularPolygon.h"
#include "CircularArc.h"
#include <map>
#include <memory>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PolylineGeneratorProxy: proxy for polyline generators
// ======================
template<IsReal R>
class PolylineGeneratorProxy
{
public:
    using PolyGen = PolylineGenerator<R>;

    enum class GenType : std::size_t { RegularPolygon, CircularArc };

    struct PolyGenEntry
    {
        const char* name;
        std::unique_ptr<PolyGen> generator;
    };

    using PolyGenMap = std::map<GenType, PolyGenEntry>;

    PolylineGeneratorProxy()
    {
        // adding possible polylines
        add(GenType::RegularPolygon, "RegularPolygon", std::make_unique<RegularPolygon<R>>());
        add(GenType::CircularArc, "CircularArc", std::make_unique<CircularArc<R>>());

        build();
    }

    // copies the state of every generator (used to duplicate sweep proxies)
    PolylineGeneratorProxy& operator =(const PolylineGeneratorProxy& other)
    {
        for (auto& [type, entry] : _entries)
            entry.generator->set(other._entries.at(type).generator->attributes());
        _activeGenType = other._activeGenType;
        _polyline = other._polyline;
        _dirtyPoly = other._dirtyPoly;
        return *this;
    }

    GenType activeGenType() const {
        return _activeGenType;
    }

    const PolyGenMap& entries() const
    {
        return _entries;
    }

    PolyGen& activeGen()
    {
        return *_entries.at(_activeGenType).generator;
    }

    const PolyGen& activeGen() const
    {
        return *_entries.at(_activeGenType).generator;
    }

    void setType(GenType type)
    {
        if (type == _activeGenType)
            return;

        _entries.at(type).generator->set(activeGen().attributes());
        _activeGenType = type;
        _dirtyPoly = true;
    }

    const Polyline<R>& polyline() const
    {
        return _polyline;
    }

    const bool isDirty() const
    {
        return _dirtyPoly;
    }

    void setDirty()
    {
        _dirtyPoly = true;
    }

    void build()
    {
        if (!_dirtyPoly)
            return;

        _polyline = activeGen().generate();
        _dirtyPoly = false;
    }

private:

    void add(GenType type, const char* name, std::unique_ptr<PolyGen> generator)
    {
        _entries.emplace(type, PolyGenEntry{name, std::move(generator)});
    }

    PolyGenMap _entries;
    GenType _activeGenType{GenType::RegularPolygon};
    Polyline<R> _polyline;
    bool _dirtyPoly{true};

}; // PolylineGeneratorProxy

} // end namespace cg

#endif // __PolylineGeneratorProxy_h
