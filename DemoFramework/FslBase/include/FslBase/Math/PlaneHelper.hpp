#ifndef FSLBASE_MATH_PLANEHELPER_HPP
#define FSLBASE_MATH_PLANEHELPER_HPP
/*
MIT License
Copyright (C) 2006 The Mono.Xna Team

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// The functions in this file are a port of an MIT licensed library: MonoGame - Plane.cs.

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  struct Vector3;
  struct Plane;

  class PlaneHelper
  {
  public:
    //! @brief Returns a value indicating what side (positive/negative) of a plane a point is
    //! @param point The point to check with
    //! @param plane The plane to check against
    //! @return Greater than zero if on the positive side, less than zero if on the negative size, 0 otherwise
    static float ClassifyPoint(const Vector3& point, const Plane& plane);

    //! @brief Returns the perpendicular distance from a point to a plane
    //! @param point The point to check
    //! @param plane The place to check
    //! @return The perpendicular distance from the point to the plane
    static float PerpendicularDistance(const Vector3& point, const Plane& plane);
  };
}

#endif
