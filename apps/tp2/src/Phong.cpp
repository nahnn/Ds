// OVERVIEW: Phong.cpp
// ========
// Class implementation for Phong Material. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 11/11/2026

#include "../include/Phong.h"
#include "../RayTracer.h"

using namespace cg;

Color Phong::shade(const vec3& P, const vec3& N, const Light& light) const
{
  auto color = _scene->ambientLight * ambient;
}