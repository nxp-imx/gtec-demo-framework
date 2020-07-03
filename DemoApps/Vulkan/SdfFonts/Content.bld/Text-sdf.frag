#version 310 es
precision mediump float;

layout(binding = 0) uniform sampler2D Texture;

layout(push_constant) uniform LayerBlock
{
  uniform mat4 ModelViewProj;
  // 0.25f / (spread * scale)
  uniform float Smoothing;
  // Between 0 and spread / textureSize
  uniform float ShadowOffsetX;
  uniform float ShadowOffsetY;
  // Between 0 and 0.5
  uniform float ShadowSmoothing;
  // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
  uniform float OutlineDistance;
}
g_pushConstant;

layout(location = 0) in vec2 v_fragTextureCoord;
layout(location = 0) out vec4 FragColor;

const vec4 FontColor = vec4(1.0);
void main()
{
  float distance = texture(Texture, v_fragTextureCoord).a;
  float alpha = smoothstep(0.5 - g_pushConstant.Smoothing, 0.5 + g_pushConstant.Smoothing, distance);
  FragColor = vec4(FontColor.rgb, FontColor.a * alpha);
}
