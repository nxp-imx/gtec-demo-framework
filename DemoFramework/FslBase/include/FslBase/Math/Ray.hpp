#ifndef FSLBASE_MATH_RAY_HPP
#define FSLBASE_MATH_RAY_HPP
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

// The functions in this file are a port of an MIT licensed library: MonoGame - Ray.cs.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Vector3.hpp>

namespace Fsl
{
  struct BoundingBox;
  struct BoundingSphere;
  struct Plane;

  struct Ray
  {
    Vector3 Position;
    Vector3 Direction;

    constexpr Ray() = default;

    constexpr Ray(const Vector3& position, const Vector3& direction)
      : Position(position)
      , Direction(direction)
    {
    }

    bool Intersects(const BoundingBox& box, float& rResult) const;
    bool Intersects(const BoundingSphere& sphere, float& rResult) const;
    bool Intersects(const Plane& plane, float& rResult) const;

    //! @brief Tests for equality.
    constexpr bool operator==(const Ray& rhs) const
    {
      return Direction == rhs.Direction && Position == rhs.Position;
    }

    //! @brief Tests for inequality.
    constexpr bool operator!=(const Ray& rhs) const
    {
      return Direction != rhs.Direction || Position != rhs.Position;
    }
  };
}

#endif
