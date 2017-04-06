/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

// The functions in this file are a port of an MIT licensed library: MonaGame - BoundingSphere.cs.

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

#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Plane.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include "MatrixFields.hpp"

namespace Fsl
{

  BoundingSphere BoundingSphere::Transform(const Matrix& matrix)
  {
    const float* mat = matrix.DirectAccess();
    BoundingSphere sphere;
    sphere.Center = Vector3::Transform(Center, matrix);
    sphere.Radius = Radius * ((float)std::sqrt((double)std::max(((mat[_M11] * mat[_M11]) + (mat[_M12] * mat[_M12])) + (mat[_M13] * mat[_M13]), std::max(((mat[_M21] * mat[_M21]) + (mat[_M22] * mat[_M22])) + (mat[_M23] * mat[_M23]), ((mat[_M31] * mat[_M31]) + (mat[_M32] * mat[_M32])) + (mat[_M33] * mat[_M33])))));
    return sphere;
  }


  void BoundingSphere::Transform(const Matrix& matrix, BoundingSphere& rResult)
  {
    const float* mat = matrix.DirectAccess();
    rResult.Center = Vector3::Transform(Center, matrix);
    rResult.Radius = Radius * ((float)std::sqrt((double)std::max(((mat[_M11] * mat[_M11]) + (mat[_M12] * mat[_M12])) + (mat[_M13] * mat[_M13]), std::max(((mat[_M21] * mat[_M21]) + (mat[_M22] * mat[_M22])) + (mat[_M23] * mat[_M23]), ((mat[_M31] * mat[_M31]) + (mat[_M32] * mat[_M32])) + (mat[_M33] * mat[_M33])))));
  }

  ContainmentType BoundingSphere::Contains(const BoundingBox& box) const
  {
    //check if all corner is in sphere
    bool inside = true;
    // FIX: Update this code so it doens't utilize dynamic memory
    const auto corners = box.GetCorners();
    for (uint32_t i = 0; i < corners.size(); ++i)
    {
      if (Contains(corners[i]) == ContainmentType::Disjoint)
      {
        inside = false;
        break;
      }
    }

    if (inside)
      return ContainmentType::Contains;

    //check if the distance from sphere center to cube face < radius
    double dmin = 0;

    if (Center.X < box.Min.X)
      dmin += (Center.X - box.Min.X) * (Center.X - box.Min.X);
    else if (Center.X > box.Max.X)
      dmin += (Center.X - box.Max.X) * (Center.X - box.Max.X);

    if (Center.Y < box.Min.Y)
      dmin += (Center.Y - box.Min.Y) * (Center.Y - box.Min.Y);
    else if (Center.Y > box.Max.Y)
      dmin += (Center.Y - box.Max.Y) * (Center.Y - box.Max.Y);

    if (Center.Z < box.Min.Z)
      dmin += (Center.Z - box.Min.Z) * (Center.Z - box.Min.Z);
    else if (Center.Z > box.Max.Z)
      dmin += (Center.Z - box.Max.Z) * (Center.Z - box.Max.Z);

    if (dmin <= Radius * Radius)
      return ContainmentType::Intersects;

    //else disjoint
    return ContainmentType::Disjoint;
  }

  void BoundingSphere::Contains(const BoundingBox& box, ContainmentType& rResult) const
  {
    rResult = Contains(box);
  }

  ContainmentType BoundingSphere::Contains(const BoundingFrustum& frustum) const
  {
    //check if all corner is in sphere
    bool inside = true;

    // FIX: Update this code so it doens't utilize dynamic memory
    const auto corners = frustum.GetCorners();
    for (uint32_t i = 0; i < corners.size(); ++i)
    {
      if (Contains(corners[i]) == ContainmentType::Disjoint)
      {
        inside = false;
        break;
      }
    }
    if (inside)
      return ContainmentType::Contains;

    //check if the distance from sphere center to frustum face < radius
    double dmin = 0;
    //TODO : calcul dmin

    if (dmin <= Radius * Radius)
      return ContainmentType::Intersects;

    //else disjoint
    return ContainmentType::Disjoint;
  }


  ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
  {
    ContainmentType result;
    Contains(sphere, result);
    return result;
  }


  void BoundingSphere::Contains(const BoundingSphere& sphere, ContainmentType& rResult) const
  {
    const float sqDistance = Vector3::DistanceSquared(sphere.Center, Center);
    if (sqDistance > (sphere.Radius + Radius) * (sphere.Radius + Radius))
      rResult = ContainmentType::Disjoint;
    else if (sqDistance <= (Radius - sphere.Radius) * (Radius - sphere.Radius))
      rResult = ContainmentType::Contains;
    else
      rResult = ContainmentType::Intersects;
  }


