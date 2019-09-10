#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0, set = 1) uniform UBO
{
  uniform vec3 LightDirection;
  uniform vec3 LightColor;
  uniform vec3 AmbientColor;
}
g_ubo;

layout(binding = 1, set = 1) uniform sampler2D Texture0;

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_TexCoord;
// layout (location = 2) in vec3 v_Eye;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 color = texture(Texture0, v_TexCoord);

  vec3 N = normalize(v_Normal);
  vec3 L = normalize(g_ubo.LightDirection);

  float lightAmount = max(dot(N, L), 0.0);
  vec3 lighting = g_ubo.AmbientColor + lightAmount * g_ubo.LightColor;
  color.xyz *= lighting;
  o_fragColor = color;
}
