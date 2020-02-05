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
  vec4 color;
  color = 0.0351562500000 * texture(Texture0, v_TexCoord - vec2(0.0, g_pushConstant.TexSize * 3.1111111640930));
  color += 0.3281250000000 * texture(Texture0, v_TexCoord - vec2(0.0, g_pushConstant.TexSize * 1.3333333730698));
  color += 0.2734375000000 * texture(Texture0, v_TexCoord);
  color += 0.3281250000000 * texture(Texture0, v_TexCoord + vec2(0.0, g_pushConstant.TexSize * 1.3333333730698));
  color += 0.0351562500000 * texture(Texture0, v_TexCoord + vec2(0.0, g_pushConstant.TexSize * 3.1111111640930));
  o_fragColor = color;
  o_fragColor = color;
}
