#version 310 es
// BEWARE: This is a example shader and it has not been optimized for speed.
precision mediump float;

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
layout(location = 3) in vec3 VertexTangent;
layout(location = 4) in vec2 VertexTexCoord;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec3 v_LightVec;
layout(location = 2) out vec3 v_HalfVec;
layout(location = 3) out vec2 v_TexCoord;

void main()
{
  v_Color = VertexColor;

  // Build a 'matrix' to convert from 'Eye Space' to 'Tangent Space'
  vec3 n = normalize(mat3(g_ubo.NormalMatrix) * VertexNormal);
  vec3 t = normalize(mat3(g_ubo.NormalMatrix) * VertexTangent);
  vec3 b = cross(n, t);

  // Transform light from eye to tangent space
  vec3 lightDir;
  lightDir.x = dot(g_ubo.LightDirection.xyz, t);
  lightDir.y = dot(g_ubo.LightDirection.xyz, b);
  lightDir.z = dot(g_ubo.LightDirection.xyz, n);
  v_LightVec = normalize(lightDir);

  // Transform halfVec from eye to tangent space
  vec3 vertexPosition = normalize(-(g_ubo.WorldView * vec4(VertexPosition, 1.0)).xyz);
  vec3 halfVector = normalize(vertexPosition + g_ubo.LightDirection.xyz);
  vec3 transformedHalfVec;
  transformedHalfVec.x = dot(halfVector, t);
  transformedHalfVec.y = dot(halfVector, b);
  transformedHalfVec.z = dot(halfVector, n);
  // We dont normalize as t,b,n are normal vectors
  v_HalfVec = transformedHalfVec;

  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
