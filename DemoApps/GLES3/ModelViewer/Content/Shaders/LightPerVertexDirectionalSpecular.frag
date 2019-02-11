#version 300 es
// BEWARE: This is a example shader and it has not been optimized for speed.


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
