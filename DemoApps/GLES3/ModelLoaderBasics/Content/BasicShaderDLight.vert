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
in vec4 VertexColor;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
  v_Color = VertexColor;
  v_Normal = NormalMatrix * VertexNormal;
  v_TexCoord = VertexTexCoord;
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
