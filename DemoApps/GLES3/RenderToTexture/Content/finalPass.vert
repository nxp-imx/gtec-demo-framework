#version 300 es

uniform mat4 g_matMVP;

in vec3 g_vPosition;
in vec2 g_vTexCoord;

out vec2 g_vVSTexCoord;

void main()
{
  g_vVSTexCoord = vec2(g_vTexCoord);
  gl_Position = g_matMVP * vec4(g_vPosition, 1.0);
}
