#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform float DeltaTime;
uniform float UpperBoundaryY;    // 100
uniform float LowerBoundaryY;    // -50

layout(location = 0) in vec3 ParticlePosition;
layout(location = 1) in vec3 ParticleVelocity;
layout(location = 2) in float ParticleEnergy;

out vec3 Out_ParticlePosition;
out vec3 Out_ParticleVelocity;
out float Out_ParticleEnergy;

void main()
{
  Out_ParticlePosition = ParticlePosition + ParticleVelocity * DeltaTime;
  Out_ParticleVelocity = ParticleVelocity;
  Out_ParticleEnergy = ParticleEnergy - DeltaTime;
  if (Out_ParticlePosition.y <= LowerBoundaryY)
  {
    // reset particle to new random position
    Out_ParticleEnergy = 10000.0;
    Out_ParticlePosition.y = UpperBoundaryY;
  }
}
