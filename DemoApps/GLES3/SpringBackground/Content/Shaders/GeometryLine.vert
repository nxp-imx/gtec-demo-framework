#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec4 VertexColor;

out block
{
  vec4 VertexColor;
}
Out;

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  Out.VertexColor = VertexColor;
}
