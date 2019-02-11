#ifndef FSLBASE_MATH_BOUNDINGBOX_HPP
#define FSLBASE_MATH_BOUNDINGBOX_HPP
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

// The functions in this file are a port of an MIT licensed library: MonoGame - BoundingBox.cs.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/ContainmentType.hpp>
#include <FslBase/Math/PlaneIntersectionType.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <array>
#include <vector>

namespace Fsl
{
  class BoundingFrustum;
  struct BoundingSphere;
  struct Plane;
  struct Ray;

  struct BoundingBox
  {
    static const int32_t CornerCount = 8;

    Vector3 Min;
    Vector3 Max;

    constexpr BoundingBox() = default;

    constexpr BoundingBox(const Vector3& min, const Vector3& max)
      : Min(min)
      , Max(max)
    {
    }

    ContainmentType Contains(const BoundingBox& box) const;
    // ContainmentType Contains(const BoundingFrustum& frustum) const;
    ContainmentType Contains(const BoundingSphere& sphere) const;
    ContainmentType Contains(const Vector3& point) const;

    //! @brief Create a bounding box from the given list of points.
    //! @param points The list of Vector3 instances defining the point cloud to bound
    //! @return A bounding box that encapsulates the given point cloud.
    /// @throws std::invalid_argument Thrown if the given vector has no points.
    static BoundingBox CreateFromPoints(const std::vector<Vector3>& points);
    static BoundingBox CreateFromSphere(const BoundingSphere& sphere);
    static void CreateFromSphere(const BoundingSphere& sphere, BoundingBox& rResult);
    static BoundingBox CreateMerged(const BoundingBox& original, const BoundingBox& additional);
    static void CreateMerged(const BoundingBox& original, const BoundingBox& additional, BoundingBox& rResult);

    std::vector<Vector3> GetCorners() const;
    void GetCorners(std::vector<Vector3>& corners) const;
    void GetCorners(std::array<Vector3, 8>& corners) const;

    bool Intersects(const BoundingBox& box) const;
    void Intersects(const BoundingBox& box, bool& rResult) const;
    bool Intersects(const BoundingFrustum& frustum) const;
    bool Intersects(const BoundingSphere& sphere) const;
    PlaneIntersectionType Intersects(const Plane& plane) const;
    bool Intersects(const Ray& ray, float& rResult) const;

    //! @brief Tests for equality.
    bool operator==(const BoundingBox& rhs) const
    {
      return Min == rhs.Min && Max == rhs.Max;
    }

    //! @brief Tests for inequality.
    bool operator!=(const BoundingBox& rhs) const
    {
      return Min != rhs.Min || Max != rhs.Max;
    }
  };
}

#endif
