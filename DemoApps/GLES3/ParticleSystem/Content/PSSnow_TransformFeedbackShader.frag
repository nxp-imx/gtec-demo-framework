#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


in vec3 Out_ParticlePosition;
in vec3 Out_ParticleVelocity;
in float Out_ParticleEnergy;

out vec4 Out_FragColor;

void main()
{
  Out_FragColor = vec4(1.0);
}
