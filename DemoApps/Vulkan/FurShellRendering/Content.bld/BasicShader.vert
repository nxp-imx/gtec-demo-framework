#version 310 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  v_TexCoord = VertexTexCoord;
}
