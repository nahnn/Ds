// OVERVIEW: UMaterial.h
// ========
// Class definition for UMaterial. U stands for universal. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __UMaterial_h
#define __UMaterial_h

#include "Material.h"
#include "Light.h"
#include "Intersection.h"

namespace cg
{ // begin namespace cg

// TODO: attrs de materiais ou classes para materiais 

class UMaterial : public Material
{
public:
  using vec3 = vec3f;
  using ray3 = Ray3f;

  /// @brief returns the color given by the direct ilumination model 
  virtual Color shade(const vec3& P, const vec3& N, const Light& light) const = 0; // used in rt

  // -- Secondary Ray tracing methods (all used in rt) --
  bool isTransparent(const vec3& P) const;
  bool isReflexive(const vec3& P) const;
  Color& reflectionColor(const ray3& ray, const vec3& P) const;
  Color& refractionColor(const ray3& ray, const vec3& P) const;

  /// @brief Returns the fragment shader code for a given material
  virtual const char* shaderCode() const = 0;

private:

}; // UMaterial

} // end namespace cg

#endif // __UMaterial_h