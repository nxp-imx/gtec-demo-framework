#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0, set = 0) uniform UBO
{
  // uniform mat4 WorldView;
  uniform mat4 WorldViewProjection;
  uniform mat3 NormalMatrix;
}
g_ubo;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec2 v_TexCoord;
// layout (location = 2) out vec3 v_Eye;

void main()
{
  v_Normal = g_ubo.NormalMatrix * VertexNormal;
  v_TexCoord = VertexTexCoord;
  //  v_Eye = (g_ubo.WorldView * vec4(VertexPosition, 1.0)).xyz;
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
