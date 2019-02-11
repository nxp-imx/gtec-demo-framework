#version 300 es
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexTexCoord;

uniform mat4 g_view;
uniform mat4 g_projection;

out vec3 v_texCoords;

void main()
{
  v_texCoords = VertexTexCoord;
  gl_Position = g_projection * g_view * vec4(VertexPosition, 1.0);
}