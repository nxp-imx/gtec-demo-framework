#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D my_Texture0;
uniform sampler2D my_Texture1;
in vec2 vTexcoor;
out vec4 out_color;

void main()
{
  float Y = texture(my_Texture0, vTexcoor).a;
  float U = texture(my_Texture1, vTexcoor).z;
  float V = texture(my_Texture1, vTexcoor).a;

  float R = Y + 1.140 * (V - 0.5);
  float G = Y - 0.395 * (U - 0.5) - 0.581 * (V - 0.5);
  float B = Y + 2.032 * (U - 0.5);

  out_color = vec4(R, G, B, 1.0);
}