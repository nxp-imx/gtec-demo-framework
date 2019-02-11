#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;
uniform float ResolutionMod;

in vec3 VertexPosition;
// in vec4 VertexColor;
in float VertexPointSize;

// out vec4 v_Color;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
  // v_Color = VertexColor;
  // Adjust the particles size according to basic perspective
  gl_PointSize = (((VertexPointSize * 500.0 * ResolutionMod) / gl_Position.w) * ResolutionMod);
}
