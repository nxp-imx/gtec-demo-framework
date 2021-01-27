#version 310 es

layout(binding = 0) uniform UBO
{
  mat4 matModelView;
  mat4 matProj;
}
ubo;


layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
  gl_Position = ubo.matProj * ubo.matModelView * vec4(VertexPosition, 1.0);
  v_TexCoord = VertexTexCoord;
}
