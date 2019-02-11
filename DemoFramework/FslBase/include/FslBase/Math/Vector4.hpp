#ifndef FSLBASE_MATH_VECTOR4_HPP
#define FSLBASE_MATH_VECTOR4_HPP
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

#include <FslBase/Attributes.hpp>
//#include <FslBase/OptimizationFlag.hpp>
#include <cstddef>

namespace Fsl
{
  struct Vector2;
  struct Vector3;
  struct Matrix;

  struct Vector4
  {
    using value_type = float;
    using size_type = std::size_t;

    static constexpr size_type NumElements = 4;

    constexpr size_type size() const
    {
      return NumElements;
    }

    float X;
    float Y;
    float Z;
    float W;

    constexpr Vector4()
      : X{0.0f}
      , Y{0.0f}
      , Z{0.0f}
      , W{0.0f}
    {
    }

    explicit Vector4(const Vector2& v, const float z, const float w);
    explicit Vector4(const Vector3& v, const float w);

    constexpr Vector4(const float x, const float y, const float z, const float w)
      : X(x)
      , Y(y)
      , Z(z)
      , W(w)
    {
    }

    //! @brief A optimization constructor that doesn't initialize the Vector4 so when this is called the content of the Vector4 in undefined!!!
    // coverity[uninit_member]
    // Vector4(const OptimizationFlag flag)
    //{
    //}

    //! @brief Get direct access to the elements
    const float* DirectAccess() const
    {
      return &X;
    }

