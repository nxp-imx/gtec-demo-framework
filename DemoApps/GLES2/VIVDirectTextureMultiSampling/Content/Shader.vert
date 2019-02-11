attribute vec4 my_Vertex;
attribute vec2 my_Texcoor;
uniform mat4 my_TransMatrix;
varying vec2 vTexcoor;

void main()
{
  vTexcoor = my_Texcoor;

  gl_Position = my_TransMatrix * my_Vertex;
}