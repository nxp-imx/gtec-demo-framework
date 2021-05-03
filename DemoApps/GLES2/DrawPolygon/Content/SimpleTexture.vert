uniform mat4 u_matMVP;

attribute vec4 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;

void main()
{
  v_texCoords = a_texCoords;
  gl_Position = u_matMVP * a_position;
}
