#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


uniform sampler2D Texture0;
uniform float TexSize;

in vec2 v_TexCoord;

out vec4 o_fragColor;

void main()
{
  vec4 color;
  color = ##REPLACE0_WEIGHT## * texture(Texture0, v_TexCoord - vec2(0.0, TexSize * ##REPLACE0_OFFSET##));
  color += ##REPLACE1_WEIGHT## * texture(Texture0, v_TexCoord);
  color += ##REPLACE0_WEIGHT## * texture(Texture0, v_TexCoord + vec2(0.0, TexSize * ##REPLACE0_OFFSET##));
  o_fragColor = color;
}
