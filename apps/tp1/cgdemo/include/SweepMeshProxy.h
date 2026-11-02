// OVERVIEW: SweepMeshProxy.h
// ========
// Class definition for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __SweepMeshProxy_h
#define __SweepMeshProxy_h

#include "cg/include/graph/PrimitiveProxy.h"
#include "include/PolylineSweeper.h"
#include "include/SpiralSweeper.h"
#include "include/TorsionSweeper.h"
#include "include/RegularPolygon.h"
#include "include/CircularArc.h"
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
  
  enum PolylineGeneratorType : std::size_t { RegularPolygon, CircularArc };

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

  // O novo build inteligente
  virtual void build()
  {
    if (_dirtyPolyline)
    {
      _creationFunctions[activeGen]();
      _dirtyPolyline = false; // Limpa a sujeira
    }

    if (_dirtySweep)
    {
      this->sweep();
      _dirtySweep = false; // Limpa a sujeira
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
  template<typename G, typename A> 
  using CreateFunction =  std::function<void()>;

  template<typename G, typename A>
  using CreateFunctionsMap = std::unordered_map<size_t, CreateFunction<G, A>>;

  template<typename G, typename A>
  void registerCreateFunction(size_t type, CreateFunction<G, A> function)
  {
    assert(function);
    _creationFunctions[type] = (CreateFunction<G, A>)function;
  }

  template<typename G, typename A>
  void generatePolylineBase()
  {
    _generator.set(dynamic_cast<A>(attrs));
    _polyline = dynamic_cast<G>(_generator).generate();
  }

  SweepMeshProxy(const TriangleMesh& mesh, const std::string& meshName):
    PrimitiveProxy{*new TriangleMeshMapper{mesh}},
    _meshName{meshName}
  {
    registerCreateFunction(static_cast<size_t>(PolylineGeneratorType::RegularPolygon),
      generatePolylineBase<RegularPolygon<R>, RegularPolygonAttrs<R>>);
    registerCreateFunction(static_cast<size_t>(PolylineGeneratorType::CircularArc), 
      generatePolylineBase<CircularArc<R>, CircularArcAttrs<R>>);
  }

  // Sweep method implemented in derived proxys
  virtual void sweep() = 0;

  void setMesh(const TriangleMesh& mesh)
  {
    mapper()->setMesh(mesh);
  }
  
  Polyline<R> _polyline;
  PolylineGenerator<R> _generator;
  CreateFunctionsMap _creationFunctions;
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
    return new SpiralProxy{TriangleMesh{}, name};
  }

protected:
  void sweep() override
  {
    SpiralSweeper<R> sweeper;
    sweeper.set(attr);
    
    this->setMesh(sweeper.sweep(this->_polyline)); 
  }

private:
  SpiralProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    // do nothing
  }
};


template<IsReal R>
class TorsionProxy : public SweepMeshProxy<R>
{
public:
  TorsionSweeperAttrs<R> attr;

  [[nodiscard]] static auto New(const std::string& name = "Torsion")
  {
    return new TorsionProxy{TriangleMesh{}, name};
  }

protected:
  void sweep() override
  {
    TorsionSweeper<R> sweeper;
    sweeper.set(attr);
    
    this->setMesh(sweeper.sweep(this->_polyline));
  }

private:
  TorsionProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    //do nothing
  }
};

} // end namespace cg::graph

#endif // __SweepMeshProxy_h