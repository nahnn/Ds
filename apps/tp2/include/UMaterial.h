// OVERVIEW: Material.h
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
  [[nodiscard]] Color shade(const vec3& pos, const vec3& normal, const Light& light) const; // used in rt

  // -- Secondary Ray tracing methods --
  [[nodiscard]] bool isTransparent(const vec3& pos) const;
  [[nodiscard]] bool isReflexive(const vec3& pos) const;
  [[nodiscard]] Color& reflectionColor(const ray3& ray, const vec3& pos) const; // used in rt
  [[nodiscard]] Color& refractionColor(const ray3& ray, const vec3& pos) const; // used in rt

  auto matFragmentShader() const;

private:

}; // UMaterial

} // end namespace cg

#endif // __UMaterial_h