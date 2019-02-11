#version 300 es
precision highp float;

in vec3 texCoord0;    // normal
in vec3 texCoord1;    // view vector (eye - pixel position)
uniform vec3 my_EyePos;
uniform samplerCube samplerCb;
out vec4 out_color;

#define f 1.06

void main(void)
{
  vec3 N = normalize(texCoord0);
  vec3 P = my_EyePos - texCoord1;
  vec3 V = normalize(texCoord1);

  float NdotV = dot(N, V);

  // R = 2N * (N . V) - V
  vec3 reflectionVector = 2.0 * N * NdotV - V;
  // sample reflection
  vec4 reflectionColor = texture(samplerCb, reflectionVector);

  // compute refraction vector
  // T =  ( f * ( N . V ) - sqrt ( 1 - f * f * ( 1 - (N . V) * (N . V) ) ) ) * N - f * V
  vec3 T = (f * NdotV - sqrt(abs(1.0 - (f * f * (1.0 - NdotV * NdotV))))) * N - f * V;

  // T = (l, m, n)
  // P = (a, b, c)
  // solve the quadraic equation: A * x^2 + B * x + C = 0
  //		A = (l^2 + m^2 + n^2) = 1.0f
  //		B = 2 * l * a + 2 * m * b + 2 * n * c = 2 * (T . P)
  float Tw = 2.0 * dot(T, P);
  //		C = a^2 + b^2 + c^2 = 0.0f

  // T' = P - T * x
  vec3 Tr = P - Tw * T;

  // do cheap second refraction
  // R = scale * (2T * (T . T') - T') + T
  vec3 R = 0.02 * (2.0 * dot(T, Tr) * T - Tr) + T;

  // sample refraction
  vec4 refractionColor = texture(samplerCb, R);

  // color = reflection * ratio + refraction
  out_color = reflectionColor * 0.5 + refractionColor;
}
