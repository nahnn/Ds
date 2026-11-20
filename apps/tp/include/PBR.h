// OVERVIEW: PBR.h
// ========
// Class definition for PBR Material. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 11/11/2026

#ifndef __PBR_h
#define __PBR_h

#include "UMaterial.h"

namespace cg
{ // begin namespace cg 

struct PBRAttributes : public UMaterialAttributes
{
public:
  static Color constexpr minOs{.04f, .04f, .04f, 1.f};

  Color od; // diffuse refletion color
  Color os; // specular refletion color
  float m; // metalic
  float r; // roughness

  PBRAttributes(const Color& Od, const Color& Os, float metalic = .3, float roughness = .3):
    od{Od},
    os{Os},
    m{metalic},
    r{roughness}
  {
    // do nothing
  }

  bool operator ==(const PBRAttributes& other) const
  {
    return od == other.od && os == other.os && r == other.r && m == other.m;
  }

  bool operator !=(const PBRAttributes& other) const
  {
    return !operator==(other);
  }

private:
  void clampAttrs()
  {
    // TODO
  }

}; // PBRAttributes

class PBR : public UMaterial
{
public:
  using type = PBR;
  using attrs_type = PBRAttributes;

  PBRAttributes attrs;

  static const char* fragmentShader();

  PBR() = default;

  PBR(const Color& color):
    UMaterial{color},
    attrs{color, color}
  {
    // do nothing
  }

  PBR(const Color& Od, const Color& Os, float metalic = .3, float roughness = .3):
    UMaterial{},
    attrs{Od, Os, metalic, roughness}
  {
    // do nothing
  }

  bool operator ==(const PBR& other) const
  {
    return attrs == other.attrs;
  }

  bool operator !=(const PBR& other) const
  {
    return !operator==(other);
  }

  Color shade(const vec3&, const vec3&, const vec3&, const float, const float, const Color&) const override;
  const char* shaderCode() const override;
  Color specularReflectionColor(float) const override;
  Color BRDF(Color) const;

private:

};

} // end namespace cg

#endif // __PBR_h