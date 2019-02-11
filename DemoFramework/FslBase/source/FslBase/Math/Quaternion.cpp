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

#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>

namespace Fsl
{
  using namespace MatrixFields;

  Quaternion::Quaternion(const Vector3& vectorPart, const float scalarPart)
    : X(vectorPart.X)
    , Y(vectorPart.Y)
    , Z(vectorPart.Z)
    , W(scalarPart)
  {
  }


  Quaternion Quaternion::Add(const Quaternion& lhs, const Quaternion& rhs)
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W};
  }


  void Quaternion::Add(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    rResult.X = lhs.X + rhs.X;
    rResult.Y = lhs.Y + rhs.Y;
    rResult.Z = lhs.Z + rhs.Z;
    rResult.W = lhs.W + rhs.W;
  }


  Quaternion Quaternion::Concatenate(const Quaternion& lhs, const Quaternion& rhs)
  {
    float x = rhs.X;
    float y = rhs.Y;
    float z = rhs.Z;
    float w = rhs.W;
    float num4 = lhs.X;
    float num3 = lhs.Y;
    float num2 = lhs.Z;
    float num = lhs.W;
    float num12 = (y * num2) - (z * num3);
    float num11 = (z * num4) - (x * num2);
    float num10 = (x * num3) - (y * num4);
    float num9 = ((x * num4) + (y * num3)) + (z * num2);

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = ((x * num) + (num4 * w)) + num12;
    quaternion.Y = ((y * num) + (num3 * w)) + num11;
    quaternion.Z = ((z * num) + (num2 * w)) + num10;
    quaternion.W = (w * num) - num9;
    return quaternion;
  }


  void Quaternion::Concatenate(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    const float x = rhs.X;
    const float y = rhs.Y;
    const float z = rhs.Z;
    const float w = rhs.W;
    const float num4 = lhs.X;
    const float num3 = lhs.Y;
    const float num2 = lhs.Z;
    const float num = lhs.W;
    const float num12 = (y * num2) - (z * num3);
    const float num11 = (z * num4) - (x * num2);
    const float num10 = (x * num3) - (y * num4);
    const float num9 = ((x * num4) + (y * num3)) + (z * num2);
    rResult.X = ((x * num) + (num4 * w)) + num12;
    rResult.Y = ((y * num) + (num3 * w)) + num11;
    rResult.Z = ((z * num) + (num2 * w)) + num10;
    rResult.W = (w * num) - num9;
  }


  void Quaternion::Conjugate()
  {
    X = -X;
    Y = -Y;
    Z = -Z;
  }


  Quaternion Quaternion::Conjugate(const Quaternion& value)
  {
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = -value.X;
    quaternion.Y = -value.Y;
    quaternion.Z = -value.Z;
    quaternion.W = value.W;
    return quaternion;
  }


  void Quaternion::Conjugate(Quaternion& rResult, const Quaternion& value)
  {
    rResult.X = -value.X;
    rResult.Y = -value.Y;
    rResult.Z = -value.Z;
    rResult.W = value.W;
  }


  Quaternion Quaternion::CreateFromAxisAngle(const Vector3& axis, const float angle)
  {
    const float num2 = angle * 0.5f;
    const auto num = static_cast<float>(std::sin(static_cast<double>(num2)));

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = axis.X * num;
    quaternion.Y = axis.Y * num;
    quaternion.Z = axis.Z * num;
    quaternion.W = static_cast<float>(std::cos(static_cast<double>(num2)));
    return quaternion;
  }


  void Quaternion::CreateFromAxisAngle(Quaternion& rResult, const Vector3& axis, const float angle)
  {
    const float num2 = angle * 0.5f;
    const auto num = static_cast<float>(std::sin(static_cast<double>(num2)));
    rResult.X = axis.X * num;
    rResult.Y = axis.Y * num;
    rResult.Z = axis.Z * num;
    rResult.W = static_cast<float>(std::cos(static_cast<double>(num2)));
  }


  Quaternion Quaternion::CreateFromRotationMatrix(const Matrix& matrix)
  {
    const float* const pMatrix = matrix.DirectAccess();

    float num8 = (pMatrix[_M11] + pMatrix[_M22]) + pMatrix[_M33];
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    if (num8 > 0.0f)
    {
      auto num = std::sqrt(num8 + 1.0f);
      quaternion.W = num * 0.5f;
      num = 0.5f / num;
      quaternion.X = (pMatrix[_M23] - pMatrix[_M32]) * num;
      quaternion.Y = (pMatrix[_M31] - pMatrix[_M13]) * num;
      quaternion.Z = (pMatrix[_M12] - pMatrix[_M21]) * num;
      return quaternion;
    }
    if ((pMatrix[_M11] >= pMatrix[_M22]) && (pMatrix[_M11] >= pMatrix[_M33]))
    {
      const auto num7 = std::sqrt(((1.0f + pMatrix[_M11]) - pMatrix[_M22]) - pMatrix[_M33]);
      const float num4 = 0.5f / num7;
      quaternion.X = 0.5f * num7;
      quaternion.Y = (pMatrix[_M12] + pMatrix[_M21]) * num4;
      quaternion.Z = (pMatrix[_M13] + pMatrix[_M31]) * num4;
      quaternion.W = (pMatrix[_M23] - pMatrix[_M32]) * num4;
      return quaternion;
    }
    if (pMatrix[_M22] > pMatrix[_M33])
    {
      const auto num6 = std::sqrt(((1.0f + pMatrix[_M22]) - pMatrix[_M11]) - pMatrix[_M33]);
      const float num3 = 0.5f / num6;
      quaternion.X = (pMatrix[_M21] + pMatrix[_M12]) * num3;
      quaternion.Y = 0.5f * num6;
      quaternion.Z = (pMatrix[_M32] + pMatrix[_M23]) * num3;
      quaternion.W = (pMatrix[_M31] - pMatrix[_M13]) * num3;
      return quaternion;
    }
    const auto num5 = std::sqrt(((1.0f + pMatrix[_M33]) - pMatrix[_M11]) - pMatrix[_M22]);
    const float num2 = 0.5f / num5;
    quaternion.X = (pMatrix[_M31] + pMatrix[_M13]) * num2;
    quaternion.Y = (pMatrix[_M32] + pMatrix[_M23]) * num2;
    quaternion.Z = 0.5f * num5;
    quaternion.W = (pMatrix[_M12] - pMatrix[_M21]) * num2;
    return quaternion;
  }


  void Quaternion::CreateFromRotationMatrix(Quaternion& rResult, const Matrix& matrix)
  {
    const float* const pMatrix = matrix.DirectAccess();

    const float num8 = (pMatrix[_M11] + pMatrix[_M22]) + pMatrix[_M33];
    if (num8 > 0.0f)
    {
      auto num = static_cast<float>(std::sqrt(static_cast<double>(num8 + 1.0f)));
      rResult.W = num * 0.5f;
      num = 0.5f / num;
      rResult.X = (pMatrix[_M23] - pMatrix[_M32]) * num;
      rResult.Y = (pMatrix[_M31] - pMatrix[_M13]) * num;
      rResult.Z = (pMatrix[_M12] - pMatrix[_M21]) * num;
    }
    else if ((pMatrix[_M11] >= pMatrix[_M22]) && (pMatrix[_M11] >= pMatrix[_M33]))
    {
      const auto num7 = static_cast<float>(std::sqrt(static_cast<double>(((1.0f + pMatrix[_M11]) - pMatrix[_M22]) - pMatrix[_M33])));
      const float num4 = 0.5f / num7;
      rResult.X = 0.5f * num7;
      rResult.Y = (pMatrix[_M12] + pMatrix[_M21]) * num4;
      rResult.Z = (pMatrix[_M13] + pMatrix[_M31]) * num4;
      rResult.W = (pMatrix[_M23] - pMatrix[_M32]) * num4;
    }
    else if (pMatrix[_M22] > pMatrix[_M33])
    {
      const auto num6 = static_cast<float>(std::sqrt(static_cast<double>(((1.0f + pMatrix[_M22]) - pMatrix[_M11]) - pMatrix[_M33])));
      const float num3 = 0.5f / num6;
      rResult.X = (pMatrix[_M21] + pMatrix[_M12]) * num3;
      rResult.Y = 0.5f * num6;
      rResult.Z = (pMatrix[_M32] + pMatrix[_M23]) * num3;
      rResult.W = (pMatrix[_M31] - pMatrix[_M13]) * num3;
    }
    else
    {
      const auto num5 = static_cast<float>(std::sqrt(static_cast<double>(((1.0f + pMatrix[_M33]) - pMatrix[_M11]) - pMatrix[_M22])));
      const float num2 = 0.5f / num5;
      rResult.X = (pMatrix[_M31] + pMatrix[_M13]) * num2;
      rResult.Y = (pMatrix[_M32] + pMatrix[_M23]) * num2;
      rResult.Z = 0.5f * num5;
      rResult.W = (pMatrix[_M12] - pMatrix[_M21]) * num2;
    }
  }


  Quaternion Quaternion::CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll)
  {
    const float num9 = roll * 0.5f;
    const auto num6 = static_cast<float>(std::sin(static_cast<double>(num9)));
    const auto num5 = static_cast<float>(std::cos(static_cast<double>(num9)));
    const float num8 = pitch * 0.5f;
    const auto num4 = static_cast<float>(std::sin(static_cast<double>(num8)));
    const auto num3 = static_cast<float>(std::cos(static_cast<double>(num8)));
    const float num7 = yaw * 0.5f;
    const auto num2 = static_cast<float>(std::sin(static_cast<double>(num7)));
    const auto num = static_cast<float>(std::cos(static_cast<double>(num7)));

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = ((num * num4) * num5) + ((num2 * num3) * num6);
    quaternion.Y = ((num2 * num3) * num5) - ((num * num4) * num6);
    quaternion.Z = ((num * num3) * num6) - ((num2 * num4) * num5);
    quaternion.W = ((num * num3) * num5) + ((num2 * num4) * num6);
    return quaternion;
  }


  void Quaternion::CreateFromYawPitchRoll(Quaternion& rResult, const float yaw, const float pitch, const float roll)
  {
    const float num9 = roll * 0.5f;
    const auto num6 = static_cast<float>(std::sin(static_cast<double>(num9)));
    const auto num5 = static_cast<float>(std::cos(static_cast<double>(num9)));
    const float num8 = pitch * 0.5f;
    const auto num4 = static_cast<float>(std::sin(static_cast<double>(num8)));
    const auto num3 = static_cast<float>(std::cos(static_cast<double>(num8)));
    const float num7 = yaw * 0.5f;
    const auto num2 = static_cast<float>(std::sin(static_cast<double>(num7)));
    const auto num = static_cast<float>(std::cos(static_cast<double>(num7)));
    rResult.X = ((num * num4) * num5) + ((num2 * num3) * num6);
    rResult.Y = ((num2 * num3) * num5) - ((num * num4) * num6);
    rResult.Z = ((num * num3) * num6) - ((num2 * num4) * num5);
    rResult.W = ((num * num3) * num5) + ((num2 * num4) * num6);
  }


  Quaternion Quaternion::Divide(const Quaternion& lhs, const Quaternion& rhs)
  {
    const float x = lhs.X;
    const float y = lhs.Y;
    const float z = lhs.Z;
    const float w = lhs.W;
    const float num14 = (((rhs.X * rhs.X) + (rhs.Y * rhs.Y)) + (rhs.Z * rhs.Z)) + (rhs.W * rhs.W);
    const float num5 = 1.0f / num14;
    const float num4 = -rhs.X * num5;
    const float num3 = -rhs.Y * num5;
    const float num2 = -rhs.Z * num5;
    const float num = rhs.W * num5;
    const float num13 = (y * num2) - (z * num3);
    const float num12 = (z * num4) - (x * num2);
    const float num11 = (x * num3) - (y * num4);
    const float num10 = ((x * num4) + (y * num3)) + (z * num2);

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = ((x * num) + (num4 * w)) + num13;
    quaternion.Y = ((y * num) + (num3 * w)) + num12;
    quaternion.Z = ((z * num) + (num2 * w)) + num11;
    quaternion.W = (w * num) - num10;
    return quaternion;
  }


  void Quaternion::Divide(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    const float x = lhs.X;
    const float y = lhs.Y;
    const float z = lhs.Z;
    const float w = lhs.W;
    const float num14 = (((rhs.X * rhs.X) + (rhs.Y * rhs.Y)) + (rhs.Z * rhs.Z)) + (rhs.W * rhs.W);
    const float num5 = 1.0f / num14;
    const float num4 = -rhs.X * num5;
    const float num3 = -rhs.Y * num5;
    const float num2 = -rhs.Z * num5;
    const float num = rhs.W * num5;
    const float num13 = (y * num2) - (z * num3);
    const float num12 = (z * num4) - (x * num2);
    const float num11 = (x * num3) - (y * num4);
    const float num10 = ((x * num4) + (y * num3)) + (z * num2);
    rResult.X = ((x * num) + (num4 * w)) + num13;
    rResult.Y = ((y * num) + (num3 * w)) + num12;
    rResult.Z = ((z * num) + (num2 * w)) + num11;
    rResult.W = (w * num) - num10;
  }


  float Quaternion::Dot(const Quaternion& lhs, const Quaternion& rhs)
  {
    return ((((lhs.X * rhs.X) + (lhs.Y * rhs.Y)) + (lhs.Z * rhs.Z)) + (lhs.W * rhs.W));
  }


  void Quaternion::Dot(float& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    rResult = (((lhs.X * rhs.X) + (lhs.Y * rhs.Y)) + (lhs.Z * rhs.Z)) + (lhs.W * rhs.W);
  }


  // public bool Equals(Quaternion other)
  //{
  //  return ((((this.X == other.X) && (this.Y == other.Y)) && (this.Z == other.Z)) && (this.W == other.W));
  //}


  // public override int GetHashCode()
  //{
  //  return (((this.X.GetHashCode() + this.Y.GetHashCode()) + this.Z.GetHashCode()) + this.W.GetHashCode());
  //}


  Quaternion Quaternion::Inverse(const Quaternion& quaternion)
  {
    const float num2 =
      (((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y)) + (quaternion.Z * quaternion.Z)) + (quaternion.W * quaternion.W);
    const float num = 1.0f / num2;

    // Quaternion result(OptimizationFlag::NoInitialization);
    Quaternion result;
    result.X = -quaternion.X * num;
    result.Y = -quaternion.Y * num;
    result.Z = -quaternion.Z * num;
    result.W = quaternion.W * num;
    return result;
  }


  void Quaternion::Inverse(Quaternion& rResult, const Quaternion& quaternion)
  {
    const float num2 =
      (((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y)) + (quaternion.Z * quaternion.Z)) + (quaternion.W * quaternion.W);
    const float num = 1.0f / num2;
    rResult.X = -quaternion.X * num;
    rResult.Y = -quaternion.Y * num;
    rResult.Z = -quaternion.Z * num;
    rResult.W = quaternion.W * num;
  }


  float Quaternion::Length() const
  {
    const float lenSquared = (((X * X) + (Y * Y)) + (Z * Z)) + (W * W);
    return std::sqrt(lenSquared);
  }


  float Quaternion::LengthSquared() const
  {
    return ((((X * X) + (Y * Y)) + (Z * Z)) + (W * W));
  }


  Quaternion Quaternion::Lerp(const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount)
  {
    const float num = amount;
    const float num2 = 1.0f - num;
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    float num5 =
      (((quaternion1.X * quaternion2.X) + (quaternion1.Y * quaternion2.Y)) + (quaternion1.Z * quaternion2.Z)) + (quaternion1.W * quaternion2.W);
    if (num5 >= 0.0f)
    {
      quaternion.X = (num2 * quaternion1.X) + (num * quaternion2.X);
      quaternion.Y = (num2 * quaternion1.Y) + (num * quaternion2.Y);
      quaternion.Z = (num2 * quaternion1.Z) + (num * quaternion2.Z);
      quaternion.W = (num2 * quaternion1.W) + (num * quaternion2.W);
    }
    else
    {
      quaternion.X = (num2 * quaternion1.X) - (num * quaternion2.X);
      quaternion.Y = (num2 * quaternion1.Y) - (num * quaternion2.Y);
      quaternion.Z = (num2 * quaternion1.Z) - (num * quaternion2.Z);
      quaternion.W = (num2 * quaternion1.W) - (num * quaternion2.W);
    }
    float num4 = (((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y)) + (quaternion.Z * quaternion.Z)) + (quaternion.W * quaternion.W);
    float num3 = 1.0f / (std::sqrt(num4));
    quaternion.X *= num3;
    quaternion.Y *= num3;
    quaternion.Z *= num3;
    quaternion.W *= num3;
    return quaternion;
  }


  void Quaternion::Lerp(Quaternion& rResult, const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount)
  {
    const float num = amount;
    const float num2 = 1.0f - num;
    const float num5 =
      (((quaternion1.X * quaternion2.X) + (quaternion1.Y * quaternion2.Y)) + (quaternion1.Z * quaternion2.Z)) + (quaternion1.W * quaternion2.W);
    if (num5 >= 0.0f)
    {
      rResult.X = (num2 * quaternion1.X) + (num * quaternion2.X);
      rResult.Y = (num2 * quaternion1.Y) + (num * quaternion2.Y);
      rResult.Z = (num2 * quaternion1.Z) + (num * quaternion2.Z);
      rResult.W = (num2 * quaternion1.W) + (num * quaternion2.W);
    }
    else
    {
      rResult.X = (num2 * quaternion1.X) - (num * quaternion2.X);
      rResult.Y = (num2 * quaternion1.Y) - (num * quaternion2.Y);
      rResult.Z = (num2 * quaternion1.Z) - (num * quaternion2.Z);
      rResult.W = (num2 * quaternion1.W) - (num * quaternion2.W);
    }
    const float num4 = (((rResult.X * rResult.X) + (rResult.Y * rResult.Y)) + (rResult.Z * rResult.Z)) + (rResult.W * rResult.W);
    const float num3 = 1.0f / (static_cast<float>(std::sqrt(static_cast<double>(num4))));
    rResult.X *= num3;
    rResult.Y *= num3;
    rResult.Z *= num3;
    rResult.W *= num3;
  }


  Quaternion Quaternion::Slerp(const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount)
  {
    float num2;
    float num3;
    float num = amount;
    float num4 =
      (((quaternion1.X * quaternion2.X) + (quaternion1.Y * quaternion2.Y)) + (quaternion1.Z * quaternion2.Z)) + (quaternion1.W * quaternion2.W);
    bool flag = false;
    if (num4 < 0.0f)
    {
      flag = true;
      num4 = -num4;
    }
    if (num4 > 0.999999f)
    {
      num3 = 1.0f - num;
      num2 = flag ? -num : num;
    }
    else
    {
      auto num5 = static_cast<float>(std::acos(static_cast<double>(num4)));
      auto num6 = static_cast<float>(1.0 / std::sin(static_cast<double>(num5)));
      num3 = (static_cast<float>(std::sin(static_cast<double>((1.0f - num) * num5)))) * num6;
      num2 = flag ? ((static_cast<float>(-std::sin(static_cast<double>(num * num5)))) * num6)
                  : ((static_cast<float>(std::sin(static_cast<double>(num * num5)))) * num6);
    }

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = (num3 * quaternion1.X) + (num2 * quaternion2.X);
    quaternion.Y = (num3 * quaternion1.Y) + (num2 * quaternion2.Y);
    quaternion.Z = (num3 * quaternion1.Z) + (num2 * quaternion2.Z);
    quaternion.W = (num3 * quaternion1.W) + (num2 * quaternion2.W);
    return quaternion;
  }


  void Quaternion::Slerp(Quaternion& rResult, const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount)
  {
    float num2;
    float num3;
    float num = amount;
    float num4 =
      (((quaternion1.X * quaternion2.X) + (quaternion1.Y * quaternion2.Y)) + (quaternion1.Z * quaternion2.Z)) + (quaternion1.W * quaternion2.W);
    bool flag = false;
    if (num4 < 0.0f)
    {
      flag = true;
      num4 = -num4;
    }
    if (num4 > 0.999999f)
    {
      num3 = 1.0f - num;
      num2 = flag ? -num : num;
    }
    else
    {
      auto num5 = static_cast<float>(std::acos(static_cast<double>(num4)));
      auto num6 = static_cast<float>(1.0 / std::sin(static_cast<double>(num5)));
      num3 = (static_cast<float>(std::sin(static_cast<double>((1.0f - num) * num5)))) * num6;
      num2 = flag ? ((static_cast<float>(-std::sin(static_cast<double>(num * num5)))) * num6)
                  : ((static_cast<float>(std::sin(static_cast<double>(num * num5)))) * num6);
    }
    rResult.X = (num3 * quaternion1.X) + (num2 * quaternion2.X);
    rResult.Y = (num3 * quaternion1.Y) + (num2 * quaternion2.Y);
    rResult.Z = (num3 * quaternion1.Z) + (num2 * quaternion2.Z);
    rResult.W = (num3 * quaternion1.W) + (num2 * quaternion2.W);
  }


  Quaternion Quaternion::Subtract(const Quaternion& lhs, const Quaternion& rhs)
  {
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = lhs.X - rhs.X;
    quaternion.Y = lhs.Y - rhs.Y;
    quaternion.Z = lhs.Z - rhs.Z;
    quaternion.W = lhs.W - rhs.W;
    return quaternion;
  }


  void Quaternion::Subtract(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    rResult.X = lhs.X - rhs.X;
    rResult.Y = lhs.Y - rhs.Y;
    rResult.Z = lhs.Z - rhs.Z;
    rResult.W = lhs.W - rhs.W;
  }


  Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs)
  {
    const float x = lhs.X;
    const float y = lhs.Y;
    const float z = lhs.Z;
    const float w = lhs.W;
    const float num4 = rhs.X;
    const float num3 = rhs.Y;
    const float num2 = rhs.Z;
    const float num = rhs.W;
    const float num12 = (y * num2) - (z * num3);
    const float num11 = (z * num4) - (x * num2);
    const float num10 = (x * num3) - (y * num4);
    const float num9 = ((x * num4) + (y * num3)) + (z * num2);

    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = ((x * num) + (num4 * w)) + num12;
    quaternion.Y = ((y * num) + (num3 * w)) + num11;
    quaternion.Z = ((z * num) + (num2 * w)) + num10;
    quaternion.W = (w * num) - num9;
    return quaternion;
  }


  Quaternion Quaternion::Multiply(const Quaternion& lhs, const float scaleFactor)
  {
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    quaternion.X = lhs.X * scaleFactor;
    quaternion.Y = lhs.Y * scaleFactor;
    quaternion.Z = lhs.Z * scaleFactor;
    quaternion.W = lhs.W * scaleFactor;
    return quaternion;
  }


  void Quaternion::Multiply(Quaternion& rResult, const Quaternion& lhs, const float& scaleFactor)
  {
    rResult.X = lhs.X * scaleFactor;
    rResult.Y = lhs.Y * scaleFactor;
    rResult.Z = lhs.Z * scaleFactor;
    rResult.W = lhs.W * scaleFactor;
  }


  void Quaternion::Multiply(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs)
  {
    const float x = lhs.X;
    const float y = lhs.Y;
    const float z = lhs.Z;
    const float w = lhs.W;
    const float num4 = rhs.X;
    const float num3 = rhs.Y;
    const float num2 = rhs.Z;
    const float num = rhs.W;
    const float num12 = (y * num2) - (z * num3);
    const float num11 = (z * num4) - (x * num2);
    const float num10 = (x * num3) - (y * num4);
    const float num9 = ((x * num4) + (y * num3)) + (z * num2);
    rResult.X = ((x * num) + (num4 * w)) + num12;
    rResult.Y = ((y * num) + (num3 * w)) + num11;
    rResult.Z = ((z * num) + (num2 * w)) + num10;
    rResult.W = (w * num) - num9;
  }


  Quaternion Quaternion::Negate(const Quaternion& quaternion)
  {
    // Quaternion result(OptimizationFlag::NoInitialization);
    Quaternion result;
    result.X = -quaternion.X;
    result.Y = -quaternion.Y;
    result.Z = -quaternion.Z;
    result.W = -quaternion.W;
    return result;
  }


  void Quaternion::Negate(Quaternion& rResult, const Quaternion& quaternion)
  {
    rResult.X = -quaternion.X;
    rResult.Y = -quaternion.Y;
    rResult.Z = -quaternion.Z;
    rResult.W = -quaternion.W;
  }


  void Quaternion::Normalize()
  {
    const float num2 = (((X * X) + (Y * Y)) + (Z * Z)) + (W * W);
    const float num = 1.0f / (static_cast<float>(std::sqrt(static_cast<double>(num2))));
    X *= num;
    Y *= num;
    Z *= num;
    W *= num;
  }


  Quaternion Quaternion::Normalize(const Quaternion& quaternion)
  {
    const float num2 =
      (((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y)) + (quaternion.Z * quaternion.Z)) + (quaternion.W * quaternion.W);
    const float num = 1.0f / (static_cast<float>(std::sqrt(static_cast<double>(num2))));

    // Quaternion result(OptimizationFlag::NoInitialization);
    Quaternion result;
    result.X = quaternion.X * num;
    result.Y = quaternion.Y * num;
    result.Z = quaternion.Z * num;
    result.W = quaternion.W * num;
    return result;
  }


  void Quaternion::Normalize(Quaternion& rResult, const Quaternion& quaternion)
  {
    const float num2 =
      (((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y)) + (quaternion.Z * quaternion.Z)) + (quaternion.W * quaternion.W);
    const float num = 1.0f / (static_cast<float>(std::sqrt(static_cast<double>(num2))));
    rResult.X = quaternion.X * num;
    rResult.Y = quaternion.Y * num;
    rResult.Z = quaternion.Z * num;
    rResult.W = quaternion.W * num;
  }

  // public static Quaternion Quaternion::operator -(Quaternion quaternion)
  //{
  //  Quaternion result;
  //  result.X = -quaternion.X;
  //  result.Y = -quaternion.Y;
  //  result.Z = -quaternion.Z;
  //  result.W = -quaternion.W;
  //  return result;
  //}

  // internal string Quaternion::DebugDisplayString
  //{
  //  get
  //  {
  //    if (this == Quaternion.identity)
  //    {
  //      return "Identity";
  //    }

  //    return string.Concat(
  //      this.X.ToString(), " ",
  //      this.Y.ToString(), " ",
  //      this.Z.ToString(), " ",
  //      this.W.ToString()
  //      );
  //  }
  //}

  // public override string Quaternion::ToString()
  //  {
  //    System.Text.StringBuilder sb = new System.Text.StringBuilder(32);
  //    sb.Append("{X:");
  //    sb.Append(this.X);
  //    sb.Append(" Y:");
  //    sb.Append(this.Y);
  //    sb.Append(" Z:");
  //    sb.Append(this.Z);
  //    sb.Append(" W:");
  //    sb.Append(this.W);
  //    sb.Append("}");
  //    return sb.ToString();
  //  }

  //  internal Matrix Quaternion::ToMatrix()
  //  {
  //    Matrix matrix = Matrix.Identity;
  //    ToMatrix(out matrix);
  //    return matrix;
  //  }

  //  internal void Quaternion::ToMatrix(out Matrix matrix)
  //  {
  //    Quaternion.ToMatrix(this, out matrix);
  //  }

  //  internal static void Quaternion::ToMatrix(Quaternion quaternion, out Matrix matrix)
  //  {

  //    // source -> http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation#Quaternion_to_Matrix
  //    float x2 = quaternion.X * quaternion.X;
  //    float y2 = quaternion.Y * quaternion.Y;
  //    float z2 = quaternion.Z * quaternion.Z;
  //    float xy = quaternion.X * quaternion.Y;
  //    float xz = quaternion.X * quaternion.Z;
  //    float yz = quaternion.Y * quaternion.Z;
  //    float wx = quaternion.W * quaternion.X;
  //    float wy = quaternion.W * quaternion.Y;
  //    float wz = quaternion.W * quaternion.Z;

  //    // This calculation would be a lot more complicated for non-unit length quaternions
  //    // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
  //    //   OpenGL
  //    pMatrix[_M11] = 1.0f - 2.0f * (y2 + z2);
  //    pMatrix[_M12] = 2.0f * (xy - wz);
  //    pMatrix[_M13] = 2.0f * (xz + wy);
  //    pMatrix[_M14] = 0.0f;

  //    pMatrix[_M21] = 2.0f * (xy + wz);
  //    pMatrix[_M22] = 1.0f - 2.0f * (x2 + z2);
  //    pMatrix[_M23] = 2.0f * (yz - wx);
  //    pMatrix[_M24] = 0.0f;

  //    pMatrix[_M31] = 2.0f * (xz - wy);
  //    pMatrix[_M32] = 2.0f * (yz + wx);
  //    pMatrix[_M33] = 1.0f - 2.0f * (x2 + y2);
  //    pMatrix[_M34] = 0.0f;

  //    pMatrix[_M41] = 2.0f * (xz - wy);
  //    pMatrix[_M42] = 2.0f * (yz + wx);
  //    pMatrix[_M43] = 1.0f - 2.0f * (x2 + y2);
  //    pMatrix[_M44] = 0.0f;

  //    //return Matrix4( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
  //    //    2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
  //    //    2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
  //    //    0.0f, 0.0f, 0.0f, 1.0f)
  //    //  }
  //  }

  //  internal Vector3 Xyz
  //  {
  //    get{
  //      return new Vector3(X, Y, Z);
  //    }

  //      set{
  //      X = value.X;
  //      Y = value.Y;
  //      Z = value.Z;
  //    }
  //  }
}


