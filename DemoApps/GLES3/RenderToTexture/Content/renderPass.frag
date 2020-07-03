#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 g_vVSColor;
out vec4 out_color;

void main()
{
  out_color = vec4(g_vVSColor, 1.0);
}
