#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec4 v_VertexColor;

void main()
{
  gl_FragColor = v_VertexColor;
}