#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D s_texture;

in vec2 v_texcoord;

out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(s_texture, v_texcoord);
}