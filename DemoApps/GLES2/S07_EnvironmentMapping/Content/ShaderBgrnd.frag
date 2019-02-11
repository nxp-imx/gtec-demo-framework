#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec3 texCoord0;
varying vec3 texCoord1;    // view vector (eye - pixel position)
uniform samplerCube samplerCb;

void main(void)
{
  gl_FragColor = textureCube(samplerCb, texCoord0);
}
