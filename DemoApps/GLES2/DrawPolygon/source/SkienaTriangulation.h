#pragma once
/* 

Hugo: Used Skiena's book as it provided the simplest ear cutting algorithm
My subset implementation requires the polygon points to be defined in a clockwise winding order

Copyright 2003 by Steven S. Skiena; all rights reserved.

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/

#include<cstdint>
#include<array>
#include<vector>
#include <GLES2/gl2.h>
#include "CommonTypes.h"

namespace Fsl
{
  namespace HugoTriangulate
  {
    constexpr double PI = 3.1415926;
    constexpr double EPSILON = 0.00000001;

    struct point
    {
      GLfloat x;
      GLfloat y;
    };

    struct polygon
    {
      uint32_t n{0};    // Todo: Not really needed with a vector, but handy to have in case I need to port it to C.
      std::vector<point> p;
    };

    struct triangle
    {
      std::array<point, 3> p;
    };

    struct triangulation
    {
      uint32_t n{0};    // Todo: not really needed with a vector, but handy to have in case I need to port it to C.
      std::vector<std::array<uint32_t, 3>> triangles;
    };

    bool TriangulateCpp(const std::vector<pointsForGL>& inputPoints, std::vector<GLuint>& indices);
  }
}