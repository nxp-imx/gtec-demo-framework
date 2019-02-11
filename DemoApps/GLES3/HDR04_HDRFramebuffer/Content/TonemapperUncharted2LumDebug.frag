#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec2 v_TexCoord;

// Tonemapping
uniform float g_exposure;

uniform sampler2D g_hdrTexture;

out vec4 FragColor;

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
  vec3 hdrColor = texture(g_hdrTexture, v_TexCoord).rgb;

  // Exposure
  hdrColor *= g_exposure;

  const float exposureBias = 2.0f;
  vec3 curr = Uncharted2Tonemap(exposureBias * hdrColor);

  const float W = 11.2;
  vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
  hdrColor = curr * whiteScale;

  // Linear framebuffer so we dont need gamma correction.
  // hdrColor = pow(hdrColor, vec3(1.0 / 2.2));

  if (mod(floor(gl_FragCoord.y / 64.0), 2.0) == 0.0)
    hdrColor = clamp(hdrColor, vec3(0, 0, 0), vec3(1.0, 1.0, 1.0));

  FragColor = vec4(hdrColor, 1.0);
}
