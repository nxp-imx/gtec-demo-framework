#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

const float Power = 4.0;
const float Scale = 1.83;
const float Bias = 0.27;

uniform sampler2D Texture0;

varying vec2 v_TexCoord;

void main()
{
  vec3 color = texture2D(Texture0, v_TexCoord).rgb;
  gl_FragColor = vec4((-color + (pow(color, vec3(Power)) * Scale)) / Bias, 1.0);
}
