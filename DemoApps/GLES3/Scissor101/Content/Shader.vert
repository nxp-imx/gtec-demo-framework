#version 300 es
uniform mat4 g_matModelView;
uniform mat4 g_matProj;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

out vec2 v_TexCoord;

void main()
{
  gl_Position = g_matProj * g_matModelView * vec4(VertexPosition, 1.0);
  v_TexCoord = VertexTexCoord;
}
