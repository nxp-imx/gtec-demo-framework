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
  vec4 color;
  color = ##REPLACE0_WEIGHT## * texture2D(Texture0, v_TexCoord - vec2(TexSize * ##REPLACE0_OFFSET##, 0.0));
  color += ##REPLACE1_WEIGHT## * texture2D(Texture0, v_TexCoord - vec2(TexSize * ##REPLACE1_OFFSET##, 0.0));
  color += ##REPLACE2_WEIGHT## * texture2D(Texture0, v_TexCoord);
  color += ##REPLACE1_WEIGHT## * texture2D(Texture0, v_TexCoord + vec2(TexSize * ##REPLACE1_OFFSET##, 0.0));
  color += ##REPLACE0_WEIGHT## * texture2D(Texture0, v_TexCoord + vec2(TexSize * ##REPLACE0_OFFSET##, 0.0));
  gl_FragColor = color;
}
