precision highp float;
varying mediump vec3 v_normal;
varying mediump vec2 v_texcoord;
uniform float CurrentLayer;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 LightDirection1;
uniform vec3 LightColor1;
uniform vec3 LightDirection2;
uniform vec3 LightColor2;
uniform vec3 LightDirection3;
uniform vec3 LightColor3;
uniform vec3 LightDirection4;
uniform vec3 LightColor4;
uniform vec3 AmbientColor;

void main()
{
  vec4 furColor = texture2D(Texture0, v_texcoord);
  vec4 furData = texture2D(Texture1, v_texcoord);
  // based on layer depth, choose the amount of shading.
  // we lerp between two values to avoid having the base of the fur pure black.

  float dist = mix(1.0, 0.2, CurrentLayer);
  float shadow = mix(0.4, 1.0, CurrentLayer);
  furColor *= shadow;
  float furVisibility = (CurrentLayer > furData.r) ? 0.0 : furData.a * dist;
  furColor.a = (CurrentLayer == 0.0) ? 1.0 : furVisibility;
  vec3 N = normalize(v_normal);
  vec3 L1 = normalize(-LightDirection1);
  vec3 L2 = normalize(-LightDirection2);
  vec3 L3 = normalize(-LightDirection3);
  vec3 L4 = normalize(-LightDirection4);
  float lightAmount1 = max(dot(N, L1), 0.0);
  float lightAmount2 = max(dot(N, L2), 0.0);
  float lightAmount3 = max(dot(N, L3), 0.0);
  float lightAmount4 = max(dot(N, L4), 0.0);
  vec3 lighting =
    AmbientColor + (lightAmount1 * LightColor1) + (lightAmount2 * LightColor2) + (lightAmount3 * LightColor3) + (lightAmount4 * LightColor4);
  furColor.xyz *= lighting;
  gl_FragColor = furColor;
}
