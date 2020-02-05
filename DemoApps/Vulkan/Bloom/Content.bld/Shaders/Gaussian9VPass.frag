#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  uniform float Weight0;
  uniform float Weight1;
  uniform float Weight2;
  uniform float Offset0;
  uniform float Offset1;
}
g_pushConstant;

layout(binding = 0) uniform sampler2D Texture0;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 color;
  color = g_pushConstant.Weight0 * texture(Texture0, v_TexCoord - vec2(0.0, g_pushConstant.Offset0));
  color += g_pushConstant.Weight1 * texture(Texture0, v_TexCoord - vec2(0.0, g_pushConstant.Offset1));
  color += g_pushConstant.Weight2 * texture(Texture0, v_TexCoord);
  color += g_pushConstant.Weight1 * texture(Texture0, v_TexCoord + vec2(0.0, g_pushConstant.Offset1));
  color += g_pushConstant.Weight0 * texture(Texture0, v_TexCoord + vec2(0.0, g_pushConstant.Offset0));
  o_fragColor = color;
}
