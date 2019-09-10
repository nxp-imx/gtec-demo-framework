#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 WorldViewProjection;
}
g_ubo;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
