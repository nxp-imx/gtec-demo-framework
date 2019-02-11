#version 300 es

in vec4 my_Vertex;
uniform mat4 my_TransformMatrix;
uniform float my_Radius;
uniform vec3 my_EyePos;
out vec3 texCoord0;
out vec3 texCoord1;

void main()
{
  /* Get actual dimensions of the sphere */
  vec4 temp = (vec4(my_Radius, my_Radius, my_Radius, 1) * my_Vertex);
  /* pass the normalized normal vector */
  texCoord0 = normalize(my_Vertex.xyz);
  /* pass the normalized view vector */
  texCoord1 = my_EyePos - temp.xyz;
  /* rotate, project the sphere vertices */
  gl_Position = my_TransformMatrix * temp;
}
