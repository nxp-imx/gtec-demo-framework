#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec2 Coordinate;

varying vec2 v_texcoord;

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

  float co = float(count) / float(MaxIterations);
  gl_FragColor = vec4(co, co, co, 1.0);
}