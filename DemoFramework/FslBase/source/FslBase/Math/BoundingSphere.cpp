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

#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Plane.hpp>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Fsl
{
  using namespace MatrixFields;


  BoundingSphere BoundingSphere::Transform(const Matrix& matrix)
  {
    const float* mat = matrix.DirectAccess();
    BoundingSphere sphere;
    sphere.Center = Vector3::Transform(Center, matrix);
    sphere.Radius =
      Radius * (static_cast<float>(
                 std::sqrt(static_cast<double>(std::max(((mat[_M11] * mat[_M11]) + (mat[_M12] * mat[_M12])) + (mat[_M13] * mat[_M13]),
                                                        std::max(((mat[_M21] * mat[_M21]) + (mat[_M22] * mat[_M22])) + (mat[_M23] * mat[_M23]),
                                                                 ((mat[_M31] * mat[_M31]) + (mat[_M32] * mat[_M32])) + (mat[_M33] * mat[_M33])))))));
    return sphere;
  }


  void BoundingSphere::Transform(const Matrix& matrix, BoundingSphere& rResult)
  {
    const float* mat = matrix.DirectAccess();
    rResult.Center = Vector3::Transform(Center, matrix);
    rResult.Radius =
      Radius * (static_cast<float>(
                 std::sqrt(static_cast<double>(std::max(((mat[_M11] * mat[_M11]) + (mat[_M12] * mat[_M12])) + (mat[_M13] * mat[_M13]),
                                                        std::max(((mat[_M21] * mat[_M21]) + (mat[_M22] * mat[_M22])) + (mat[_M23] * mat[_M23]),
                                                                 ((mat[_M31] * mat[_M31]) + (mat[_M32] * mat[_M32])) + (mat[_M33] * mat[_M33])))))));
  }

  ContainmentType BoundingSphere::Contains(const BoundingBox& box) const
  {
    // check if all corner is in sphere
    bool inside = true;
    std::array<Vector3, 8> corners;
    box.GetCorners(corners);
    for (auto corner : corners)
    {
      if (Contains(corner) == ContainmentType::Disjoint)
      {
        inside = false;
        break;
      }
    }

    if (inside)
    {
      return ContainmentType::Contains;
    }

    // check if the distance from sphere center to cube face < radius
    double dmin = 0;

    if (Center.X < box.Min.X)
    {
      dmin += (Center.X - box.Min.X) * (Center.X - box.Min.X);
    }
    else if (Center.X > box.Max.X)
    {
      dmin += (Center.X - box.Max.X) * (Center.X - box.Max.X);
    }

    if (Center.Y < box.Min.Y)
    {
      dmin += (Center.Y - box.Min.Y) * (Center.Y - box.Min.Y);
    }
    else if (Center.Y > box.Max.Y)
    {
      dmin += (Center.Y - box.Max.Y) * (Center.Y - box.Max.Y);
    }

    if (Center.Z < box.Min.Z)
    {
      dmin += (Center.Z - box.Min.Z) * (Center.Z - box.Min.Z);
    }
    else if (Center.Z > box.Max.Z)
    {
      dmin += (Center.Z - box.Max.Z) * (Center.Z - box.Max.Z);
    }

    if (dmin <= Radius * Radius)
    {
      return ContainmentType::Intersects;
    }

    // else disjoint
    return ContainmentType::Disjoint;
  }


  ContainmentType BoundingSphere::Contains(const BoundingFrustum& frustum) const
  {
    // check if all corner is in sphere
    bool inside = true;

    std::array<Vector3, 8> corners;
    frustum.GetCorners(corners);
    for (auto corner : corners)
    {
      if (Contains(corner) == ContainmentType::Disjoint)
      {
        inside = false;
        break;
      }
    }
    if (inside)
    {
      return ContainmentType::Contains;
    }

    // check if the distance from sphere center to frustum face < radius
    double dmin = 0;
    // TODO: calcul dmin

    if (dmin <= Radius * Radius)
    {
      return ContainmentType::Intersects;
    }

    // else disjoint
    return ContainmentType::Disjoint;
  }


  ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
  {
    const float sqDistance = Vector3::DistanceSquared(sphere.Center, Center);
    if (sqDistance > ((sphere.Radius + Radius) * (sphere.Radius + Radius)))
    {
      return ContainmentType::Disjoint;
    }
    if (sqDistance <= ((Radius - sphere.Radius) * (Radius - sphere.Radius)) && Radius >= sphere.Radius)
    {
      return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
  }


  ContainmentType BoundingSphere::Contains(const Vector3& point) const
  {
    const float sqRadius = Radius * Radius;
    const float sqDistance = Vector3::DistanceSquared(point, Center);
    if (sqDistance > sqRadius)
    {
      return ContainmentType::Disjoint;
    }
    if (sqDistance < sqRadius)
    {
      return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
  }


  BoundingSphere BoundingSphere::CreateFromBoundingBox(const BoundingBox& box)
  {
    BoundingSphere result;
    CreateFromBoundingBox(box, result);
    return result;
  }


  void BoundingSphere::CreateFromBoundingBox(const BoundingBox& box, BoundingSphere& rResult)
  {
    // Find the center of the box.
    const Vector3 center((box.Min.X + box.Max.X) / 2.0f, (box.Min.Y + box.Max.Y) / 2.0f, (box.Min.Z + box.Max.Z) / 2.0f);

    // Find the distance between the center and one of the corners of the box.
    const float radius = Vector3::Distance(center, box.Max);
    rResult = BoundingSphere(center, radius);
  }


  BoundingSphere BoundingSphere::CreateFromFrustum(const BoundingFrustum& frustum)
  {
    return BoundingSphere::CreateFromPoints(frustum.GetCorners());
  }


  BoundingSphere BoundingSphere::CreateFromPoints(const std::vector<Vector3>& points)
  {
    if (points.empty())
    {
      throw std::invalid_argument("You should have at least one point in points.");
    }

    // From "Real-Time Collision Detection" (Page 89)

    Vector3 minx(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vector3 maxx(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    auto miny = minx;
    auto maxy = maxx;
    auto minz = minx;
    auto maxz = maxx;

    // Find the most extreme points along the principle axis.
    for (auto point : points)
    {
      if (point.X < minx.X)
      {
        minx = point;
      }
      if (point.X > maxx.X)
      {
        maxx = point;
      }
      if (point.Y < miny.Y)
      {
        miny = point;
      }
      if (point.Y > maxy.Y)
      {
        maxy = point;
      }
      if (point.Z < minz.Z)
      {
        minz = point;
      }
      if (point.Z > maxz.Z)
      {
        maxz = point;
      }
    }

    const auto sqDistX = Vector3::DistanceSquared(maxx, minx);
    const auto sqDistY = Vector3::DistanceSquared(maxy, miny);
    const auto sqDistZ = Vector3::DistanceSquared(maxz, minz);

    // Pick the pair of most distant points.
    auto min = minx;
    auto max = maxx;
    if (sqDistY > sqDistX && sqDistY > sqDistZ)
    {
      max = maxy;
      min = miny;
    }
    if (sqDistZ > sqDistX && sqDistZ > sqDistY)
    {
      max = maxz;
      min = minz;
    }

    auto center = (min + max) * 0.5f;
    auto radius = Vector3::Distance(max, center);

    // Test every point and expand the sphere.
    // The current bounding sphere is just a good approximation and may not enclose all points.
    // From: Mathematics for 3D Game Programming and Computer Graphics, Eric Lengyel, Third Edition.
    // Page 218
    float sqRadius = radius * radius;
    for (auto point : points)
    {
      const Vector3 diff = (point - center);
      const float sqDist = diff.LengthSquared();
      if (sqDist > sqRadius)
      {
        const float distance = std::sqrt(sqDist);    // equal to diff.Length();
        Vector3 direction = diff / distance;
        Vector3 G = center - radius * direction;
        center = (G + point) / 2;
        radius = Vector3::Distance(point, center);
        sqRadius = radius * radius;
      }
    }
    return {center, radius};
  }


  BoundingSphere BoundingSphere::CreateMerged(const BoundingSphere& original, const BoundingSphere& additional)
  {
    BoundingSphere result;
    CreateMerged(original, additional, result);
    return result;
  }


  void BoundingSphere::CreateMerged(const BoundingSphere& original, const BoundingSphere& additional, BoundingSphere& rResult)
  {
    Vector3 ocenterToaCenter = additional.Center - original.Center;
    const float distance = ocenterToaCenter.Length();
    if (distance <= original.Radius + additional.Radius)    // intersect
    {
      if (distance <= original.Radius - additional.Radius)    // original contain additional
      {
        rResult = original;
        return;
      }
      if (distance <= additional.Radius - original.Radius)    // additional contain original
      {
        rResult = additional;
        return;
      }
    }
    // else find center of new sphere and radius
    const float leftRadius = std::max(original.Radius - distance, additional.Radius);
    const float Rightradius = std::max(original.Radius + distance, additional.Radius);
    ocenterToaCenter = ocenterToaCenter + (((leftRadius - Rightradius) / (2 * ocenterToaCenter.Length())) * ocenterToaCenter);    //

    rResult = BoundingSphere(original.Center + ocenterToaCenter, (leftRadius + Rightradius) / 2.0f);
  }


  bool BoundingSphere::Intersects(const BoundingBox& box) const
  {
    return box.Intersects(*this);
  }


  bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
  {
    const float sqDistance = Vector3::DistanceSquared(sphere.Center, Center);
    return (sqDistance <= (sphere.Radius + Radius) * (sphere.Radius + Radius));
  }


  PlaneIntersectionType BoundingSphere::Intersects(const Plane& plane) const
  {
    auto distance = Vector3::Dot(plane.Normal, Center);
    distance += plane.D;
    if (distance > Radius)
    {
      return PlaneIntersectionType::Front;
    }
    if (distance < -Radius)
    {
      return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
  }


  bool BoundingSphere::Intersects(const Ray& ray, float& rResult) const
  {
    return ray.Intersects(*this, rResult);
  }
}
