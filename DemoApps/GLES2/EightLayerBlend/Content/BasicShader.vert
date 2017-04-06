#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
uniform mat4 g_matModelViewProj;

attribute vec4 VertexPosition;
attribute vec2 VertexTextureCoord;

varying vec2 FragTextureCoord;

void main()
{
  gl_Position = g_matModelViewProj * VertexPosition;
  FragTextureCoord = VertexTextureCoord;
}
