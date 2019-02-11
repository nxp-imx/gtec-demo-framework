#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 VertexPosition;
// in vec4 VertexColor;
in float VertexPointSize;

out block
{
  // vec4 Color;
  float Size;
}
Out;

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  // Out.Color = VertexColor;
  Out.Size = VertexPointSize;
}
