#version 310 es
#extension GL_EXT_tessellation_shader : enable

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

in block
{
  vec3 LightVec;
  vec3 HalfVec;
  vec2 TexCoord;
}
In;

out vec4 Out_FragColor;

void main()
{
  // Read the normal from the normal map and move it from the [0,1] range to [-1,1] and normalize it
  vec3 normal = normalize((texture(TextureNormal, In.TexCoord).xyz * 2.0) - 1.0);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(In.LightVec, normal), 0.0);

  vec4 diffuse = vec4(0);
  if (intensity > 0.0)
  {
    diffuse = texture(TextureDiffuse, In.TexCoord);

    // Calc the specular term into spec
    float intSpec = max(dot(normalize(In.HalfVec), normal), 0.0);
    diffuse.xyz *= vec3(intensity) + (MatSpecular * pow(intSpec, MatShininess));
    // diffuse.xyz *= (MatSpecular * pow(intSpec, MatShininess));
  }
  diffuse.xyz += MatAmbient;
  Out_FragColor = diffuse;
}
