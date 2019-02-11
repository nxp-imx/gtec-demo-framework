// BEWARE: This is a example shader and it has not been optimized for speed.

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec4 v_Color;

void main()
{
  gl_FragColor = v_Color;
}
