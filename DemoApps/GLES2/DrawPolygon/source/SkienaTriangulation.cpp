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


#include "SkienaTriangulation.h"

namespace Fsl
{
  namespace HugoTriangulate
  {
    bool EarClip(int i, int j, int k, const std::vector<pointsForGL>& points);
    bool PointInTriangle(const pointsForGL& p, std::array<pointsForGL, 3> triangle);
    void PrintTriangulation(triangulation* t);

    void CopyPoint(const pointsForGL& a, pointsForGL& b)
    {
      b.x = a.x;
      b.y = a.y;
    }

    double CalculateAreaInTriangle(const pointsForGL& a, const pointsForGL& b, const pointsForGL& c)
    {
      return ((a.x * b.y - a.y * b.x + a.y * c.x - a.x * c.y + b.x * c.y - c.x * b.y) / 2.0);
    }

    bool IsClockWise(const pointsForGL& a, const pointsForGL& b, const pointsForGL& c)
    {
      double testSignedTriangleArea = CalculateAreaInTriangle(a, b, c);
      return (testSignedTriangleArea > EPSILON);
    }

    bool TriangulateCpp(const std::vector<pointsForGL>& inputPoints, std::vector<GLuint>& indices)
    {
      std::vector<uint32_t> vertexList(inputPoints.size(), 0);
      for (uint32_t i = 0; i < vertexList.size(); i++)
      {
        vertexList[i] = i;
      }
      int32_t currentIndex = 0;
      uint32_t earClippingTries = 0;
      while ((indices.size() / 3) < (inputPoints.size() - 2))
      {
        int32_t leftIndex = ((currentIndex - 1) + vertexList.size()) % vertexList.size();
        int32_t rightIndex = ((currentIndex + 1) + vertexList.size()) % vertexList.size();
        if (EarClip(vertexList[leftIndex], vertexList[currentIndex], vertexList[rightIndex], inputPoints))
        {
          indices.push_back(vertexList[leftIndex]);
          indices.push_back(vertexList[currentIndex]);
          indices.push_back(vertexList[rightIndex]);
          vertexList.erase(vertexList.begin() + currentIndex);
          currentIndex = ((currentIndex - 1) + vertexList.size()) % vertexList.size();
          earClippingTries = 0;
        }
        else
        {
          currentIndex = rightIndex;
          earClippingTries++;
        }
        // This means that the whole polygon has been checked at least twice and no more ears have been found
        // This indicates that you have a wrong winding order in your polygon.
        if (earClippingTries > (inputPoints.size() * 2))
        {
          return false;
        }
      }
      return true;
    }

    bool EarClip(int i, int j, int k, const std::vector<pointsForGL>& points)
    {
      std::array<pointsForGL, 3> triangle{points[i], points[j], points[k]};

      if (!IsClockWise(triangle[0], triangle[1], triangle[2]))
        return (false);

      for (uint32_t m = 0; m < points.size(); m++)
      {
        if ((m != i) && (m != j) && (m != k))
          if (PointInTriangle(points[m], triangle))
            return (false);
      }

      return (true);
    }

    bool PointInTriangle(const pointsForGL& p, std::array<pointsForGL, 3> triangle)
    {
      // If all points have a ClockWise winding order with the point we want to verify, it means that the point is within the triangle!!
      for (uint32_t i = 0; i < 3; i++)
        if (!IsClockWise(triangle[i], triangle[(i + 1) % 3], p))
          return (false);

      return (true);
    }
  }
}
