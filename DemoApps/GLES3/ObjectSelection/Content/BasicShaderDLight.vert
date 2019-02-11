#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec3 v_Normal;
// out vec3 v_Eye;
out vec2 v_TexCoord;

void main()
{
  v_Normal = NormalMatrix * VertexNormal;
  //  v_Eye = (WorldView * vec4(VertexPosition, 1.0)).xyz;
  v_TexCoord = VertexTexCoord;
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
