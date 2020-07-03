#version 300 es
uniform mat4 g_matMVP;

in vec4 g_vPosition;
in vec3 g_vColor;

out vec3 g_vVSColor;

void main()
{
  gl_Position = g_matMVP * g_vPosition;
  g_vVSColor = g_vColor;
}
