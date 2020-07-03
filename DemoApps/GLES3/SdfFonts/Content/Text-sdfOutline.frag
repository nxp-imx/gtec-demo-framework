#version 300 es
precision mediump float;

uniform sampler2D Texture;

// 0.25f / (spread * scale)
uniform float g_smoothing;
// Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
uniform float g_outlineDistance;

in vec2 v_fragTextureCoord;
out vec4 FragColor;

const vec4 OutlineColor = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 FontColor = vec4(1.0);

void main()
{
  float distance = texture(Texture, v_fragTextureCoord).a;
  float outlineFactor = smoothstep(0.5 - g_smoothing, 0.5 + g_smoothing, distance);
  vec4 color = mix(OutlineColor, FontColor, outlineFactor);
  float alpha = smoothstep(g_outlineDistance - g_smoothing, g_outlineDistance + g_smoothing, distance);
  FragColor = vec4(color.rgb, color.a * alpha);
}
