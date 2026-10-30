// OVERVIEW: Spiral.cpp
// ========
// Implementation for Spirals.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#include "include/Spiral.h"

namespace cg
{ // begin namespace cg

void setVertices();

void setVertexNormals();

template<IsReal R>
void setTriangles(TriangleMesh::Data& t, 
  Vector3<R>*& points, 
  index_t i, 
  index_t j, 
  index_t np
)
{
  // p/ cd ponto e seu correspondente da outra layer, ligá-los
  for(index_t k = 0; k < np; ++i)
  {
    //
  }

}
 
template<IsReal R>
TriangleMesh* Spiral<R>::build()
{
  // TODO: def pos inicial, ang de rot total?, num tot de rot?, num tot de passos?
  auto de = (_we - _poly.width()) / 2;
  auto te = static_cast<R>(cg::math::toRadians(_re * 360));
  auto steps = math::max<R>(floor(_re + .5), 1) * _nes + 1;

  // TODO: criar malha de triângulos, ligar os pontos que já pertencem à geratriz nela
  vec3*& points = _poly.points();
  int nv = _poly.length();
  int nt{};
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new vec3();

  

  // TODO: p/ cada passo add mais np pontos (rotacionando em y e variando altura e largura) 
  // ligar os pontos na malha de triângulos
}

} // end namespace cg
