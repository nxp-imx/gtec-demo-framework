#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec4 VertexColor;

out vec4 v_Color;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
  v_Color = VertexColor;
}
