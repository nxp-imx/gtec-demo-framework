#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec4 v_Color;

out vec4 o_fragColor;

void main()
{
  o_fragColor = v_Color;
}