#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec2 v_texCoords;
uniform sampler2D u_texSampler;

void main()
{
  gl_FragColor = texture2D(u_texSampler, v_texCoords);
}
