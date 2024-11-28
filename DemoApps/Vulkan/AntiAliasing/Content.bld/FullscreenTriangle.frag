#version 310 es
precision mediump float;

layout(binding = 1) uniform sampler2D u_texture;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 0) out vec4 fragColor;

void main()
{
  // Sample the texture at the given coordinate
  fragColor = texture(u_texture, v_TexCoord);
}