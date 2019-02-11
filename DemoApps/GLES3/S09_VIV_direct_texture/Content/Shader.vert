#version 300 es

in vec4 my_Vertex;
uniform mat4 my_TransformMatrix;
in vec2 my_Texcoor;
out vec2 vTexcoor;

void main()
{
  vTexcoor = my_Texcoor;
  gl_Position = my_TransformMatrix * my_Vertex;
}
