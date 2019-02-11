#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec3 g_vVSColor;

void main()
{
  gl_FragColor = vec4(g_vVSColor, 1.0);
}
