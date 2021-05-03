attribute vec4 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;

uniform mat4 u_matMVP;

void main(void)
{
    v_texCoords = a_texCoords;
    gl_Position = u_matMVP * a_position;
}
