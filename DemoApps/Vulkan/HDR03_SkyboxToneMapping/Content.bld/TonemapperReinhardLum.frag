#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(location = 0) in vec2 v_TexCoord;

layout(std140, binding = 0) uniform UBO
{
  // Tonemapping
  uniform float Exposure;
}
g_ubo;

layout(binding = 1) uniform sampler2D g_hdrTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
  vec3 hdrColor = texture(g_hdrTexture, v_TexCoord).rgb;

  // Exposure
  hdrColor *= g_ubo.Exposure;

  // Simple reinhard
  hdrColor = hdrColor / (1.0f + hdrColor);

  // Gamma correction
  hdrColor = pow(hdrColor, vec3(1.0 / 2.2));
  FragColor = vec4(hdrColor, 1.0);
}
