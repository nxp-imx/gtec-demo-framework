#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform vec2 CoordinateOffset;
uniform float CoordinateScale;

varying vec2 v_texcoord;

const int MaxIterations = ##MAX_ITERATIONS##;

void main()
{
  vec2 c = (v_texcoord.xy * CoordinateScale) + CoordinateOffset;
  vec2 v = vec2(0.0);

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
  gl_FragColor = vec4(co, co, co, 1.0);
}
