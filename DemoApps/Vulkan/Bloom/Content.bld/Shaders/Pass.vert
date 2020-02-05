#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


// Local space
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = vec4(VertexPosition, 1.0);
}
