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

namespace cg::graph
{ // begin namespace cg::graph

//Basically a copy of TriangleMeshProxy, since this class is final in the original repo
template<IsReal R>
class SweepMeshProxy : public PrimitiveProxy
{
public:
  [[nodiscard]] static auto New(const TriangleMesh& mesh, const std::string& meshName)
  {
    return new SweepMeshProxy{mesh, meshName};
  }

  [[nodiscard]] const char* meshName() const
  {
    return _meshName.c_str();
  }

  [[nodiscard]] auto mapper() const
  {
    return ((TriangleMeshMapper*)PrimitiveProxy::mapper());
  }

  //builds the mesh
  virtual void build(const Polyline<R>& polyline, const std::string& meshName){
    _meshName = meshName;
    //do nothing
  }

protected:

  void setMesh(const TriangleMesh& mesh, const std::string& meshName)
  {
    mapper()->setMesh(mesh);
  }

private:
  std::string _meshName;
  // TODO: controles de especificação de geratriz e dos métodos de varredura.

  SweepMeshProxy(const TriangleMesh& mesh, const std::string& meshName):
    PrimitiveProxy{*new TriangleMeshMapper{mesh}},
    _meshName{meshName}
  {
    // do nothing
  }
}; // SweepMeshProxy

template<IsReal R>
class SpiralProxy: public SweepMeshProxy<R>
{
public:
  SpiralSweeperAttrs<R> attr;

  void build(const Polyline<R>& polyline, const std::string& meshName = "TorsionMesh") override{
    _sweeper.set(attr);

    this->setMesh(_sweeper.sweep(polyline))
  }

private:
  SpiralSweeper<R>& _sweeper{};
};

template<IsReal R>
class TorsionProxy: public SweepMeshProxy<R>
{
public:
  TorsionSweeperAttrs<R> attr;

  void build(const Polyline<R>& polyline, const std::string& meshName = "TorsionMesh") override{
    _sweeper.set(attr);

    this->setMesh(_sweeper.sweep(polyline))
  }

private:
  TorsionSweeper<R>& _sweeper{};
};

} // end namespace cg::graph

#endif // __SweepMeshProxy_h