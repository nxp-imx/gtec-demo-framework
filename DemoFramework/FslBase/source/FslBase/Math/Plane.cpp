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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Fsl
{
  Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
  {
    Vector3 ab = b - a;
    Vector3 ac = c - a;

    Vector3 cross = Vector3::Cross(ab, ac);
    Normal = Vector3::Normalize(cross);
    D = -(Vector3::Dot(Normal, a));
  }


  float Plane::Dot(const Vector4& value) const
  {
    return ((((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z)) + (D * value.W));
  }


  void Plane::Dot(const Vector4& value, float& rResult) const
  {
    rResult = (((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z)) + (D * value.W);
  }


  float Plane::DotCoordinate(const Vector3& value) const
  {
    return ((((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z)) + D);
  }


  void Plane::DotCoordinate(const Vector3& value, float& rResult) const
  {
    rResult = (((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z)) + D;
  }


  float Plane::DotNormal(const Vector3& value) const
  {
    return (((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z));
  }


  void Plane::DotNormal(const Vector3& value, float& rResult) const
  {
    rResult = ((Normal.X * value.X) + (Normal.Y * value.Y)) + (Normal.Z * value.Z);
  }


  Plane Plane::Transform(const Plane& plane, const Matrix& matrix)
  {
    Plane result;
    Transform(plane, matrix, result);
    return result;
  }


  void Plane::Transform(const Plane& plane, const Matrix& matrix, Plane& rResult)
  {
    // See "Transforming Normals" in http://www.glprogramming.com/red/appendixf.html
    // for an explanation of how this works.
    Matrix transformedMatrix;
    Matrix::Invert(matrix, transformedMatrix);
    // dumb cast necessary until we remove the deprecated static functions
    Matrix::Transpose(static_cast<const Matrix>(transformedMatrix), transformedMatrix);

    auto vector = Vector4(plane.Normal, plane.D);
    Vector4 transformedVector;
    Vector4::Transform(vector, transformedMatrix, transformedVector);
    rResult = Plane(transformedVector);
  }


  Plane Plane::Transform(const Plane& plane, const Quaternion& rotation)
  {
    Plane result;
    Transform(plane, rotation, result);
    return result;
  }


  void Plane::Transform(const Plane& plane, const Quaternion& rotation, Plane& rResult)
  {
    rResult.Normal = Vector3::Transform(plane.Normal, rotation);
    rResult.D = plane.D;
  }


  void Plane::Normalize()
  {
    const Vector3 normal = Vector3::Normalize(Normal);
    const float factor = static_cast<float>(std::sqrt(Normal.X * Normal.X + Normal.Y * Normal.Y + Normal.Z * Normal.Z)) /
                         static_cast<float>(std::sqrt(normal.X * normal.X + normal.Y * normal.Y + normal.Z * normal.Z));
    D = D * factor;
  }


  Plane Plane::Normalize(const Plane& value)
  {
    Plane ret;
    Normalize(value, ret);
    return ret;
  }


  void Plane::Normalize(const Plane& value, Plane& rResult)
  {
    rResult.Normal = Vector3::Normalize(value.Normal);
    const float factor =
      static_cast<float>(std::sqrt(rResult.Normal.X * rResult.Normal.X + rResult.Normal.Y * rResult.Normal.Y + rResult.Normal.Z * rResult.Normal.Z)) /
      static_cast<float>(std::sqrt(value.Normal.X * value.Normal.X + value.Normal.Y * value.Normal.Y + value.Normal.Z * value.Normal.Z));
    rResult.D = value.D * factor;
  }


  PlaneIntersectionType Plane::Intersects(const BoundingBox& box) const
  {
    return box.Intersects(*this);
  }


  PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const
  {
    return frustum.Intersects(*this);
  }


  PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const
  {
    return sphere.Intersects(*this);
  }


  PlaneIntersectionType Plane::Intersects(const Vector3& point) const
  {
    const float distance = DotCoordinate(point);
    if (distance > 0)
    {
      return PlaneIntersectionType::Front;
    }
    if (distance < 0)
    {
      return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
  }
}
