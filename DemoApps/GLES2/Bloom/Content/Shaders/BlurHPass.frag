#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


uniform sampler2D Texture0;
uniform float TexSize;

varying vec2 v_TexCoord;

void main()
{
  vec4 color = texture2D(Texture0, v_TexCoord + vec2(-3.0 * TexSize, 0.0)) * 1.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(-2.0 * TexSize, 0.0)) * 10.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(-1.0 * TexSize, 0.0)) * 20.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(0.0 * TexSize, 0.0)) * 26.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(+1.0 * TexSize, 0.0)) * 20.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(+2.0 * TexSize, 0.0)) * 10.0 / 32.0;
  color += texture2D(Texture0, v_TexCoord + vec2(+3.0 * TexSize, 0.0)) * 1.0 / 32.0;
  gl_FragColor = color;
}
