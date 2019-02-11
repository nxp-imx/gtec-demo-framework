#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

const float BRIGHT_PASS_THRESHOLD = 0.8;
const float BRIGHT_PASS_OFFSET = 0.01;
const float BRIGHT_RANGE = 1.0 - BRIGHT_PASS_THRESHOLD;

// const float Luminance = 0.08;
// const float fMiddleGray = 0.18;
// const float fWhiteCutoff = 0.8;

uniform sampler2D Texture0;

varying vec2 v_TexCoord;

void main()
{
  vec3 color = texture2D(Texture0, v_TexCoord).rgb;

  color = max(color - BRIGHT_PASS_THRESHOLD, 0.0);
  gl_FragColor = vec4(color / (color + BRIGHT_PASS_OFFSET), 1.0);
  // gl_FragColor = vec4(color + , 1.0);
  // gl_FragColor = vec4(color / BRIGHT_RANGE, 1.0);


  // color *= fMiddleGray / ( Luminance + 0.001 );
  // color *= ( 1.0 + ( color / ( fWhiteCutoff * fWhiteCutoff ) ) );
  // color -= 5.0;
  // color = max(color, 0.0);
  // color /= ( 10.0 + color );
  // gl_FragColor = vec4( color, 1.0 );
}
