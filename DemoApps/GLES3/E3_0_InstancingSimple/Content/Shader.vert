#version 300 es

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in mat4 a_mvpMatrix;
out vec4 v_color;

void main()
{
  gl_Position = a_mvpMatrix * a_position;
  v_color = a_color;
}
