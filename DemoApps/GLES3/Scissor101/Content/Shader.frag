#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D s_texture;
in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
  FragColor = texture(s_texture, v_TexCoord);
}
