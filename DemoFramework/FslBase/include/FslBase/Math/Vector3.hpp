#ifndef FSLBASE_MATH_VECTOR3_HPP
#define FSLBASE_MATH_VECTOR3_HPP
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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector3.cs.

#include <FslBase/OptimizationFlag.hpp>

namespace Fsl
{
  //! forward declare
  struct Quaternion;
  struct Matrix;

  struct Vector3
  {
  public:
    float X;
    float Y;
    float Z;

    Vector3()
      : X(0), Y(0), Z(0)
    {
    }

    Vector3(const float x, const float y, const float z)
      : X(x), Y(y), Z(z)
    {
    }

    //! @brief A optimization constructor that doesn't initialize the Vector3 so when this is called the content of the Vector3 in undefined!!!
    // coverity[uninit_member]
    Vector3(const OptimizationFlag flag) {};

    //! @brief Get direct access to the elements
    const float* DirectAccess() const;

    // @brief Returns the vector with all components being zero (1, 1, 1)
    static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }

    // @brief Returns the x unit vector (1, 0, 0)
    static Vector3 UnitX() { return Vector3(1.0f, 0.0f, 0.0f); }

    // @brief Returns the y unit vector (0, 1, 0)
    static Vector3 UnitY() { return Vector3(0.0f, 1.0f, 0.0f); }

    // @brief Returns the z unit vector (0, 0, 1)
    static Vector3 UnitZ() { return Vector3(0.0f, 0.0f, 1.0f); }

    // @brief Returns the vector with all components being zero (0, 0, 0)
    static Vector3 Zero() { return Vector3(); }

