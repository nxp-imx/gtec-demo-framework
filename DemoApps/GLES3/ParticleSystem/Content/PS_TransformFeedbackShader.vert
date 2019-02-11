#version 310 es
#extension GL_EXT_shader_io_blocks : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform float DeltaTime;

layout(location = 0) in vec3 ParticlePosition;
layout(location = 1) in vec3 ParticleVelocity;
layout(location = 2) in float ParticleEnergy;
layout(location = 3) in float ParticleType;

out block
{
  vec3 ParticlePosition;
  vec3 ParticleVelocity;
  float ParticleEnergy;
  float ParticleType;
}
Out;

void main()
{
  Out.ParticlePosition = ParticlePosition;
  Out.ParticleVelocity = ParticleVelocity;
  Out.ParticleEnergy = ParticleEnergy;
  Out.ParticleType = ParticleType;
}
