// OVERVIEW: UMaterial.h
// ========
// Class definition for UMaterial. U stands for universal. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __UMaterial_h
#define __UMaterial_h

#include "graphics/Material.h"
#include "graphics/Light.h"
#include "geometry/Intersection.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// UMaterialAttributes: attributes of the universal material
// ===================
struct UMaterialAttributes
{
  // empty
}; // UMaterialAttributes


/////////////////////////////////////////////////////////////////////
//
// UMaterial: universal material base class
// =========
class UMaterial : public Material
{
public:
  using vec3 = vec3f;
  using ray3 = Ray3f;

  UMaterial():
    Material{Color::white}
  {
    // do nothing
  }

  UMaterial(const Color& color):
    Material{color}
  {
    // do nothing
  }

  UMaterial& operator =(UMaterial&&) noexcept = default;

  // -- Ray Tracing --
  /// returns the color given by the direct illumination model 
  virtual Color shade(const vec3&, const vec3&, const vec3&, const float, const float, const Color&) const = 0; // used in rt

  // Secondary Ray tracing
  bool isTransparent(const vec3& P) const;
  bool isReflexive(const vec3& P) const;
  Color& reflectionColor(const ray3& ray, const vec3& P) const;
  Color& refractionColor(const ray3& ray, const vec3& P) const;
  virtual Color specularReflectionColor(float) const = 0;

  // -- Shading Language --
  /// Returns the fragment shader code for a given material
  virtual const char* shaderCode() const = 0;

}; // UMaterial

} // end namespace cg

#endif // __UMaterial_h