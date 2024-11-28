#version 300 es
uniform mat4 WorldViewProjection;

in vec3 VertexPosition;
in vec2 VertexTexCoord;

out vec2 v_texCoord;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
  v_texCoord = VertexTexCoord;
}
