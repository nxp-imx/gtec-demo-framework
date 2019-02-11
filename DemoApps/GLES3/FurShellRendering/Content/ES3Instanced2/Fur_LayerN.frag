#version 300 es
precision lowp float;

in mediump vec3 v_normal;
in mediump vec2 v_texcoord;
in mediump float v_instanceId;
in mediump float v_dist;
in mediump float v_shadow;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 LightDirection1;
uniform vec3 LightColor1;
uniform vec3 AmbientColor;

out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);
  vec4 furData = texture(Texture1, v_texcoord);

  furColor *= v_shadow;
  furColor.a = (v_instanceId <= furData.r) ? furData.a * v_dist : 0.0;

  float lightAmount = max(dot(v_normal, -LightDirection1), 0.0);
  vec3 lighting = AmbientColor + lightAmount * LightColor1;
  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
