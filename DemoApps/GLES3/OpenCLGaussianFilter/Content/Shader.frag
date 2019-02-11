#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D s_texture;

in vec2 g_vVSTexCoord;
out vec4 out_color;

void main()
{
  out_color = vec4(texture(s_texture, g_vVSTexCoord).a);
}
