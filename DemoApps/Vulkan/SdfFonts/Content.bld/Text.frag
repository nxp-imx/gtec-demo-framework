#version 310 es
precision mediump float;

layout(binding = 0) uniform sampler2D Texture;

layout(location = 0) in vec2 v_fragTextureCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = texture(Texture, v_fragTextureCoord);
  // FragColor = vec4(texture2D(Texture, v_fragTextureCoord).a);
}
