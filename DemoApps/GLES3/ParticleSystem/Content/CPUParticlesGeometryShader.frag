#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D TextureDiffuse;

in block
{
  //  vec4 Color;
  vec2 TexCoord;
}
In;

out vec4 Out_FragColor;

void main()
{
  //  Out_FragColor = vec4(1,1,1,1);   //texture(TextureDiffuse, In.TexCoord) * In.Color;
  Out_FragColor = texture(TextureDiffuse, In.TexCoord);
}
