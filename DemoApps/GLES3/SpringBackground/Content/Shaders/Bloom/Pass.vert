#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


// Local space
in vec3 VertexPosition;
in vec2 VertexTexCoord;

out vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = vec4(VertexPosition, 1.0);
}