    // @brief Returns the vector with all components being zero (1, 1, 1, 1)
    static constexpr Vector4 One()
    {
      return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // @brief Returns the x unit vector (1, 0, 0, 0)
    static constexpr Vector4 UnitX()
    {
      return Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    // @brief Returns the y unit vector (0, 1, 0, 0)
    static constexpr Vector4 UnitY()
    {
      return Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    // @brief Returns the z unit vector (0, 0, 1, 0)
    static constexpr Vector4 UnitZ()
    {
      return Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    // @brief Returns the w unit vector (0, 0, 0, 1)
    static constexpr Vector4 UnitW()
    {
      return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // @brief Returns the vector with all components being zero (0, 0, 0, 0)
    static constexpr Vector4 Zero()
    {
      return {};
    }

    //! @brief Creates a new Vector4 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to
    //! 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar b2 which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar b3 which represents a weighting factor towards third vector of 4d-triangle.
    //! @return The Cartesian translation of barycentric coordinates.
    static Vector4 Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2);

    //! @brief Creates a new Vector4 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to
    //! 4d-triangle.
    //! @param value1 The first vector of 4d-triangle.
    //! @param value2 The second vector of 4d-triangle.
    //! @param value3 The third vector of 4d-triangle.
    //! @param amount1 Barycentric scalar <c>b2</c> which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar <c>b3</c> which represents a weighting factor towards third vector of 4d-triangle.
    //! @param result The Cartesian translation of barycentric coordinates as an output parameter.
    static void Barycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2,
                            Vector4& rResult);

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
    static void CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount,
                           Vector4& rResult);

    //! @brief Restricts a value to be within a specified range.
    static Vector4 Clamp(const Vector4& value, const Vector4& min, const Vector4& max);

    //! @brief Restricts a value to be within a specified range.
    static void Clamp(const Vector4& value, const Vector4& min, const Vector4& max, Vector4& rResult);

    //! @brief Restricts a value to be within a specified range.
    FSL_ATTR_DEPRECATED static void Clamp(Vector4& rResult, const Vector4& value, const Vector4& min, const Vector4& max)
    {
      Clamp(value, min, max, rResult);
    }

    //! @brief Calculates the distance between two vectors
    static float Distance(const Vector4& vector1, const Vector4& vector2);

    //! @brief Calculates the distance between two vectors squared.
    static constexpr float DistanceSquared(const Vector4& vector1, const Vector4& vector2)
    {
      return (vector1.X - vector2.X) * (vector1.X - vector2.X) + (vector1.Y - vector2.Y) * (vector1.Y - vector2.Y) +
             (vector1.Z - vector2.Z) * (vector1.Z - vector2.Z) + (vector1.W - vector2.W) * (vector1.W - vector2.W);
    }

    //! @brief Divides the components of a Vector4 by the components of another Vector4.
    //! @param value1 Source
    //! @param value2 Divisor
    //! @return The result of dividing the vectors.
    static constexpr Vector4 Divide(const Vector4& value1, const Vector4& value2)
    {
      return Vector4(value1.X / value2.X, value1.Y / value2.Y, value1.Z / value2.Z, value1.W / value2.W);
    }

    //! @brief Divides the components of a Vector4 by a scalar.
    //! @param value1
    //! @param divider Divisor scalar.
    //! @return The result of dividing a vector by a scalar.
    static constexpr Vector4 Divide(const Vector4& value, const float divider)
    {
      return Vector4(value.X / divider, value.Y / divider, value.Z / divider, value.W / divider);
    }

    //! @brief Calculates the dot product of two vectors. If the two vectors are unit vectors,
    //         the dot product returns a floating point value between -1 and 1 that can be used
    //         to determine some properties of the angle between two vectors. For example,
    //         it can show whether the vectors are orthogonal, parallel, or have an acute or
    //         obtuse angle between them.
    static constexpr float Dot(const Vector4& vector1, const Vector4& vector2)
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
    static void Hermite(const Vector4& value1, const Vector4& tangent1, const Vector4& value2, const Vector4& tangent2, const float amount,
                        Vector4& rResult);

    //! @brief Calculates the length of the vector.
    float Length() const;

    //! @brief Calculates the length of the vector squared.
    constexpr float LengthSquared() const
    {
      return (X * X) + (Y * Y) + (Z * Z) + (W * W);
    }

    //! @brief Performs a linear interpolation between two vectors.
    static Vector4 Lerp(const Vector4& value1, const Vector4 value2, const float amount);

    //! @brief Performs a linear interpolation between two vectors.
    FSL_ATTR_DEPRECATED static void Lerp(Vector4& rResult, const Vector4& value1, const Vector4 value2, const float amount)
    {
      Lerp(value1, value2, amount, rResult);
    }

    //! @brief Performs a linear interpolation between two vectors.
    static void Lerp(const Vector4& value1, const Vector4 value2, const float amount, Vector4& rResult);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static Vector4 Max(const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    FSL_ATTR_DEPRECATED static void Max(Vector4& rResult, const Vector4& value1, const Vector4& value2)
    {
      Max(value1, value2, rResult);
    }

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static void Max(const Vector4& value1, const Vector4& value2, Vector4& rResult);


    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static Vector4 Min(const Vector4& value1, const Vector4& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    FSL_ATTR_DEPRECATED static void Min(Vector4& rResult, const Vector4& value1, const Vector4& value2)
    {
      Min(value1, value2, rResult);
    }

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static void Min(const Vector4& value1, const Vector4& value2, Vector4& rResult);

    //! @brief Returns a vector pointing in the opposite direction.
    static constexpr Vector4 Negate(const Vector4& value)
    {
      return Vector4(-value.X, -value.Y, -value.Z, -value.W);
    }

    //! @brief Returns a vector pointing in the opposite direction.
    FSL_ATTR_DEPRECATED static void Negate(Vector4& rResult, const Vector4& value)
    {
      Negate(value, rResult);
    }

    //! @brief Returns a vector pointing in the opposite direction.
    static void Negate(const Vector4& value, Vector4& rResult);

    //! @brief Returns a vector pointing in the opposite direction.
    constexpr Vector4 operator-() const
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
    FSL_ATTR_DEPRECATED static void Normalize(Vector4& rResult, const Vector4& value)
    {
      Normalize(value, rResult);
    }

    static void Normalize(const Vector4& value, Vector4& rResult);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static Vector4 Reflect(const Vector4& vector, const Vector4& normal);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    FSL_ATTR_DEPRECATED static void Reflect(Vector4& rResult, const Vector4& vector, const Vector4& normal)
    {
      Reflect(vector, normal, rResult);
    }

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static void Reflect(const Vector4& vector, const Vector4& normal, Vector4& rResult);

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
    FSL_ATTR_DEPRECATED static void Transform(Vector4& rResult, const Vector2& position, const Matrix& matrix)
    {
      Transform(position, matrix, rResult);
    }

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(const Vector2& position, const Matrix& matrix, Vector4& rResult);

    //! @brief Transforms a vector by the specified Matrix
    static Vector4 Transform(const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    FSL_ATTR_DEPRECATED static void Transform(Vector4& rResult, const Vector3& position, const Matrix& matrix)
    {
      Transform(position, matrix, rResult);
    }

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(const Vector3& position, const Matrix& matrix, Vector4& rResult);

    //! @brief Transforms a vector by the specified Matrix
    static Vector4 Transform(const Vector4& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    FSL_ATTR_DEPRECATED static void Transform(Vector4& rResult, const Vector4& position, const Matrix& matrix)
    {
      Transform(position, matrix, rResult);
    }

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(const Vector4& position, const Matrix& matrix, Vector4& rResult);

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
    constexpr bool operator==(const Vector4& rhs) const
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
    }

    //! @brief Tests for inequality.
    constexpr bool operator!=(const Vector4& rhs) const
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W;
    }

    //! @brief rResult = lhs + rhs;
    //! Improvement: constexpr when we adopt C++14
    static inline void Add(const Vector4& lhs, const Vector4& rhs, Vector4& rResult)
    {
      rResult.X = lhs.X + rhs.X;
      rResult.Y = lhs.Y + rhs.Y;
      rResult.Z = lhs.Z + rhs.Z;
      rResult.W = lhs.W + rhs.W;
    }

    //! @brief rResult = lhs - rhs;
    //! Improvement: constexpr when we adopt C++14
    static inline void Subtract(const Vector4& lhs, const Vector4& rhs, Vector4& rResult)
    {
      rResult.X = lhs.X - rhs.X;
      rResult.Y = lhs.Y - rhs.Y;
      rResult.Z = lhs.Z - rhs.Z;
      rResult.W = lhs.W - rhs.W;
    }

    //! @brief rResult = lhs * rhs;
    //! Improvement: constexpr when we adopt C++14
    static inline void Multiply(const Vector4& lhs, const Vector4& rhs, Vector4& rResult)
    {
      rResult.X = lhs.X * rhs.X;
      rResult.Y = lhs.Y * rhs.Y;
      rResult.Z = lhs.Z * rhs.Z;
      rResult.W = lhs.W * rhs.W;
    }

    //! @brief rResult = lhs / rhs;
    //! Improvement: constexpr when we adopt C++14
    static inline void Divide(const Vector4& lhs, const Vector4& rhs, Vector4& rResult)
    {
      rResult.X = lhs.X / rhs.X;
      rResult.Y = lhs.Y / rhs.Y;
      rResult.Z = lhs.Z / rhs.Z;
      rResult.W = lhs.W / rhs.W;
    }
  };
}

//! @brief Add a vector to a vector.
constexpr inline Fsl::Vector4 operator+(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W);
}

//! @brief Subtracts a vector from a vector.
constexpr inline Fsl::Vector4 operator-(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W);
}

//! @brief multiply a vector with a vector
constexpr inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W);
}

