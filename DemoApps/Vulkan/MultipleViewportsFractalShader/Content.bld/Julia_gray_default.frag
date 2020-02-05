#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  uniform vec2 Coordinate;
}
g_pushConstant;

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_fragColor;

// Currently set to match the GLES samples default value
const int MaxIterations = 20;

void main()
{
  vec2 c = g_pushConstant.Coordinate;
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