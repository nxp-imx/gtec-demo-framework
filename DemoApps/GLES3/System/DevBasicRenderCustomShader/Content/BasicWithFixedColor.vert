#version 300 es
precision mediump float;
uniform mat4 MatModelViewProj;

layout(location = 0) in vec4 inVertexPosition;
layout(location = 1) in vec4 inVertexColor;
layout(location = 2) in vec2 inVertexTextureCoord;

out vec4 FragColor;
out vec2 FragTextureCoord;

void main()
{
  gl_Position = MatModelViewProj * inVertexPosition;
  FragColor = inVertexColor * vec4(0.2, 0.2, 0.75, 1.0);
  FragTextureCoord = inVertexTextureCoord;
}
