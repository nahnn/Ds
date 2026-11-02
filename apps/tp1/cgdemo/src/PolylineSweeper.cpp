// OVERVIEW: Spiral.cpp
// ========
// Implementation for Spirals.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#include "include/SpiralSweeper.h"
#include "TorsionSweeper.h"

namespace cg
{ // begin namespace cg

void setVertices();

void setVertexNormals();

template<IsReal R>
inline void setTriangles(TriangleMesh::Triangle* t,  
  index_t i, 
  index_t j,
  index_t np
)
{
  for(index_t k = 0; k < np; ++i)
  {
    t->setVertices(i*k, i*k + 1, j*k);
  }
}

template<IsReal R>
inline void setVertexArray(Vector3<R>*& to, Vector3<R>*& from, size_t total)
{
  for(index_t i = 0; i < total; ++i) to[i] = from[i];
}
 
template<IsReal R>
TriangleMesh* SpiralSweeper<R>::sweep(const Polyline<R>& poly) const
{
  // computing process parameters
  auto de = (_attr.we - poly.width()) / 2;
  auto te = static_cast<R>(cg::math::toRadians(_attr.re * 360));
  auto steps = math::max<size_t>(floor(_attr.re + .5), 1) * _attr.nes + 1;
  auto deltaHeight = _attr.re * _attr.dh;
  vec3 dH{0, deltaHeight, 0};
  auto deltaWidth = _attr.re * _attr.dw;
  vec3 dW{de + deltaWidth, 0, 0};
  vec3 t{dW + dH};
  auto deltaAngle = te / (steps - 1);
  auto rotMat = mat3{quat{vec3{0, deltaAngle, 0}}};

  int nv = poly.length() * steps;
  int nt{(2 * nv - 2) * steps};
  TriangleMesh::Data data;

  data.vertexCount = nv;
  data.triangleCount = nt;
  data.vertices = new vec3[nv];
  data.vertexNormals = new vec3[nv];
  data.triangles = new vec3[nt];
  vec3* vertCache = new vec3[nv]; // se pa que deletar no fim do método

  vec3*& points = poly.points();
  setVertexArray<R>(data.vertices, points, poly.length());
  setVertexArray<R>(vertCache, points, poly.length());

  // computing steps
  for(index_t i = 0; i < steps; ++i) // current line 
    for(index_t j = 0; j < nv; ++j) // points
    {
      vec3 pi = new vec3{rotMat * vertCache[i * nv + j]}; // new point rotated
      pi = pi + i * t;
      vertCache[i * nv + j] = data.vertices[i * nv + j] = pi;
      setTriangles<R>(data.triangles, i, i + 1, nv);  
    }
  
  // TODO: open / closed and capped / uncapped (conectar os vértices da primeira e última linha e / ou fechar)
  TriangleMesh* t = new TriangleMesh{std::move(data)};
  t->computeNormals();
  return t;
}

template<IsReal R>
TriangleMesh* TorsionSweeper<R>::sweep(const Polyline<R>& poly) const
{
  //TODO
}

} // end namespace cg
