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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector4.cs.

#include <cassert>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include "MatrixInternals.hpp"

namespace Fsl
{
  // Verify that our assumption about the structure packing is correct
  static_assert(offsetof(Vector4, X) == (sizeof(float) * 0), "Vector4.X component not at the expected offset");
  static_assert(offsetof(Vector4, Y) == (sizeof(float) * 1), "Vector4.Y component not at the expected offset");
  static_assert(offsetof(Vector4, Z) == (sizeof(float) * 2), "Vector4.Z component not at the expected offset");
  static_assert(offsetof(Vector4, W) == (sizeof(float) * 3), "Vector4.W component not at the expected offset");
  static_assert(sizeof(Vector4) == (sizeof(float) * 4), "Vector4 not of the expected size");

  Vector4::Vector4(const Vector2& v, const float z, const float w)
    : X(v.X)
    , Y(v.Y)
    , Z(z)
    , W(w)
  {
  }


  Vector4::Vector4(const Vector3& v, const float w)
    : X(v.X)
    , Y(v.Y)
    , Z(v.Z)
    , W(w)
  {
  }


  Vector4 Vector4::Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2)
  {
    return Vector4(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2),
                   MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2),
                   MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2),
                   MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2));
  }


  void Vector4::Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2,
                            Vector4& rResult)
  {
    rResult.X = MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2);
    rResult.Y = MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2);
    rResult.Z = MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2);
    rResult.W = MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2);
  }


  Vector4 Vector4::CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount)
  {
    return Vector4(
      MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
      MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount), MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount));
  }


  void Vector4::CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount,
                           Vector4& rResult)
  {
    rResult.X = MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount);
    rResult.Y = MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount);
    rResult.Z = MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount);
    rResult.W = MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount);
  }


  Vector4 Vector4::Clamp(const Vector4& value, const Vector4& min, const Vector4& max)
  {
    return Vector4(MathHelper::Clamp(value.X, min.X, max.X), MathHelper::Clamp(value.Y, min.Y, max.Y), MathHelper::Clamp(value.Z, min.Z, max.Z),
                   MathHelper::Clamp(value.W, min.W, max.W));
  }


  void Vector4::Clamp(const Vector4& value, const Vector4& min, const Vector4& max, Vector4& rResult)
  {
    rResult = Vector4(MathHelper::Clamp(value.X, min.X, max.X), MathHelper::Clamp(value.Y, min.Y, max.Y), MathHelper::Clamp(value.Z, min.Z, max.Z),
                      MathHelper::Clamp(value.W, min.W, max.W));
  }


  float Vector4::Distance(const Vector4& vector1, const Vector4& vector2)
  {
    return std::sqrt(DistanceSquared(vector1, vector2));
  }


  Vector4 Vector4::Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount)
  {
    return Vector4(MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount),
                   MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount),
                   MathHelper::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount),
                   MathHelper::Hermite(value1.W, tangent1.W, value2.W, tangent2.W, amount));
  }


  void Vector4::Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount,
                        Vector4& rResult)
  {
    rResult.X = MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount);
    rResult.Y = MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount);
    rResult.Z = MathHelper::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount);
    rResult.W = MathHelper::Hermite(value1.W, tangent1.W, value2.W, tangent2.W, amount);
  }


  float Vector4::Length() const
  {
    return std::sqrt((X * X) + (Y * Y) + (Z * Z) + (W * W));
  }


  Vector4 Vector4::Lerp(const Vector4& value1, const Vector4 value2, const float amount)
  {
    return Vector4(MathHelper::Lerp(value1.X, value2.X, amount), MathHelper::Lerp(value1.Y, value2.Y, amount),
                   MathHelper::Lerp(value1.Z, value2.Z, amount), MathHelper::Lerp(value1.W, value2.W, amount));
  }


  void Vector4::Lerp(const Vector4& value1, const Vector4 value2, const float amount, Vector4& rResult)
  {
    rResult = Vector4(MathHelper::Lerp(value1.X, value2.X, amount), MathHelper::Lerp(value1.Y, value2.Y, amount),
                      MathHelper::Lerp(value1.Z, value2.Z, amount), MathHelper::Lerp(value1.W, value2.W, amount));
  }


  Vector4 Vector4::Max(const Vector4& value1, const Vector4& value2)
  {
    return Vector4(std::max(value1.X, value2.X), std::max(value1.Y, value2.Y), std::max(value1.Z, value2.Z), std::max(value1.W, value2.W));
  }


  void Vector4::Max(const Vector4& value1, const Vector4& value2, Vector4& rResult)
  {
    rResult = Vector4(std::max(value1.X, value2.X), std::max(value1.Y, value2.Y), std::max(value1.Z, value2.Z), std::max(value1.W, value2.W));
  }


  Vector4 Vector4::Min(const Vector4& value1, const Vector4& value2)
  {
    return Vector4(std::min(value1.X, value2.X), std::min(value1.Y, value2.Y), std::min(value1.Z, value2.Z), std::min(value1.W, value2.W));
  }


  void Vector4::Min(const Vector4& value1, const Vector4& value2, Vector4& rResult)
  {
    rResult = Vector4(std::min(value1.X, value2.X), std::min(value1.Y, value2.Y), std::min(value1.Z, value2.Z), std::min(value1.W, value2.W));
  }


  void Vector4::Negate(const Vector4& value, Vector4& rResult)
  {
    rResult.X = -value.X;
    rResult.Y = -value.Y;
    rResult.Z = -value.Z;
    rResult.W = -value.W;
  }


  void Vector4::Normalize()
  {
    float factor = Length();
    assert(factor != 0.0f);
    factor = 1.0f / factor;
    X *= factor;
    Y *= factor;
    Z *= factor;
    W *= factor;
  }


  Vector4 Vector4::Normalize(const Vector4& vector)
  {
    // Vector4 result(OptimizationFlag::NoInitialization);
    Vector4 result;
    Normalize(vector, result);
    return result;
  }


  void Vector4::Normalize(const Vector4& value, Vector4& rResult)
  {
    float factor = value.Length();
    assert(factor != 0.0f);
    factor = 1.0f / factor;
    rResult.X = value.X * factor;
    rResult.Y = value.Y * factor;
    rResult.Z = value.Z * factor;
    rResult.W = value.W * factor;
  }


  Vector4 Vector4::Reflect(const Vector4& vector, const Vector4& normal)
  {
    // I is the original array
    // N is the normal of the incident plane
    // R = I - (2 * N * ( DotProduct[ I,N] ))
    const float factor = 2.0f * ((vector.X * normal.X) + (vector.Y * normal.Y) + (vector.Z * normal.Z) + (vector.W * normal.W));

    return Vector4(vector.X - (normal.X * factor), vector.Y - (normal.Y * factor), vector.Z - (normal.Z * factor), vector.W - (normal.W * factor));
  }


  void Vector4::Reflect(const Vector4& vector, const Vector4& normal, Vector4& rResult)
  {
    // I is the original array
    // N is the normal of the incident plane
    // R = I - (2 * N * ( DotProduct[ I,N] ))
    const float factor = 2.0f * ((vector.X * normal.X) + (vector.Y * normal.Y) + (vector.Z * normal.Z) + (vector.W * normal.W));

    rResult.X = vector.X - (normal.X * factor);
    rResult.Y = vector.Y - (normal.Y * factor);
    rResult.Z = vector.Z - (normal.Z * factor);
    rResult.W = vector.W - (normal.W * factor);
  }


  Vector4 Vector4::SmoothStep(const Vector4& value1, const Vector4& value2, const float amount)
  {
    return Vector4(MathHelper::SmoothStep(value1.X, value2.X, amount), MathHelper::SmoothStep(value1.Y, value2.Y, amount),
                   MathHelper::SmoothStep(value1.Z, value2.Z, amount), MathHelper::SmoothStep(value1.W, value2.W, amount));
  }


  void Vector4::SmoothStep(const Vector4& value1, const Vector4& value2, const float amount, Vector4& rResult)
  {
    rResult.X = MathHelper::SmoothStep(value1.X, value2.X, amount);
    rResult.Y = MathHelper::SmoothStep(value1.Y, value2.Y, amount);
    rResult.Z = MathHelper::SmoothStep(value1.Z, value2.Z, amount);
    rResult.W = MathHelper::SmoothStep(value1.W, value2.W, amount);
  }


  Vector4 Vector4::Transform(const Vector2& position, const Matrix& matrix)
  {
    // Vector4 result(OptimizationFlag::NoInitialization);
    Vector4 result;
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(const Vector2& position, const Matrix& matrix, Vector4& rResult)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }


  Vector4 Vector4::Transform(const Vector3& position, const Matrix& matrix)
  {
    // Vector4 result(OptimizationFlag::NoInitialization);
    Vector4 result;
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(const Vector3& position, const Matrix& matrix, Vector4& rResult)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }


  Vector4 Vector4::Transform(const Vector4& position, const Matrix& matrix)
  {
    // Vector4 result(OptimizationFlag::NoInitialization);
    Vector4 result;
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(const Vector4& position, const Matrix& matrix, Vector4& rResult)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }
}
