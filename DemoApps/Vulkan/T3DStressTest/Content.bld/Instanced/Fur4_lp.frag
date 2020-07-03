#version 310 es
precision lowp float;

layout(location = 0) in mediump vec3 v_normal;
layout(location = 1) in mediump vec2 v_texcoord;
layout(location = 2) in mediump float v_layerDepth;

layout(std140, binding = 0) uniform UBO
{
  mat4 World;
  mat4 View;
  mat4 Projection;
  vec4 Displacement;
  vec4 AmbientColor;
  float MaxHairLength;
  float InstanceMul;
  float Reserved0;
  float Reserved1;
  vec4 LightDirection1;
  vec4 LightColor1;
  vec4 LightDirection2;
  vec4 LightColor2;
  vec4 LightDirection3;
  vec4 LightColor3;
  vec4 LightDirection4;
  vec4 LightColor4;
}
g_ubo;

layout(binding = 1) uniform sampler2D Texture0;
layout(binding = 2) uniform sampler2D Texture1;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);
  vec4 furData = texture(Texture1, v_texcoord);
  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.

  float dist = mix(1.0, 0.2, v_layerDepth);
  float shadow = mix(0.4, 1.0, v_layerDepth);
  furColor *= shadow;
  float furVisibility = (v_layerDepth > furData.r) ? 0.0 : furData.a * dist;
  furColor.a = (v_layerDepth == 0.0) ? 1.0 : furVisibility;
  vec3 N = normalize(v_normal);
  vec3 L1 = normalize(-g_ubo.LightDirection1.xyz);
  vec3 L2 = normalize(-g_ubo.LightDirection2.xyz);
  vec3 L3 = normalize(-g_ubo.LightDirection3.xyz);
  vec3 L4 = normalize(-g_ubo.LightDirection4.xyz);
  float lightAmount1 = max(dot(N, L1), 0.0);
  float lightAmount2 = max(dot(N, L2), 0.0);
  float lightAmount3 = max(dot(N, L3), 0.0);
  float lightAmount4 = max(dot(N, L4), 0.0);
  vec3 lighting = g_ubo.AmbientColor.xyz + (lightAmount1 * g_ubo.LightColor1.xyz) + (lightAmount2 * g_ubo.LightColor2.xyz) +
                  (lightAmount3 * g_ubo.LightColor3.xyz) + (lightAmount4 * g_ubo.LightColor4.xyz);
  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
