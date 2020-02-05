#version 310 es

precision mediump float;

layout(binding = 1) uniform sampler2D s_texture;

layout(location = 0) in vec2 v_TexCoord0;
// layout(location = 1) in vec2 v_TexCoord1;

layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = texture(s_texture, v_TexCoord0);
}