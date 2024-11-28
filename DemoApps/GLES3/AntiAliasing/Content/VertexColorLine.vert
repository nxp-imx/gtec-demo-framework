#version 300 es
precision mediump float;

uniform mat4 WorldViewProjection;

in vec3 VertexPosition;
in vec4 VertexColor;

out vec4 v_VertexColor;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
  v_VertexColor = VertexColor;
}
