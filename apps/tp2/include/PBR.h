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
  static Color constexpr minOs{.04, .04, .04, 1};

  Color od; // diffuse refletion color
  Color os; // specular refletion color
  float r; // roughness
  float k{}; // roughness constant
  float alpha{}; // roughness constant
  float m; // metalic

  PBRAttributes(const Color& Od, const Color& Os, float metalic = .3, float roughness = .3):
    od{(1 - metalic) * Od},
    os{(1 - metalic) * minOs * Color::white + metalic * Os},
    r{roughness},
    m{metalic}
  {
    k = powf(r + 1, 2) / 8;
    alpha = powf(r, 2);
  }

  PBRAttributes(PBRAttributes&& other) noexcept = default;

  PBRAttributes& operator =(PBRAttributes&& other) noexcept = default;

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

  static const char* fragmentShader();

  PBR() = default;

  PBR(const Color& color):
    UMaterial{color},
    _attrs{color, color}
  {
    // do nothing
  }

  PBR(const Color& Od, const Color& Os, float metalic = .3, float roughness = .3):
    UMaterial{},
    _attrs{Od, Os, metalic, roughness}
  {
    // do nothing
  }

  /// @brief Constructor used when whe must 
  PBR(attrs_type&& attrs) noexcept:
    UMaterial{},
    _attrs{std::move(attrs)}
  {
    // do nothing
  }

  bool operator ==(const PBR& other) const
  {
    return _attrs == other._attrs;
  }

  bool operator !=(const PBR& other) const
  {
    return !operator==(other);
  }

  PBR& operator =(PBR&& other) noexcept
  {
    if (this != &other)
    {
      // move data
      // TODO: mover clase base?
      _attrs = std::move(other._attrs);
    }
    return *this;
  }

  const attrs_type& attributes() const
  {
    return _attrs;
  }

  Color shade(const vec3&, const vec3&, const vec3&, const float, const float, const Color&) const override;
  const char* shaderCode() const override;
  Color specularReflectionColor(float) const override;
  Color BRDF(Color) const;

private:
  PBRAttributes _attrs;

};

} // end namespace cg

#endif // __PBR_h