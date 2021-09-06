#version 450 core

layout(location = 0) in vec3 a_vertex;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  gl_Position = vec4(a_vertex, 1);
}
