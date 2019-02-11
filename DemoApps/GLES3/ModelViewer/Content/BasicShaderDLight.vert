#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 World;
uniform mat4 WorldViewProjection;

in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);

  v_Color = VertexColor;
  v_Normal = (World * vec4(VertexNormal, 1.0)).xyz;
  v_TexCoord = VertexTexCoord;
}
