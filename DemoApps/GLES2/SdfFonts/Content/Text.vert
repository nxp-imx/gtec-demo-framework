precision mediump float;
uniform mat4 g_matModelViewProj;

attribute vec4 VertexPosition;
attribute vec2 VertexTextureCoord;

varying vec2 v_fragTextureCoord;

void main()
{
  gl_Position = g_matModelViewProj * VertexPosition;
  v_fragTextureCoord = VertexTextureCoord;
}