//! @brief multiply a vector with a scalar
constexpr inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const int rhs)
{
  return {lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs};
}

//! @brief multiply a vector with a scalar
constexpr inline Fsl::Vector4 operator*(const Fsl::Vector4& lhs, const float rhs)
{
  return {lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs};
}

//! @brief multiply a scalar with a vector
constexpr inline Fsl::Vector4 operator*(const int lhs, const Fsl::Vector4& rhs)
{
  return rhs * lhs;
}

//! @brief multiply a scalar with a vector
constexpr inline Fsl::Vector4 operator*(const float lhs, const Fsl::Vector4& rhs)
{
  return rhs * lhs;
}

//! @brief divide a vector by a vector
constexpr inline Fsl::Vector4 operator/(const Fsl::Vector4& lhs, const Fsl::Vector4& rhs)
{
  return Fsl::Vector4(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z, lhs.W / rhs.W);
}

//! @brief Divide a vector by a scalar
constexpr inline Fsl::Vector4 operator/(const Fsl::Vector4& lhs, const int rhs)
{
  return Fsl::Vector4(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs, lhs.W / rhs);
}

//! @brief Divide a vector by a scalar
constexpr inline Fsl::Vector4 operator/(const Fsl::Vector4& lhs, const float rhs)
{
  return Fsl::Vector4(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs, lhs.W / rhs);
}


#endif
