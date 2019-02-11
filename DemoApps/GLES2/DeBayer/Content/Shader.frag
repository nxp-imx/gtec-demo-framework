precision mediump float;
uniform sampler2D my_Texture0;
varying vec4 center;
varying vec4 yCoord;
varying vec4 xCoord;
void main()
{
#define fetch(x, y) texture2D(my_Texture0, vec2(x, y)).a
  float C = texture2D(my_Texture0, center.xy).a;
  const vec4 kC = vec4(4.0, 6.0, 5.0, 5.0) / 8.0;
  const float Xunit = 1.0 / 1920.0;
  const float Yunit = 1.0 / 1088.0;
  vec2 locR, locG, locB;
  vec2 alternate = mod(floor(center.zw), 2.0);
  if (alternate.y == 0.0)
  {
    if (alternate.x == 0.0)
    {
      locR = vec2(center.x, center.y);
      locG = vec2(center.x - Xunit, center.y);
      locB = vec2(center.x - Xunit, center.y - Yunit);
      gl_FragColor.rgb = vec3(texture2D(my_Texture0, locR).a, texture2D(my_Texture0, locG).a, texture2D(my_Texture0, locB).a);
    }
    else
    {
      locR = vec2(center.x - Xunit, center.y);
      locG = vec2(center.x, center.y);
      locB = vec2(center.x, center.y - Yunit);
      gl_FragColor.rgb = vec3(texture2D(my_Texture0, locR).a, texture2D(my_Texture0, locG).a, texture2D(my_Texture0, locB).a);
    }
  }
  else
  {
    if (alternate.x == 0.0)
    {
      locR = vec2(center.x, center.y - Yunit);
      locG = vec2(center.x, center.y);
      locB = vec2(center.x - Xunit, center.y);
      gl_FragColor.rgb = vec3(texture2D(my_Texture0, locR).a, texture2D(my_Texture0, locG).a, texture2D(my_Texture0, locB).a);
    }
    else
    {
      locR = vec2(center.x - Xunit, center.y - Yunit);
      locG = vec2(center.x - Xunit, center.y);
      locB = vec2(center.x, center.y);
      gl_FragColor.rgb = vec3(texture2D(my_Texture0, locR).a, texture2D(my_Texture0, locG).a, texture2D(my_Texture0, locB).a);
    }
  }
}
