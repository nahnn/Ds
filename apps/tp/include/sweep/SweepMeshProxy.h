// OVERVIEW: SweepMeshProxy.h
// ========
// Class definition for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 18/11/2026

#ifndef __SweepMeshProxy_h
#define __SweepMeshProxy_h

#include "graph/PrimitiveProxy.h"
#include "PolylineSweeper.h"
#include "SpiralSweeper.h"
#include "TorsionSweeper.h"
#include "RegularPolygon.h"
#include "CircularArc.h"
#include "SweepMeshWriter.h"
#include <string>
#include "graphics/Application.h"
#include "PolylineGeneratorProxy.h"


namespace cg::graph
{ // begin namespace cg::graph

/////////////////////////////////////////////////////////////////////
//
// SweepMeshProxy: sweep mesh proxy implementation
// ==============
template<IsReal R>
class SweepMeshProxy : public PrimitiveProxy
{
public:
  using type = SweepMeshProxy<R>;
  using value_type = R;
  using PolyGenProxy = PolylineGeneratorProxy<R>;
  
  [[nodiscard]] const char* meshName() const
  {
    return _meshName.c_str();
  }

  [[nodiscard]] auto mapper() const
  {
    return ((TriangleMeshMapper*)PrimitiveProxy::mapper());
  }

  const PolyGenProxy& polyGen() const
  {
    return _polyGenProxy;
  }

  PolyGenProxy& polyGen()
  {
    return _polyGenProxy;
  }

  /// Current generatrix (built polyline)
  const Polyline<R>& polyline() const
  {
    return _polyGenProxy.polyline();
  }

  /// Common attributes of the active polyline generator
  const PolylineAttrs<R>& genAttrs() const
  {
    return _polyGenProxy.activeGen().attributes();
  }

  void setPolylineDirty() 
  { 
    _polyGenProxy.setDirty(); 
    _dirtySweep = true; // if polyline changes, sweep changes
  }

  const bool isDirty() const 
  {
    return _dirtySweep;
  }

  void setSweepDirty() 
  { 
    _dirtySweep = true;
  }

  virtual void build()
  {
    if (_polyGenProxy.isDirty())
    {
      _polyGenProxy.build();
      _dirtySweep = true;
    }

    if (_dirtySweep)
    {
      this->sweep();
      _dirtySweep = false;
    }
  }

  void setMesh(const TriangleMesh& mesh, const std::string& meshName)
  {
    mapper()->setMesh(mesh);
    _meshName = meshName;
  }

  Component* duplicate(const SceneObject*) const override
  {
    // TODO
    return nullptr;
  }

  // keeps the previous name if the new one is empty or has quotes
  void setMeshName(const std::string& name)
  {
    if (!name.empty() && name.find('"') == std::string::npos)
      _meshName = name;
  }

  bool saveOBJ() const
  {
    if (!SweepMeshWriter::isValidName(_meshName))
      return false;

    const auto relative = "meshes/" + _meshName + ".obj";
    const auto path = Application::assetFilePath(relative.c_str());

    return SweepMeshWriter::writeOBJ(*mapper()->mesh(), path.c_str());
  }

protected:

  SweepMeshProxy(const TriangleMesh& mesh, const std::string& meshName):
    PrimitiveProxy{*new TriangleMeshMapper{mesh}},
    _meshName{meshName}
  {
    // do nothing
  }

  // Sweep method implemented in derived proxys
  virtual void sweep() = 0;

  void setMesh(const TriangleMesh& mesh)
  {
    mapper()->setMesh(mesh);
  }

  PolylineGeneratorProxy<R> _polyGenProxy;
  std::string _meshName;
  bool _dirtySweep{true};

}; // SweepMeshProxy


/////////////////////////////////////////////////////////////////////
//
// SpiralProxy: proxy for spiral sweep meshes
// ===========
template<IsReal R>
class SpiralProxy : public SweepMeshProxy<R>
{
public:
  SpiralSweeperAttrs<R> attr;

  [[nodiscard]] static auto New(const std::string& name = "Spiral")
  {

    PolylineGeneratorProxy<R> gen;

    SpiralSweeper<R> sweeper;
    auto initialMesh = sweeper.sweep(gen.polyline());

    auto proxy = new SpiralProxy{*initialMesh, name};
    
    proxy->_sweeper = sweeper;
    proxy->_dirtySweep = false;

    return proxy;
  }

protected:
  void sweep() override
  {
    _sweeper.set(attr);
    this->setMesh(*_sweeper.sweep(this->_polyGenProxy.polyline()));
  }

private:
  SpiralProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    // do nothing
  }

  SpiralSweeper<R> _sweeper;
};


/////////////////////////////////////////////////////////////////////
//
// TorsionProxy: proxy for torsion sweep meshes
// ============
template<IsReal R>
class TorsionProxy : public SweepMeshProxy<R>
{
public:
  TorsionSweeperAttrs<R> attr;

  [[nodiscard]] static auto New(const std::string& name = "Torsion")
  {

    PolylineGeneratorProxy<R> gen;

    TorsionSweeper<R> sweeper;
    auto initialMesh = sweeper.sweep(gen.polyline());

    auto proxy = new TorsionProxy{*initialMesh, name};
    
    proxy->_sweeper = sweeper;
    proxy->_dirtySweep = false;

    return proxy;
  }

protected:
  void sweep() override
  {
    _sweeper.set(attr);
    this->setMesh(*_sweeper.sweep(this->_polyGenProxy.polyline()));
  }

private:
  TorsionProxy(const TriangleMesh& mesh, const std::string& name): 
    SweepMeshProxy<R>{mesh, name} 
  {
    // do nothing
  }

  TorsionSweeper<R> _sweeper;
};

} // end namespace cg::graph

#endif // __SweepMeshProxy_h