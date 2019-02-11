#version 300 es
//#ifdef GL_FRAGMENT_PRECISION_HIGH
//  precision highp float;
//#else
precision mediump float;
//#endif

uniform int texCount;
uniform sampler2D texUnit1;
uniform sampler2D texUnit2;
uniform vec4 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;
uniform float mixFactor;

in vec3 normal;
in vec2 texCoord;
in vec3 fragmentPosition;

out vec4 outputColor;

vec3 ads()
{
  vec3 s = normalize(vec3(lightPosition) - fragmentPosition);
  vec3 v = normalize(vec3(-fragmentPosition));
  vec3 h = normalize(v + s);
  return lightIntensity * (Ka + (Kd * max(dot(s, normal), 0.0)) + (Ks * pow(max(dot(h, normal), 0.0), shininess)));
}

void main()
{
  vec3 color1;
  color1 = vec3(texture(texUnit1, texCoord));
  vec3 color2;
  color2 = vec3(texture(texUnit2, texCoord));
  outputColor = vec4(ads() * (mix(color1, color2, mixFactor)), 1.0f);
}
