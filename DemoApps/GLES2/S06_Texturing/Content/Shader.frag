#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D s_texture;
varying vec3 g_vVSColor;
varying vec2 g_vVSTexCoord;

void main()
{
  gl_FragColor = texture2D(s_texture, g_vVSTexCoord);
}
