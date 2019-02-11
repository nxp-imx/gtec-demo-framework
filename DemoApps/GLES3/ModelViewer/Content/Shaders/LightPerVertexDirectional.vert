#version 300 es
// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;
uniform mat3 NormalMatrix;

// Camera space
uniform vec3 LightDirection;

// Local space
in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;

out vec4 v_Color;

void main()
{
  // transform normal to camera space and normalize it
  vec3 n = normalize(NormalMatrix * VertexNormal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, LightDirection), 0.0);

  // Calc the color per vertex
  v_Color = VertexColor * intensity;

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
