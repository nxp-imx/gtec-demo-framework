#version 310 es
precision lowp float;

layout(std140, binding = 0) uniform UBO
{
  uniform mat4 WorldView;
  uniform mat4 WorldViewProjection;
  uniform mat4 NormalMatrix;    // Consider mat4x3
  // Camera space
  uniform vec4 LightDirection;
  uniform vec4 LightColor;
  uniform vec4 MatAmbient;
  uniform vec4 MatSpecular;
  uniform float MatShininess;
}
g_ubo;

// Local space
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec4 VertexColor;
layout(location = 2) in vec3 VertexNormal;

layout(location = 0) out vec4 v_Color;

void main()
{
  v_Color = VertexColor;

  // transform the vertex coordinates
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
