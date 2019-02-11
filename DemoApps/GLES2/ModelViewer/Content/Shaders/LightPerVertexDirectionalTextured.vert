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
attribute vec3 VertexPosition;
attribute vec4 VertexColor;
attribute vec3 VertexNormal;
attribute vec2 VertexTexCoord;

varying vec4 v_Color;
varying vec2 v_TexCoord;

void main()
{
  // transform normal to camera space and normalize it
  vec3 n = normalize(NormalMatrix * VertexNormal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, LightDirection), 0.0);

  // Calc the color per vertex
  v_Color = VertexColor * intensity;
  v_TexCoord = VertexTexCoord;

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
