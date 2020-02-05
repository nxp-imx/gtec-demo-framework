#version 310 es
// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

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
  // transform normal to camera space and normalize it
  vec3 n = normalize(mat3(g_ubo.NormalMatrix) * VertexNormal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, g_ubo.LightDirection.xyz), 0.0);

  // Calc the color per vertex
  v_Color = VertexColor * intensity;

  // transform the vertex coordinates
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
