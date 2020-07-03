#version 300 es
precision mediump float;

uniform sampler2D Texture;

in vec2 v_fragTextureCoord;
out vec4 FragColor;

void main()
{
  FragColor = texture(Texture, v_fragTextureCoord);
  // FragColor = vec4(texture2D(Texture, v_fragTextureCoord).a);
}
