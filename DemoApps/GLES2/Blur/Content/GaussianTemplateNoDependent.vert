#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

attribute vec3 VertexPosition;
attribute vec2 TexCoord;

varying vec2 v_texcoord[##BLUR_COUNT##];

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  ##REPLACE##
}