    // @brief Returns a up vector (0, 1, 0)
    static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }

    // @brief Returns a down vector (0, -1, 0)
    static Vector3 Down() { return Vector3(0.0f, -1.0f, 0.0f); }

    // @brief Returns a Right vector (1, 0, 0)
    static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }

    // @brief Returns a Left vector (-1, 0, 0)
    static Vector3 Left() { return Vector3(-1.0f, 0.0f, 0.0f); }

    // @brief Returns a Forward vector (0, 0, -1)
    static Vector3 Forward() { return Vector3(0.0f, 0.0f, -1.0f); }

    // @brief Returns a Backward vector (0, 0, 1)
    static Vector3 Backward() { return Vector3(0.0f, 0.0f, 1.0f); }

    //! @brief Creates a new Vector3 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 3d-triangle.
    //! @param value2 The second vector of 3d-triangle.
    //! @param value3 The third vector of 3d-triangle.
    //! @param amount1 Barycentric scalar b2 which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar b3 which represents a weighting factor towards third vector of 4d-triangle.
    //! @return The Cartesian translation of barycentric coordinates.
    static Vector3 Barycentric(const Vector3& value1, const Vector3& value2, const Vector3& value3, const float amount1, const float amount2);

    //! @brief Creates a new Vector3 that contains the Cartesian coordinates of a vector specified in barycentric coordinates and relative to 4d-triangle.
    //! @param value1 The first vector of 3d-triangle.
    //! @param value2 The second vector of 3d-triangle.
    //! @param value3 The third vector of 3d-triangle.
    //! @param amount1 Barycentric scalar <c>b2</c> which represents a weighting factor towards second vector of 4d-triangle.
    //! @param amount2 Barycentric scalar <c>b3</c> which represents a weighting factor towards third vector of 4d-triangle.
    //! @param result The Cartesian translation of barycentric coordinates as an output parameter.
    static void Barycentric(const Vector3& value1, const Vector3& value2, const Vector3& value3, const float amount1, const float amount2, Vector3& rResult);

    //! @brief Creates a new Vector3 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @return The result of CatmullRom interpolation.
    static Vector3 CatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3, const Vector3& value4, const float amount);

    //! @brief Creates a new Vector3 that contains CatmullRom interpolation of the specified vectors.
    //! @param value1 The first vector in interpolation.
    //! @param value2 The second vector in interpolation.
    //! @param value3 The third vector in interpolation.
    //! @param value4 The fourth vector in interpolation.
    //! @param amount Weighting factor.
    //! @param result The result of CatmullRom interpolation as an output parameter.
    static void CatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3, const Vector3& value4, const float amount, Vector3& rResult);

    //! @brief Restricts a value to be within a specified range.
    static Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);

    //! @brief Restricts a value to be within a specified range.
    static void Clamp(Vector3& rResult, const Vector3& value, const Vector3& min, const Vector3& max);

    //! @brief Calculates the cross product of two vectors.
    static Vector3 Cross(const Vector3& vector1, const Vector3& vector2);

    //! @brief Calculates the cross product of two vectors.
    static void Cross(Vector3& rResult, const Vector3& vector1, const Vector3& vector2);

    //! @brief Calculates the distance between two vectors
    static float Distance(const Vector3& vector1, const Vector3& vector2);

    //! @brief Calculates the distance between two vectors squared.
    static float DistanceSquared(const Vector3& vector1, const Vector3& vector2)
    {
      return (vector1.X - vector2.X) * (vector1.X - vector2.X) +
        (vector1.Y - vector2.Y) * (vector1.Y - vector2.Y) +
        (vector1.Z - vector2.Z) * (vector1.Z - vector2.Z);
    }

    //! @brief Calculates the dot product of two vectors. If the two vectors are unit vectors,
    //         the dot product returns a floating point value between -1 and 1 that can be used
    //         to determine some properties of the angle between two vectors. For example,
    //         it can show whether the vectors are orthogonal, parallel, or have an acute or
    //         obtuse angle between them.
    static float Dot(const Vector3& vector1, const Vector3& vector2)
    {
      return vector1.X * vector2.X + vector1.Y * vector2.Y + vector1.Z * vector2.Z;
    }

    //! @brief Creates a new Vector3 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @return The hermite spline interpolation vector.
    static Vector3 Hermite(const Vector3& value1, const Vector3& tangent1, const Vector3& value2, const Vector3& tangent2, const float amount);

    //! @brief Creates a new Vector3 that contains hermite spline interpolation.
    //! @param value1 The first position vector.
    //! @param tangent1 The first tangent vector.
    //! @param value2 The second position vector.
    //! @param tangent2 The second tangent vector.
    //! @param amount Weighting factor.
    //! @param result The hermite spline interpolation vector as an output parameter.
    static void Hermite(const Vector3& value1, const Vector3& tangent1, const Vector3& value2, const Vector3& tangent2, const float amount, Vector3& rResult);

    //! @brief Calculates the length of the vector.
    float Length() const;

    //! @brief Calculates the length of the vector squared.
    float LengthSquared() const
    {
      return (X*X) + (Y*Y) + (Z*Z);
    }

    //! @brief Performs a linear interpolation between two vectors.
    static Vector3 Lerp(const Vector3& value1, const Vector3 value2, const float amount);

    //! @brief Performs a linear interpolation between two vectors.
    static void Lerp(Vector3& rResult, const Vector3& value1, const Vector3 value2, const float amount);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static Vector3 Max(const Vector3& value1, const Vector3& value2);

    //! @brief Returns a vector that contains the highest value from each matching pair of components.
    static void Max(Vector3& rResult, const Vector3& value1, const Vector3& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static Vector3 Min(const Vector3& value1, const Vector3& value2);

    //! @brief Returns a vector that contains the lowest value from each matching pair of components.
    static void Min(Vector3& rResult, const Vector3& value1, const Vector3& value2);

    //! @brief Returns a vector pointing in the opposite direction.
    static Vector3 Negate(const Vector3& value)
    {
      return Vector3(-value.X, -value.Y, -value.Z);
    }

    //! @brief Returns a vector pointing in the opposite direction.
    static void Negate(Vector3& rResult, const Vector3& value);

    //! @brief Returns a vector pointing in the opposite direction.
    Vector3 operator-() const
    {
      return Vector3(-X, -Y, -Z);
    }

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    void Normalize();

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static Vector3 Normalize(const Vector3& vector);

    //! @brief Creates a unit vector from the specified vector. The result is a vector one unit in length
    //         pointing in the same direction as the original vector.
    static void Normalize(Vector3& rResult, const Vector3& value);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static Vector3 Reflect(const Vector3& vector, const Vector3& normal);

    //! @brief Returns the reflection of a vector off a surface that has the specified normal.
    static void Reflect(Vector3& rResult, const Vector3& vector, const Vector3& normal);

    //! @brief Creates a new Vector3 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source.
    //! @param value2 Source.
    //! @param amount Weighting value.
    //! @return Cubic interpolation of the specified vectors.
    static Vector3 SmoothStep(const Vector3& value1, const Vector3& value2, const float amount);

    //! @brief Creates a new Vector3 that contains cubic interpolation of the specified vectors.
    //! @param value1 Source
    //! @param value2 Source
    //! @param amount Weighting value.
    //! @param result Cubic interpolation of the specified vectors as an output parameter.
    static void SmoothStep(const Vector3& value1, const Vector3& value2, const float amount, Vector3& rResult);

    //! @brief Transforms a vector by the specified Matrix
    static Vector3 Transform(const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a vector by the specified Matrix
    static void Transform(Vector3& rResult, const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a vector by a quaternion rotation.
    //! @param vec The vector to transform.
    //! @param quat The quaternion to rotate the vector by.
    //! @return The result of the operation.
    static Vector3 Transform(const Vector3& vec, const Quaternion& quat);

    //! @brief Transforms a vector by a quaternion rotation.
    //! @param value The vector to transform.
    //! @param rotation The quaternion to rotate the vector by.
    //! @param result The result of the operation.
    static void Transform(const Vector3& value, const Quaternion& rotation, Vector3& rResult);

    //! @brief Transforms a normal vector by the specified Matrix
    static Vector3 TransformNormal(const Vector3& position, const Matrix& matrix);

    //! @brief Transforms a normal vector by the specified Matrix
    static void TransformNormal(Vector3& rResult, const Vector3& position, const Matrix& matrix);

    //! @brief Add vector to vector
    Vector3& operator+=(const Vector3& arg)
    {
      X += arg.X;
      Y += arg.Y;
      Z += arg.Z;
      return *this;
    }

    //! @brief Subtracts vector from vector
    Vector3& operator-=(const Vector3& arg)
    {
      X -= arg.X;
      Y -= arg.Y;
      Z -= arg.Z;
      return *this;
    }

    //! @brief Multiply vector by vector
    Vector3& operator*=(const Vector3& arg)
    {
      X *= arg.X;
      Y *= arg.Y;
      Z *= arg.Z;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector3& operator*=(const int arg)
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      return *this;
    }

    //! @brief Multiply vector by scalar
    Vector3& operator*=(const float arg)
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector3& operator/=(const int arg)
    {
      X /= arg;
      Y /= arg;
      Z /= arg;
      return *this;
    }

    //! @brief Divide vector by scalar
    Vector3& operator/=(const float arg)
    {
      X /= arg;
      Y /= arg;
      Z /= arg;
      return *this;
    }

    //! @brief Tests for equality.
    bool operator==(const Vector3 &rhs) const
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
    }

    //! @brief Tests for inequality.
    bool operator!=(const Vector3 &rhs) const
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z;
    }

  };
}



