#version 310 es

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 MatModelView;
  uniform mat4 MatProj;
}
g_ubo;

layout(location = 0) in vec4 g_vPosition;
layout(location = 1) in vec2 g_vTexCoord;

layout(location = 0) out vec2 g_vVSTexCoord;

void main()
{
  vec4 vPositionES = g_ubo.MatModelView * g_vPosition;
  gl_Position = g_ubo.MatProj * vPositionES;
  g_vVSTexCoord = g_vTexCoord;
}
