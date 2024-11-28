#version 310 es
precision mediump float;

layout(location = 0) in vec4 v_VertexColor;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  o_fragColor = v_VertexColor;
}