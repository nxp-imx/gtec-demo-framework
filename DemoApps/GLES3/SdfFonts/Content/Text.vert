#version 300 es
precision mediump float;
uniform mat4 g_matModelViewProj;

in vec4 VertexPosition;
in vec2 VertexTextureCoord;

out vec2 v_fragTextureCoord;

void main()
{
  gl_Position = g_matModelViewProj * VertexPosition;
  v_fragTextureCoord = VertexTextureCoord;
}
