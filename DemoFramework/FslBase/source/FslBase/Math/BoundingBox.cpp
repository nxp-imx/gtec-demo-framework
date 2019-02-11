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

#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/Ray.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Fsl
{
  namespace
  {
    const Vector3 MaxVector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    const Vector3 MinVector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
  }


  ContainmentType BoundingBox::Contains(const BoundingBox& box) const
  {
    // test if all corner is in the same side of a face by just checking min and max
    if (box.Max.X < Min.X || box.Min.X > Max.X || box.Max.Y < Min.Y || box.Min.Y > Max.Y || box.Max.Z < Min.Z || box.Min.Z > Max.Z)
    {
      return ContainmentType::Disjoint;
    }
    if (box.Min.X >= Min.X && box.Max.X <= Max.X && box.Min.Y >= Min.Y && box.Max.Y <= Max.Y && box.Min.Z >= Min.Z && box.Max.Z <= Max.Z)
    {
      return ContainmentType::Contains;
    }


    return ContainmentType::Intersects;
  }


  // ContainmentType BoundingBox::Contains(const BoundingFrustum& frustum) const
  //{
  //  //TODO: bad done here need a fix.
  //  //Because question is not frustum contain box but reverse and this is not the same
  //  int i;
  //  ContainmentType contained;
  // FIX: Update this code so it doens't utilize dynamic memory
  //  const auto corners = frustum.GetCorners();

  //  // First we check if frustum is in box
  //  for (i = 0; i < corners.Length; i++)
  //  {
  //    Contains(corners[i], contained);
  //    if (contained == ContainmentType::Disjoint)
  //      break;
  //  }

  //  if (i == corners.Length) // This means we checked all the corners and they were all contain or instersect
  //    return ContainmentType::Contains;

  //  if (i != 0)             // if i is not equal to zero, we can fastpath and say that this box intersects
  //    return ContainmentType::Intersects;


  //  // If we get here, it means the first (and only) point we checked was actually contained in the frustum.
  //  // So we assume that all other points will also be contained. If one of the points is disjoint, we can
  //  // exit immediately saying that the result is Intersects
  //  i++;
  //  for (; i < corners.Length; i++)
  //  {
  //    Contains(corners[i], contained);
  //    if (contained != ContainmentType::Contains)
  //      return ContainmentType::Intersects;

  //  }

  //  // If we get here, then we know all the points were actually contained, therefore result is Contains
  //  return ContainmentType::Contains;
  //}


  ContainmentType BoundingBox::Contains(const BoundingSphere& sphere) const
  {
    if ((sphere.Center.X - Min.X) >= sphere.Radius && (sphere.Center.Y - Min.Y) >= sphere.Radius && (sphere.Center.Z - Min.Z) >= sphere.Radius &&
        (Max.X - sphere.Center.X) >= sphere.Radius && (Max.Y - sphere.Center.Y) >= sphere.Radius && (Max.Z - sphere.Center.Z) >= sphere.Radius)
    {
      return ContainmentType::Contains;
    }

    double dmin = 0;
    double e = sphere.Center.X - Min.X;
    if (e < 0)
    {
      if (e < -sphere.Radius)
      {
        return ContainmentType::Disjoint;
      }
      dmin += e * e;
    }
    else
    {
      e = sphere.Center.X - Max.X;
      if (e > 0)
      {
        if (e > sphere.Radius)
        {
          return ContainmentType::Disjoint;
        }
        dmin += e * e;
      }
    }

    e = sphere.Center.Y - Min.Y;
    if (e < 0)
    {
      if (e < -sphere.Radius)
      {
        return ContainmentType::Disjoint;
      }
      dmin += e * e;
    }
    else
    {
      e = sphere.Center.Y - Max.Y;
      if (e > 0)
      {
        if (e > sphere.Radius)
        {
          return ContainmentType::Disjoint;
        }
        dmin += e * e;
      }
    }

    e = sphere.Center.Z - Min.Z;
    if (e < 0)
    {
      if (e < -sphere.Radius)
      {
        return ContainmentType::Disjoint;
      }
      dmin += e * e;
    }
    else
    {
      e = sphere.Center.Z - Max.Z;
      if (e > 0)
      {
        if (e > sphere.Radius)
        {
          return ContainmentType::Disjoint;
        }
        dmin += e * e;
      }
    }

    if (dmin <= (sphere.Radius * sphere.Radius))
    {
      return ContainmentType::Intersects;
    }
    return ContainmentType::Disjoint;
  }


  ContainmentType BoundingBox::Contains(const Vector3& point) const
  {
    // first we get if point is out of box
    if (point.X < Min.X || point.X > Max.X || point.Y < Min.Y || point.Y > Max.Y || point.Z < Min.Z || point.Z > Max.Z)
    {
      return ContainmentType::Disjoint;
    }
    return ContainmentType::Contains;
  }


  BoundingBox BoundingBox::CreateFromPoints(const std::vector<Vector3>& points)
  {
    if (points.empty())
    {
      throw std::invalid_argument("You should have at least one point in points.");
    }

    auto minVec = MaxVector3;
    auto maxVec = MinVector3;
    for (auto point : points)
    {
      minVec.X = (minVec.X < point.X) ? minVec.X : point.X;
      minVec.Y = (minVec.Y < point.Y) ? minVec.Y : point.Y;
      minVec.Z = (minVec.Z < point.Z) ? minVec.Z : point.Z;

      maxVec.X = (maxVec.X > point.X) ? maxVec.X : point.X;
      maxVec.Y = (maxVec.Y > point.Y) ? maxVec.Y : point.Y;
      maxVec.Z = (maxVec.Z > point.Z) ? maxVec.Z : point.Z;
    }
    return BoundingBox(minVec, maxVec);
  }


  BoundingBox BoundingBox::CreateFromSphere(const BoundingSphere& sphere)
  {
    BoundingBox result;
    CreateFromSphere(sphere, result);
    return result;
  }


  void BoundingBox::CreateFromSphere(const BoundingSphere& sphere, BoundingBox& rResult)
  {
    const Vector3 corner(sphere.Radius, sphere.Radius, sphere.Radius);
    rResult = BoundingBox(sphere.Center - corner, sphere.Center + corner);
  }


  BoundingBox BoundingBox::CreateMerged(const BoundingBox& original, const BoundingBox& additional)
  {
    BoundingBox result;
    CreateMerged(original, additional, result);
    return result;
  }


  void BoundingBox::CreateMerged(const BoundingBox& original, const BoundingBox& additional, BoundingBox& rResult)
  {
    rResult.Min.X = std::min(original.Min.X, additional.Min.X);
    rResult.Min.Y = std::min(original.Min.Y, additional.Min.Y);
    rResult.Min.Z = std::min(original.Min.Z, additional.Min.Z);
    rResult.Max.X = std::max(original.Max.X, additional.Max.X);
    rResult.Max.Y = std::max(original.Max.Y, additional.Max.Y);
    rResult.Max.Z = std::max(original.Max.Z, additional.Max.Z);
  }


  std::vector<Vector3> BoundingBox::GetCorners() const
  {
    std::vector<Vector3> result(CornerCount);
    result[0] = Vector3(Min.X, Max.Y, Max.Z);
    result[1] = Vector3(Max.X, Max.Y, Max.Z);
    result[2] = Vector3(Max.X, Min.Y, Max.Z);
    result[3] = Vector3(Min.X, Min.Y, Max.Z);
    result[4] = Vector3(Min.X, Max.Y, Min.Z);
    result[5] = Vector3(Max.X, Max.Y, Min.Z);
    result[6] = Vector3(Max.X, Min.Y, Min.Z);
    result[7] = Vector3(Min.X, Min.Y, Min.Z);
    return result;
  }


  void BoundingBox::GetCorners(std::vector<Vector3>& corners) const
  {
    if (corners.size() < 8)
    {
      throw std::invalid_argument("Not enought room for all corners");
    }

    corners[0].X = Min.X;
    corners[0].Y = Max.Y;
    corners[0].Z = Max.Z;
    corners[1].X = Max.X;
    corners[1].Y = Max.Y;
    corners[1].Z = Max.Z;
    corners[2].X = Max.X;
    corners[2].Y = Min.Y;
    corners[2].Z = Max.Z;
    corners[3].X = Min.X;
    corners[3].Y = Min.Y;
    corners[3].Z = Max.Z;
    corners[4].X = Min.X;
    corners[4].Y = Max.Y;
    corners[4].Z = Min.Z;
    corners[5].X = Max.X;
    corners[5].Y = Max.Y;
    corners[5].Z = Min.Z;
    corners[6].X = Max.X;
    corners[6].Y = Min.Y;
    corners[6].Z = Min.Z;
    corners[7].X = Min.X;
    corners[7].Y = Min.Y;
    corners[7].Z = Min.Z;
  }

  void BoundingBox::GetCorners(std::array<Vector3, 8>& corners) const
  {
    corners[0].X = Min.X;
    corners[0].Y = Max.Y;
    corners[0].Z = Max.Z;
    corners[1].X = Max.X;
    corners[1].Y = Max.Y;
    corners[1].Z = Max.Z;
    corners[2].X = Max.X;
    corners[2].Y = Min.Y;
    corners[2].Z = Max.Z;
    corners[3].X = Min.X;
    corners[3].Y = Min.Y;
    corners[3].Z = Max.Z;
    corners[4].X = Min.X;
    corners[4].Y = Max.Y;
    corners[4].Z = Min.Z;
    corners[5].X = Max.X;
    corners[5].Y = Max.Y;
    corners[5].Z = Min.Z;
    corners[6].X = Max.X;
    corners[6].Y = Min.Y;
    corners[6].Z = Min.Z;
    corners[7].X = Min.X;
    corners[7].Y = Min.Y;
    corners[7].Z = Min.Z;
  }


  bool BoundingBox::Intersects(const BoundingBox& box) const
  {
    bool result;
    Intersects(box, result);
    return result;
  }


  void BoundingBox::Intersects(const BoundingBox& box, bool& rResult) const
  {
    if ((Max.X >= box.Min.X) && (Min.X <= box.Max.X))
    {
      if ((Max.Y < box.Min.Y) || (Min.Y > box.Max.Y))
      {
        rResult = false;
        return;
      }

      rResult = (Max.Z >= box.Min.Z) && (Min.Z <= box.Max.Z);
      return;
    }

    rResult = false;
  }


  bool BoundingBox::Intersects(const BoundingFrustum& frustum) const
  {
    return frustum.Intersects(*this);
  }


  bool BoundingBox::Intersects(const BoundingSphere& sphere) const
  {
    const double sphereRadius = sphere.Radius;
    const double sphereX = sphere.Center.X;
    const double sphereY = sphere.Center.Y;
    const double sphereZ = sphere.Center.Z;
    if ((sphereX - Min.X) > sphereRadius && (sphereY - Min.Y) > sphereRadius && (sphereZ - Min.Z) > sphereRadius &&
        (Max.X - sphereX) > sphereRadius && (Max.Y - sphereY) > sphereRadius && (Max.Z - sphereZ) > sphereRadius)
    {
      return true;
    }

    double dmin = 0.0;

    if (sphereX < Min.X)
    {
      dmin += (Min.X - sphereX) * (Min.X - sphereX);
    }
    else if (sphereX > Min.X)
    {
      dmin += (sphereX - Max.X) * (sphereX - Max.X);
    }

    if (sphereY < Min.Y)
    {
      dmin += (Min.Y - sphereY) * (Min.Y - sphereY);
    }
    else if (sphereY > Max.Y)
    {
      dmin += (sphereY - Max.Y) * (sphereY - Max.Y);
    }

    if (sphereZ < Min.Z)
    {
      dmin += (Min.Z - sphereY) * (Min.Z - sphereZ);
    }
    else if (sphereZ > Max.Z)
    {
      dmin += (sphereZ - Max.Z) * (sphereZ - Max.Z);
    }

    return (dmin <= (sphereRadius * sphereRadius));
  }


  PlaneIntersectionType BoundingBox::Intersects(const Plane& plane) const
  {
    // See http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html

    Vector3 positiveVertex;
    Vector3 negativeVertex;

    if (plane.Normal.X >= 0)
    {
      positiveVertex.X = Max.X;
      negativeVertex.X = Min.X;
    }
    else
    {
      positiveVertex.X = Min.X;
      negativeVertex.X = Max.X;
    }

    if (plane.Normal.Y >= 0)
    {
      positiveVertex.Y = Max.Y;
      negativeVertex.Y = Min.Y;
    }
    else
    {
      positiveVertex.Y = Min.Y;
      negativeVertex.Y = Max.Y;
    }

    if (plane.Normal.Z >= 0)
    {
      positiveVertex.Z = Max.Z;
      negativeVertex.Z = Min.Z;
    }
    else
    {
      positiveVertex.Z = Min.Z;
      negativeVertex.Z = Max.Z;
    }

    // Inline Vector3.Dot(plane.Normal, negativeVertex) + plane.D;
    auto distance = plane.Normal.X * negativeVertex.X + plane.Normal.Y * negativeVertex.Y + plane.Normal.Z * negativeVertex.Z + plane.D;
    if (distance > 0)
    {
      return PlaneIntersectionType::Front;
    }

    // Inline Vector3.Dot(plane.Normal, positiveVertex) + plane.D;
    distance = plane.Normal.X * positiveVertex.X + plane.Normal.Y * positiveVertex.Y + plane.Normal.Z * positiveVertex.Z + plane.D;
    if (distance < 0)
    {
      return PlaneIntersectionType::Back;
    }

    return PlaneIntersectionType::Intersecting;
  }


  bool BoundingBox::Intersects(const Ray& ray, float& rResult) const
  {
    return ray.Intersects(*this, rResult);
  }
}
