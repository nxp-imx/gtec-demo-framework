#version 310 es
#extension GL_EXT_shader_io_blocks : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in block
{
  vec3 ParticlePosition;
  vec3 ParticleVelocity;
  float ParticleEnergy;
  float ParticleType;
}
In;

out vec4 Out_FragColor;

void main()
{
  Out_FragColor = vec4(1.0);
}
