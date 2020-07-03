precision mediump float;

uniform sampler2D Texture;

varying vec2 v_fragTextureCoord;

void main()
{
  gl_FragColor = texture2D(Texture, v_fragTextureCoord);
  // gl_FragColor = vec4(texture2D(Texture, v_fragTextureCoord).a);
}
