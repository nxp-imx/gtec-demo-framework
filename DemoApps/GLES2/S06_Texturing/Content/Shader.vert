uniform mat4 g_matModelView;
uniform mat4 g_matProj;

attribute vec4 g_vPosition;
attribute vec3 g_vColor;
attribute vec2 g_vTexCoord;

varying vec3 g_vVSColor;
varying vec2 g_vVSTexCoord;

void main()
{
  vec4 vPositionES = g_matModelView * g_vPosition;
  gl_Position = g_matProj * vPositionES;
  g_vVSColor = g_vColor;
  g_vVSTexCoord = g_vTexCoord;
}
