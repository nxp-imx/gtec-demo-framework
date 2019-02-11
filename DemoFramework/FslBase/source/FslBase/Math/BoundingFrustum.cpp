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

#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/PlaneHelper.hpp>
#include <FslBase/Math/Ray.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Fsl
{
  using namespace MatrixFields;

  namespace
  {
    Vector3 IntersectionPoint(const Plane& a, const Plane& b, const Plane& c)
    {
      // Formula used
      //                d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
      // P =   -------------------------------------------------------------------------
      //                             N1 . ( N2 * N3 )
      //
      // Note: N refers to the normal, d refers to the displacement. '.' means dot product. '*' means cross product

      Vector3 cross = Vector3::Cross(b.Normal, c.Normal);
      const float f = Vector3::Dot(a.Normal, cross) * -1.0f;
      const Vector3 v1 = Vector3::Cross(b.Normal, c.Normal) * a.D;
      const Vector3 v2 = Vector3::Cross(c.Normal, a.Normal) * b.D;
      const Vector3 v3 = Vector3::Cross(a.Normal, b.Normal) * c.D;

      return Vector3((v1.X + v2.X + v3.X) / f, (v1.Y + v2.Y + v3.Y) / f, (v1.Z + v2.Z + v3.Z) / f);
    }


    void NormalizePlane(Plane& rPlane)
    {
      float factor = 1.0f / rPlane.Normal.Length();
      rPlane.Normal.X *= factor;
      rPlane.Normal.Y *= factor;
      rPlane.Normal.Z *= factor;
      rPlane.D *= factor;
    }
  }


  void BoundingFrustum::SetMatrix(const Matrix& value)
  {
    m_matrix = value;
    CreatePlanes();     // FIXME: The odds are the planes will be used a lot more often than the matrix
    CreateCorners();    // is updated, so this should help performance. I hope ;)
  }


  BoundingFrustum::BoundingFrustum(const Matrix& value)
    : m_matrix(value)
  {
    CreatePlanes();
    CreateCorners();
  }


  ContainmentType BoundingFrustum::Contains(const BoundingBox& box) const
  {
    bool intersects = false;
    for (int32_t i = 0; i < PlaneCount; ++i)
    {
      switch (box.Intersects(m_planes[i]))
      {
      case PlaneIntersectionType::Front:
        return ContainmentType::Disjoint;
      case PlaneIntersectionType::Intersecting:
        intersects = true;
        break;
      default:
        break;
      }
    }
    return intersects ? ContainmentType::Intersects : ContainmentType::Contains;
  }


  ContainmentType BoundingFrustum::Contains(const BoundingFrustum& frustum) const
  {
    if (this == &frustum)    // We check to see if the two frustums are equal
    {
      return ContainmentType::Contains;    // If they are, there's no need to go any further.
    }

    bool intersects = false;
    for (int32_t i = 0; i < PlaneCount; ++i)
    {
      switch (frustum.Intersects(m_planes[i]))
      {
      case PlaneIntersectionType::Front:
        return ContainmentType::Disjoint;
      case PlaneIntersectionType::Intersecting:
        intersects = true;
        break;
      default:
        break;
      }
    }
    return intersects ? ContainmentType::Intersects : ContainmentType::Contains;
  }


  ContainmentType BoundingFrustum::Contains(const BoundingSphere& sphere) const
  {
    bool intersects = false;
    for (int32_t i = 0; i < PlaneCount; ++i)
    {
      // TODO: we might want to inline this for performance reasons
      switch (sphere.Intersects(m_planes[i]))
      {
      case PlaneIntersectionType::Front:
        return ContainmentType::Disjoint;
      case PlaneIntersectionType::Intersecting:
        intersects = true;
        break;
      default:
        break;
      }
    }
    return intersects ? ContainmentType::Intersects : ContainmentType::Contains;
  }


  ContainmentType BoundingFrustum::Contains(const Vector3& point) const
  {
    for (int32_t i = 0; i < PlaneCount; ++i)
    {
      if (PlaneHelper::ClassifyPoint(point, m_planes[i]) > 0)
      {
        return ContainmentType::Disjoint;
      }
    }
    return ContainmentType::Contains;
  }


  std::vector<Vector3> BoundingFrustum::GetCorners() const
  {
    std::vector<Vector3> result(CornerCount);
    GetCorners(result);
    return result;
  }


  void BoundingFrustum::GetCorners(std::array<Vector3, CornerCount>& rCorners) const
  {
    for (uint32_t i = 0; i < m_corners.size(); ++i)
    {
      rCorners[i] = m_corners[i];
    }
  }


  void BoundingFrustum::GetCorners(std::vector<Vector3>& rCorners) const
  {
    if (rCorners.size() < static_cast<uint32_t>(CornerCount))
    {
      throw std::invalid_argument("rCorners");
    }

    for (uint32_t i = 0; i < m_corners.size(); ++i)
    {
      rCorners[i] = m_corners[i];
    }
  }


  bool BoundingFrustum::Intersects(const BoundingBox& box) const
  {
    return Contains(box) != ContainmentType::Disjoint;
  }


  bool BoundingFrustum::Intersects(const BoundingFrustum& frustum) const
  {
    return Contains(frustum) != ContainmentType::Disjoint;
  }


  bool BoundingFrustum::Intersects(const BoundingSphere& sphere) const
  {
    return Contains(sphere) != ContainmentType::Disjoint;
  }


  PlaneIntersectionType BoundingFrustum::Intersects(const Plane& plane) const
  {
    auto result = plane.Intersects(m_corners[0]);
    for (uint32_t i = 1; i < m_corners.size(); ++i)
    {
      if (plane.Intersects(m_corners[i]) != result)
      {
        result = PlaneIntersectionType::Intersecting;
      }
    }
    return result;
  }


  bool BoundingFrustum::Intersects(const Ray& ray, float& rResult) const
  {
    rResult = 0.0f;
    switch (Contains(ray.Position))
    {
    case ContainmentType::Disjoint:
      return false;
    case ContainmentType::Contains:
      return true;
    case ContainmentType::Intersects:
    {
      // TODO: Needs additional test for not 0.0 and null results.
      float minVal = std::numeric_limits<float>::max();
      float maxVal = std::numeric_limits<float>::lowest();
      for (auto plane : m_planes)
      {
        const auto normal = plane.Normal;

        float result2 = Vector3::Dot(ray.Direction, normal);
        float result3 = Vector3::Dot(ray.Position, normal);

        result3 += plane.D;

        if (static_cast<double>(std::abs(result2)) < 9.99999974737875E-06)
        {
          if (static_cast<double>(result3) > 0.0)
          {
            return false;
          }
        }
        else
        {
          float result4 = -result3 / result2;
          if (static_cast<double>(result2) < 0.0)
          {
            if (static_cast<double>(result4) > static_cast<double>(maxVal))
            {
              return false;
            }
            if (static_cast<double>(result4) > static_cast<double>(minVal))
            {
              minVal = result4;
            }
          }
          else
          {
            if (static_cast<double>(result4) < static_cast<double>(minVal))
            {
              return false;
            }
            if (static_cast<double>(result4) < static_cast<double>(maxVal))
            {
              maxVal = result4;
            }
          }
        }

        float distance;
        if (ray.Intersects(plane, distance))
        {
          minVal = std::min(minVal, distance);
          maxVal = std::max(maxVal, distance);
        }
      }

      float temp = minVal >= 0.0 ? minVal : maxVal;
      if (temp < 0.0)
      {
        return false;
      }

      rResult = temp;
      return true;
    }
    default:
      throw std::invalid_argument("argument out of range");
    }
  }

  void BoundingFrustum::CreateCorners()
  {
    //-                              near         left         top
    m_corners[0] = IntersectionPoint(m_planes[0], m_planes[2], m_planes[4]);
    //-                              near         right        top
    m_corners[1] = IntersectionPoint(m_planes[0], m_planes[3], m_planes[4]);
    //-                              near         right        bottom
    m_corners[2] = IntersectionPoint(m_planes[0], m_planes[3], m_planes[5]);
    //-                              near         left         bottom
    m_corners[3] = IntersectionPoint(m_planes[0], m_planes[2], m_planes[5]);
    //-                              far          left         top
    m_corners[4] = IntersectionPoint(m_planes[1], m_planes[2], m_planes[4]);
    //-                              far          right        top
    m_corners[5] = IntersectionPoint(m_planes[1], m_planes[3], m_planes[4]);
    //-                              far          right        bottom
    m_corners[6] = IntersectionPoint(m_planes[1], m_planes[3], m_planes[5]);
    //-                              far          left         bottom
    m_corners[7] = IntersectionPoint(m_planes[1], m_planes[2], m_planes[5]);
  }


  void BoundingFrustum::CreatePlanes()
  {
    const float* mat = m_matrix.DirectAccess();

    m_planes[0] = Plane(-mat[_M13], -mat[_M23], -mat[_M33], -mat[_M43]);
    m_planes[1] = Plane(mat[_M13] - mat[_M14], mat[_M23] - mat[_M24], mat[_M33] - mat[_M34], mat[_M43] - mat[_M44]);
    m_planes[2] = Plane(-mat[_M14] - mat[_M11], -mat[_M24] - mat[_M21], -mat[_M34] - mat[_M31], -mat[_M44] - mat[_M41]);
    m_planes[3] = Plane(mat[_M11] - mat[_M14], mat[_M21] - mat[_M24], mat[_M31] - mat[_M34], mat[_M41] - mat[_M44]);
    m_planes[4] = Plane(mat[_M12] - mat[_M14], mat[_M22] - mat[_M24], mat[_M32] - mat[_M34], mat[_M42] - mat[_M44]);
    m_planes[5] = Plane(-mat[_M14] - mat[_M12], -mat[_M24] - mat[_M22], -mat[_M34] - mat[_M32], -mat[_M44] - mat[_M42]);

    NormalizePlane(m_planes[0]);
    NormalizePlane(m_planes[1]);
    NormalizePlane(m_planes[2]);
    NormalizePlane(m_planes[3]);
    NormalizePlane(m_planes[4]);
    NormalizePlane(m_planes[5]);
  }
}
