#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D TextureDiffuse;
uniform sampler2D TextureNormal;

// Material
uniform vec3 MatAmbient;
uniform vec3 MatSpecular;
uniform float MatShininess;

in vec3 v_LightVec;
in vec3 v_HalfVec;
in vec2 v_TexCoord;

out vec4 o_fragColor;

void main()
{
  // Read the normal from the normal map and move it from the [0,1] range to [-1,1] and normalize it
  vec3 normal = normalize((texture(TextureNormal, v_TexCoord).xyz * 2.0) - 1.0);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(normal, v_LightVec), 0.0);

  vec4 diffuse = vec4(0);
  if (intensity > 0.0)
  {
    diffuse = texture(TextureDiffuse, v_TexCoord);

    // Calc the specular term into spec
    float intSpec = max(dot(v_HalfVec, normal), 0.0);
    diffuse.xyz *= vec3(intensity) + (MatSpecular * pow(intSpec, MatShininess));
    // diffuse.xyz *= (MatSpecular * pow(intSpec, MatShininess));
    // diffuse.xyz *= pow(intSpec, 10.0f);
  }
  diffuse.xyz += MatAmbient;
  o_fragColor = diffuse;
}
