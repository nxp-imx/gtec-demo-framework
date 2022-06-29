precision mediump float;

uniform sampler2D Texture;

varying vec4 FragColor;
varying vec2 FragTextureCoord;

void main()
{
  gl_FragColor = texture2D(Texture, FragTextureCoord) * FragColor * vec4(0.75, 0.2, 0.2, 1.0);
}
