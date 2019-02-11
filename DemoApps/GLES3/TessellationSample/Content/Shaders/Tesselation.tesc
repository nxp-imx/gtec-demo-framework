#version 310 es
#extension GL_EXT_tessellation_shader : enable


#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform float TessLevelInner;
uniform float TessLevelOuter;

layout(vertices = 3) out;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];

in block
{
  vec3 Normal;
  vec3 Tangent;
  vec2 TexCoord;
}
In[];

out gl_PerVertex
{
  vec4 gl_Position;
}
gl_out[];

out block
{
  vec3 Normal;
  vec3 Tangent;
  vec2 TexCoord;
}
Out[];

void main()
{
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  Out[gl_InvocationID].Normal = In[gl_InvocationID].Normal;
  Out[gl_InvocationID].Tangent = In[gl_InvocationID].Tangent;
  Out[gl_InvocationID].TexCoord = In[gl_InvocationID].TexCoord;

  gl_TessLevelInner[0] = TessLevelInner;
  gl_TessLevelInner[1] = TessLevelInner;
  gl_TessLevelOuter[0] = TessLevelOuter;
  gl_TessLevelOuter[1] = TessLevelOuter;
  gl_TessLevelOuter[2] = TessLevelOuter;
}
