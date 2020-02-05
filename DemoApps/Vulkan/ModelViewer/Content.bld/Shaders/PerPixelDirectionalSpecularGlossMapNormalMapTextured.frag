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

layout(binding = 1) uniform sampler2D Texture0;
layout(binding = 2) uniform sampler2D TextureNormal;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec3 v_LightVec;
layout(location = 2) in vec3 v_HalfVec;
layout(location = 3) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  // Read the normal from the normal map and move it from the [0,1] range to [-1,1] and normalize it
  vec3 normal = normalize((texture(TextureNormal, v_TexCoord).xyz * 2.0) - 1.0);

  // set the specular term to black
  vec4 spec = vec4(0.0);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(normal, v_LightVec), 0.0);

  vec4 diffuse = texture(Texture0, v_TexCoord);

  // if the vertex is lit calc the specular term
  if (intensity > 0.0)
  {
    // Calc the specular term into spec
    float intSpec = max(dot(normalize(v_HalfVec), normal), 0.0);
    spec = g_ubo.MatSpecular * pow(intSpec, g_ubo.MatShininess);
    spec *= diffuse.w;
  }

  o_fragColor = diffuse * ((v_Color * intensity) + spec + g_ubo.MatAmbient);
}
