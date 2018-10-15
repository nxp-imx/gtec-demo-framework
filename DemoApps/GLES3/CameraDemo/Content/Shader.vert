#version 300 es
uniform mat4 g_matModel;
uniform mat4 g_matView;
uniform mat4 g_matProj;

in vec3 g_vPosition;
in vec2 g_vTexCoord;

out vec2 g_vVSTexCoord;

void main()
{
  g_vVSTexCoord = g_vTexCoord;
  gl_Position = g_matProj * g_matView * g_matModel * vec4(g_vPosition,1.0);
}