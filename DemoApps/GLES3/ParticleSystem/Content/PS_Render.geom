#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(points, invocations = 1) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 WorldView;
uniform mat4 WorldViewProjection;

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

out block
{
  vec2 TexCoord;
}
Out;

void main()
{
  // Extract the view direction from the WorldView matrix
  vec3 viewDirection = vec3(WorldView[0][2], WorldView[1][2], WorldView[2][2]);
  vec3 rightVector = normalize(vec3(WorldView[0][0], WorldView[1][0], WorldView[2][0]));
  vec3 upVector = normalize(vec3(WorldView[0][1], WorldView[1][1], WorldView[2][1]));

  // Offset to the left or right.
  vec3 vectorX = rightVector * 0.5;
  // Offset upward if we are one of the top two vertices.
  vec3 vectorY = upVector * 0.5;

  vec4 position = gl_in[0].gl_Position;
  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz - vectorX + vectorY, 1.0);
  Out.TexCoord = vec2(0, 1);
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz - vectorX - vectorY, 1.0);
  Out.TexCoord = vec2(0, 0);
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz + vectorX + vectorY, 1.0);
  Out.TexCoord = vec2(1, 1);
  EmitVertex();

  gl_Position = WorldViewProjection * vec4(gl_in[0].gl_Position.xyz + vectorX - vectorY, 1.0);
  Out.TexCoord = vec2(1, 0);
  EmitVertex();

  EndPrimitive();
}
