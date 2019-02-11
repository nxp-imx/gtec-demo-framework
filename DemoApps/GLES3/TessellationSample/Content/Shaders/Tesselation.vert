#version 310 es
#extension GL_EXT_tessellation_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec2 TexCoord;

out block
{
  vec3 Normal;
  vec3 Tangent;
  vec2 TexCoord;
}
Out;

void main()
{
  gl_Position = vec4(Position.xyz, 1.0);
  Out.Normal = Normal;
  Out.Tangent = Tangent;
  Out.TexCoord = TexCoord;
}
