#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;

in vec3 VertexPosition;
in vec2 VertexTexCoord;

out vec2 v_TexCoord;

void main()
{
  v_TexCoord = VertexTexCoord;
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
