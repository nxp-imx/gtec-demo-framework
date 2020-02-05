#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  uniform float Level;
}
g_pushConstant;

layout(binding = 0) uniform sampler2D Texture256;
layout(binding = 1) uniform sampler2D Texture128;
layout(binding = 2) uniform sampler2D Texture64;
layout(binding = 3) uniform sampler2D Texture32;
layout(binding = 4) uniform sampler2D Texture16;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec3 s1 = texture(Texture256, v_TexCoord).rgb;
  vec3 s2 = texture(Texture128, v_TexCoord).rgb;
  vec3 s3 = texture(Texture64, v_TexCoord).rgb;
  vec3 s4 = texture(Texture32, v_TexCoord).rgb;
  vec3 s5 = texture(Texture16, v_TexCoord).rgb;

  o_fragColor = vec4((s1 + s2 + s3 + s4 + s5) * g_pushConstant.Level, 1.0);
  // o_fragColor = vec4(s5 + s5 + s5 + s5 + s5 + s5, 1.0);
  // o_fragColor = vec4(s3 + s3 + s3 + s3 + s3 + s3, 1.0);
}
