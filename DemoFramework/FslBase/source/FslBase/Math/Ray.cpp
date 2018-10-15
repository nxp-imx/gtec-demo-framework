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

#include <FslBase/Math/Ray.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Plane.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Fsl
{
  // adapted from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-box-intersection/
  bool Ray::Intersects(const BoundingBox& box, float& rResult) const
  {
    const float Epsilon = 1e-6f;

    bool hasMin = false;
    bool hasMax = false;
    float tMin = 0.0f;
    float tMax = 0.0f;

    if (std::abs(Direction.X) < Epsilon)
    {
      if (Position.X < box.Min.X || Position.X > box.Max.X)
      {
        rResult = 0.0f;
        return false;
      }
    }
    else
    {
      tMin = (box.Min.X - Position.X) / Direction.X;
      tMax = (box.Max.X - Position.X) / Direction.X;
      hasMin = true;
      hasMax = true;

      if (tMin > tMax)
      {
        auto temp = tMin;
        tMin = tMax;
        tMax = temp;
      }
    }

    if (std::abs(Direction.Y) < Epsilon)
    {
      if (Position.Y < box.Min.Y || Position.Y > box.Max.Y)
      {
        rResult = 0.0f;
        return false;
      }
    }
    else
    {
      auto tMinY = (box.Min.Y - Position.Y) / Direction.Y;
      auto tMaxY = (box.Max.Y - Position.Y) / Direction.Y;

      if (tMinY > tMaxY)
      {
        auto temp = tMinY;
        tMinY = tMaxY;
        tMaxY = temp;
      }

      if ((hasMin && tMin > tMaxY) || (hasMax && tMinY > tMax))
      {
        rResult = 0.0f;
        return false;
      }

      if (!hasMin || tMinY > tMin)
      {
        tMin = tMinY;
        hasMin = true;
      }
      if (!hasMax || tMaxY < tMax)
      {
        tMax = tMaxY;
        hasMax = true;
      }
    }

    if (std::abs(Direction.Z) < Epsilon)
    {
      if (Position.Z < box.Min.Z || Position.Z > box.Max.Z)
      {
        rResult = 0.0f;
        return false;
      }
    }
    else
    {
      auto tMinZ = (box.Min.Z - Position.Z) / Direction.Z;
      auto tMaxZ = (box.Max.Z - Position.Z) / Direction.Z;

      if (tMinZ > tMaxZ)
      {
        auto temp = tMinZ;
        tMinZ = tMaxZ;
        tMaxZ = temp;
      }

      if ((hasMin && tMin > tMaxZ) || (hasMax && tMinZ > tMax))
      {
        rResult = 0.0f;
        return false;
      }

      if (!hasMin || tMinZ > tMin)
      {
        tMin = tMinZ;
        hasMin = true;
      }
      if (!hasMax || tMaxZ < tMax)
      {
        tMax = tMaxZ;
        // Disabled since no one checks it below
        // hasMax = true;
      }
    }

    // having a positive tMin and a negative tMax means the ray is inside the box
    // we expect the intersection distance to be 0 in that case
    if ((hasMin && tMin < 0) && tMax > 0)
    {
      rResult = 0.0f;
      return true;
    }
    // a negative tMin means that the intersection point is behind the ray's origin
    // we discard these as not hitting the AABB
    if (tMin < 0)
    {
      rResult = 0.0f;
      return false;
    }
    rResult = tMin;
    return true;
  }


  bool Ray::Intersects(const BoundingSphere& sphere, float& rResult) const
  {
    // Find the vector between where the ray starts the the sphere's center
    Vector3 difference = sphere.Center - Position;

    float differenceLengthSquared = difference.LengthSquared();
    float sphereRadiusSquared = sphere.Radius * sphere.Radius;

    float distanceAlongRay;

    // If the distance between the ray start and the sphere's center is less than
    // the radius of the sphere, it means we've intersected. N.B. checking the LengthSquared is faster.
    if (differenceLengthSquared < sphereRadiusSquared)
    {
      rResult = 0.0f;
      return true;
    }

    distanceAlongRay = Vector3::Dot(Direction, difference);
    // If the ray is pointing away from the sphere then we don't ever intersect
    if (distanceAlongRay < 0)
    {
      rResult = 0.0f;
      return false;
    }

    // Next we kinda use Pythagoras to check if we are within the bounds of the sphere
    // if x = radius of sphere
    // if y = distance between ray position and sphere center
    // if z = the distance we've traveled along the ray
    // if x^2 + z^2 - y^2 < 0, we do not intersect
    float dist = sphereRadiusSquared + distanceAlongRay * distanceAlongRay - differenceLengthSquared;

    if (dist < 0)
    {
      rResult = 0.0f;
      return false;
    }
    rResult = distanceAlongRay - std::sqrt(dist);
    return true;
  }


  bool Ray::Intersects(const Plane& plane, float& rResult) const
  {
    auto den = Vector3::Dot(Direction, plane.Normal);
    if (std::abs(den) < 0.00001f)
    {
      rResult = 0;
      return false;
    }

    rResult = (-plane.D - Vector3::Dot(plane.Normal, Position)) / den;
    if (rResult < 0.0f)
    {
      if (rResult < -0.00001f)
      {
        rResult = 0.0f;
        return false;
      }
      rResult = 0.0f;
    }
    return true;
  }
}
