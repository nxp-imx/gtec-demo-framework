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

  // Gamma correction
  hdrColor = pow(hdrColor, vec3(1.0 / 2.2));

  // Clamp colors to 1.0 in bands to make it easy to verify if the display is showing real HDR
  // Here we emulate the 'blocks' on LDR displays by using a 0.8 instead of 1.0
  if (mod(floor(gl_FragCoord.y / 64.0), 2.0) == 0.0)
    hdrColor = clamp(hdrColor, vec3(0, 0, 0), vec3(0.8, 0.8, 0.8));

  FragColor = vec4(hdrColor, 1.0);
}
