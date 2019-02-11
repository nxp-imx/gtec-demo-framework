#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldView;
uniform mat4 Projection;

in vec3 VertexPosition;
in vec4 VertexColor;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
  vec4 position = WorldView * vec4(VertexPosition, 1.0);
  gl_Position = Projection * position;
  v_Color = VertexColor;
  v_TexCoord = VertexTexCoord;
}
