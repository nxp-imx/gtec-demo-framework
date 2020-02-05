#version 300 es

precision lowp float;

in vec4 v_Color;
out vec4 o_fragColor;

void main()
{
  o_fragColor = v_Color;
}
