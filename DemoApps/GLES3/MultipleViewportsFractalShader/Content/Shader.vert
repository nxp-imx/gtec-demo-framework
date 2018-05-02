#version 300 es

in vec3 VertexPosition;
in vec2 TexCoord;

out vec2 v_texcoord;

void main()
{
  gl_Position = vec4(VertexPosition, 1.0);
  v_texcoord = TexCoord;
}
