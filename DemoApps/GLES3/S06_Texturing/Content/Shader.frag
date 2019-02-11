#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D s_texture;
in vec3 g_vVSColor;
in vec2 g_vVSTexCoord;
out vec4 out_color;

void main()
{
  out_color = texture(s_texture, g_vVSTexCoord);
}
