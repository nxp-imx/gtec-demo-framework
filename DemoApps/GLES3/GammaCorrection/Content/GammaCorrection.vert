#version 300 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

uniform mat4 g_matModelView;
uniform mat4 g_matProj;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
  v_Position = VertexPosition;
  v_Normal = VertexNormal;
  v_TexCoord = VertexTexCoord;
  gl_Position = g_matProj * g_matModelView * vec4(VertexPosition, 1.0);
}