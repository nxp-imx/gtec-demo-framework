#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;
uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform vec3 AmbientColor;

varying vec4 v_Color;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main()
{
  vec4 color = texture2D(Texture0, v_TexCoord) * v_Color;

  vec3 N = normalize(v_Normal);
  vec3 L = normalize(LightDirection);
  float lightAmount = max(dot(N, L), 0.0);
  vec3 lighting = AmbientColor + lightAmount * LightColor;
  color.xyz *= lighting;
  gl_FragColor = color;
}