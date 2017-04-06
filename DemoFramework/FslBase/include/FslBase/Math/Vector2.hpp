#ifndef FSLBASE_MATH_VECTOR2_HPP
#define FSLBASE_MATH_VECTOR2_HPP
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

// The functions in this file are a port of an MIT licensed library: MonaGame - Vector2.cs.

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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/OptimizationFlag.hpp>

namespace Fsl
{
  struct Quaternion;
  struct Matrix;

  struct Vector2
  {
  public:
    float X;
    float Y;

    Vector2()
      : X(0), Y(0)
    {
    }

    //! overload to prevent annoying warning
    Vector2(const float x, const float y)
      : X(x), Y(y)
    {
    }

    //! overload to prevent annoying warning
    Vector2(const int32_t x, const int32_t y)
      : X(static_cast<float>(x)), Y(static_cast<float>(y))
    {
    }


    //! overload to prevent annoying warning
    Vector2(const float x, const int32_t y)
      : X(x), Y(static_cast<float>(y))
    {
    }

    //! overload to prevent annoying warning
    Vector2(const int32_t x, const float y)
      : X(static_cast<float>(x)), Y(y)
    {
    }

    //! @brief A optimization constructor that doesn't initialize the Vector2 so when this is called the content of the Vector2 in undefined!!!
    Vector2(const OptimizationFlag flag) {};

    //! @brief Get direct access to the elements
    const float* DirectAccess() const;

    // @brief Returns the vector with all components being zero (1, 1)
    static Vector2 One() { return Vector2(1.0f, 1.0f); }

    // @brief Returns the x unit vector (1, 0)
    static Vector2 UnitX() { return Vector2(1.0f, 0.0f); }

    // @brief Returns the y unit vector (0, 1)
    static Vector2 UnitY() { return Vector2(0.0f, 1.0f); }

    // @brief Returns the vector with all components being zero (0, 0)
    static Vector2 Zero() { return Vector2(); }

    //! @brief Creates a new Vector2 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar b2 which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar b3 which represents a weighting factor towards third vector of 4d-triangle.
    //! @return The Cartesian translation of barycentric coordinates.
    static Vector2 Barycentric(const Vector2& value1, const Vector2& value2, const Vector2& value3, const float amount1, const float amount2);

    //! @brief Creates a new Vector2 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar <c>b2</c> which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar <c>b3</c> which represents a weighting factor towards third vector of 4d-triangle.
    //! @param result The Cartesian translation of barycentric coordinates as an output parameter.
    static void Barycentric(const Vector2& value1, const Vector2& value2, const Vector2& value3, const float amount1, const float amount2, Vector2& rResult);

    //! @brief Creates a new Vector2 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @return The result of CatmullRom interpolation.
    static Vector2 CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount);

