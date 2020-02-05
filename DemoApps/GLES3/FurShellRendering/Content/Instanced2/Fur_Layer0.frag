#version 300 es
precision lowp float;

in mediump vec3 v_normal;
in mediump vec2 v_texcoord;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 LightDirection1;
uniform vec3 LightColor1;
uniform vec3 AmbientColor;

out vec4 o_fragColor;

void main()
{
  vec4 furColor = texture(Texture0, v_texcoord);

  furColor *= 0.4;

  float lightAmount = max(dot(v_normal, -LightDirection1), 0.0);
  vec3 lighting = AmbientColor + lightAmount * LightColor1;
  furColor.xyz *= lighting;
  o_fragColor = furColor;
}
