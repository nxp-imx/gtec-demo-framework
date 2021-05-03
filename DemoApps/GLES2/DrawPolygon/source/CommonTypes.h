#pragma once

#include<vector>
#include <GLES2/gl2.h>

namespace Fsl
{
  struct pointsForGL
  {
    GLfloat x;
    GLfloat y;
  };
  
  struct polygonGL
  {
    std::vector<pointsForGL> points;
  };
}