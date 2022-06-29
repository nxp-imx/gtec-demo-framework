#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;
uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform vec3 AmbientColor;

in vec4 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 o_fragColor;

void main()
{
  vec4 color = texture(Texture0, v_TexCoord) * v_Color;

  vec3 N = normalize(v_Normal);
  vec3 L = normalize(LightDirection);
  float lightAmount = max(dot(N, L), 0.0);
  vec3 lighting = AmbientColor + lightAmount * LightColor;
  color.xyz *= lighting;
  o_fragColor = color;
}