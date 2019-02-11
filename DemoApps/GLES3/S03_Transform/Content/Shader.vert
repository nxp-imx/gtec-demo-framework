#version 300 es

in vec4 my_Vertex;
in vec4 my_Color;
uniform mat4 my_TransformMatrix;
out vec4 color;

void main()
{
  color = my_Color;
  gl_Position = my_TransformMatrix * my_Vertex;
}
