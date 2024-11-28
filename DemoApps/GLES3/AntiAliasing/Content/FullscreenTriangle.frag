#version 300 es
precision mediump float;

in vec2 v_TexCoord;
uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  // Sample the texture at the given coordinate
  fragColor = texture(u_texture, v_TexCoord);
}