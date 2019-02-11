#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 ParticlePosition;

void main()
{
  gl_Position = vec4(ParticlePosition, 1.0);
}
