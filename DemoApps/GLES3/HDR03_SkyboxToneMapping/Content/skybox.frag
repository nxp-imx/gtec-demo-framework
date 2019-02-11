#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 v_texCoords;

uniform samplerCube g_skyboxSampler;

out vec4 FragColor;

void main()
{
  FragColor = texture(g_skyboxSampler, v_texCoords);
}