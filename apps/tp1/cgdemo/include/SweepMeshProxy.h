// OVERVIEW: SweepMeshProxy.h
// ========
// Class definition for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#ifndef __SweepMeshProxy_h
#define __SweepMeshProxy_h

#include "cg/include/graph/PrimitiveProxy.h"

namespace cg::graph
{ // begin namespace cg::graph

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

} // end namespace cg::graph

#endif // __SweepMeshProxy_h