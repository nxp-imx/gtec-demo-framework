#version 310 es

layout(location = 0) in vec4 VertexPosition;

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 MatViewProj;
}
g_ubo;

void main()
{
  gl_Position = g_ubo.MatViewProj * VertexPosition;
}
