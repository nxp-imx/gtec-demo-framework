#version 310 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(location = 0) in vec3 v_texCoords;

layout(binding = 1) uniform samplerCube g_skyboxSampler;

layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = texture(g_skyboxSampler, v_texCoords);
}