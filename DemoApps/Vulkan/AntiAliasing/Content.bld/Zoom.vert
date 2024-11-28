#version 310 es
precision mediump float;

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 MatWorldViewProjection;
}
g_ubo;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

layout(location = 0) out vec2 v_texCoord;

void main()
{
  gl_Position = g_ubo.MatWorldViewProjection * vec4(VertexPosition, 1.0);
  v_texCoord = VertexTexCoord;
}
