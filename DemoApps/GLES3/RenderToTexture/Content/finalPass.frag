#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D g_texSampler;

in vec2 g_vVSTexCoord;

out vec4 outputColor;

void main()
{
  outputColor = vec4(texture(g_texSampler, g_vVSTexCoord));
}
