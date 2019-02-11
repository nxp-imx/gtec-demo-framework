#version 300 es

uniform mat4 u_modelViewMatrix;
uniform mat4 u_modelViewProjectionMatrix;
uniform mat3 u_normalMatrix;

in vec3 g_vinPosition;
in vec3 g_vinNormal;
in vec2 g_vinTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 fragmentPosition;


void main()
{
  texCoord = vec2(g_vinTexCoord);
  normal = normalize(u_normalMatrix * g_vinNormal);
  fragmentPosition = vec3(u_modelViewMatrix * vec4(g_vinPosition, 1.0));
  gl_Position = u_modelViewProjectionMatrix * vec4(g_vinPosition, 1.0);
}
