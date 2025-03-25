#version 310 es
precision mediump float;

layout(binding = 0) uniform sampler2D Texture;

layout(push_constant) uniform LayerBlock
{
  uniform mat4 ModelViewProj;
  // 0.25f / (spread * scale)
  uniform float Smoothing;
  // Between 0 and spread / textureSize
  uniform float ShadowOffsetX;
  uniform float ShadowOffsetY;
  // Between 0 and 0.5
  uniform float ShadowSmoothing;
  // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
  uniform float OutlineDistance;
  uniform float ContourScale;
}
g_pushConstant;

layout(location = 0) in vec2 v_fragTextureCoord;
layout(location = 0) out vec4 FragColor;


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
  float distance = texture(Texture, v_fragTextureCoord).a;
  // FragColor = vec4(colorize(1.0 - (distance*2.0)), 1.0f);
  // FragColor = vec4(colorize((0.5 - distance) * 0.75), 1.0f);
  FragColor = vec4(colorize((0.5 - distance) * g_pushConstant.ContourScale), 1.0f);
}
