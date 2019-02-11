// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;

// Camera space
uniform vec3 LightDirection;

// Material
uniform vec4 MatAmbient;
uniform vec4 MatSpecular;
uniform float MatShininess;


varying vec3 v_Eye;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main()
{
  // set the specular term to black
  vec4 spec = vec4(0.0);

  // Normalize the input normal
  vec3 n = normalize(v_Normal);

  // Calc the intensity as the dot product the max prevents negative intensity values
  float intensity = max(dot(n, LightDirection), 0.0);

  vec4 diffuse = texture2D(Texture0, v_TexCoord);

  // if the vertex is lit calc the specular term
  if (intensity > 0.0)
  {
    // Calc the half vector
    vec3 h = normalize(LightDirection + normalize(v_Eye));

    // Calc the specular term into spec
    float intSpec = max(dot(h, n), 0.0);
    spec = MatSpecular * pow(intSpec, MatShininess);
    spec *= diffuse.w;
  }

  gl_FragColor = diffuse * (intensity + spec + MatAmbient);
}
