#version 300 es
precision mediump float;

uniform sampler2D Texture;

in vec4 FragColor;
in vec2 FragTextureCoord;

out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(Texture, FragTextureCoord) * FragColor * vec4(0.75, 0.2, 0.2, 1.0);
}
