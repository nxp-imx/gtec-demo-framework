#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

const float Power = 4.0;
const float Scale = 1.83;
const float Bias = 0.27;

layout(binding = 0) uniform sampler2D Texture0;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec3 color = texture(Texture0, v_TexCoord).rgb;
  o_fragColor = vec4((-color + (pow(color, vec3(Power)) * Scale)) / Bias, 1.0);
}
