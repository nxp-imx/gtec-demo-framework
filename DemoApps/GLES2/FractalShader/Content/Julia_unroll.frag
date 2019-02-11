#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec2 Coordinate;

varying vec2 v_texcoord;

void main()
{
  vec2 c = Coordinate;
  vec2 v = v_texcoord.xy;

  ##REPLACE##
}