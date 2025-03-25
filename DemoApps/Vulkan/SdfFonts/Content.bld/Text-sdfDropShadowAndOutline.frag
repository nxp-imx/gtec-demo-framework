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
  uniform float ContourScale;
}
g_pushConstant;

layout(location = 0) in vec2 v_fragTextureCoord;
layout(location = 0) out vec4 FragColor;

const vec4 FontColor = vec4(1.0);
const vec4 OutlineColor = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 ShadowColor = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
  float distance = texture(Texture, v_fragTextureCoord).a;

  float outlineFactor = smoothstep(0.5 - g_pushConstant.Smoothing, 0.5 + g_pushConstant.Smoothing, distance);
  vec4 color = mix(OutlineColor, FontColor, outlineFactor);
  float alpha =
    smoothstep(g_pushConstant.OutlineDistance - g_pushConstant.Smoothing, g_pushConstant.OutlineDistance + g_pushConstant.Smoothing, distance);
  vec4 text = vec4(color.rgb, color.a * alpha);

  float shadowDistance = texture(Texture, v_fragTextureCoord - vec2(g_pushConstant.ShadowOffsetX, g_pushConstant.ShadowOffsetY)).a;
  float shadowAlpha = smoothstep(0.5 - g_pushConstant.ShadowSmoothing, 0.5 + g_pushConstant.ShadowSmoothing, shadowDistance);
  //  float shadowAlpha = smoothstep(g_pushConstant.OutlineDistance - g_pushConstant.ShadowSmoothing, g_pushConstant.OutlineDistance +
  //  g_pushConstant.ShadowSmoothing, shadowDistance);
  vec4 shadow = vec4(ShadowColor.rgb, ShadowColor.a * shadowAlpha);

  FragColor = mix(shadow, text, text.a);
}
