#version 300 es
uniform mat4 g_matModelView;
uniform mat4 g_matProj;

in vec4 g_vPosition;
in vec3 g_vColor;
in vec2 g_vTexCoord;

out vec3 g_vVSColor;
out vec2 g_vVSTexCoord;

void main()
{
  vec4 vPositionES = g_matModelView * g_vPosition;
  gl_Position = g_matProj * vPositionES;
  g_vVSColor = g_vColor;
  g_vVSTexCoord = g_vTexCoord;
}
