#ifdef GL_FRAGMENT_PRECISION_HIGH
 precision highp float;
#else
 precision mediump float;
#endif

attribute vec4 vPosition;
attribute vec2 my_Texcoor;
uniform  mat4 u_TransMatrix;
uniform  vec4 sourceSize;
uniform  vec2 firstRed;
varying  vec4 center;
varying  vec4 xCoord;
varying  vec4 yCoord;
void main()
{
  center.xy = my_Texcoor.xy;
  center.zw = my_Texcoor.xy * sourceSize.xy + firstRed;
  vec2 invSize = sourceSize.zw ;
  xCoord = center.x + vec4(-2.0*invSize.x, -invSize.x, invSize.x, 2.0*invSize.x);
  yCoord = center.y + vec4(-2.0*invSize.y, -invSize.y, invSize.y, 2.0*invSize.y);
  gl_Position = u_TransMatrix*vPosition;
}

