#version 300 es
precision mediump float;

in vec3 texCoord0;    // normal
in vec3 texCoord1;    // view vector
uniform vec3 my_EyePos;
uniform samplerCube samplerCb;
out vec4 out_color;

void main(void)
{
  vec3 normal = normalize(texCoord0);
  vec3 viewVector = normalize(texCoord1);
  vec3 reflectionVector = 2.0 * dot(normal, viewVector) * normal - viewVector;
  /* sample using reflection vector */
  out_color = texture(samplerCb, reflectionVector);
}
