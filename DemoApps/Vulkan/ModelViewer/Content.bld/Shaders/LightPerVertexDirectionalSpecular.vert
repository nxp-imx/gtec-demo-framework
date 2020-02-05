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
  // set the specular term to black
  vec4 spec = vec4(0.0);

  // transform normal to camera space and normalize it
  vec3 n = normalize(mat3(g_ubo.NormalMatrix) * VertexNormal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, g_ubo.LightDirection.xyz), 0.0);

  // if the vertex is lit calc the specular term
  if (intensity > 0.0)
  {
    // Calc position in camera space
    vec3 pos = (g_ubo.WorldView * vec4(VertexPosition, 1)).xyz;
    // Calc eye vector and normalize it
    vec3 eye = normalize(-pos);
    // Calc the half vector
    vec3 h = normalize(g_ubo.LightDirection.xyz + eye);

    // Calc the specular term into spec
    float intSpec = max(dot(h, n), 0.0);
    spec = g_ubo.MatSpecular * pow(intSpec, g_ubo.MatShininess);
  }

  // add the specular term
  v_Color = max((VertexColor * intensity) + spec, g_ubo.MatAmbient);

  // transform the vertex coordinates
  gl_Position = g_ubo.WorldViewProjection * vec4(VertexPosition, 1.0);
}
