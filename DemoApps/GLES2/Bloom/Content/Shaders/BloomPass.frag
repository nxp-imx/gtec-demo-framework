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

varying vec2 v_TexCoord;

void main()
{
  vec3 s1 = texture2D(Texture256, v_TexCoord).rgb;
  vec3 s2 = texture2D(Texture128, v_TexCoord).rgb;
  vec3 s3 = texture2D(Texture64, v_TexCoord).rgb;
  vec3 s4 = texture2D(Texture32, v_TexCoord).rgb;
  vec3 s5 = texture2D(Texture16, v_TexCoord).rgb;

  gl_FragColor = vec4((s1 + s2 + s3 + s4 + s5) * Level, 1.0);
  // gl_FragColor = vec4(s5 + s5 + s5 + s5 + s5 + s5, 1.0);
  // gl_FragColor = vec4(s3 + s3 + s3 + s3 + s3 + s3, 1.0);
}
