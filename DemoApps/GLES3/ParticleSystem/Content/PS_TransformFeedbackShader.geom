#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#define PARTICLE_TYPE_NORMAL 0.0
#define PARTICLE_TYPE_EMITTER 1.0

layout(points, invocations = 1) in;
layout(points, max_vertices = 100) out;

uniform float DeltaTime;
uniform sampler2D TextureRandom;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];


in block
{
  vec3 ParticlePosition;
  vec3 ParticleVelocity;
  float ParticleEnergy;
  float ParticleType;
}
In[];

out gl_PerVertex
{
  vec4 gl_Position;
};

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
  if (In[0].ParticleType == PARTICLE_TYPE_NORMAL)
  {
    Out.ParticlePosition = In[0].ParticlePosition + In[0].ParticleVelocity * DeltaTime;
    Out.ParticleVelocity = In[0].ParticleVelocity;
    Out.ParticleEnergy = In[0].ParticleEnergy - DeltaTime;
    Out.ParticleType = In[0].ParticleType;

    if (Out.ParticlePosition.y >= 100.0)
    {
      // reset particle to new random position
      Out.ParticleEnergy = 10000.0;
      Out.ParticlePosition.y = -100.0;
    }
    EmitVertex();
    EndPrimitive();
  }
  else
  {
    Out.ParticlePosition = In[0].ParticlePosition;
    Out.ParticleVelocity = In[0].ParticleVelocity;
    Out.ParticleEnergy = In[0].ParticleEnergy - DeltaTime;
    Out.ParticleType = In[0].ParticleType;
    EmitVertex();
    EndPrimitive();

    Out.ParticlePosition = In[0].ParticlePosition;
    Out.ParticleVelocity = vec3(0, 8, 0);
    Out.ParticleEnergy = In[0].ParticleEnergy - DeltaTime;
    Out.ParticleType = PARTICLE_TYPE_NORMAL;
    EmitVertex();
    EndPrimitive();
  }
}
