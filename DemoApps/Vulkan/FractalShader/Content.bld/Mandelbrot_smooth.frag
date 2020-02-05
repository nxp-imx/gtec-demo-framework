#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  uniform vec2 CoordinateOffset;
  uniform float CoordinateScale;
  uniform int MaxIterations;
}
g_pushConstant;

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  vec2 c = (v_texcoord.xy * g_pushConstant.CoordinateScale) + g_pushConstant.CoordinateOffset;
  vec2 v = vec2(0.0);


  float n = 0.0;
  for (int i = 0; i < g_pushConstant.MaxIterations; ++i)
  {
    v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);
    if (dot(v, v) > 1e4)
      break;
    ++n;
  }

  o_fragColor = (0.5 * cos(vec4(3, 4, 11, 0) + 0.05 * (n - log2(log2(dot(v, v)))))) + 0.5;
}