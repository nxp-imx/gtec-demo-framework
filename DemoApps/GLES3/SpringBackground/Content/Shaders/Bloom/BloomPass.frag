#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture256;
uniform sampler2D Texture128;
uniform sampler2D Texture64;
uniform sampler2D Texture32;
uniform sampler2D Texture16;
uniform float Level;

in vec2 v_TexCoord;

out vec4 o_fragColor;

void main()
{
  vec3 s1 = texture(Texture256, v_TexCoord).rgb;
  vec3 s2 = texture(Texture128, v_TexCoord).rgb;
  vec3 s3 = texture(Texture64, v_TexCoord).rgb;
  vec3 s4 = texture(Texture32, v_TexCoord).rgb;
  vec3 s5 = texture(Texture16, v_TexCoord).rgb;

  o_fragColor = vec4((s1 + s2 + s3 + s4 + s5) * Level, 1.0);
  // o_fragColor = vec4(s5 + s5 + s5 + s5 + s5 + s5, 1.0);
  // o_fragColor = vec4(s3 + s3 + s3 + s3 + s3 + s3, 1.0);
}
