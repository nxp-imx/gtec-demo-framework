#version 300 es
precision mediump float;

uniform sampler2D my_Sampler;
in vec2 vTexcoor;
out vec4 out_color;

void main(void)
{
  vec4 tex = texture(my_Sampler, vTexcoor);
  out_color = tex;
}
