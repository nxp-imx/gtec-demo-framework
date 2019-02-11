#ifndef FSLBASE_MATH_BOUNDINGSPHERE_HPP
#define FSLBASE_MATH_BOUNDINGSPHERE_HPP
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

// The functions in this file are a port of an MIT licensed library: MonoGame - BoundingSphere.cs.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/ContainmentType.hpp>
#include <FslBase/Math/PlaneIntersectionType.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <vector>

namespace Fsl
{
  struct BoundingBox;
  class BoundingFrustum;
  struct Plane;
  struct Ray;

  struct BoundingSphere
  {
    Vector3 Center;
    float Radius{0.0f};

    constexpr BoundingSphere() = default;

    constexpr BoundingSphere(Vector3 center, float radius)
      : Center(center)
      , Radius(radius)
    {
    }

    BoundingSphere Transform(const Matrix& matrix);
    void Transform(const Matrix& matrix, BoundingSphere& rResult);

    ContainmentType Contains(const BoundingBox& box) const;
    ContainmentType Contains(const BoundingFrustum& frustum) const;
    ContainmentType Contains(const BoundingSphere& sphere) const;
    ContainmentType Contains(const Vector3& point) const;

    static BoundingSphere CreateFromBoundingBox(const BoundingBox& box);
    static void CreateFromBoundingBox(const BoundingBox& box, BoundingSphere& rResult);
    static BoundingSphere CreateFromFrustum(const BoundingFrustum& frustum);
    static BoundingSphere CreateFromPoints(const std::vector<Vector3>& points);
    static BoundingSphere CreateMerged(const BoundingSphere& original, const BoundingSphere& additional);
    static void CreateMerged(const BoundingSphere& original, const BoundingSphere& additional, BoundingSphere& rResult);

    bool Intersects(const BoundingBox& box) const;
    bool Intersects(const BoundingSphere& sphere) const;
    PlaneIntersectionType Intersects(const Plane& plane) const;
    bool Intersects(const Ray& ray, float& rResult) const;

    //! @brief Tests for equality.
    constexpr bool operator==(const BoundingSphere& rhs) const
    {
      return Center == rhs.Center && Radius == rhs.Radius;
    }

    //! @brief Tests for inequality.
    constexpr bool operator!=(const BoundingSphere& rhs) const
    {
      return Center != rhs.Center || Radius != rhs.Radius;
    }
  };
}

#endif
