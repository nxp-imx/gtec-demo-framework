precision mediump float;

uniform sampler2D Texture;

// 0.25f / (spread * scale)
uniform float g_smoothing;
uniform float g_contourScale;

varying vec2 v_fragTextureCoord;

const vec4 FontColor = vec4(1.0);

// sdf colors from Inigo Quilez (e.g. https://www.shadertoy.com/view/4llXD7) (MIT license)
vec3 colorize(float d)
{
  vec3 col = (d > 0.0) ? vec3(0.9, 0.6, 0.3) : vec3(0.65, 0.85, 1.0);
  col *= 1.0 - exp(-6.0 * abs(d));
  col *= 0.8 + 0.2 * cos(150.0 * d);
  return mix(col, vec3(1.0), 1.0 - smoothstep(0.0, 0.01, abs(d)));
}

void main()
{
  vec3 msd = texture2D(Texture, v_fragTextureCoord).rgb;
  // Calculate the median
  float distance = max(min(msd.r, msd.g), min(max(msd.r, msd.g), msd.b));

  gl_FragColor = vec4(colorize((0.5 - distance) * g_contourScale), 1.0);
}
