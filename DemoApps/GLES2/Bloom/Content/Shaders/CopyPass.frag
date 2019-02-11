#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;

varying vec2 v_TexCoord;

void main()
{
  gl_FragColor = texture2D(Texture0, v_TexCoord);
}
