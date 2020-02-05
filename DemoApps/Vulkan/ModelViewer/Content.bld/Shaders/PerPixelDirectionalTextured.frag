#version 310 es
// BEWARE: This is a example shader and it has not been optimized for speed.

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
  // Camera space
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
  // Normalize the input normal
  vec3 n = normalize(v_Normal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, g_ubo.LightDirection.xyz), 0.0);

  o_fragColor = texture(Texture0, v_TexCoord) * ((v_Color * intensity) + g_ubo.MatAmbient);
}