  ContainmentType BoundingSphere::Contains(const Vector3& point) const
  {
    ContainmentType result;
    Contains(point, result);
    return result;
  }


  void BoundingSphere::Contains(const Vector3& point, ContainmentType& rResult) const
  {
    const float sqRadius = Radius * Radius;
    const float sqDistance = Vector3::DistanceSquared(point, Center);
    if (sqDistance > sqRadius)
      rResult = ContainmentType::Disjoint;
    else if (sqDistance < sqRadius)
      rResult = ContainmentType::Contains;
    else
      rResult = ContainmentType::Intersects;
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
    const Vector3 center((box.Min.X + box.Max.X) / 2.0f,
      (box.Min.Y + box.Max.Y) / 2.0f,
      (box.Min.Z + box.Max.Z) / 2.0f);

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
    if (points.size() <= 0)
      throw std::invalid_argument("You should have at least one point in points.");

    // From "Real-Time Collision Detection" (Page 89)

    Vector3 minx(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vector3 maxx(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    auto miny = minx;
    auto maxy = maxx;
    auto minz = minx;
    auto maxz = maxx;

    // Find the most extreme points along the principle axis.
    for (uint32_t i = 0; i < points.size(); ++i)
    {
      if (points[i].X < minx.X)
        minx = points[i];
      if (points[i].X > maxx.X)
        maxx = points[i];
      if (points[i].Y < miny.Y)
        miny = points[i];
      if (points[i].Y > maxy.Y)
        maxy = points[i];
      if (points[i].Z < minz.Z)
        minz = points[i];
      if (points[i].Z > maxz.Z)
        maxz = points[i];
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
    for (uint32_t i = 0; i < points.size(); ++i)
    {
      const Vector3 diff = (points[i] - center);
      const float sqDist = diff.LengthSquared();
      if (sqDist > sqRadius)
      {
        const float distance = std::sqrt(sqDist); // equal to diff.Length();
        Vector3 direction = diff / distance;
        Vector3 G = center - radius * direction;
        center = (G + points[i]) / 2;
        radius = Vector3::Distance(points[i], center);
        sqRadius = radius * radius;
      }
    }
    return BoundingSphere(center, radius);
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
    if (distance <= original.Radius + additional.Radius)//intersect
    {
      if (distance <= original.Radius - additional.Radius)//original contain additional
      {
        rResult = original;
        return;
      }
      if (distance <= additional.Radius - original.Radius)//additional contain original
      {
        rResult = additional;
        return;
      }
    }
    //else find center of new sphere and radius
    const float leftRadius = std::max(original.Radius - distance, additional.Radius);
    const float Rightradius = std::max(original.Radius + distance, additional.Radius);
    ocenterToaCenter = ocenterToaCenter + (((leftRadius - Rightradius) / (2 * ocenterToaCenter.Length())) * ocenterToaCenter);//

    rResult = BoundingSphere(original.Center + ocenterToaCenter, (leftRadius + Rightradius) / 2.0f);
  }


  bool BoundingSphere::Intersects(const BoundingBox& box) const
  {
    return box.Intersects(*this);
  }


  void BoundingSphere::Intersects(const BoundingBox& box, bool& rResult) const
  {
    box.Intersects(*this, rResult);
  }


  bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
  {
    bool result;
    Intersects(sphere, result);
    return result;
  }


  void BoundingSphere::Intersects(const BoundingSphere& sphere, bool& rResult) const
  {
    const float sqDistance = Vector3::DistanceSquared(sphere.Center, Center);
    rResult = (sqDistance <= (sphere.Radius + Radius) * (sphere.Radius + Radius));
  }


  PlaneIntersectionType BoundingSphere::Intersects(const Plane& plane) const
  {
    PlaneIntersectionType result;
    Intersects(plane, result);
    return result;
  }

  void BoundingSphere::Intersects(const Plane& plane, PlaneIntersectionType& rResult) const
  {
    auto distance = Vector3::Dot(plane.Normal, Center);
    distance += plane.D;
    if (distance > Radius)
      rResult = PlaneIntersectionType::Front;
    else if (distance < -Radius)
      rResult = PlaneIntersectionType::Back;
    else
      rResult = PlaneIntersectionType::Intersecting;
  }


  bool BoundingSphere::Intersects(const Ray& ray, float& rResult) const
  {
    return ray.Intersects(*this, rResult);
  }



}
