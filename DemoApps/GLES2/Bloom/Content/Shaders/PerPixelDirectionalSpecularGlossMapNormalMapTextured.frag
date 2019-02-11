// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;
uniform sampler2D TextureNormal;

// Material
uniform vec4 MatAmbient;
uniform vec4 MatSpecular;
uniform float MatShininess;


varying vec3 v_LightVec;
varying vec3 v_HalfVec;
varying vec2 v_TexCoord;


void main()
{
  // Read the normal from the normal map and move it from the [0,1] range to [-1,1] and normalize it
  vec3 normal = normalize((texture2D(TextureNormal, v_TexCoord).xyz * 2.0) - 1.0);

  // set the specular term to black
  vec4 spec = vec4(0.0);


  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(normal, v_LightVec), 0.0);

  vec4 diffuse = texture2D(Texture0, v_TexCoord);

  // if the vertex is lit calc the specular term
  if (intensity > 0.0)
  {
    // Calc the specular term into spec
    float intSpec = max(dot(normalize(v_HalfVec), normal), 0.0);
    spec = MatSpecular * pow(intSpec, MatShininess);
    spec *= diffuse.w;
  }

  gl_FragColor = diffuse * max(intensity + spec, MatAmbient);
}
