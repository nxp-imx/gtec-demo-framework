#ifndef FSLBASE_MATH_VECTOR4_HPP
#define FSLBASE_MATH_VECTOR4_HPP
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


#include <FslBase/OptimizationFlag.hpp>

namespace Fsl
{
  struct Vector2;
  struct Vector3;
  struct Matrix;

  struct Vector4
  {
  public:
    float X;
    float Y;
    float Z;
    float W;

    Vector4()
      : X(0), Y(0), Z(0), W(0)
    {
    }

    explicit Vector4(const Vector2& v, const float z, const float w);
    explicit Vector4(const Vector3& v, const float w);

    Vector4(const float x, const float y, const float z, const float w)
      : X(x), Y(y), Z(z), W(w)
    {
    }

    //! @brief A optimization constructor that doesn't initialize the Vector4 so when this is called the content of the Vector4 in undefined!!!
    Vector4(const OptimizationFlag flag) {};

    //! @brief Get direct access to the elements
    const float* DirectAccess() const;

    // @brief Returns the vector with all components being zero (1, 1, 1, 1)
    static Vector4 One() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }

    // @brief Returns the x unit vector (1, 0, 0, 0)
    static Vector4 UnitX() { return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }

    // @brief Returns the y unit vector (0, 1, 0, 0)
    static Vector4 UnitY() { return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }

    // @brief Returns the z unit vector (0, 0, 1, 0)
    static Vector4 UnitZ() { return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }

    // @brief Returns the w unit vector (0, 0, 0, 1)
    static Vector4 UnitW() { return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

    // @brief Returns the vector with all components being zero (0, 0, 0, 0)
    static Vector4 Zero() { return Vector4(); }

    //! @brief Creates a new Vector4 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar b2 which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar b3 which represents a weighting factor towards third vector of 4d-triangle.
    //! @return The Cartesian translation of barycentric coordinates.
    static Vector4 Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2);

