#version 310 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexTexCoord;

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 MatView;
  uniform mat4 MatProj;
}
g_ubo;

layout(location = 0) out vec3 v_texCoords;

void main()
{
  v_texCoords = VertexTexCoord;
  gl_Position = g_ubo.MatProj * g_ubo.MatView * vec4(VertexPosition, 1.0);
}