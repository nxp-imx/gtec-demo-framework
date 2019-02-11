#version 300 es
uniform mat4 g_matModelView;
uniform mat4 g_matProj;

in vec4 g_vPosition;
in vec2 g_vTexCoord;

out vec2 g_vVSTexCoord;

void main()
{
  vec4 vPositionES = g_matModelView * g_vPosition;
  gl_Position = g_matProj * vPositionES;
  g_vVSTexCoord = g_vTexCoord;
}
