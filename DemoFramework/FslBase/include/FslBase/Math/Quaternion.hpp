#ifndef FSLBASE_MATH_QUATERNION_HPP
#define FSLBASE_MATH_QUATERNION_HPP
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

#include <FslBase/BasicTypes.hpp>
// #include <FslBase/OptimizationFlag.hpp>

namespace Fsl
{
  struct Vector3;
  struct Matrix;

  struct Quaternion
  {
    float X{0};
    float Y{0};
    float Z{0};
    float W{0};

    constexpr Quaternion() = default;
    // coverity[uninit_member]
    // Quaternion(const OptimizationFlag flag){};
    constexpr Quaternion(const float x, const float y, const float z, const float w)
      : X(x)
      , Y(y)
      , Z(z)
      , W(w)
    {
    }

    Quaternion(const Vector3& vectorPart, const float scalarPart);

    static constexpr Quaternion Identity()
    {
      return Quaternion(0, 0, 0, 1);
    }

    static Quaternion Add(const Quaternion& lhs, const Quaternion& rhs);
    static void Add(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs);
    static Quaternion Concatenate(const Quaternion& lhs, const Quaternion& rhs);
    static void Concatenate(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs);

    void Conjugate();
    static Quaternion Conjugate(const Quaternion& value);
    static void Conjugate(Quaternion& rResult, const Quaternion& value);

    static Quaternion CreateFromAxisAngle(const Vector3& axis, const float angle);
    static void CreateFromAxisAngle(Quaternion& rResult, const Vector3& axis, const float angle);
    static Quaternion CreateFromRotationMatrix(const Matrix& matrix);
    static void CreateFromRotationMatrix(Quaternion& rResult, const Matrix& matrix);
    static Quaternion CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll);
    static void CreateFromYawPitchRoll(Quaternion& rResult, const float yaw, const float pitch, const float roll);
    static Quaternion Divide(const Quaternion& lhs, const Quaternion& rhs);
    static void Divide(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs);
    static float Dot(const Quaternion& lhs, const Quaternion& rhs);
    static void Dot(float& rResult, const Quaternion& lhs, const Quaternion& rhs);
    static Quaternion Inverse(const Quaternion& quaternion);
    static void Inverse(Quaternion& rResult, const Quaternion& quaternion);

    float Length() const;
    float LengthSquared() const;

    static Quaternion Lerp(const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount);
    static void Lerp(Quaternion& rResult, const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount);
    static Quaternion Slerp(const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount);
    static void Slerp(Quaternion& rResult, const Quaternion& quaternion1, const Quaternion& quaternion2, const float amount);

    static Quaternion Subtract(const Quaternion& lhs, const Quaternion& rhs);
    static void Subtract(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs);
    static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
    static Quaternion Multiply(const Quaternion& lhs, const float scaleFactor);
    static void Multiply(Quaternion& rResult, const Quaternion& lhs, const float& scaleFactor);
    static void Multiply(Quaternion& rResult, const Quaternion& lhs, const Quaternion& rhs);
    static Quaternion Negate(const Quaternion& quaternion);
    static void Negate(Quaternion& rResult, const Quaternion& quaternion);
    void Normalize();
    static Quaternion Normalize(const Quaternion& quaternion);
    static void Normalize(Quaternion& rResult, const Quaternion& quaternion);

    //! @brief Tests for equality.
    bool operator==(const Quaternion& rhs) const
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
    }

    //! @brief Tests for inequality.
    bool operator!=(const Quaternion& rhs) const
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W;
    }

    Quaternion& operator+=(const Quaternion& rhs)
    {
      X += rhs.X;
      Y += rhs.Y;
      Z += rhs.Z;
      W += rhs.W;
      return *this;
    }

    Quaternion& operator-=(const Quaternion& rhs)
    {
      X -= rhs.X;
      Y -= rhs.Y;
      Z -= rhs.Z;
      W -= rhs.W;
      return *this;
    }
  };
}


inline Fsl::Quaternion operator+(const Fsl::Quaternion& lhs, const Fsl::Quaternion& rhs)
{
  return Fsl::Quaternion(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
}


inline Fsl::Quaternion operator-(const Fsl::Quaternion& lhs, const Fsl::Quaternion& rhs)
{
  return Fsl::Quaternion(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
}


inline Fsl::Quaternion operator*(const Fsl::Quaternion lhs, const float scaleFactor)
{
  return Fsl::Quaternion(lhs.X * scaleFactor, lhs.Y * scaleFactor, lhs.Z * scaleFactor, lhs.W * scaleFactor);
}


Fsl::Quaternion operator/(const Fsl::Quaternion lhs, const Fsl::Quaternion& rhs);
Fsl::Quaternion operator*(const Fsl::Quaternion lhs, const Fsl::Quaternion& rhs);


#endif
