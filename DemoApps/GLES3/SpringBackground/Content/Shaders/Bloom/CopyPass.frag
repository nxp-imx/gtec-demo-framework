#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D Texture0;

in vec2 v_TexCoord;

out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(Texture0, v_TexCoord);
}
