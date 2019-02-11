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
attribute vec3 VertexPosition;
attribute vec3 VertexNormal;
attribute vec2 VertexTexCoord;

varying vec3 v_Eye;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main()
{
  //! transform position to eye space
  v_Eye = (-(WorldView * vec4(VertexPosition, 1.0))).xyz;
  // transform normal to camera space and normalize it
  v_Normal = normalize(NormalMatrix * VertexNormal);

  v_TexCoord = VertexTexCoord;
  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
