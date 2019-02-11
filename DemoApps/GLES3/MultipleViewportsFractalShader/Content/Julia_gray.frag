#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec2 Coordinate;

in vec2 v_texcoord;

out vec4 o_fragColor;

const int MaxIterations = ##MAX_ITERATIONS##;

void main()
{
  vec2 c = Coordinate;
  vec2 v = v_texcoord.xy;

  int count = MaxIterations;
  for (int i = 0; i < MaxIterations; ++i)
  {
    v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);
    if (dot(v, v) > 4.0)
    {
      count = i;
      break;
    }
  }

  float co = float(MaxIterations - count) / float(MaxIterations);
  o_fragColor = vec4(co, co, co, 1.0);
}