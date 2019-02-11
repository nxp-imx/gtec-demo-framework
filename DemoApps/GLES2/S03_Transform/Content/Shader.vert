attribute vec4 my_Vertex;
attribute vec4 my_Color;
uniform mat4 my_TransformMatrix;
varying vec4 color;

void main()
{
  color = my_Color;
  gl_Position = my_TransformMatrix * my_Vertex;
}
