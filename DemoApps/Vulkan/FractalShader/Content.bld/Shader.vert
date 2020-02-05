#version 310 es

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 v_texcoord;

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  v_texcoord = TexCoord;
}
