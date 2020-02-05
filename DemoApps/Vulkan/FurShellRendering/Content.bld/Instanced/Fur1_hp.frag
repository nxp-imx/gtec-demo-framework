#version 310 es
precision highp float;

layout(location = 0) in mediump vec3 v_normal;
layout(location = 1) in mediump vec2 v_texcoord;
layout(location = 2) in mediump float v_instanceId;
layout(location = 3) in mediump float v_dist;
layout(location = 4) in mediump float v_shadow;

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
}
g_ubo;

layout(binding = 1) uniform sampler2D Texture0;
layout(binding = 2) uniform sampler2D Texture1;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);
  vec4 furData = texture(Texture1, v_texcoord);

  furColor *= v_shadow;

  float furVisibility = (v_instanceId > furData.r) ? 0.0 : furData.a * v_dist;
  furColor.a = (v_instanceId == 0.0) ? 1.0 : furVisibility;

  float lightAmount = max(dot(v_normal, -g_ubo.LightDirection1.xyz), 0.0);
  vec3 lighting = g_ubo.AmbientColor.xyz + lightAmount * g_ubo.LightColor1.xyz;
  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
