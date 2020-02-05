#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  uniform float TexSize;
}
g_pushConstant;

layout(binding = 0) uniform sampler2D Texture0;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 color = texture(Texture0, v_TexCoord + vec2(0.0, -3.0 * g_pushConstant.TexSize)) * 1.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, -2.0 * g_pushConstant.TexSize)) * 10.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, -1.0 * g_pushConstant.TexSize)) * 20.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, 0.0 * g_pushConstant.TexSize)) * 26.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, +1.0 * g_pushConstant.TexSize)) * 20.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, +2.0 * g_pushConstant.TexSize)) * 10.0 / 32.0;
  color += texture(Texture0, v_TexCoord + vec2(0.0, +3.0 * g_pushConstant.TexSize)) * 1.0 / 32.0;
  o_fragColor = color;
}
