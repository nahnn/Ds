// OVERVIEW: PBR.h
// ========
// Class implementation for PBR Material. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 14/11/2026

#include "../include/PBR.h"
#include "RayTracer.h"

using namespace cg;


namespace cg
{ // begin namespace cg

Color PBR::shade(const vec3& P, const vec3& N, const Light& L, const float& d) const
{
  Color color{};
  auto lc = L.lightColor(d); // Ll in italic
  // TODO: calculate hl, fresnel, G, D, specular brdf
  //

  return color;
}

} // end namespace cg