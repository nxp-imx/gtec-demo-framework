#version 300 es
// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

// Camera space
uniform vec3 LightDirection;

// Local space
in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;
in vec3 VertexTangent;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec3 v_LightVec;
out vec3 v_HalfVec;
out vec2 v_TexCoord;

void main()
{
  v_Color = VertexColor;

  // Build a 'matrix' to convert from 'Eye Space' to 'Tangent Space'
  vec3 n = normalize(NormalMatrix * VertexNormal);
  vec3 t = normalize(NormalMatrix * VertexTangent);
  vec3 b = cross(n, t);

  // Transform light from eye to tangent space
  vec3 lightDir;
  lightDir.x = dot(LightDirection, t);
  lightDir.y = dot(LightDirection, b);
  lightDir.z = dot(LightDirection, n);
  v_LightVec = normalize(lightDir);

  // Transform halfVec from eye to tangent space
  vec3 vertexPosition = normalize(-(WorldView * vec4(VertexPosition, 1.0)).xyz);
  vec3 halfVector = normalize(vertexPosition + LightDirection);
  vec3 transformedHalfVec;
  transformedHalfVec.x = dot(halfVector, t);
  transformedHalfVec.y = dot(halfVector, b);
  transformedHalfVec.z = dot(halfVector, n);
  // We dont normalize as t,b,n are normal vectors
  v_HalfVec = transformedHalfVec;

  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
