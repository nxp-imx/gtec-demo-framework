#version 310 es
#extension GL_EXT_tessellation_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 Position;

void main()
{
  gl_Position = vec4(Position.xyz, 1.0);
}
