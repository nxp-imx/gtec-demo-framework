#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec3 texCoord0;    // normal
varying vec3 texCoord1;    // view vector
uniform vec3 my_EyePos;
uniform samplerCube samplerCb;

void main(void)
{
  vec3 normal = normalize(texCoord0);
  vec3 viewVector = normalize(texCoord1);

  vec3 reflectionVector = 2.0 * dot(normal, viewVector) * normal - viewVector;

  /* sample using reflection vector */
  gl_FragColor = textureCube(samplerCb, reflectionVector);
}
