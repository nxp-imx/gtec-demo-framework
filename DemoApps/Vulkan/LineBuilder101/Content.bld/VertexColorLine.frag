#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(location = 0) in vec4 v_VertexColor;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  o_fragColor = v_VertexColor;
}