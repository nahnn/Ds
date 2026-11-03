// OVERVIEW: SweepMeshProxy.h
// ========
// Class definition for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __SweepMeshProxy_h
#define __SweepMeshProxy_h

#include "graph/PrimitiveProxy.h"
#include "PolylineSweeper.h"
#include "SpiralSweeper.h"
#include "TorsionSweeper.h"
#include "RegularPolygon.h"
#include "CircularArc.h"
#include <unordered_map>
#include <functional>

namespace cg::graph
{

/// @brief SweepMeshProxy implementation.
//TODO: Essa é uma implementação extremamente ingenua. Pensar em uma forma melhor de fazer isso.
template<IsReal R>
class SweepMeshProxy : public PrimitiveProxy
{
public:
  using type = SweepMeshProxy<R>;
  using value_type = R;
  
  enum class PolylineGeneratorType : std::size_t { RegularPolygon, CircularArc };

  [[nodiscard]] const char* meshName() const
  {
    return _meshName.c_str();
  }

  [[nodiscard]] auto mapper() const
  {
    return ((TriangleMeshMapper*)PrimitiveProxy::mapper());
  }

  void setPolylineDirty() 
  { 
    _dirtyPolyline = true; 
    _dirtySweep = true; // if polyline changes, sweep changes
  }

  void setSweepDirty() 
  { 
    _dirtySweep = true;
  }

  virtual void build()
  {
    if (_dirtyPolyline)
    {
      _creationFunctions[static_cast<size_t>(activeGen)]();
      _dirtyPolyline = false;
    }

    if (_dirtySweep)
    {
      this->sweep();
      _dirtySweep = false;
    }
  }

  Component* duplicate(const SceneObject*) const override
  {
    return nullptr; // A ser implementado, se necessário pela engine
  }

  // generator attrs
  PolylineGeneratorType activeGen{PolylineGeneratorType::RegularPolygon};
  PolylineAttrs<R> attrs;

protected:
  // TODO: adicionar concepts para a verificação do parametros do template
  using CreateFunction =  std::function<void()>;

  using CreateFunctionsMap = std::unordered_map<size_t, CreateFunction>;

  void registerCreateFunction(size_t type, CreateFunction function)
  {
    assert(function);
    _creationFunctions[type] = function;
  }

  template<typename G, typename A>
  void generatePolylineBase()
  {
    G generator;

    generator.set(attrs);
    _polyline = generator.generate();
  }

  SweepMeshProxy(const TriangleMesh& mesh, const std::string& meshName):
    PrimitiveProxy{*new TriangleMeshMapper{mesh}},
    _meshName{meshName}
  {
    registerCreateFunction(
      static_cast<size_t>(PolylineGeneratorType::RegularPolygon),
      [this]() { this->generatePolylineBase<RegularPolygon<R>, RegularPolygonAttrs<R>>(); }
    );

    registerCreateFunction(
      static_cast<size_t>(PolylineGeneratorType::CircularArc), 
      [this]() { this->generatePolylineBase<CircularArc<R>, CircularArcAttrs<R>>(); }
    );
  }

  // Sweep method implemented in derived proxys
  virtual void sweep() = 0;

  void setMesh(const TriangleMesh& mesh)
  {
    mapper()->setMesh(mesh);
  }
  
  Polyline<R> _polyline;
  CreateFunctionsMap  _creationFunctions;
  std::string _meshName;
  bool _dirtyPolyline{true};
  bool _dirtySweep{true};

}; // SweepMeshProxy

template<IsReal R>
class SpiralProxy : public SweepMeshProxy<R>
{
public:
  SpiralSweeperAttrs<R> attr;

  [[nodiscard]] static auto New(const std::string& name = "Spiral")
  {
    RegularPolygon<R> polyGen;
    auto initialPolyline = polyGen.generate();

    SpiralSweeper<R> sweeper; // Instância que será guardada
    auto initialMesh = sweeper.sweep(initialPolyline);

    auto proxy = new SpiralProxy{*initialMesh, name};
    
    proxy->_sweeper = sweeper;
    proxy->_polyline = initialPolyline;
    proxy->_dirtyPolyline = false; 
    proxy->_dirtySweep = false;

    return proxy;
  }

protected:
  void sweep() override
  {
    _sweeper.set(attr);
    this->setMesh(*_sweeper.sweep(this->_polyline));
  }

private:
  SpiralProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    // do nothing
  }

  SpiralSweeper<R> _sweeper;
};


template<IsReal R>
class TorsionProxy : public SweepMeshProxy<R>
{
public:
  TorsionSweeperAttrs<R> attr;

  [[nodiscard]] static auto New(const std::string& name = "Torsion")
  {
    RegularPolygon<R> polyGen;
    auto initialPolyline = polyGen.generate();

    TorsionSweeper<R> sweeper;
    auto initialMesh = sweeper.sweep(initialPolyline);

    auto proxy = new TorsionProxy{*initialMesh, name};
    
    proxy->_sweeper = sweeper;
    proxy->_polyline = initialPolyline;
    proxy->_dirtyPolyline = false;
    proxy->_dirtySweep = false;

    return proxy;
  }

protected:
  void sweep() override
  {
    _sweeper.set(attr);
    this->setMesh(*_sweeper.sweep(this->_polyline));
  }

private:
  TorsionProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    //do nothing
  }

  TorsionSweeper<R> _sweeper;
};

} // end namespace cg::graph

#endif // __SweepMeshProxy_h