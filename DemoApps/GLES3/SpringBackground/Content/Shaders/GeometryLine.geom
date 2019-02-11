#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(lines, invocations = 1) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 WorldViewProjection;

// Defining gl_PerVertex is not really necessary according to the standard (and maybe even wrong)
// But it makes it work under most emulators and no target its been tested on complained so far.
in gl_PerVertex
{
  vec4 gl_Position;
}
gl_in[];


in block
{
  vec4 VertexColor;
}
In[];


out gl_PerVertex
{
  vec4 gl_Position;
};


out block
{
  vec4 VertexColor;
}
Out;


void main()
{
  vec3 diff = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
  // the normals are (-delta.Y, delta.X) and(delta.Y, -delta.X).
  vec3 vectorL = vec3(-diff.y, diff.x, 0) * 1.5;
  vec3 vectorR = vec3(diff.y, -diff.x, 0) * 1.5;

  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz + vectorL, 1.0);
  Out.VertexColor = In[0].VertexColor;
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz + vectorR, 1.0);
  Out.VertexColor = In[0].VertexColor;
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[1].gl_Position.xyz + vectorL, 1.0);
  Out.VertexColor = In[1].VertexColor;
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[1].gl_Position.xyz + vectorR, 1.0);
  Out.VertexColor = In[1].VertexColor;
  EmitVertex();


  EndPrimitive();
}
