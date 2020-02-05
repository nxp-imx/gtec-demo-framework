#version 310 es
// BEWARE: This is a example shader and it has not been optimized for speed.


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(binding = 1) uniform sampler2D Texture0;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(Texture0, v_TexCoord) * v_Color;
}