//! @brief Add a vector to a vector.
inline Fsl::Vector3 operator+(const Fsl::Vector3& lhs, const Fsl::Vector3& rhs)
{
  return Fsl::Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
}

//! @brief Subtracts a vector from a vector.
inline Fsl::Vector3 operator-(const Fsl::Vector3& lhs, const Fsl::Vector3& rhs)
{
  return Fsl::Vector3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
}

//! @brief multiply a vector with a vector
inline Fsl::Vector3 operator*(const Fsl::Vector3& lhs, const Fsl::Vector3& rhs)
{
  return Fsl::Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector3 operator*(const Fsl::Vector3& lhs, const int rhs)
{
  return Fsl::Vector3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
}

//! @brief multiply a vector with a scalar
inline Fsl::Vector3 operator*(const Fsl::Vector3& lhs, const float rhs)
{
  return Fsl::Vector3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector3 operator*(const int lhs, const Fsl::Vector3& rhs)
{
  return rhs * lhs;
}

//! @brief multiply a scalar with a vector
inline Fsl::Vector3 operator*(const float lhs, const Fsl::Vector3& rhs)
{
  return rhs * lhs;
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector3 operator/(const Fsl::Vector3& lhs, const int rhs)
{
  return Fsl::Vector3(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs);
}

//! @brief Divide a vector by a scalar
inline Fsl::Vector3 operator/(const Fsl::Vector3& lhs, const float rhs)
{
  return Fsl::Vector3(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs);
}

#endif
