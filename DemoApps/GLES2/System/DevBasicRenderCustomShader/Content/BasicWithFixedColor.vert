precision mediump float;
uniform mat4 MatModelViewProj;

attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec2 inVertexTextureCoord;

varying vec4 FragColor;
varying vec2 FragTextureCoord;

void main()
{
  gl_Position = MatModelViewProj * inVertexPosition;
  FragColor = inVertexColor * vec4(0.2, 0.2, 0.75, 1.0);
  FragTextureCoord = inVertexTextureCoord;
}
