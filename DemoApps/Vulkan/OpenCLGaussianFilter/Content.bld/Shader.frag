#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(binding = 1) uniform sampler2D s_texture;

layout(location = 0) in vec2 g_vVSTexCoord;
layout(location = 0) out vec4 out_color;

void main()
{
  out_color = vec4(texture(s_texture, g_vVSTexCoord).a);
}
