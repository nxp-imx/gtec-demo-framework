#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec2 Coordinate;

in vec2 v_texcoord;

out vec4 o_fragColor;

void main()
{
  vec2 c = Coordinate;
  vec2 v = v_texcoord.xy;

  ##REPLACE##
}
