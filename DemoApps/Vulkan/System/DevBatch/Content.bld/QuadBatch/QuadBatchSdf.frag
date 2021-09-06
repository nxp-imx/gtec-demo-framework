#version 450 core

layout(set = 1, binding = 0) uniform sampler2D Texture;

layout(location = 0) in vec4 inFragColor;
layout(push_constant) uniform LayerBlock
{
  // 0.25f / (spread * scale)
  uniform float Smoothing;
}
g_pushConstant;

layout(location = 1) in vec2 inFragTextureCoord;

layout(location = 0) out vec4 outFragColor;

void main()
{
  float distance = texture(Texture, inFragTextureCoord).a;
  float alpha = smoothstep(0.5 - g_pushConstant.Smoothing, 0.5 + g_pushConstant.Smoothing, distance);
  outFragColor = vec4(inFragColor.rgb, inFragColor.a * alpha);
}
