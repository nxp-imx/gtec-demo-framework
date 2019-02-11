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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector2.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include "MatrixInternals.hpp"

namespace Fsl
{
  // Verify that our assumption about the structure packing is correct
  static_assert(offsetof(Vector2, X) == (sizeof(float) * 0), "Vector2.X component not at the expected offset");
  static_assert(offsetof(Vector2, Y) == (sizeof(float) * 1), "Vector2.Y component not at the expected offset");
  static_assert(sizeof(Vector2) == (sizeof(float) * 2), "Vector2 not of the expected size");


  Vector2 Vector2::Barycentric(const Vector2& value1, const Vector2& value2, const Vector2& value3, const float amount1, const float amount2)
  {
    return Vector2(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2),
                   MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2));
  }


  void Vector2::Barycentric(const Vector2& value1, const Vector2& value2, const Vector2& value3, const float amount1, const float amount2,
                            Vector2& rResult)
  {
    rResult.X = MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2);
    rResult.Y = MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2);
  }


  Vector2 Vector2::CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount)
  {
    return Vector2(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
                   MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount));
  }


  void Vector2::CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount,
                           Vector2& rResult)
  {
    rResult.X = MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount);
    rResult.Y = MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount);
  }


  Vector2 Vector2::Clamp(const Vector2& value, const Vector2& min, const Vector2& max)
  {
    return Vector2(MathHelper::Clamp(value.X, min.X, max.X), MathHelper::Clamp(value.Y, min.Y, max.Y));
  }


  void Vector2::Clamp(const Vector2& value, const Vector2& min, const Vector2& max, Vector2& rResult)
  {
    rResult = Vector2(MathHelper::Clamp(value.X, min.X, max.X), MathHelper::Clamp(value.Y, min.Y, max.Y));
  }


  float Vector2::Distance(const Vector2& vector1, const Vector2& vector2)
  {
    return std::sqrt(DistanceSquared(vector1, vector2));
  }


  Vector2 Vector2::Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2, const Vector2& tangent2, const float amount)
  {
    return Vector2(MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount),
                   MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount));
  }


  void Vector2::Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2, const Vector2& tangent2, const float amount,
                        Vector2& rResult)
  {
    rResult.X = MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount);
    rResult.Y = MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount);
  }


  float Vector2::Length() const
  {
    return std::sqrt((X * X) + (Y * Y));
  }


  Vector2 Vector2::Lerp(const Vector2& value1, const Vector2 value2, const float amount)
  {
    return Vector2(MathHelper::Lerp(value1.X, value2.X, amount), MathHelper::Lerp(value1.Y, value2.Y, amount));
  }


  void Vector2::Lerp(const Vector2& value1, const Vector2 value2, const float amount, Vector2& rResult)
  {
    rResult = Vector2(MathHelper::Lerp(value1.X, value2.X, amount), MathHelper::Lerp(value1.Y, value2.Y, amount));
  }


  Vector2 Vector2::Max(const Vector2& value1, const Vector2& value2)
  {
    return Vector2(std::max(value1.X, value2.X), std::max(value1.Y, value2.Y));
  }


  void Vector2::Max(const Vector2& value1, const Vector2& value2, Vector2& rResult)
  {
    rResult = Vector2(std::max(value1.X, value2.X), std::max(value1.Y, value2.Y));
  }


  Vector2 Vector2::Min(const Vector2& value1, const Vector2& value2)
  {
    return Vector2(std::min(value1.X, value2.X), std::min(value1.Y, value2.Y));
  }


  void Vector2::Min(const Vector2& value1, const Vector2& value2, Vector2& rResult)
  {
    rResult = Vector2(std::min(value1.X, value2.X), std::min(value1.Y, value2.Y));
  }


  void Vector2::Normalize()
  {
    float factor = Length();
    X /= factor;
    Y /= factor;
  }


  Vector2 Vector2::Normalize(const Vector2& vector)
  {
    // Vector2 result(OptimizationFlag::NoInitialization);
    Vector2 result;
    Normalize(vector, result);
    return result;
  }


  void Vector2::Normalize(const Vector2& value, Vector2& rResult)
  {
    float factor = value.Length();
    rResult.X = value.X / factor;
    rResult.Y = value.Y / factor;
  }


  Vector2 Vector2::Reflect(const Vector2& vector, const Vector2& normal)
  {
    // I is the original array
    // N is the normal of the incident plane
    // R = I - (2 * N * ( DotProduct[ I,N] ))
    const float factor = 2.0f * ((vector.X * normal.X) + (vector.Y * normal.Y));

    return Vector2(vector.X - (normal.X * factor), vector.Y - (normal.Y * factor));
  }


  void Vector2::Reflect(const Vector2& vector, const Vector2& normal, Vector2& rResult)
  {
    // I is the original array
    // N is the normal of the incident plane
    // R = I - (2 * N * ( DotProduct[ I,N] ))
    const float factor = 2.0f * ((vector.X * normal.X) + (vector.Y * normal.Y));

    rResult.X = vector.X - (normal.X * factor);
    rResult.Y = vector.Y - (normal.Y * factor);
  }


  Vector2 Vector2::SmoothStep(const Vector2& value1, const Vector2& value2, const float amount)
  {
    return Vector2(MathHelper::SmoothStep(value1.X, value2.X, amount), MathHelper::SmoothStep(value1.Y, value2.Y, amount));
  }


  void Vector2::SmoothStep(const Vector2& value1, const Vector2& value2, const float amount, Vector2& rResult)
  {
    rResult.X = MathHelper::SmoothStep(value1.X, value2.X, amount);
    rResult.Y = MathHelper::SmoothStep(value1.Y, value2.Y, amount);
  }

  Vector2 Vector2::Transform(const Vector2& position, const Matrix& matrix)
  {
    // Vector2 result(OptimizationFlag::NoInitialization);
    Vector2 result;
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector2::Transform(const Vector2& position, const Matrix& matrix, Vector2& rResult)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }


  Vector2 Vector2::Transform(const Vector2& value, const Quaternion& rotation)
  {
    Vector2 result;
    Transform(value, rotation, result);
    return result;
  }


  void Vector2::Transform(const Vector2& value, const Quaternion& rotation, Vector2& rResult)
  {
    const Vector3 rot1 = Vector3(rotation.X + rotation.X, rotation.Y + rotation.Y, rotation.Z + rotation.Z);
    const Vector3 rot2 = Vector3(rotation.X, rotation.X, rotation.W);
    const Vector3 rot3 = Vector3(1, rotation.Y, rotation.Z);
    auto rot4 = rot1 * rot2;
    auto rot5 = rot1 * rot3;

    Vector2 v;
    v.X = static_cast<float>(static_cast<double>(value.X) * (1.0 - static_cast<double>(rot5.Y) - static_cast<double>(rot5.Z)) +
                             static_cast<double>(value.Y) * (static_cast<double>(rot4.Y) - static_cast<double>(rot4.Z)));
    v.Y = static_cast<float>(static_cast<double>(value.X) * (static_cast<double>(rot4.Y) + static_cast<double>(rot4.Z)) +
                             static_cast<double>(value.Y) * (1.0 - static_cast<double>(rot4.X) - static_cast<double>(rot5.Z)));
    rResult.X = v.X;
    rResult.Y = v.Y;
  }


  Vector2 Vector2::TransformNormal(const Vector2& position, const Matrix& matrix)
  {
    // Vector2 result(OptimizationFlag::NoInitialization);
    Vector2 result;
    MatrixInternals::TransformNormal(result, position, matrix);
    return result;
  }


  void Vector2::TransformNormal(const Vector2& position, const Matrix& matrix, Vector2& rResult)
  {
    MatrixInternals::TransformNormal(rResult, position, matrix);
  }
}
