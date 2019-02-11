#version 300 es

precision mediump float;
layout(location = 0) out vec4 fragData0;
layout(location = 1) out vec4 fragData1;
layout(location = 2) out vec4 fragData2;
layout(location = 3) out vec4 fragData3;

void main()
{
  // first buffer will contain red color
  fragData0 = vec4(1, 0, 0, 1);

  // second buffer will contain green color
  fragData1 = vec4(0, 1, 0, 1);

  // third buffer will contain blue color
  fragData2 = vec4(0, 0, 1, 1);

  // fourth buffer will contain gray color
  fragData3 = vec4(0.5, 0.5, 0.5, 1);
}
