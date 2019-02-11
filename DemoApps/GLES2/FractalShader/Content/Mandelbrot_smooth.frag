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


  float n = 0.0;
  for (int i = 0; i < MaxIterations; ++i)
  {
    v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);
    if (dot(v, v) > 1e4)
      break;
    ++n;
  }

  gl_FragColor = (0.5 * cos(vec4(3, 4, 11, 0) + 0.05 * (n - log2(log2(dot(v, v)))))) + 0.5;
}