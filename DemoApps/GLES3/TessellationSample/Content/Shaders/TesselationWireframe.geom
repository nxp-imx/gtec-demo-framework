#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(triangles, invocations = 1) in;
layout(line_strip, max_vertices = 4) out;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];

in block
{
  vec3 LightVec;
  vec3 HalfVec;
  vec2 TexCoord;
}
In[];


out gl_PerVertex
{
  vec4 gl_Position;
};

out block
{
  vec3 LightVec;
  vec3 HalfVec;
  vec2 TexCoord;
}
Out;


void main()
{
  gl_Position = gl_in[0].gl_Position;
  Out.LightVec = In[0].LightVec;
  Out.HalfVec = In[0].HalfVec;
  Out.TexCoord = In[0].TexCoord;
  EmitVertex();

  gl_Position = gl_in[1].gl_Position;
  Out.LightVec = In[1].LightVec;
  Out.HalfVec = In[1].HalfVec;
  Out.TexCoord = In[1].TexCoord;
  EmitVertex();

  gl_Position = gl_in[2].gl_Position;
  Out.LightVec = In[2].LightVec;
  Out.HalfVec = In[2].HalfVec;
  Out.TexCoord = In[2].TexCoord;
  EmitVertex();

  gl_Position = gl_in[0].gl_Position;
  Out.LightVec = In[0].LightVec;
  Out.HalfVec = In[0].HalfVec;
  Out.TexCoord = In[0].TexCoord;
  EmitVertex();

  EndPrimitive();
}
