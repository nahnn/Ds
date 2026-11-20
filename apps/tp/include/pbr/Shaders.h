// OVERVIEW: Material.h
// ========
// Class definition for general shaders
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __Shaders_h
#define __Shaders_h

namespace cg
{ // begin namespace cg

static const char* vertexShader = R"glsl(
  #version 400
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec3 normal;
  layout(location = 2) in vec2 uv;
  layout(location = 3) in vec4 color;
  uniform mat4 mvMatrix;
  uniform mat3 normalMatrix;
  uniform mat4 mvpMatrix;
  out vec3 v_position;
  out vec3 v_normal;
  out vec2 v_uv;
  out vec4 v_color;

  void main()
  {
    gl_Position = mvpMatrix * position;
    v_position = vec3(mvMatrix * position);
    v_normal = normalMatrix * normal;
    v_color = color;
    v_uv = uv;
  }
)glsl";

static const char* geometryShader = R"glsl(
  #version 400
  layout(triangles) in;
  layout(triangle_strip, max_vertices = 3) out;

  in vec3 v_position[];
  in vec3 v_normal[];
  in vec2 v_uv[];
  in vec4 v_color[];
  uniform mat4 viewportMatrix;
  out vec3 g_position;
  out vec3 g_normal;
  out vec2 g_uv;
  out vec4 g_color;
  noperspective out vec3 g_edgeDistance;

  void main()
  {
    vec2 p0 = vec2(viewportMatrix *
      (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(viewportMatrix *
      (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(viewportMatrix *
      (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0;
    float a = v2.x * v1.y - v1.x * v2.y;

    gl_Position = gl_in[0].gl_Position;
    g_edgeDistance = vec3(abs(a / length(v0)), 0, 0);
    g_position = v_position[0];
    g_normal = v_normal[0];
    g_color = v_color[0];
    g_uv = v_uv[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    g_edgeDistance = vec3(0, abs(a / length(v1)), 0);
    g_position = v_position[1];
    g_normal = v_normal[1];
    g_color = v_color[1];
    g_uv = v_uv[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    g_edgeDistance = vec3(0, 0, abs(a / length(v2)));
    g_position = v_position[2];
    g_normal = v_normal[2];
    g_color = v_color[2];
    g_uv = v_uv[2];
    EmitVertex();
    EndPrimitive();
  }
)glsl";

} // end namespace cg

#endif // __Shaders_h