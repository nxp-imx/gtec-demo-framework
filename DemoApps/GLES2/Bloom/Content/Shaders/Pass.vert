#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


// Local space
attribute vec3 VertexPosition;
attribute vec2 VertexTexCoord;

varying vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = vec4(VertexPosition, 1.0);
}
