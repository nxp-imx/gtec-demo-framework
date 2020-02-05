#version 310 es
precision lowp float;

layout(location = 0) in mediump vec3 v_normal;
layout(location = 1) in mediump vec2 v_texcoord;

layout(std140, binding = 0) uniform UBO
{
  mat4 World;
  mat4 View;
  mat4 Projection;
  vec4 Displacement;
  vec4 AmbientColor;
  float MaxHairLength;
  float Reserved0;
  float Reserved1;
  float Reserved2;
  vec4 LightDirection1;
  vec4 LightColor1;
  vec4 LightDirection2;
  vec4 LightColor2;
}
g_ubo;

layout(push_constant) uniform LayerBlock
{
  layout(offset = 0) float CurrentLayer;
}
g_pushConstant;


layout(binding = 1) uniform sampler2D Texture0;
layout(binding = 2) uniform sampler2D Texture1;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);
  vec4 furData = texture(Texture1, v_texcoord);
  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.

  float dist = mix(1.0, 0.2, g_pushConstant.CurrentLayer);
  float shadow = mix(0.4, 1.0, g_pushConstant.CurrentLayer);
  furColor *= shadow;
  float furVisibility = (g_pushConstant.CurrentLayer > furData.r) ? 0.0 : furData.a * dist;
  furColor.a = (g_pushConstant.CurrentLayer == 0.0) ? 1.0 : furVisibility;
  vec3 N = normalize(v_normal);
  vec3 L1 = normalize(-g_ubo.LightDirection1.xyz);
  vec3 L2 = normalize(-g_ubo.LightDirection2.xyz);
  float lightAmount1 = max(dot(N, L1), 0.0);
  float lightAmount2 = max(dot(N, L2), 0.0);
  vec3 lighting = g_ubo.AmbientColor.xyz + (lightAmount1 * g_ubo.LightColor1.xyz) + (lightAmount2 * g_ubo.LightColor2.xyz);
  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
