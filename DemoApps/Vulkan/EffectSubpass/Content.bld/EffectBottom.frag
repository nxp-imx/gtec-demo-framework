#version 310 es

precision mediump float;

layout(std140, binding = 0) uniform UBO
{
  uniform float Time;
}
g_ubo;

layout(binding = 1) uniform sampler2D s_texture;
layout(binding = 2) uniform sampler2D s_textureMod;

layout(location = 0) in vec2 v_TexCoord0;
layout(location = 1) in vec2 v_TexCoord1;

layout(location = 0) out vec4 FragColor;

void main()
{
  float mask = texture(s_textureMod, v_TexCoord1).y;

  vec2 panner0 = vec2(g_ubo.Time * 0.10, g_ubo.Time * -0.10);
  float mod0 = texture(s_textureMod, (v_TexCoord1 + panner0) * 0.5).x;

  vec2 panner1 = vec2(g_ubo.Time * 0.05, g_ubo.Time * -0.30);
  float mod1 = texture(s_textureMod, (v_TexCoord1 + panner1) * 0.5).z;

  float col = (((mod1 * 0.25) + (mod0 * 1.2)) - 0.3) * 0.60f;
  float texMod = (col - 0.1) * 0.2 * mask;
  vec2 modded = vec2(v_TexCoord0.x + texMod, v_TexCoord0.y + texMod);
  col *= 1.5 * mask;
  FragColor = (texture(s_texture, v_TexCoord0 + texMod) + vec4(col * 0.5, col * 0.75, col, 0.0)) * 0.6;
  // FragColor = texture(s_texture, v_TexCoord0 + texMod)  * 0.6;
  // FragColor = vec4(texture(s_textureMod, (v_TexCoord1 + g_ubo.TexOffset) * 0.1).x);
  // FragColor = vec4(texture(s_textureMod, (v_TexCoord1 + g_ubo.TexOffset) * 0.1).x) * mask;
  // FragColor = vec4(col, col, col, 0.0);
  // FragColor = vec4(col * 0.5, col * 0.75, col, 0.0);
}