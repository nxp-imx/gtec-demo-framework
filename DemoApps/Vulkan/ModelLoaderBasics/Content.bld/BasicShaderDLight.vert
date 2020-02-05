#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 WorldViewProjection;
  uniform mat4 NormalMatrix;    // Consider mat4x3
  uniform vec4 LightDirection;
  uniform vec4 LightColor;
  uniform vec4 AmbientColor;
}
g_ubo;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec4 VertexColor;
layout(location = 2) in vec3 VertexNormal;
layout(location = 3) in vec2 VertexTexCoord;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
  v_Color = VertexColor;
  v_Normal = mat3(g_ubo.NormalMatrix) * VertexNormal;
  v_TexCoord = VertexTexCoord;
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
