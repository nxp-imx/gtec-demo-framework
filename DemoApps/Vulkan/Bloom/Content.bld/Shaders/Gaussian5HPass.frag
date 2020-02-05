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
  uniform float Offset0;
}
g_pushConstant;

layout(binding = 0) uniform sampler2D Texture0;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec4 color;
  color = g_pushConstant.Weight0 * texture(Texture0, v_TexCoord - vec2(g_pushConstant.Offset0, 0.0));
  color += g_pushConstant.Weight1 * texture(Texture0, v_TexCoord);
  color += g_pushConstant.Weight0 * texture(Texture0, v_TexCoord + vec2(g_pushConstant.Offset0, 0.0));
  o_fragColor = color;
}
