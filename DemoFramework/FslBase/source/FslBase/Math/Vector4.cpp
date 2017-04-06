/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

// The functions in this file are a port of an MIT licensed library: MonaGame - Vector4.cs.

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

  Vector4::Vector4(const Vector2& v, const float z, const float w)
    : X(v.X), Y(v.Y), Z(z), W(w)
  {
  }


  Vector4::Vector4(const Vector3& v, const float w)
    : X(v.X), Y(v.Y), Z(v.Z), W(w)
  {
  }

  const float* Vector4::DirectAccess() const
  {
    // Verify that our assumption about the structure packing is correct
    assert(offsetof(Vector4, X) == (sizeof(float) * 0));
    assert(offsetof(Vector4, Y) == (sizeof(float) * 1));
    assert(offsetof(Vector4, Z) == (sizeof(float) * 2));
    assert(offsetof(Vector4, W) == (sizeof(float) * 3));
    return &X;
  }


  Vector4 Vector4::Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2)
  {
    return Vector4(
      MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2),
      MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2),
      MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2),
      MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2));
  }


  void Vector4::Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2, Vector4& rResult)
  {
    rResult.X = MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2);
    rResult.Y = MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2);
    rResult.Z = MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2);
    rResult.W = MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2);
  }


  Vector4 Vector4::CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount)
  {
    return Vector4(
      MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
      MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
      MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount),
      MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount));
  }


  void Vector4::CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount, Vector4& rResult)
  {
    rResult.X = MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount);
    rResult.Y = MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount);
    rResult.Z = MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount);
    rResult.W = MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount);
  }


  Vector4 Vector4::Clamp(const Vector4& value, const Vector4& min, const Vector4& max)
  {
    return Vector4(
      MathHelper::Clamp(value.X, min.X, max.X),
      MathHelper::Clamp(value.Y, min.Y, max.Y),
      MathHelper::Clamp(value.Z, min.Z, max.Z),
      MathHelper::Clamp(value.W, min.W, max.W));
  }


  void Vector4::Clamp(Vector4& rResult, const Vector4& value, const Vector4& min, const Vector4& max)
  {
    rResult = Vector4(
      MathHelper::Clamp(value.X, min.X, max.X),
      MathHelper::Clamp(value.Y, min.Y, max.Y),
      MathHelper::Clamp(value.Z, min.Z, max.Z),
      MathHelper::Clamp(value.W, min.W, max.W));
  }


  float Vector4::Distance(const Vector4& vector1, const Vector4& vector2)
  {
    return std::sqrt(DistanceSquared(vector1, vector2));
  }


  Vector4 Vector4::Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount)
  {
    return Vector4(
      MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount),
      MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount),
      MathHelper::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount),
      MathHelper::Hermite(value1.W, tangent1.W, value2.W, tangent2.W, amount));
  }


  void Vector4::Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount, Vector4& rResult)
  {
    rResult.X = MathHelper::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount);
    rResult.Y = MathHelper::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount);
    rResult.Z = MathHelper::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount);
    rResult.W = MathHelper::Hermite(value1.W, tangent1.W, value2.W, tangent2.W, amount);
  }


  float Vector4::Length() const
  {
    return std::sqrt((X*X) + (Y*Y) + (Z*Z) + (W*W));
  }


  Vector4 Vector4::Lerp(const Vector4& value1, const Vector4 value2, const float amount)
  {
    return Vector4(
      MathHelper::Lerp(value1.X, value2.X, amount),
      MathHelper::Lerp(value1.Y, value2.Y, amount),
      MathHelper::Lerp(value1.Z, value2.Z, amount),
      MathHelper::Lerp(value1.W, value2.W, amount));
  }


  void Vector4::Lerp(Vector4& rResult, const Vector4& value1, const Vector4 value2, const float amount)
  {
    rResult = Vector4(
      MathHelper::Lerp(value1.X, value2.X, amount),
      MathHelper::Lerp(value1.Y, value2.Y, amount),
      MathHelper::Lerp(value1.Z, value2.Z, amount),
      MathHelper::Lerp(value1.W, value2.W, amount));
  }


  Vector4 Vector4::Max(const Vector4& value1, const Vector4& value2)
  {
    return Vector4(
      std::max(value1.X, value2.X),
      std::max(value1.Y, value2.Y),
      std::max(value1.Z, value2.Z),
      std::max(value1.W, value2.W));
  }


  void Vector4::Max(Vector4& rResult, const Vector4& value1, const Vector4& value2)
  {
    rResult = Vector4(
      std::max(value1.X, value2.X),
      std::max(value1.Y, value2.Y),
      std::max(value1.Z, value2.Z),
      std::max(value1.W, value2.W));
  }


  Vector4 Vector4::Min(const Vector4& value1, const Vector4& value2)
  {
    return Vector4(
      std::min(value1.X, value2.X),
      std::min(value1.Y, value2.Y),
      std::min(value1.Z, value2.Z),
      std::min(value1.W, value2.W));
  }


  void Vector4::Min(Vector4& rResult, const Vector4& value1, const Vector4& value2)
  {
    rResult = Vector4(
      std::min(value1.X, value2.X),
      std::min(value1.Y, value2.Y),
      std::min(value1.Z, value2.Z),
      std::min(value1.W, value2.W));
  }


  Vector4 Vector4::Negate(const Vector4& value)
  {
    return Vector4(-value.X, -value.Y, -value.Z, -value.W);
  }


  void Vector4::Negate(Vector4& rResult, const Vector4& value)
  {
    rResult.X = -value.X;
    rResult.Y = -value.Y;
    rResult.Z = -value.Z;
    rResult.W = -value.W;
  }


  void Vector4::Normalize()
  {
    Normalize(*this, *this);
  }


  Vector4 Vector4::Normalize(const Vector4& vector)
  {
    Vector4 result(OptimizationFlag::NoInitialization);
    Normalize(result, vector);
    return result;
  }


  void Vector4::Normalize(Vector4& rResult, const Vector4& value)
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

    return Vector4(
      vector.X - (normal.X * factor),
      vector.Y - (normal.Y * factor),
      vector.Z - (normal.Z * factor),
      vector.W - (normal.W * factor));
  }


  void Vector4::Reflect(Vector4& rResult, const Vector4& vector, const Vector4& normal)
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
    return Vector4(
      MathHelper::SmoothStep(value1.X, value2.X, amount),
      MathHelper::SmoothStep(value1.Y, value2.Y, amount),
      MathHelper::SmoothStep(value1.Z, value2.Z, amount),
      MathHelper::SmoothStep(value1.W, value2.W, amount));
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
    Vector4 result(OptimizationFlag::NoInitialization);
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(Vector4& rResult, const Vector2& position, const Matrix& matrix)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }


  Vector4 Vector4::Transform(const Vector3& position, const Matrix& matrix)
  {
    Vector4 result(OptimizationFlag::NoInitialization);
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(Vector4& rResult, const Vector3& position, const Matrix& matrix)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }


  Vector4 Vector4::Transform(const Vector4& position, const Matrix& matrix)
  {
    Vector4 result(OptimizationFlag::NoInitialization);
    MatrixInternals::Transform(result, position, matrix);
    return result;
  }


  void Vector4::Transform(Vector4& rResult, const Vector4& position, const Matrix& matrix)
  {
    MatrixInternals::Transform(rResult, position, matrix);
  }
}
