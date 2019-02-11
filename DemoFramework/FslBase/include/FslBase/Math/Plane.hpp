#ifndef FSLBASE_MATH_PLANE_HPP
#define FSLBASE_MATH_PLANE_HPP
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
#include <FslBase/Math/PlaneIntersectionType.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>

namespace Fsl
{
  struct BoundingBox;
  class BoundingFrustum;
  struct BoundingSphere;
  struct Matrix;
  struct Quaternion;

  struct Plane
  {
    Vector3 Normal;
    float D{0};

    Plane() = default;

    Plane(const Vector4& value)
      : Normal(value.X, value.Y, value.Z)
      , D(value.W)
    {
    }

    Plane(const Vector3& normal, const float d)
      : Normal(normal)
      , D(d)
    {
    }

    Plane(const Vector3& a, const Vector3& b, const Vector3& c);

    Plane(const float a, const float b, const float c, const float d)
      : Normal(a, b, c)
      , D(d)
    {
    }

    float Dot(const Vector4& value) const;
    void Dot(const Vector4& value, float& rResult) const;
    float DotCoordinate(const Vector3& value) const;
    void DotCoordinate(const Vector3& value, float& rResult) const;
    float DotNormal(const Vector3& value) const;
    void DotNormal(const Vector3& value, float& rResult) const;

    //! @brief Transforms a normalized plane by a matrix.
    //! @param plane The normalized plane to transform.
    //! @param matrix The transformation matrix.
    //! @return The transformed plane.
    static Plane Transform(const Plane& plane, const Matrix& matrix);

    //! @brief Transforms a normalized plane by a matrix.
    //! @param plane The normalized plane to transform.
    //! @param matrix The transformation matrix.
    //! @param result The transformed plane.
    static void Transform(const Plane& plane, const Matrix& matrix, Plane& rResult);

    //! @brief Transforms a normalized plane by a quaternion rotation.
    //! @param plane The normalized plane to transform.
    //! @param rotation The quaternion rotation.
    //! @return The transformed plane.
    static Plane Transform(const Plane& plane, const Quaternion& rotation);

    //! @brief Transforms a normalized plane by a quaternion rotation.
    //! @param plane The normalized plane to transform.
    //! @param rotation The quaternion rotation.
    //! @param rResult The transformed plane.
    static void Transform(const Plane& plane, const Quaternion& rotation, Plane& rResult);

    void Normalize();
    static Plane Normalize(const Plane& value);
    static void Normalize(const Plane& value, Plane& rResult);

    PlaneIntersectionType Intersects(const BoundingBox& box) const;
    PlaneIntersectionType Intersects(const BoundingFrustum& frustum) const;
    PlaneIntersectionType Intersects(const BoundingSphere& sphere) const;
    PlaneIntersectionType Intersects(const Vector3& point) const;

    //! @brief Tests for equality.
    bool operator==(const Plane& rhs) const
    {
      return Normal == rhs.Normal && D == rhs.D;
    }

    //! @brief Tests for inequality.
    bool operator!=(const Plane& rhs) const
    {
      return Normal != rhs.Normal || D != rhs.D;
    }
  };
}

#endif
