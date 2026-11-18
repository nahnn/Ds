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

Color // TODO: might be inline
PBR::shade(const vec3& N, // Normal 
  const vec3& V, // DOP
  const vec3& L, // Light vector
  const float NV, // dot product N . V
  const float NL, // dot product N . L 
  const Color& lc) const // light color
{
  // calculating BRDF
  auto Hl = (L + V).versor();
  Color fresnel = _attrs.os + (Color::white -_attrs.os) * powf(1 - L.dot(Hl), 5);
  float g1L = NL * math::inverse(NL * (1 - _attrs.k) + _attrs.k);
  float g1V = NV * math::inverse(NV * (1 - _attrs.k) + _attrs.k);
  float G = g1L * g1V;
  float D = powf(_attrs.alpha, 2) * math::inverse(
    math::pi<float> * powf(powf(N.dot(Hl), 2) * (powf(_attrs.alpha, 2) - 1) + 1, 2));
  
  Color f{BRDF(fresnel * G * D * math::inverse(4 * (-NL) * (-NV)))}; // BRDF

  return lc * f * NL;
}

Color
PBR::specularReflectionColor(float NV) const
{
  Color F0{_attrs.os};
  Color c0{-1, -0.0275, -0.572, 0.022}, c1{1, 0.0425, 1.04, -0.04};
  Color k{_attrs.r * c0 + c1};
  float a{math::min(powf(k.x, 2), powf(2, -9.28 * NV)) * k.x + k.y};
  float A{-1.04 * a + k.z};
  float B{1.04 * a + k.w};

  return math::clamp(A * F0 + B * Color::white, attrs_type::minOs, Color::white);
}

inline Color
PBR::BRDF(Color s) const
{
  Color fd{_attrs.od * math::inverse<float>(math::pi<float>)}; // Diffuse BRDF
  Color fs{s.x, s.y, s.z, 1.0}; // Specular BRDF
  return fd + fs;
}

inline const char*
PBR::shaderCode() const
{
  return fragmentShader();
}

} // end namespace cg