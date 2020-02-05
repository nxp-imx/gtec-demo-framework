uniform mat4 g_matModelView;
uniform mat4 g_matProj;

attribute vec4 g_vPosition;

void main()
{
  vec4 vPositionES = g_matModelView * g_vPosition;
  gl_Position = g_matProj * vPositionES;
}
