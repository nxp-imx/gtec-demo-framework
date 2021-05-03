uniform mat4 u_matMVP;

attribute vec4 a_Position;

void main()
{
  gl_Position = u_matMVP * a_Position;
}
