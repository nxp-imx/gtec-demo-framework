#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;
uniform mat4 WorldView;
uniform mat4 Projection;

in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
  // Extract the view direction from the WorldView matrix
  vec3 viewDirection = vec3(WorldView[0][2], WorldView[1][2], WorldView[2][2]);
  vec3 rightVector = normalize(vec3(WorldView[0][0], WorldView[1][0], WorldView[2][0]));
  vec3 upVector = normalize(vec3(WorldView[0][1], WorldView[1][1], WorldView[2][1]));

  // Calculate the position of this billboard vertex.
  vec3 position = VertexPosition;

  // Offset to the left or right.
  position += rightVector * VertexNormal.x * VertexNormal.z;

  // Offset upward if we are one of the top two vertices.
  position += upVector * VertexNormal.y * VertexNormal.z;

  gl_Position = WorldViewProjection * vec4(position, 1.0);
  v_Color = VertexColor;
  v_TexCoord = VertexTexCoord;
}
