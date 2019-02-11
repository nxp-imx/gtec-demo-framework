#ifndef FSLBASE_MATH_BOUNDINGFRUSTUM_HPP
#define FSLBASE_MATH_BOUNDINGFRUSTUM_HPP
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

// The functions in this file are a port of an MIT licensed library: MonoGame - BoundingFrustum.cs.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/ContainmentType.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Plane.hpp>
#include <array>
#include <vector>

namespace Fsl
{
  struct Ray;

  class BoundingFrustum
  {
  public:
    //! The number of planes in the frustum.
    static constexpr const int32_t PlaneCount = 6;

    //! The number of corner points in the frustum.
    static constexpr const int32_t CornerCount = 8;

  private:
    Matrix m_matrix;

    // Corners
    // 0 | near | left  | top
    // 1 | near | right | top
    // 2 | near | right | bottom
    // 3 | near | left  | bottom
    // 4 | far  | left  | top
    // 5 | far  | right | top
    // 6 | far  | right | bottom
    // 7 | far  | left  | bottom
    std::array<Vector3, CornerCount> m_corners;
    std::array<Plane, PlaneCount> m_planes;

  public:
    Matrix GetMatrix() const
    {
      return m_matrix;
    }

    void SetMatrix(const Matrix& value);

    //! @brief Gets the near plane of the frustum.
    Plane Near() const
    {
      return m_planes[0];
    }

    //! @brief Gets the far plane of the frustum.
    Plane Far() const
    {
      return m_planes[1];
    }

    //! @brief Gets the left plane of the frustum.
    Plane Left() const
    {
      return m_planes[2];
    }

    //! @brief Gets the right plane of the frustum.
    Plane Right() const
    {
      return m_planes[3];
    }

    //! @brief Gets the top plane of the frustum.
    Plane Top() const
    {
      return m_planes[4];
    }

    //! @brief Gets the bottom plane of the frustum.
    Plane Bottom() const
    {
      return m_planes[5];
    }

    //! @brief Creates a new instance of BoundingFrustum
    //! @param value Combined matrix which usually is (View * Projection).
    BoundingFrustum(const Matrix& value);

    //! @brief Containment test between this BoundingFrustum and specified BoundingBox.
    //! @param box A BoundingBox for testing.
    //! @return Result of testing for containment between this BoundingFrustum and specified BoundingBox.
    ContainmentType Contains(const BoundingBox& box) const;

    //! @brief Containment test between this BoundingFrustum and specified BoundingFrustum.
    //! @param frustum A BoundingFrustum for testing.
    //! @return Result of testing for containment between this BoundingFrustum and specified BoundingFrustum.
    ContainmentType Contains(const BoundingFrustum& frustum) const;

    //! @brief Containment test between this BoundingFrustum and specified BoundingSphere.
    //! @param sphere A BoundingSphere for testing.
    //! @return Result of testing for containment between this BoundingFrustumand specified BoundingSphere.
    ContainmentType Contains(const BoundingSphere& sphere) const;

    //! @brief Containment test between this BoundingFrustum and specified Vector3.
    //! @param point A Vector3 for testing.
    //! @return Result of testing for containment between this BoundingFrustum and specified Vector3.
    ContainmentType Contains(const Vector3& point) const;

    //! @brief Returns a copy of internal corners array.
    //! @return The array of corners.
    std::vector<Vector3> GetCorners() const;

    //! @brief Returns a copy of internal corners array.
    //! @param corners The array which values will be replaced to corner values of this instance. It must have size of BoundingFrustum.CornerCount.
    void GetCorners(std::array<Vector3, CornerCount>& rCorners) const;

    //! @brief Returns a copy of internal corners array.
    //! @param corners The array which values will be replaced to corner values of this instance. It must have size of BoundingFrustum.CornerCount.
    void GetCorners(std::vector<Vector3>& rCorners) const;

    //! @brief Gets whether or not a specified BoundingBox intersects with this BoundingFrustum.
    //! @param box A BoundingBox for intersection test.
    //! @return true if specified BoundingBox intersects with this BoundingFrustum; false otherwise.
    bool Intersects(const BoundingBox& box) const;

    //! @brief Gets whether or not a specified BoundingFrustum intersects with this BoundingFrustum.
    //! @param frustum An other BoundingFrustum for intersection test.
    //! @return true if other BoundingFrustum intersects with this BoundingFrustum; false otherwise.
    bool Intersects(const BoundingFrustum& frustum) const;

    //! @brief Gets whether or not a specified BoundingSphere intersects with this BoundingFrustum.
    //! @param sphere A BoundingSphere for intersection test.
    //! @return true if specified BoundingSphere intersects with this BoundingFrustum; false otherwise.
    bool Intersects(const BoundingSphere& sphere) const;

    //! @brief Gets type of intersection between specified Plane and this BoundingFrustum.
    //! @param plane A Plane for intersection test.
    //! @return A plane intersection type.
    PlaneIntersectionType Intersects(const Plane& plane) const;

    //! @brief Gets the distance of intersection of Ray and this BoundingFrustum or false if no intersection happens.
    //! @param ray A Ray for intersection test.
    //! @param rResult Distance at which ray intersects with this BoundingFrustum (if this returns true).
    //! @param true if a intersection happens; false if no intersection happens.
    bool Intersects(const Ray& ray, float& rResult) const;

    //! @brief Tests for equality.
    bool operator==(const BoundingFrustum& rhs) const
    {
      return m_matrix == rhs.m_matrix;
    }

    //! @brief Tests for inequality.
    bool operator!=(const BoundingFrustum& rhs) const
    {
      return m_matrix != rhs.m_matrix;
    }

  private:
    void CreateCorners();
    void CreatePlanes();
  };
}

#endif
