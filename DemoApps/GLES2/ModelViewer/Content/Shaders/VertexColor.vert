precision lowp float;

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

// Local space
attribute vec3 VertexPosition;
attribute vec4 VertexColor;
attribute vec3 VertexNormal;

varying vec4 v_Color;

void main()
{
  v_Color = VertexColor;

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
