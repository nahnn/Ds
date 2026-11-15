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

Color
PBR::shade(const ray3& ray, const Intersection& hit, const vec3& N, const Light& light)
{
  Color color{};

  vec3 L; // bold Ll
  float d;

  const auto& V = ray.direction;
  const auto P = ray(hit.distance);

  light.lightVector(P, L, d);
  auto lc = light.lightColor(d); // Ll in italic

  // calculating BRDF
  auto Hl = (L + V).versor();
  vec3 fresnel = vec3{_attrs.os} + (vec3{1, 1, 1} - vec3{_attrs.os}) * powf(1 - L.dot(Hl), 5);
  float g1L = N.dot(L) * math::inverse(N.dot(L) * (1 - _attrs.k) + _attrs.k);
  float g1V = N.dot(V) * math::inverse(N.dot(V) * (1 - _attrs.k) + _attrs.k);
  float G = g1L * g1V;
  float D = powf(_attrs.alpha, 2) * math::inverse(
    math::pi<float> * powf(powf(N.dot(Hl), 2) * (powf(_attrs.alpha, 2) - 1) + 1, 2));
  
  vec3 fs{fresnel * G * D * math::inverse(4 * (-N.dot(L)) * (-N.dot(V)))};
  _attrs.fs.setRGB(fs.x, fs.y, fs.z, 1.0); // shade deveria ser const? fs deveria se alterar?
  _attrs.f = _attrs.fd + _attrs.fs;

  color = lc * N.dot(L) * _attrs.f;
  return color;
}

} // end namespace cg