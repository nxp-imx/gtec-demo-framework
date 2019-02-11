#version 300 es
in vec4 VertexPosition;
uniform mat4 mvp;

void main()
{
  gl_Position = mvp * VertexPosition;
}
