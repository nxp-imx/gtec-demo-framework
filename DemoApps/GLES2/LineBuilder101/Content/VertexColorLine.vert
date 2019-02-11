#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;

attribute vec3 VertexPosition;
attribute vec4 VertexColor;

varying vec4 v_VertexColor;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
  v_VertexColor = VertexColor;
}
