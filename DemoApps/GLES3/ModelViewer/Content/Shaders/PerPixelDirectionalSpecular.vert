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

// Local space
in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;

out vec4 v_Color;
out vec3 v_Eye;
out vec3 v_Normal;

void main()
{
  v_Color = VertexColor;

  //! transform position to eye space
  v_Eye = (-(WorldView * vec4(VertexPosition, 1.0))).xyz;
  // transform normal to camera space and normalize it
  v_Normal = normalize(NormalMatrix * VertexNormal);

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
