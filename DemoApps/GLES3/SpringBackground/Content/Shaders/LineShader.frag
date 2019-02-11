#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec4 AmbientColor;

out vec4 o_fragColor;

void main()
{
  o_fragColor = AmbientColor;
}
