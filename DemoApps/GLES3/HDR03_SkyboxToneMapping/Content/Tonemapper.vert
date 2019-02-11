#version 300 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

out vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  gl_Position = vec4(VertexPosition, 1.0);
}