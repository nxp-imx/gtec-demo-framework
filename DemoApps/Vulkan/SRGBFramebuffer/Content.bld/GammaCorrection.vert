#version 310 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 MatModelView;
  uniform mat4 MatProj;
}
g_ubo;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
  v_Position = VertexPosition;
  v_Normal = VertexNormal;
  v_TexCoord = VertexTexCoord;
  gl_Position = g_ubo.MatProj * g_ubo.MatModelView * vec4(VertexPosition, 1.0);
}