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

// implementation of fs
const char*
PBR::fragmentShader()
{
  return R"glsl(
  #version 400
  struct LightProps
  {
    int type; // DIRECTIONAL/POINT/SPOT
    vec4 color; // color
    vec3 position; // VRC position
    vec3 direction; // VRC direction
    int falloff; // CONSTANT/LINEAR/QUADRATIC
    float range; // range (== 0 INFINITE)
    float angle; // spot angle
  };

  struct MaterialProps // TODO: alterar
  {
    vec4 Od; // diffuse color
    vec4 Os; // specular spot color
    float r; // roughness
    float m; // metalic
  };

  struct LineProps
  {
    float width;
    vec4 color;
  };

  in vec3 g_position;
  in vec3 g_normal;
  in vec2 g_uv;
  in vec4 g_color;
  noperspective in vec3 g_edgeDistance;
  //uniform vec4 backFaceColor;
  uniform int projectionType; // PERSPECTIVE/PARALLEL
  uniform vec4 ambientLight;
  uniform vec4 minOs; // min Os value 
  uniform int lightCount;
  uniform LightProps lights[8];
  uniform MaterialProps material;
  uniform LineProps line;
  uniform sampler2D sampler;
  uniform int hiddenLinesFlag;
  uniform int useTexture;
  uniform int useVertexColors;
  layout(location = 0) out vec4 fragmentColor;

  MaterialProps materialProps()
  {
    MaterialProps m;

    if (useVertexColors != 0)
    {
      m.Od = (1 - material.m) * material.Od * (g_color);
      m.Os = (1 - material.m) * minOs * vec4(1, 1, 1, 1) + material.m * material.Os;
      m.r = material.r;
      m.m = material.m;
    }
    else
    {
      m.Od = useTexture != 0 ? texture(sampler, g_uv) : material.Od;
      m.Os = material.Os;
      m.r = material.r;
      m.m = material.m;
    }
    return m;
  }

  bool lightVector(int i, vec3 P, out vec3 L, out float d)
  {
    int type = lights[i].type;

    // DIRECTIONAL
    if (type == 0)
    {
      L = -lights[i].direction;
      return true;
    }
    L = lights[i].position - P;
    d = length(L);

    float range = lights[i].range;

    if (d == 0 || (range > 0 && d > range))
      return false;
    L /= d;
    // POINT
    if (type == 1)
      return true;

    // SPOT
    float DL = dot(lights[i].direction, L);
    return DL < 0 && lights[i].angle > radians(acos(DL));
  }

  vec4 lightColor(int i, float d)
  {
    int falloff = lights[i].falloff;

    // directional light or constant falloff
    if (lights[i].type == 0 || falloff == 0)
      return lights[i].color;

    float range = lights[i].range;
    float f;

    if (range == 0) // infinite range
    {
      f = 1 / d;
      if (falloff == 2) // quadratic falloff
        f *= f;
    }
    else
    {
      f = d / range;
      f = falloff == 2 ? 1 + f * (f - 2) : 1 - f;
    }
    return lights[i].color * f;
  }

  vec4 PBR(vec3 P, vec3 N) // TODO: alterar
  {
    MaterialProps m = materialProps();
    vec3 V = projectionType == 0 ?
      // PERSPECTIVE
      normalize(P) :
      // PARALLEL
      vec3(0, 0, -1);

    if (dot(N, V) > 0)
      //return backFaceColor;
      N *= -1;

    vec3 R = reflect(V, N);
    vec4 color = vec4(0, 0, 0, 0);
    const float PI = 3.14159265;

    for (int i = 0; i < lightCount; i++)
    {
      vec3 L; float d;

      if (lightVector(i, P, L, d))
      {
        vec4 I = lightColor(i, d);

        // PBR color
        float NL = dot(N, L);
        if (NL <= 0) continue; // behind surface
        float NV = max(-dot(N, V), -1e4);
        vec3 Hl = normalize(L - V);
        float LH = max(dot(L, Hl), 0);
        float NH = max(dot(N, Hl), 0);
        float k = pow(m.r + 1, 2) / 8;
        float alpha = pow(m.r, 2);
        float a2 = max(pow(m.r, 4), 1e-6);
        vec4 fresnel = m.Os + (vec4(1,1,1,1) - m.Os) * pow(1 - LH, 5);
        float g = NL / (NL * (1 - k) + k) * NV / (NV * (1 - k) + k);
        float D = a2 / (PI * pow(pow(NH, 2) * (a2 - 1) + 1, 2));
        vec4 f = m.Od / PI + F * G * D / (4 * NL * NV);
        color += I * f * NL;
      }
    }
    color *= PI;
    return min(color, vec4(1));
  }

  float min3(vec3 v)
  {
    return min(min(v.x, v.y), v.z);
  }

  vec4 mixLineColor(vec4 color)
  {
    float d = min3(g_edgeDistance);
    float mixVal;

    if (d < line.width - 1)
      mixVal = 1;
    else if (d > line.width + 1)
      mixVal = 0;
    else
    {
      float x = d - (line.width - 1);
      mixVal = exp2(-2 * (x * x));
    }
    return mix(color, line.color, mixVal);
  }

  void main() // TODO: alterar
  {
    vec4 color = PBR(g_position, normalize(g_normal));
    fragmentColor = hiddenLinesFlag != 0 ? mixLineColor(color) : color;
  }
)glsl";
}

} // end namespace cg

#endif // __PBR_h