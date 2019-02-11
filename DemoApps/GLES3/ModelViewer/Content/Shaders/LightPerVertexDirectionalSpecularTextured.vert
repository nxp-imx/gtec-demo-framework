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

// Material
uniform vec4 MatAmbient;
uniform vec4 MatSpecular;
uniform float MatShininess;

// Local space
in vec3 VertexPosition;
in vec4 VertexColor;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
  // set the specular term to black
  vec4 spec = vec4(0.0);

  // transform normal to camera space and normalize it
  vec3 n = normalize(NormalMatrix * VertexNormal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, LightDirection), 0.0);

  // if the vertex is lit calc the specular term
  if (intensity > 0.0)
  {
    // Calc position in camera space
    vec3 pos = (WorldView * vec4(VertexPosition, 1)).xyz;
    // Calc eye vector and normalize it
    vec3 eye = normalize(-pos);
    // Calc the half vector
    vec3 h = normalize(LightDirection + eye);

    // Calc the specular term into spec
    float intSpec = max(dot(h, n), 0.0);
    spec = MatSpecular * pow(intSpec, MatShininess);
  }

  // add the specular term
  v_Color = max((VertexColor * intensity) + spec, MatAmbient);
  v_TexCoord = VertexTexCoord;

  // transform the vertex coordinates
  gl_Position = WorldViewProjection * vec4(VertexPosition, 1.0);
}
