#version 310 es
#extension GL_EXT_tessellation_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform mat4 WorldViewProjection;

layout(triangles, fractional_odd_spacing, ccw) in;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  vec4 p0 = vec4(gl_TessCoord.x) * gl_in[0].gl_Position;
  vec4 p1 = vec4(gl_TessCoord.y) * gl_in[1].gl_Position;
  vec4 p2 = vec4(gl_TessCoord.z) * gl_in[2].gl_Position;
  vec4 tePosition = normalize(p0 + p1 + p2);
  tePosition = (p0 + p1 + p2) / 3.0;
  gl_Position = WorldViewProjection * tePosition;
}
