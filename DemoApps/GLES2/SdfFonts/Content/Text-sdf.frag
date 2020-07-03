precision mediump float;

uniform sampler2D Texture;

// 0.25f / (spread * scale)
uniform float g_smoothing;

varying vec2 v_fragTextureCoord;

const vec4 FontColor = vec4(1.0);
void main()
{
  float distance = texture2D(Texture, v_fragTextureCoord).a;
  float alpha = smoothstep(0.5 - g_smoothing, 0.5 + g_smoothing, distance);
  gl_FragColor = vec4(FontColor.rgb, FontColor.a * alpha);
}
