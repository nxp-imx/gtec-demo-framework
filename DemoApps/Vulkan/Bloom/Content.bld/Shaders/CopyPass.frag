#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(binding = 0) uniform sampler2D Texture0;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(Texture0, v_TexCoord);
}
