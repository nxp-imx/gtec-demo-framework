#version 300 es
precision lowp float;
#define MAX_LIGHTS 10
in mediump vec3 v_normal;
in mediump vec2 v_texcoord;
in mediump float v_layerDepth;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 LightDirection[MAX_LIGHTS];
uniform vec3 LightColor[MAX_LIGHTS];
uniform vec3 AmbientColor;
uniform float LightCount;

out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);
  vec4 furData = texture(Texture1, v_texcoord);
  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.

  float dist = mix(1.0, 0.2, v_layerDepth);
  float shadow = mix(0.4, 1.0, v_layerDepth);
  furColor *= shadow;
  float furVisibility = (v_layerDepth > furData.r) ? 0.0 : furData.a * dist;
  furColor.a = (v_layerDepth == 0.0) ? 1.0 : furVisibility;
  vec3 N = normalize(v_normal);

  vec3 lighting = AmbientColor;
  for (int i = 0; i < int(LightCount); ++i)
  {
    vec3 L = normalize(-LightDirection[i]);
    float lightAmount = max(dot(N, L), 0.0);
    lighting += lightAmount * LightColor[i];
  }

  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
