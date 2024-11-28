#version 310 es
precision mediump float;

layout(binding = 1) uniform sampler2D u_texture;

layout(location = 0) in vec2 v_texCoord;
layout(location = 0) out vec4 out_color;

void main()
{
  out_color = texture(u_texture, v_texCoord);
}
