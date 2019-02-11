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
  float val = 0.0;
  for (int i = 0; i < MaxIterations; ++i)
  {
    v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);
    if (dot(v, v) > 4.0)
    {
      count = i;
      val = 1.0;
      break;
    }
  }

  float co = float(count);
  vec3 col = (0.5 + 0.5 * cos(3.0 + co * 0.15 + vec3(0.0, 0.6, 1.0))) * val;
  gl_FragColor = vec4(col, 1.0);
}