    //! @brief Creates a new Vector4 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar <c>b2</c> which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar <c>b3</c> which represents a weighting factor towards third vector of 4d-triangle.
    //! @param result The Cartesian translation of barycentric coordinates as an output parameter.
    static void Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2, Vector4& rResult);

    //! @brief Creates a new Vector4 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @return The result of CatmullRom interpolation.
    static Vector4 CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount);

    //! @brief Creates a new Vector4 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @param result The result of CatmullRom interpolation as an output parameter.
    static void CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount, Vector4& rResult);

    //! @brief Restricts a value to be within a specified range.
    static Vector4 Clamp(const Vector4& value, const Vector4& min, const Vector4& max);

    //! @brief Restricts a value to be within a specified range.
    static void Clamp(Vector4& rResult, const Vector4& value, const Vector4& min, const Vector4& max);

    //! @brief Calculates the distance between two vectors
    static float Distance(const Vector4& vector1, const Vector4& vector2);

    //! @brief Calculates the distance between two vectors squared.
    static float DistanceSquared(const Vector4& vector1, const Vector4& vector2)
    {
      return (vector1.X - vector2.X) * (vector1.X - vector2.X) +
        (vector1.Y - vector2.Y) * (vector1.Y - vector2.Y) +
        (vector1.Z - vector2.Z) * (vector1.Z - vector2.Z) +
        (vector1.W - vector2.W) * (vector1.W - vector2.W);
    }

    //! @brief Divides the components of a Vector4 by the components of another Vector4.
    //! @param value1 Source
    //! @param value2 Divisor
    //! @return The result of dividing the vectors.
    static Vector4 Divide(const Vector4& value1, const Vector4& value2)
    {
      return Vector4(value1.X / value2.X, value1.Y / value2.Y, value1.Z / value2.Z, value1.W / value2.W);
    }

    //! @brief Divides the components of a Vector4 by a scalar.
    //! @param value1
    //! @param divider Divisor scalar.
    //! @return The result of dividing a vector by a scalar.
    static Vector4 Divide(const Vector4& value, const float divider)
    {
      return Vector4(value.X / divider, value.Y / divider, value.Z / divider, value.W / divider);
    }

    //! @brief Calculates the dot product of two vectors. If the two vectors are unit vectors,
    //         the dot product returns a floating point value between -1 and 1 that can be used
    //         to determine some properties of the angle between two vectors. For example,
    //         it can show whether the vectors are orthogonal, parallel, or have an acute or
    //         obtuse angle between them.
    static float Dot(const Vector4& vector1, const Vector4& vector2)
    {
      return vector1.X * vector2.X + vector1.Y * vector2.Y + vector1.Z * vector2.Z + vector1.W * vector2.W;
    }

    //! @brief Creates a new Vector4 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @return The hermite spline interpolation vector.
    static Vector4 Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount);

    //! @brief Creates a new Vector4 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @param result The hermite spline interpolation vector as an output parameter.
    static void Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount, Vector4& rResult);

    //! @brief Calculates the length of the vector.
    float Length() const;

    //! @brief Calculates the length of the vector squared.
    float LengthSquared() const
    {
      return (X*X) + (Y*Y) + (Z*Z) + (W*W);
    }

    //! @brief Performs a linear interpolation between two vectors.
    static Vector4 Lerp(const Vector4& value1, const Vector4 value2, const float amount);

    //! @brief Performs a linear interpolation between two vectors.
    static void Lerp(Vector4& rResult, const Vector4& value1, const Vector4 value2, const float amount);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static Vector4 Max(const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static void Max(Vector4& rResult, const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static Vector4 Min(const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static void Min(Vector4& rResult, const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector pointing in the opposite direction.
    static Vector4 Negate(const Vector4& value);

    //! @brief Returns a vector pointing in the opposite direction.
    static void Negate(Vector4& rResult, const Vector4& value);

    //! @brief Returns a vector pointing in the opposite direction.
    Vector4 operator-() const
    {
      return Vector4(-X, -Y, -Z, -W);
    }

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    void Normalize();

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static Vector4 Normalize(const Vector4& vector);

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static void Normalize(Vector4& rResult, const Vector4& value);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static Vector4 Reflect(const Vector4& vector, const Vector4& normal);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static void Reflect(Vector4& rResult, const Vector4& vector, const Vector4& normal);

    //! @brief Creates a new Vector4 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source.
    //! @param value2 Source.
    //! @param amount Weighting value.
    //! @return Cubic interpolation of the specified vectors.
    static Vector4 SmoothStep(const Vector4& value1, const Vector4& value2, const float amount);

    //! @brief Creates a new Vector4 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source
    //! @param value2 Source
    //! @param amount Weighting value.
    //! @param result Cubic interpolation of the specified vectors as an output parameter.
    static void SmoothStep(const Vector4& value1, const Vector4& value2, const float amount, Vector4& rResult);

    //! @brief Transforms a vector by the specified Matrix
    static Vector4 Transform(const Vector2& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(Vector4& rResult, const Vector2& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static Vector4 Transform(const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(Vector4& rResult, const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static Vector4 Transform(const Vector4& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(Vector4& rResult, const Vector4& position, const Matrix& matrix);

    //! @brief Add vector to vector
    Vector4& operator+=(const Vector4& arg)
    {
      X += arg.X;
      Y += arg.Y;
      Z += arg.Z;
      W += arg.W;
      return *this;
    }

    //! @brief Subtracts vector from vector
    Vector4& operator-=(const Vector4& arg)
    {
      X -= arg.X;
      Y -= arg.Y;
      Z -= arg.Z;
      W -= arg.W;
      return *this;
    }

    //! @brief Multiply vector by vector
    Vector4& operator*=(const Vector4& arg)
    {
      X *= arg.X;
      Y *= arg.Y;
      Z *= arg.Z;
      W *= arg.W;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector4& operator*=(const int arg)
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      W *= arg;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector4& operator*=(const float arg)
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      W *= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector4& operator/=(const int arg)
    {
      X /= arg;
      Y /= arg;
      Z /= arg;
      W /= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector4& operator/=(const float arg)
    {
      X /= arg;
      Y /= arg;
      Z /= arg;
      W /= arg;
      return *this;
    }

    //! @brief Tests for equality.
    bool operator==(const Vector4 &rhs) const
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
    }

    //! @brief Tests for inequality.
    bool operator!=(const Vector4 &rhs) const
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W;
    }

  };
}

//! @brief Add a vector to a vector.
inline Fsl::Vector4 operator+(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
}

//! @brief Subtracts a vector from a vector.
inline Fsl::Vector4 operator-(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
}

//! @brief multiply a vector with a vector
inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const int rhs)
{
  return Fsl::Vector4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const float rhs)
{
  return Fsl::Vector4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs);
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector4 operator*(const int lhs, const Fsl::Vector4& rhs)
{
  return rhs * lhs;
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector4 operator*(const float lhs, const Fsl::Vector4& rhs)
{
  return rhs * lhs;
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector4 operator/(const Fsl::Vector4& lhs, const int rhs)
{
  return Fsl::Vector4(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs, lhs.W / rhs);
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector4 operator/(const Fsl::Vector4& lhs, const float rhs)
{
  return Fsl::Vector4(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs, lhs.W / rhs);
}


#endif
