#version 310 es
#extension GL_EXT_geometry_shader : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif


in block
{
  vec4 VertexColor;
}
In;


out vec4 Out_FragColor;


void main()
{
  //  Out_FragColor = vec4(In.VertexColor.x, 1, 1, 1);
  Out_FragColor = In.VertexColor;
}
