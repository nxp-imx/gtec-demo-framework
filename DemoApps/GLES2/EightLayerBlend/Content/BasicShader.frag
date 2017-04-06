#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture;

varying vec2 FragTextureCoord;

void main()
{
  gl_FragColor = texture2D(Texture, FragTextureCoord);
}
