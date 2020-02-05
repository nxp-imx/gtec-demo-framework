#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 WorldView;
  uniform mat4 WorldViewProjection;
  uniform mat4 NormalMatrix;    // Consider mat4x3
  uniform vec4 LightDirection;
  uniform vec4 LightColor;
  uniform vec4 MatAmbient;
  uniform vec4 MatSpecular;
  uniform float MatShininess;
}
g_ubo;

layout(binding = 1) uniform sampler2D Texture0;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 color = texture(Texture0, v_TexCoord) * v_Color;

  vec3 N = normalize(v_Normal);
  vec3 L = normalize(g_ubo.LightDirection.xyz);
  float lightAmount = max(dot(N, L), 0.0);
  vec3 lighting = g_ubo.MatAmbient.xyz + (lightAmount * g_ubo.LightColor.xyz);
  color.xyz *= lighting;
  o_fragColor = color;
}