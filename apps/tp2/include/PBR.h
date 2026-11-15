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

// TODO: fazer com que metalic influencie Os
struct PBRAttributes : public UMaterialAttributes
{
public:
  static float constexpr minOs = .05;

  Color od; // diffuse refletion color
  Color os; // specular refletion color
  float r; // roughness
  float k{}; // roughness constant
  float alpha{}; // roughness constant
  float m; // metalic
  Color f; // BRDF
  Color fd; // Diffuse BRDF
  Color fs; // Specular BRDF

  PBRAttributes(const Color& color):
    od{color},
    os{color},
    r{.3f}, // TODO: choose default
    m{.3f}, // TODO: choose default
    fd{od * math::inverse<float>(math::pi<float>)},
    fs{}
  {
    k = (float)(pow(r + 1, 2) / 8);
    alpha = (float)pow(r, 2);
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

  PBR() = default;

  PBR(const Color& color):
    UMaterial{color},
    _attrs{color}
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

  Color shade(const ray3&, const Intersection&, const vec3&, const Light&) override;
  const char* shaderCode() const override;

private:
  PBRAttributes _attrs;

};

} // end namespace cg

#endif // __PBR_h