#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  // Tonemapping
  uniform float Exposure;
}
g_ubo;

mediump layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput g_hdrColorAttachment;

layout(location = 0) out vec4 FragColor;

vec3 Uncharted2Tonemap(vec3 x)
{
  const float A = 0.15;
  const float B = 0.50;
  const float C = 0.10;
  const float D = 0.20;
  const float E = 0.02;
  const float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

void main()
{
  vec3 hdrColor = subpassLoad(g_hdrColorAttachment).rgb;

  // Exposure
  hdrColor *= g_ubo.Exposure;

  const float exposureBias = 2.0f;
  vec3 curr = Uncharted2Tonemap(exposureBias * hdrColor);

  const float W = 11.2;
  vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
  hdrColor = curr * whiteScale;

  // Gamma correction
  hdrColor = pow(hdrColor, vec3(1.0 / 2.2));
  FragColor = vec4(hdrColor, 1.0);
}