Fsl::Quaternion operator/(const Fsl::Quaternion lhs, const Fsl::Quaternion& rhs)
{
  const float x = lhs.X;
  const float y = lhs.Y;
  const float z = lhs.Z;
  const float w = lhs.W;
  const float num14 = (((rhs.X * rhs.X) + (rhs.Y * rhs.Y)) + (rhs.Z * rhs.Z)) + (rhs.W * rhs.W);
  const float num5 = 1.0f / num14;
  const float num4 = -rhs.X * num5;
  const float num3 = -rhs.Y * num5;
  const float num2 = -rhs.Z * num5;
  const float num = rhs.W * num5;
  const float num13 = (y * num2) - (z * num3);
  const float num12 = (z * num4) - (x * num2);
  const float num11 = (x * num3) - (y * num4);
  const float num10 = ((x * num4) + (y * num3)) + (z * num2);

  // Fsl::Quaternion quaternion(Fsl::OptimizationFlag::NoInitialization);
  Fsl::Quaternion quaternion;
  quaternion.X = ((x * num) + (num4 * w)) + num13;
  quaternion.Y = ((y * num) + (num3 * w)) + num12;
  quaternion.Z = ((z * num) + (num2 * w)) + num11;
  quaternion.W = (w * num) - num10;
  return quaternion;
}


Fsl::Quaternion operator*(const Fsl::Quaternion lhs, const Fsl::Quaternion& rhs)
{
  const float x = lhs.X;
  const float y = lhs.Y;
  const float z = lhs.Z;
  const float w = lhs.W;
  const float num4 = rhs.X;
  const float num3 = rhs.Y;
  const float num2 = rhs.Z;
  const float num = rhs.W;
  const float num12 = (y * num2) - (z * num3);
  const float num11 = (z * num4) - (x * num2);
  const float num10 = (x * num3) - (y * num4);
  const float num9 = ((x * num4) + (y * num3)) + (z * num2);

  // Fsl::Quaternion quaternion(Fsl::OptimizationFlag::NoInitialization);
  Fsl::Quaternion quaternion;
  quaternion.X = ((x * num) + (num4 * w)) + num12;
  quaternion.Y = ((y * num) + (num3 * w)) + num11;
  quaternion.Z = ((z * num) + (num2 * w)) + num10;
  quaternion.W = (w * num) - num9;
  return quaternion;
}