    //! @brief Creates a new Vector2 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @param result The result of CatmullRom interpolation as an output parameter.
    static void CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount, Vector2& rResult);

    //! @brief Restricts a value to be within a specified range.
    static Vector2 Clamp(const Vector2& value, const Vector2& min, const Vector2& max);

    //! @brief Restricts a value to be within a specified range.
    static void Clamp(Vector2& rResult, const Vector2& value, const Vector2& min, const Vector2& max);

    //! @brief Calculates the distance between two vectors
    static float Distance(const Vector2& vector1, const Vector2& vector2);

    //! @brief Calculates the distance between two vectors squared.
    static float DistanceSquared(const Vector2& vector1, const Vector2& vector2)
    {
      return (vector1.X - vector2.X) * (vector1.X - vector2.X) +
        (vector1.Y - vector2.Y) * (vector1.Y - vector2.Y);
    }


    //! @brief Calculates the dot product of two vectors. If the two vectors are unit vectors,
    //         the dot product returns a floating point value between -1 and 1 that can be used
    //         to determine some properties of the angle between two vectors. For example,
    //         it can show whether the vectors are orthogonal, parallel, or have an acute or
    //         obtuse angle between them.
    static float Dot(const Vector2& vector1, const Vector2& vector2)
    {
      return vector1.X * vector2.X + vector1.Y * vector2.Y;
    }

    //! @brief Creates a new Vector2 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @return The hermite spline interpolation vector.
    static Vector2 Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2, const Vector2& tangent2, const float amount);

    //! @brief Creates a new Vector2 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @param result The hermite spline interpolation vector as an output parameter.
    static void Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2, const Vector2& tangent2, const float amount, Vector2& rResult);

    //! @brief Calculates the length of the vector.
    float Length() const;

    //! @brief Calculates the length of the vector squared.
    float LengthSquared() const
    {
      return (X*X) + (Y*Y);
    }

    //! @brief Performs a linear interpolation between two vectors.
    static Vector2 Lerp(const Vector2& value1, const Vector2 value2, const float amount);

    //! @brief Performs a linear interpolation between two vectors.
    static void Lerp(Vector2& rResult, const Vector2& value1, const Vector2 value2, const float amount);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static Vector2 Max(const Vector2& value1, const Vector2& value2);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static void Max(Vector2& rResult, const Vector2& value1, const Vector2& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static Vector2 Min(const Vector2& value1, const Vector2& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static void Min(Vector2& rResult, const Vector2& value1, const Vector2& value2);

    //! @brief Returns a vector pointing in the opposite direction.
    static Vector2 Negate(const Vector2& value);

    //! @brief Returns a vector pointing in the opposite direction.
    static void Negate(Vector2& rResult, const Vector2& value);

    //! @brief Returns a vector pointing in the opposite direction.
    Vector2 operator-() const
    {
      return Vector2(-X, -Y);
    }

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    void Normalize();

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static Vector2 Normalize(const Vector2& vector);

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static void Normalize(Vector2& rResult, const Vector2& value);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static Vector2 Reflect(const Vector2& vector, const Vector2& normal);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static void Reflect(Vector2& rResult, const Vector2& vector, const Vector2& normal);

    //! @brief Creates a new Vector2 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source.
    //! @param value2 Source.
    //! @param amount Weighting value.
    //! @return Cubic interpolation of the specified vectors.
    static Vector2 SmoothStep(const Vector2& value1, const Vector2& value2, const float amount);

    //! @brief Creates a new Vector2 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source
    //! @param value2 Source
    //! @param amount Weighting value.
    //! @param result Cubic interpolation of the specified vectors as an output parameter.
    static void SmoothStep(const Vector2& value1, const Vector2& value2, const float amount, Vector2& rResult);

    //! @brief Transforms a vector by the specified Matrix
    static Vector2 Transform(const Vector2& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(Vector2& rResult, const Vector2& position, const Matrix& matrix);

    //! @brief Creates a new Vector2 that contains a transformation of vector(position.X,position.Y,0,0) by the specified Quaternion representing the rotation.
    //! @param value Source Vector2.
    //! @param rotation The Quaternion which contains rotation transformation
    //! @return Transformed Vector2.
    static Vector2 Transform(const Vector2& value, const Quaternion& rotation);

    //! @brief Creates a new Vector2 that contains a transformation of vector(position.X,position.Y,0,0) by the specified Quaternion, representing the rotation.
    //! @param value Source Vector2.
    //! @param rotation The Quaternion which contains rotation transformation.
    //! @param result Transformed Vector2 as an output parameter.
    static void Transform(const Vector2& value, const Quaternion& rotation, Vector2& rResult);

    //! @brief Transforms a normal vector by the specified Matrix
    static Vector2 TransformNormal(const Vector2& position, const Matrix& matrix);

    //! @brief Transforms a normal vector by the specified Matrix
    static void TransformNormal(Vector2& rResult, const Vector2& position, const Matrix& matrix);

    //! @brief Add vector to vector
    Vector2& operator+=(const Vector2& arg)
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    //! @brief Subtracts vector from vector
    Vector2& operator-=(const Vector2& arg)
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    //! @brief Multiply vector by vector
    Vector2& operator*=(const Vector2& arg)
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector2& operator*=(const int arg)
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector2& operator*=(const float arg)
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector2& operator/=(const int arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector2& operator/=(const float arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    //! @brief Tests for equality.
    bool operator==(const Vector2 &rhs) const
    {
      return X == rhs.X && Y == rhs.Y;
    }

    //! @brief Tests for inequality.
    bool operator!=(const Vector2 &rhs) const
    {
      return X != rhs.X || Y != rhs.Y;
    }

  };
}

//! @brief Add a vector to a vector.
inline const Fsl::Vector2 operator+(const Fsl::Vector2& lhs, const Fsl::Vector2& rhs)
{
  return Fsl::Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
}

//! @brief Subtracts a vector from a vector.
inline Fsl::Vector2 operator-(const Fsl::Vector2& lhs, const Fsl::Vector2& rhs)
{
  return Fsl::Vector2(lhs.X - rhs.X, lhs.Y - rhs.Y);
}

//! @brief multiply a vector with a vector
inline Fsl::Vector2 operator*(const Fsl::Vector2& lhs, const Fsl::Vector2& rhs)
{
  return Fsl::Vector2(lhs.X * rhs.X, lhs.Y * rhs.Y);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector2 operator*(const Fsl::Vector2& lhs, const int rhs)
{
  return Fsl::Vector2(lhs.X * rhs, lhs.Y * rhs);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector2 operator*(const Fsl::Vector2& lhs, const float rhs)
{
  return Fsl::Vector2(lhs.X * rhs, lhs.Y * rhs);
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector2 operator*(const int lhs, const Fsl::Vector2& rhs)
{
  return rhs * lhs;
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector2 operator*(const float lhs, const Fsl::Vector2& rhs)
{
  return rhs * lhs;
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector2 operator/(const Fsl::Vector2& lhs, const int rhs)
{
  return Fsl::Vector2(lhs.X / rhs, lhs.Y / rhs);
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector2 operator/(const Fsl::Vector2& lhs, const float rhs)
{
  return Fsl::Vector2(lhs.X / rhs, lhs.Y / rhs);
}


#endif
