#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D my_Texture0;
uniform sampler2D my_Texture1;
varying vec2 vTexcoor;

void main()
{
  float Y = texture2D(my_Texture0, vTexcoor).a;
  float U = texture2D(my_Texture1, vTexcoor).a;
  float V = texture2D(my_Texture1, vTexcoor).z;

  float R = Y + 1.140 * (V - 0.5);
  float G = Y - 0.344 * (U - 0.5) - 0.714 * (V - 0.5);
  float B = Y + 1.772 * (U - 0.5);

  gl_FragColor = vec4(R, G, B, 1.0);
}