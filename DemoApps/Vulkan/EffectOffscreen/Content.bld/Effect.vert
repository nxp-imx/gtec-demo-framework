#version 310 es

precision mediump float;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord0;
layout(location = 2) in vec2 VertexTexCoord1;

layout(location = 0) out vec2 v_TexCoord0;
layout(location = 1) out vec2 v_TexCoord1;

void main()
{
  v_TexCoord0 = VertexTexCoord0;
  v_TexCoord1 = VertexTexCoord1;
  gl_Position = vec4(VertexPosition, 1.0);
}