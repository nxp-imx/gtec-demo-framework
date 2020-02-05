#version 300 es
precision lowp float;

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

// Local space
in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;

out vec4 v_Color;

void main()
{
  v_Color = VertexColor;

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
