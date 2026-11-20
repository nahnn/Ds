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
  const vec3& V, // Ray Direction
  const vec3& L, // Light vector (direction: P to Light)
  const float NV, // dot product N . V (NV <= 0)
  const float NL, // dot product N . L (NL > 0)
  const Color& lc) const // light color
{
  // calculating BRDF
  Color os{(1 - attrs.m) * attrs_type::minOs * Color::white + attrs.m * attrs.os}; // TODO: change?
  float k{powf(attrs.r + 1, 2) / 8}; // roughness constant
  float alpha{powf(attrs.r, 2)}; // roughness constant
  auto Hl = (L - V).versor();
  float nv = math::max<float>(-NV, 1e-4);
  Color fresnel = os + (Color::white - os) * powf(1 - L.dot(Hl), 5);
  float G = NL * math::inverse(NL * (1 - k) + k)  * nv * math::inverse(nv * (1 - k) + k); // G [0,1] w/ NL > 0 and NV > 0
  float NH = N.dot(Hl);
  float a2 = powf(alpha, 2);
  float D = a2 * math::inverse(math::pi<float> * powf(NH * NH * (a2 - 1) + 1, 2));
  
  Color f{BRDF(fresnel * G * D * math::inverse(4 * NL * nv))}; // BRDF

  return lc * f * NL;
}

Color
PBR::specularReflectionColor(float cosPhi) const
{
  Color F0{(1 - attrs.m) * attrs_type::minOs * Color::white + attrs.m * attrs.os};
  float r = attrs.r;

  float k1{-r + 1.f};
  float k2{-0.0275f * r + 0.0425f};
  float k3{-0.572f * r + 1.04f};
  float k4{0.022f * r - 0.04f};

  float a{math::min(k1 * k1, powf(2.f, -9.28f * cosPhi)) * k1 + k2}; // cos(phi) = -NV
  float A{-1.04f * a + k3};
  float B{1.04f * a + k4};

  return A * F0 + B * Color::white;
}

inline Color
PBR::BRDF(Color s) const
{
  Color od{(1 - attrs.m) * attrs.od}; // TODO: change?
  Color fd{od * math::inverse<float>(math::pi<float>)}; // Diffuse BRDF
  Color fs{s.x, s.y, s.z, 1.0}; // Specular BRDF
  return fd + fs;
}

const char*
PBR::shaderCode() const
{
  return fragmentShader();
}

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
  uniform int lightCount;
  uniform LightProps lights[8];
  uniform MaterialProps material;
  uniform LineProps line;
  uniform sampler2D sampler;
  uniform int hiddenLinesFlag;
  uniform int useTexture;
  uniform int useVertexColors;
  layout(location = 0) out vec4 fragmentColor;
  const vec4 minOs = vec4(.04, .04, .04, 1);

  MaterialProps materialProps()
  {
    MaterialProps p;  

    vec4 base = useVertexColors != 0 ? g_color :
    useTexture != 0 ? texture(sampler, g_uv) : material.Od;
    p.Od = (1 - material.m) * base; // (1 - m) * od
    p.Os = (1 - material.m) * minOs * vec4(1, 1, 1, 1) + material.m * material.Os; // (1-m)*smin + m*Os
    p.r = material.r;
    p.m = material.m;
    return p;
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
        float NV = max(-dot(N, V), 1e-4); // dot(N, V) <= 0
        vec3 Hl = normalize(L - V);
        float LH = max(dot(L, Hl), 1e-4);
        float NH = max(dot(N, Hl), 1e-4);
        float k = pow(m.r + 1, 2) / 8;
        float alpha = pow(m.r, 2);
        float a2 = max(pow(m.r, 4), 1e-6);
        vec4 F = m.Os + (vec4(1,1,1,1) - m.Os) * pow(1 - LH, 5);
        float G = NL / (NL * (1 - k) + k) * NV / (NV * (1 - k) + k); // G [0,1] w/ NL > 0 and NV > 0
        float D = a2 / (PI * pow(NH * NH * (a2 - 1) + 1, 2));
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


namespace
{
  inline float maxRGB(const Color& c) { return math::max(math::max(c.r, c.g), c.b); }
  inline bool isBlack(const Color& c) { return c == Color::black; }
}

//converts legacy Phong to PBR
PBR*
PBR::toPBR(const Material& m)
{
  Color od = isBlack(m.diffuse) ? m.ambient : m.diffuse;
  float metallic = math::min(1.f, maxRGB(m.specular));
  Color os = isBlack(m.specular) ? m.spot : m.specular;
  float roughness = powf(2.f / (m.shine + 2.f), .25f);

  roughness = math::max(.05f, math::min(1.f, roughness));

  auto pbr = new PBR{od, os, metallic, roughness};

  pbr->setName("%s", m.name());
  return pbr;
}



} // end namespace cg