#version 460
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_EXT_shader_realtime_clock : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(push_constant) uniform LayerBlock
{
  vec2 CoordinateOffset;
  float CoordinateScale;
  float HeatmapScale;
  int MaxIterations;
}
g_pushConstant;

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_fragColor;

// This temperature function was inspired by the sample code found here:
// https://developer.nvidia.com/blog/profiling-dxr-shaders-with-timer-instrumentation/
vec3 Temperature(float t)
{
  vec3[10] c = vec3[10](vec3(0.0f / 255.0f, 2.0f / 255.0f, 91.0f / 255.0f), vec3(0.0f / 255.0f, 108.0f / 255.0f, 251.0f / 255.0f),
                        vec3(0.0f / 255.0f, 221.0f / 255.0f, 221.0f / 255.0f), vec3(51.0f / 255.0f, 221.0f / 255.0f, 0.0f / 255.0f),
                        vec3(255.0f / 255.0f, 252.0f / 255.0f, 0.0f / 255.0f), vec3(255.0f / 255.0f, 180.0f / 255.0f, 0.0f / 255.0f),
                        vec3(255.0f / 255.0f, 104.0f / 255.0f, 0.0f / 255.0f), vec3(226.0f / 255.0f, 22.0f / 255.0f, 0.0f / 255.0f),
                        vec3(191.0f / 255.0f, 0.0f / 255.0f, 83.0f / 255.0f), vec3(145.0f / 255.0f, 0.0f / 255.0f, 65.0f / 255.0f));

  float s = t * 10.0f;

  int cur = int(s) <= 9 ? int(s) : 9;
  int prv = cur >= 1 ? cur - 1 : 0;
  int nxt = cur < 9 ? cur + 1 : 9;

  const float blur = 0.8f;

  float wc = smoothstep(float(cur) - blur, float(cur) + blur, s) * (1.0f - smoothstep(float(cur + 1) - blur, float(cur + 1) + blur, s));
  float wp = 1.0f - smoothstep(float(cur) - blur, float(cur) + blur, s);
  float wn = smoothstep(float(cur + 1) - blur, float(cur + 1) + blur, s);

  vec3 r = wc * c[cur] + wp * c[prv] + wn * c[nxt];
  return vec3(clamp(r.x, 0.0f, 1.0f), clamp(r.y, 0.0f, 1.0f), clamp(r.z, 0.0f, 1.0f));
}

void main()
{
  vec2 c = (v_texcoord.xy * g_pushConstant.CoordinateScale) + g_pushConstant.CoordinateOffset;
  vec2 v = vec2(0.0);

  uint64_t timeStart = clockRealtimeEXT();

  int count = g_pushConstant.MaxIterations;
  float val = 0.0;
  for (int i = 0; i < g_pushConstant.MaxIterations; ++i)
  {
    v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);
    if (dot(v, v) > 4.0)
    {
      count = i;
      val = 1.0;
      break;
    }
  }

  uint64_t timeEnd = clockRealtimeEXT();
  uint64_t timeDiff = timeEnd >= timeStart ? (timeEnd - timeStart) : 0;
  float timeDiffScaled = clamp(float(timeDiff) / g_pushConstant.HeatmapScale, 0.0f, 1.0f);

  o_fragColor = vec4(Temperature(timeDiffScaled), 1.0);
}