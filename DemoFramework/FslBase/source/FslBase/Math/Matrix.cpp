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

// The functions in this file are a port of an MIT licensed library: MonoGame - Matrix.cs.

#include <FslBase/Math/Matrix.hpp>
#include <cassert>
#include <cmath>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include "MatrixInternals.hpp"

// Workaround a issue with qnx signbit
using namespace std;

namespace Fsl
{
  using namespace MatrixFields;

  static_assert(sizeof(Matrix) == (sizeof(float) * 4 * 4), "Matrix not of expected size");

  Matrix Matrix::Add(const Matrix& matrix1, const Matrix& matrix2)
  {
    return Matrix(
      matrix1.m[_M11] + matrix2.m[_M11], matrix1.m[_M12] + matrix2.m[_M12], matrix1.m[_M13] + matrix2.m[_M13], matrix1.m[_M14] + matrix2.m[_M14],
      matrix1.m[_M21] + matrix2.m[_M21], matrix1.m[_M22] + matrix2.m[_M22], matrix1.m[_M23] + matrix2.m[_M23], matrix1.m[_M24] + matrix2.m[_M24],
      matrix1.m[_M31] + matrix2.m[_M31], matrix1.m[_M32] + matrix2.m[_M32], matrix1.m[_M33] + matrix2.m[_M33], matrix1.m[_M34] + matrix2.m[_M34],
      matrix1.m[_M41] + matrix2.m[_M41], matrix1.m[_M42] + matrix2.m[_M42], matrix1.m[_M43] + matrix2.m[_M43], matrix1.m[_M44] + matrix2.m[_M44]);
  }


  void Matrix::Add(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult)
  {
    rResult.m[_M11] = matrix1.m[_M11] + matrix2.m[_M11];
    rResult.m[_M12] = matrix1.m[_M12] + matrix2.m[_M12];
    rResult.m[_M13] = matrix1.m[_M13] + matrix2.m[_M13];
    rResult.m[_M14] = matrix1.m[_M14] + matrix2.m[_M14];
    rResult.m[_M21] = matrix1.m[_M21] + matrix2.m[_M21];
    rResult.m[_M22] = matrix1.m[_M22] + matrix2.m[_M22];
    rResult.m[_M23] = matrix1.m[_M23] + matrix2.m[_M23];
    rResult.m[_M24] = matrix1.m[_M24] + matrix2.m[_M24];
    rResult.m[_M31] = matrix1.m[_M31] + matrix2.m[_M31];
    rResult.m[_M32] = matrix1.m[_M32] + matrix2.m[_M32];
    rResult.m[_M33] = matrix1.m[_M33] + matrix2.m[_M33];
    rResult.m[_M34] = matrix1.m[_M34] + matrix2.m[_M34];
    rResult.m[_M41] = matrix1.m[_M41] + matrix2.m[_M41];
    rResult.m[_M42] = matrix1.m[_M42] + matrix2.m[_M42];
    rResult.m[_M43] = matrix1.m[_M43] + matrix2.m[_M43];
    rResult.m[_M44] = matrix1.m[_M44] + matrix2.m[_M44];
  }


  Vector3 Matrix::GetBackward() const
  {
    return Vector3(m[_M31], m[_M32], m[_M33]);
  }


  void Matrix::SetBackward(const Vector3& value)
  {
    m[_M31] = value.X;
    m[_M32] = value.Y;
    m[_M33] = value.Z;
  }


  Vector3 Matrix::GetDown() const
  {
    return Vector3(-m[_M21], -m[_M22], -m[_M23]);
  }


  void Matrix::SetDown(const Vector3& value)
  {
    m[_M21] = -value.X;
    m[_M22] = -value.Y;
    m[_M23] = -value.Z;
  }


  Vector3 Matrix::GetForward() const
  {
    return Vector3(-m[_M31], -m[_M32], -m[_M33]);
  }


  void Matrix::SetForward(const Vector3& value)
  {
    m[_M31] = -value.X;
    m[_M32] = -value.Y;
    m[_M33] = -value.Z;
  }


  Vector3 Matrix::GetLeft() const
  {
    return Vector3(-m[_M11], -m[_M12], -m[_M13]);
  }


  void Matrix::SetLeft(const Vector3& value)
  {
    m[_M11] = -value.X;
    m[_M12] = -value.Y;
    m[_M13] = -value.Z;
  }


  Vector3 Matrix::GetRight() const
  {
    return Vector3(m[_M11], m[_M12], m[_M13]);
  }


  void Matrix::SetRight(const Vector3& value)
  {
    m[_M11] = value.X;
    m[_M12] = value.Y;
    m[_M13] = value.Z;
  }


  Vector3 Matrix::GetTranslation() const
  {
    return Vector3(m[_M41], m[_M42], m[_M43]);
  }


  void Matrix::SetTranslation(const Vector3& value)
  {
    m[_M41] = value.X;
    m[_M42] = value.Y;
    m[_M43] = value.Z;
  }


  Vector3 Matrix::GetUp() const
  {
    return Vector3(m[_M21], m[_M22], m[_M23]);
  }


  void Matrix::SetUp(const Vector3& value)
  {
    m[_M21] = value.X;
    m[_M22] = value.Y;
    m[_M23] = value.Z;
  }


  void Matrix::Clear()
  {
    m[_M11] = 0.0f;
    m[_M12] = 0.0f;
    m[_M13] = 0.0f;
    m[_M14] = 0.0f;
    m[_M21] = 0.0f;
    m[_M22] = 0.0f;
    m[_M23] = 0.0f;
    m[_M24] = 0.0f;
    m[_M31] = 0.0f;
    m[_M32] = 0.0f;
    m[_M33] = 0.0f;
    m[_M34] = 0.0f;
    m[_M41] = 0.0f;
    m[_M42] = 0.0f;
    m[_M43] = 0.0f;
    m[_M44] = 0.0f;
  }


  Matrix Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector,
                                 const Vector3* pCameraForwardVector)
  {
    Matrix result;

    // Delegate to the other overload of the function to do the work
    CreateBillboard(objectPosition, cameraPosition, cameraUpVector, pCameraForwardVector, result);
    return result;
  }


  void Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector,
                               const Vector3* pCameraForwardVector, Matrix& rResult)
  {
    Vector3 vector;
    Vector3 vector2;
    Vector3 vector3;
    vector.X = objectPosition.X - cameraPosition.X;
    vector.Y = objectPosition.Y - cameraPosition.Y;
    vector.Z = objectPosition.Z - cameraPosition.Z;
    float num = vector.LengthSquared();
    if (num < 0.0001f)
    {
      vector = pCameraForwardVector != nullptr ? -(*pCameraForwardVector) : Vector3::Forward();
    }
    else
    {
      vector = vector * (1.0f / (std::sqrt(num)));
    }

    vector3 = Vector3::Cross(cameraUpVector, vector);
    vector3.Normalize();
    vector2 = Vector3::Cross(vector, vector3);
    rResult.m[_M11] = vector3.X;
    rResult.m[_M12] = vector3.Y;
    rResult.m[_M13] = vector3.Z;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = vector2.X;
    rResult.m[_M22] = vector2.Y;
    rResult.m[_M23] = vector2.Z;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = vector.X;
    rResult.m[_M32] = vector.Y;
    rResult.m[_M33] = vector.Z;
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = objectPosition.X;
    rResult.m[_M42] = objectPosition.Y;
    rResult.m[_M43] = objectPosition.Z;
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotateAxis,
                                            const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector)
  {
    Matrix result;
    CreateConstrainedBillboard(objectPosition, cameraPosition, rotateAxis, pCameraForwardVector, pObjectForwardVector, result);
    return result;
  }


  void Matrix::CreateConstrainedBillboard(const Vector3 objectPosition, const Vector3 cameraPosition, const Vector3 rotateAxis,
                                          const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector, Matrix& rResult)
  {
    Vector3 vector;
    Vector3 vector3;

    Vector3 distance;
    distance.X = objectPosition.X - cameraPosition.X;
    distance.Y = objectPosition.Y - cameraPosition.Y;
    distance.Z = objectPosition.Z - cameraPosition.Z;

    const float lengthSquared = distance.LengthSquared();
    if (lengthSquared < 0.0001f)
    {
      distance = pCameraForwardVector != nullptr ? -(*pCameraForwardVector) : Vector3::Forward();
    }
    else
    {
      distance = distance * (1.0f / (std::sqrt(lengthSquared)));
    }

    float num = Vector3::Dot(rotateAxis, distance);
    if (std::abs(num) > 0.9982547f)
    {
      if (pObjectForwardVector != nullptr)
      {
        vector = *pObjectForwardVector;
        num = Vector3::Dot(rotateAxis, vector);
        if (std::abs(num) > 0.9982547f)
        {
          num = ((rotateAxis.X * Vector3::Forward().X) + (rotateAxis.Y * Vector3::Forward().Y)) + (rotateAxis.Z * Vector3::Forward().Z);
          vector = (std::abs(num) > 0.9982547f) ? Vector3::Right() : Vector3::Forward();
        }
      }
      else
      {
        num = ((rotateAxis.X * Vector3::Forward().X) + (rotateAxis.Y * Vector3::Forward().Y)) + (rotateAxis.Z * Vector3::Forward().Z);
        vector = (std::abs(num) > 0.9982547f) ? Vector3::Right() : Vector3::Forward();
      }

      vector3 = Vector3::Cross(rotateAxis, vector);
      vector3.Normalize();
      vector = Vector3::Cross(vector3, rotateAxis);
      vector.Normalize();
    }
    else
    {
      vector3 = Vector3::Cross(rotateAxis, distance);
      vector3.Normalize();
      vector = Vector3::Cross(vector3, rotateAxis);
      vector.Normalize();
    }
    rResult.m[_M11] = vector3.X;
    rResult.m[_M12] = vector3.Y;
    rResult.m[_M13] = vector3.Z;
    rResult.m[_M14] = 0;
    rResult.m[_M21] = rotateAxis.X;
    rResult.m[_M22] = rotateAxis.Y;
    rResult.m[_M23] = rotateAxis.Z;
    rResult.m[_M24] = 0;
    rResult.m[_M31] = vector.X;
    rResult.m[_M32] = vector.Y;
    rResult.m[_M33] = vector.Z;
    rResult.m[_M34] = 0;
    rResult.m[_M41] = objectPosition.X;
    rResult.m[_M42] = objectPosition.Y;
    rResult.m[_M43] = objectPosition.Z;
    rResult.m[_M44] = 1;
  }


  Matrix Matrix::CreateFromAxisAngle(const Vector3& axis, const float angle)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateFromAxisAngle(axis, angle, result);
    return result;
  }


  void Matrix::CreateFromAxisAngle(const Vector3& axis, const float angle, Matrix& rResult)
  {
    const float x = axis.X;
    const float y = axis.Y;
    const float z = axis.Z;
    const float num2 = std::sin(angle);
    const float num = std::cos(angle);
    const float num11 = x * x;
    const float num10 = y * y;
    const float num9 = z * z;
    const float num8 = x * y;
    const float num7 = x * z;
    const float num6 = y * z;
    rResult.m[_M11] = num11 + (num * (1.0f - num11));
    rResult.m[_M12] = (num8 - (num * num8)) + (num2 * z);
    rResult.m[_M13] = (num7 - (num * num7)) - (num2 * y);
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = (num8 - (num * num8)) - (num2 * z);
    rResult.m[_M22] = num10 + (num * (1.0f - num10));
    rResult.m[_M23] = (num6 - (num * num6)) + (num2 * x);
    rResult.m[_M24] = 0.0;
    rResult.m[_M31] = (num7 - (num * num7)) + (num2 * y);
    rResult.m[_M32] = (num6 - (num * num6)) - (num2 * x);
    rResult.m[_M33] = num9 + (num * (1.0f - num9));
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = 0.0f;
    rResult.m[_M42] = 0.0f;
    rResult.m[_M43] = 0.0f;
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateFromYawPitchRoll(yaw, pitch, roll, result);
    return result;
  }


  Matrix Matrix::CreateFromQuaternion(const Quaternion& quaternion)
  {
    Matrix result;
    // Matrix result(OptimizationFlag::NoInitialization);
    CreateFromQuaternion(quaternion, result);
    return result;
  }


  void Matrix::CreateFromQuaternion(const Quaternion& quaternion, Matrix& rResult)
  {
    const float num9 = quaternion.X * quaternion.X;
    const float num8 = quaternion.Y * quaternion.Y;
    const float num7 = quaternion.Z * quaternion.Z;
    const float num6 = quaternion.X * quaternion.Y;
    const float num5 = quaternion.Z * quaternion.W;
    const float num4 = quaternion.Z * quaternion.X;
    const float num3 = quaternion.Y * quaternion.W;
    const float num2 = quaternion.Y * quaternion.Z;
    const float num = quaternion.X * quaternion.W;
    rResult.m[_M11] = 1.0f - (2.0f * (num8 + num7));
    rResult.m[_M12] = 2.0f * (num6 + num5);
    rResult.m[_M13] = 2.0f * (num4 - num3);
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 2.0f * (num6 - num5);
    rResult.m[_M22] = 1.0f - (2.0f * (num7 + num9));
    rResult.m[_M23] = 2.0f * (num2 + num);
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 2.0f * (num4 + num3);
    rResult.m[_M32] = 2.0f * (num2 - num);
    rResult.m[_M33] = 1.0f - (2.0f * (num8 + num9));
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = 0.0f;
    rResult.m[_M42] = 0.0f;
    rResult.m[_M43] = 0.0f;
    rResult.m[_M44] = 1.0f;
  }


  void Matrix::CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll, Matrix& rResult)
  {
    // Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion quaternion;
    Quaternion::CreateFromYawPitchRoll(quaternion, yaw, pitch, roll);
    CreateFromQuaternion(quaternion, rResult);
  }


  Matrix Matrix::CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateLookAt(cameraPosition, cameraTarget, cameraUpVector, result);
    return result;
  }


  void Matrix::CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector, Matrix& rResult)
  {
    const Vector3 f = Vector3::Normalize(cameraPosition - cameraTarget);
    const Vector3 s = Vector3::Normalize(Vector3::Cross(cameraUpVector, f));
    const Vector3 u = Vector3::Cross(f, s);
    rResult.m[_M11] = s.X;
    rResult.m[_M12] = u.X;
    rResult.m[_M13] = f.X;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = s.Y;
    rResult.m[_M22] = u.Y;
    rResult.m[_M23] = f.Y;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = s.Z;
    rResult.m[_M32] = u.Z;
    rResult.m[_M33] = f.Z;
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = -Vector3::Dot(s, cameraPosition);
    rResult.m[_M42] = -Vector3::Dot(u, cameraPosition);
    rResult.m[_M43] = -Vector3::Dot(f, cameraPosition);
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreateOrthographic(const float width, const float height, const float zNearPlane, const float zFarPlane)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateOrthographic(width, height, zNearPlane, zFarPlane, result);
    return result;
  }


  void Matrix::CreateOrthographic(const float width, const float height, const float zNearPlane, const float zFarPlane, Matrix& rResult)
  {
    if (width <= 0.0f)
    {
      throw std::invalid_argument("width <= 0.0f");
    }
    if (height <= 0.0f)
    {
      throw std::invalid_argument("height <= 0.0f");
    }
    if (zNearPlane <= 0.0f)
    {
      throw std::invalid_argument("zNearPlane <= 0.0f");
    }
    if (zNearPlane >= zFarPlane)
    {
      throw std::invalid_argument("zNearPlane >= zFarPlane");
    }

    rResult.m[_M11] = 2.0f / width;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = 2.0f / height;
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = 1.0f / (zNearPlane - zFarPlane);
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = 0.0f;
    rResult.m[_M42] = 0.0f;
    rResult.m[_M43] = zNearPlane / (zNearPlane - zFarPlane);
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane,
                                             const float zFarPlane)
  {
    Matrix matrix;
    CreateOrthographicOffCenter(left, right, bottom, top, zNearPlane, zFarPlane, matrix);
    return matrix;
  }


  Matrix Matrix::CreateOrthographicOffCenter(const Rectangle& viewingVolume, const float zNearPlane, const float zFarPlane)
  {
    Matrix matrix;
    CreateOrthographicOffCenter(static_cast<float>(viewingVolume.Left()), static_cast<float>(viewingVolume.Right()),
                                static_cast<float>(viewingVolume.Bottom()), static_cast<float>(viewingVolume.Top()), zNearPlane, zFarPlane, matrix);
    return matrix;
  }


  void Matrix::CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane,
                                           const float zFarPlane, Matrix& rResult)
  {
    rResult.m[_M11] = static_cast<float>(2.0 / (static_cast<double>(right) - static_cast<double>(left)));
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = static_cast<float>(2.0 / (static_cast<double>(top) - static_cast<double>(bottom)));
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = static_cast<float>(1.0 / (static_cast<double>(zNearPlane) - static_cast<double>(zFarPlane)));
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] =
      static_cast<float>((static_cast<double>(left) + static_cast<double>(right)) / (static_cast<double>(left) - static_cast<double>(right)));
    rResult.m[_M42] =
      static_cast<float>((static_cast<double>(top) + static_cast<double>(bottom)) / (static_cast<double>(bottom) - static_cast<double>(top)));
    rResult.m[_M43] = static_cast<float>(static_cast<double>(zNearPlane) / (static_cast<double>(zNearPlane) - static_cast<double>(zFarPlane)));
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreatePerspective(width, height, nearPlaneDistance, farPlaneDistance, result);
    return result;
  }


  void Matrix::CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance, Matrix& rResult)
  {
    if (width <= 0.0f)
    {
      throw std::invalid_argument("width <= 0.0f");
    }
    if (height <= 0.0f)
    {
      throw std::invalid_argument("height <= 0.0f");
    }
    if (nearPlaneDistance <= 0.0f)
    {
      throw std::invalid_argument("nearPlaneDistance <= 0.0f");
    }
    if (nearPlaneDistance >= farPlaneDistance)
    {
      throw std::invalid_argument("nearPlaneDistance >= farPlaneDistance");
    }

    rResult.m[_M11] = (2.0f * nearPlaneDistance) / width;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = (2.0f * nearPlaneDistance) / height;
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M34] = -1.0f;
    rResult.m[_M41] = 0.0f;
    rResult.m[_M42] = 0.0f;
    rResult.m[_M43] = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M44] = 0.0f;
  }


  Matrix Matrix::CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance,
                                              const float farPlaneDistance)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance, result);
    return result;
  }

  // This produces a similar matrix to GLM::perspectiveRH with GLM_DEPTH_ZERO_TO_ONE
  void Matrix::CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance,
                                            const float farPlaneDistance, Matrix& rResult)
  {
    if ((fieldOfView <= 0.0f) || (fieldOfView >= 3.141593f))
    {
      throw std::invalid_argument("fieldOfView <= 0 or >= PI");
    }
    if (aspectRatio == 0.0f)
    {
      throw std::invalid_argument("aspectRatio can not be zero");
    }
    if (nearPlaneDistance <= 0.0f)
    {
      throw std::invalid_argument("nearPlaneDistance <= 0");
    }
    if (farPlaneDistance <= 0.0f)
    {
      throw std::invalid_argument("farPlaneDistance <= 0");
    }
    if (nearPlaneDistance >= farPlaneDistance)
    {
      throw std::invalid_argument("nearPlaneDistance >= farPlaneDistance");
    }

    const float oneDivTanHalfFovY = 1.0f / (static_cast<float>(std::tan(fieldOfView * 0.5)));
    const float num9 = oneDivTanHalfFovY / aspectRatio;
    rResult.m[_M11] = num9;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = oneDivTanHalfFovY;
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M34] = -1.0;
    rResult.m[_M41] = 0.0f;
    rResult.m[_M42] = 0.0f;
    rResult.m[_M43] = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M44] = 0.0f;
  }


  Matrix Matrix::CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance,
                                            const float farPlaneDistance)
  {
    Matrix result;
    CreatePerspectiveOffCenter(left, right, bottom, top, nearPlaneDistance, farPlaneDistance, result);
    return result;
  }


  Matrix Matrix::CreatePerspectiveOffCenter(const Rectangle& viewingVolume, const float nearPlaneDistance, const float farPlaneDistance)
  {
    Matrix result;
    CreatePerspectiveOffCenter(static_cast<float>(viewingVolume.Left()), static_cast<float>(viewingVolume.Right()),
                               static_cast<float>(viewingVolume.Bottom()), static_cast<float>(viewingVolume.Top()), nearPlaneDistance,
                               farPlaneDistance, result);
    return result;
  }


  void Matrix::CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance,
                                          const float farPlaneDistance, Matrix& rResult)
  {
    if (nearPlaneDistance <= 0.0f)
    {
      throw std::invalid_argument("nearPlaneDistance <= 0");
    }
    if (farPlaneDistance <= 0.0f)
    {
      throw std::invalid_argument("farPlaneDistance <= 0");
    }
    if (nearPlaneDistance >= farPlaneDistance)
    {
      throw std::invalid_argument("nearPlaneDistance >= farPlaneDistance");
    }

    rResult.m[_M11] = (2.0f * nearPlaneDistance) / (right - left);
    rResult.m[_M12] = rResult.m[_M13] = rResult.m[_M14] = 0;
    rResult.m[_M22] = (2.0f * nearPlaneDistance) / (top - bottom);
    rResult.m[_M21] = rResult.m[_M23] = rResult.m[_M24] = 0;
    rResult.m[_M31] = (left + right) / (right - left);
    rResult.m[_M32] = (top + bottom) / (top - bottom);
    rResult.m[_M33] = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M34] = -1.0f;
    rResult.m[_M43] = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
    rResult.m[_M41] = rResult.m[_M42] = rResult.m[_M44] = 0;
  }


  Matrix Matrix::CreateReflection(const Plane& value)
  {
    Matrix result;
    CreateReflection(value, result);
    return result;
  }


  void Matrix::CreateReflection(const Plane& value, Matrix& rResult)
  {
    const Plane plane(Plane::Normalize(value));

    const float x = plane.Normal.X;
    const float y = plane.Normal.Y;
    const float z = plane.Normal.Z;
    const float num3 = -2.0f * x;
    const float num2 = -2.0f * y;
    const float num = -2.0f * z;
    rResult.m[_M11] = (num3 * x) + 1.0f;
    rResult.m[_M12] = num2 * x;
    rResult.m[_M13] = num * x;
    rResult.m[_M14] = 0;
    rResult.m[_M21] = num3 * y;
    rResult.m[_M22] = (num2 * y) + 1.0f;
    rResult.m[_M23] = num * y;
    rResult.m[_M24] = 0;
    rResult.m[_M31] = num3 * z;
    rResult.m[_M32] = num2 * z;
    rResult.m[_M33] = (num * z) + 1.0f;
    rResult.m[_M34] = 0;
    rResult.m[_M41] = num3 * plane.D;
    rResult.m[_M42] = num2 * plane.D;
    rResult.m[_M43] = num * plane.D;
    rResult.m[_M44] = 1;
  }


  Matrix Matrix::CreateRotationX(const float radians)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateRotationX(radians, result);
    return result;
  }


  void Matrix::CreateRotationX(const float radians, Matrix& rResult)
  {
    rResult.SetIdentity();

    const float val1 = std::cos(radians);
    const float val2 = std::sin(radians);

    rResult.m[_M22] = val1;
    rResult.m[_M23] = val2;
    rResult.m[_M32] = -val2;
    rResult.m[_M33] = val1;
  }


  Matrix Matrix::CreateRotationY(const float radians)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateRotationY(radians, result);
    return result;
  }


  void Matrix::CreateRotationY(const float radians, Matrix& rResult)
  {
    rResult.SetIdentity();

    const float val1 = std::cos(radians);
    const float val2 = std::sin(radians);

    rResult.m[_M11] = val1;
    rResult.m[_M13] = -val2;
    rResult.m[_M31] = val2;
    rResult.m[_M33] = val1;
  }


  Matrix Matrix::CreateRotationZ(const float radians)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateRotationZ(radians, result);
    return result;
  }


  void Matrix::CreateRotationZ(const float radians, Matrix& rResult)
  {
    rResult.SetIdentity();

    const float val1 = std::cos(radians);
    const float val2 = std::sin(radians);

    rResult.m[_M11] = val1;
    rResult.m[_M12] = val2;
    rResult.m[_M21] = -val2;
    rResult.m[_M22] = val1;
  }


  void Matrix::CreateScale(const float scale, Matrix& rResult)
  {
    CreateScale(scale, scale, scale, rResult);
  }


  void Matrix::CreateScale(const float scaleX, const float scaleY, const float scaleZ, Matrix& rResult)
  {
    rResult.m[_M11] = scaleX;
    rResult.m[_M12] = 0;
    rResult.m[_M13] = 0;
    rResult.m[_M14] = 0;
    rResult.m[_M21] = 0;
    rResult.m[_M22] = scaleY;
    rResult.m[_M23] = 0;
    rResult.m[_M24] = 0;
    rResult.m[_M31] = 0;
    rResult.m[_M32] = 0;
    rResult.m[_M33] = scaleZ;
    rResult.m[_M34] = 0;
    rResult.m[_M41] = 0;
    rResult.m[_M42] = 0;
    rResult.m[_M43] = 0;
    rResult.m[_M44] = 1;
  }


  void Matrix::CreateScale(const Vector3& scale, Matrix& rResult)
  {
    rResult.m[_M11] = scale.X;
    rResult.m[_M12] = 0;
    rResult.m[_M13] = 0;
    rResult.m[_M14] = 0;
    rResult.m[_M21] = 0;
    rResult.m[_M22] = scale.Y;
    rResult.m[_M23] = 0;
    rResult.m[_M24] = 0;
    rResult.m[_M31] = 0;
    rResult.m[_M32] = 0;
    rResult.m[_M33] = scale.Z;
    rResult.m[_M34] = 0;
    rResult.m[_M41] = 0;
    rResult.m[_M42] = 0;
    rResult.m[_M43] = 0;
    rResult.m[_M44] = 1;
  }


  Matrix Matrix::CreateShadow(const Vector3& lightDirection, const Plane& plane)
  {
    Matrix result;
    CreateShadow(lightDirection, plane, result);
    return result;
  }


  void Matrix::CreateShadow(const Vector3& lightDirection, const Plane& plane, Matrix& rResult)
  {
    const float dot = (plane.Normal.X * lightDirection.X) + (plane.Normal.Y * lightDirection.Y) + (plane.Normal.Z * lightDirection.Z);
    const float x = -plane.Normal.X;
    const float y = -plane.Normal.Y;
    const float z = -plane.Normal.Z;
    const float d = -plane.D;

    rResult.m[_M11] = (x * lightDirection.X) + dot;
    rResult.m[_M12] = x * lightDirection.Y;
    rResult.m[_M13] = x * lightDirection.Z;
    rResult.m[_M14] = 0;
    rResult.m[_M21] = y * lightDirection.X;
    rResult.m[_M22] = (y * lightDirection.Y) + dot;
    rResult.m[_M23] = y * lightDirection.Z;
    rResult.m[_M24] = 0;
    rResult.m[_M31] = z * lightDirection.X;
    rResult.m[_M32] = z * lightDirection.Y;
    rResult.m[_M33] = (z * lightDirection.Z) + dot;
    rResult.m[_M34] = 0;
    rResult.m[_M41] = d * lightDirection.X;
    rResult.m[_M42] = d * lightDirection.Y;
    rResult.m[_M43] = d * lightDirection.Z;
    rResult.m[_M44] = dot;
  }


  void Matrix::CreateTranslation(const float x, const float y, const float z, Matrix& rResult)
  {
    rResult.m[_M11] = 1.0f;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = 1.0f;
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = 1.0f;
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = x;
    rResult.m[_M42] = y;
    rResult.m[_M43] = z;
    rResult.m[_M44] = 1.0f;
  }


  void Matrix::CreateTranslation(const Vector3& value, Matrix& rResult)
  {
    rResult.m[_M11] = 1.0f;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = 1.0f;
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = 1.0f;
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = value.X;
    rResult.m[_M42] = value.Y;
    rResult.m[_M43] = value.Z;
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    CreateWorld(position, forward, up, result);
    return result;
  }


  void Matrix::CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up, Matrix& rResult)
  {
    Vector3 x, y, z;
    Vector3::Normalize(forward, z);
    Vector3::Cross(forward, up, x);
    // Ugly cast due to deprecated overloads of Cross
    Vector3::Cross(static_cast<const Vector3>(x), forward, y);
    x.Normalize();
    y.Normalize();

    rResult.Clear();
    rResult.SetRight(x);
    rResult.SetUp(y);
    rResult.SetForward(z);
    rResult.SetTranslation(position);
    rResult.m[_M44] = 1.0f;
  }


  bool Matrix::Decompose(Vector3& rScale, Quaternion& rRotation, Vector3& rTranslation)
  {
    rTranslation.X = m[_M41];
    rTranslation.Y = m[_M42];
    rTranslation.Z = m[_M43];

    const float xs = signbit(m[_M11] * m[_M12] * m[_M13] * m[_M14]) ? -1.0f : 1.0f;
    const float ys = signbit(m[_M21] * m[_M22] * m[_M23] * m[_M24]) ? -1.0f : 1.0f;
    const float zs = signbit(m[_M31] * m[_M32] * m[_M33] * m[_M34]) ? -1.0f : 1.0f;

    rScale.X = xs * std::sqrt(m[_M11] * m[_M11] + m[_M12] * m[_M12] + m[_M13] * m[_M13]);
    rScale.Y = ys * std::sqrt(m[_M21] * m[_M21] + m[_M22] * m[_M22] + m[_M23] * m[_M23]);
    rScale.Z = zs * std::sqrt(m[_M31] * m[_M31] + m[_M32] * m[_M32] + m[_M33] * m[_M33]);

    if (rScale.X == 0.0f || rScale.Y == 0.0f || rScale.Z == 0.0f)
    {
      rRotation = Quaternion::Identity();
      return false;
    }

    const Matrix m1(m[_M11] / rScale.X, m[_M12] / rScale.X, m[_M13] / rScale.X, 0, m[_M21] / rScale.Y, m[_M22] / rScale.Y, m[_M23] / rScale.Y, 0,
                    m[_M31] / rScale.Z, m[_M32] / rScale.Z, m[_M33] / rScale.Z, 0, 0.0f, 0.0f, 0.0f, 1.0f);
    rRotation = Quaternion::CreateFromRotationMatrix(m1);
    return true;
  }

  float Matrix::Determinant() const
  {
    const float num22 = m[_M11];
    const float num21 = m[_M12];
    const float num20 = m[_M13];
    const float num19 = m[_M14];
    const float num12 = m[_M21];
    const float num11 = m[_M22];
    const float num10 = m[_M23];
    const float num9 = m[_M24];
    const float num8 = m[_M31];
    const float num7 = m[_M32];
    const float num6 = m[_M33];
    const float num5 = m[_M34];
    const float num4 = m[_M41];
    const float num3 = m[_M42];
    const float num2 = m[_M43];
    const float num = m[_M44];
    const float num18 = (num6 * num) - (num5 * num2);
    const float num17 = (num7 * num) - (num5 * num3);
    const float num16 = (num7 * num2) - (num6 * num3);
    const float num15 = (num8 * num) - (num5 * num4);
    const float num14 = (num8 * num2) - (num6 * num4);
    const float num13 = (num8 * num3) - (num7 * num4);
    return ((((num22 * (((num11 * num18) - (num10 * num17)) + (num9 * num16))) - (num21 * (((num12 * num18) - (num10 * num15)) + (num9 * num14)))) +
             (num20 * (((num12 * num17) - (num11 * num15)) + (num9 * num13)))) -
            (num19 * (((num12 * num16) - (num11 * num14)) + (num10 * num13))));
  }


  Matrix Matrix::Invert(const Matrix& matrix)
  {
    // Matrix result(OptimizationFlag::NoInitialization);
    Matrix result;
    Invert(matrix, result);
    return result;
  }


  void Matrix::Invert(const Matrix& matrix, Matrix& rResult)
  {
    const float num1 = matrix.m[_M11];
    const float num2 = matrix.m[_M12];
    const float num3 = matrix.m[_M13];
    const float num4 = matrix.m[_M14];
    const float num5 = matrix.m[_M21];
    const float num6 = matrix.m[_M22];
    const float num7 = matrix.m[_M23];
    const float num8 = matrix.m[_M24];
    const float num9 = matrix.m[_M31];
    const float num10 = matrix.m[_M32];
    const float num11 = matrix.m[_M33];
    const float num12 = matrix.m[_M34];
    const float num13 = matrix.m[_M41];
    const float num14 = matrix.m[_M42];
    const float num15 = matrix.m[_M43];
    const float num16 = matrix.m[_M44];
    const auto num17 =
      static_cast<float>(static_cast<double>(num11) * static_cast<double>(num16) - static_cast<double>(num12) * static_cast<double>(num15));
    const auto num18 =
      static_cast<float>(static_cast<double>(num10) * static_cast<double>(num16) - static_cast<double>(num12) * static_cast<double>(num14));
    const auto num19 =
      static_cast<float>(static_cast<double>(num10) * static_cast<double>(num15) - static_cast<double>(num11) * static_cast<double>(num14));
    const auto num20 =
      static_cast<float>(static_cast<double>(num9) * static_cast<double>(num16) - static_cast<double>(num12) * static_cast<double>(num13));
    const auto num21 =
      static_cast<float>(static_cast<double>(num9) * static_cast<double>(num15) - static_cast<double>(num11) * static_cast<double>(num13));
    const auto num22 =
      static_cast<float>(static_cast<double>(num9) * static_cast<double>(num14) - static_cast<double>(num10) * static_cast<double>(num13));
    const auto num23 =
      static_cast<float>(static_cast<double>(num6) * static_cast<double>(num17) - static_cast<double>(num7) * static_cast<double>(num18) +
                         static_cast<double>(num8) * static_cast<double>(num19));
    const auto num24 =
      static_cast<float>(-(static_cast<double>(num5) * static_cast<double>(num17) - static_cast<double>(num7) * static_cast<double>(num20) +
                           static_cast<double>(num8) * static_cast<double>(num21)));
    const auto num25 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num18) - static_cast<double>(num6) * static_cast<double>(num20) +
                         static_cast<double>(num8) * static_cast<double>(num22));
    const auto num26 =
      static_cast<float>(-(static_cast<double>(num5) * static_cast<double>(num19) - static_cast<double>(num6) * static_cast<double>(num21) +
                           static_cast<double>(num7) * static_cast<double>(num22)));
    const auto num27 =
      static_cast<float>(1.0 / (static_cast<double>(num1) * static_cast<double>(num23) + static_cast<double>(num2) * static_cast<double>(num24) +
                                static_cast<double>(num3) * static_cast<double>(num25) + static_cast<double>(num4) * static_cast<double>(num26)));

    rResult.m[_M11] = num23 * num27;
    rResult.m[_M21] = num24 * num27;
    rResult.m[_M31] = num25 * num27;
    rResult.m[_M41] = num26 * num27;
    rResult.m[_M12] =
      static_cast<float>(-(static_cast<double>(num2) * static_cast<double>(num17) - static_cast<double>(num3) * static_cast<double>(num18) +
                           static_cast<double>(num4) * static_cast<double>(num19))) *
      num27;
    rResult.m[_M22] =
      static_cast<float>(static_cast<double>(num1) * static_cast<double>(num17) - static_cast<double>(num3) * static_cast<double>(num20) +
                         static_cast<double>(num4) * static_cast<double>(num21)) *
      num27;
    rResult.m[_M32] =
      static_cast<float>(-(static_cast<double>(num1) * static_cast<double>(num18) - static_cast<double>(num2) * static_cast<double>(num20) +
                           static_cast<double>(num4) * static_cast<double>(num22))) *
      num27;
    rResult.m[_M42] =
      static_cast<float>(static_cast<double>(num1) * static_cast<double>(num19) - static_cast<double>(num2) * static_cast<double>(num21) +
                         static_cast<double>(num3) * static_cast<double>(num22)) *
      num27;
    const auto num28 =
      static_cast<float>(static_cast<double>(num7) * static_cast<double>(num16) - static_cast<double>(num8) * static_cast<double>(num15));
    const auto num29 =
      static_cast<float>(static_cast<double>(num6) * static_cast<double>(num16) - static_cast<double>(num8) * static_cast<double>(num14));
    const auto num30 =
      static_cast<float>(static_cast<double>(num6) * static_cast<double>(num15) - static_cast<double>(num7) * static_cast<double>(num14));
    const auto num31 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num16) - static_cast<double>(num8) * static_cast<double>(num13));
    const auto num32 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num15) - static_cast<double>(num7) * static_cast<double>(num13));
    const auto num33 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num14) - static_cast<double>(num6) * static_cast<double>(num13));
    rResult.m[_M13] =
      static_cast<float>(static_cast<double>(num2) * static_cast<double>(num28) - static_cast<double>(num3) * static_cast<double>(num29) +
                         static_cast<double>(num4) * static_cast<double>(num30)) *
      num27;
    rResult.m[_M23] =
      static_cast<float>(-(static_cast<double>(num1) * static_cast<double>(num28) - static_cast<double>(num3) * static_cast<double>(num31) +
                           static_cast<double>(num4) * static_cast<double>(num32))) *
      num27;
    rResult.m[_M33] =
      static_cast<float>(static_cast<double>(num1) * static_cast<double>(num29) - static_cast<double>(num2) * static_cast<double>(num31) +
                         static_cast<double>(num4) * static_cast<double>(num33)) *
      num27;
    rResult.m[_M43] =
      static_cast<float>(-(static_cast<double>(num1) * static_cast<double>(num30) - static_cast<double>(num2) * static_cast<double>(num32) +
                           static_cast<double>(num3) * static_cast<double>(num33))) *
      num27;
    const auto num34 =
      static_cast<float>(static_cast<double>(num7) * static_cast<double>(num12) - static_cast<double>(num8) * static_cast<double>(num11));
    const auto num35 =
      static_cast<float>(static_cast<double>(num6) * static_cast<double>(num12) - static_cast<double>(num8) * static_cast<double>(num10));
    const auto num36 =
      static_cast<float>(static_cast<double>(num6) * static_cast<double>(num11) - static_cast<double>(num7) * static_cast<double>(num10));
    const auto num37 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num12) - static_cast<double>(num8) * static_cast<double>(num9));
    const auto num38 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num11) - static_cast<double>(num7) * static_cast<double>(num9));
    const auto num39 =
      static_cast<float>(static_cast<double>(num5) * static_cast<double>(num10) - static_cast<double>(num6) * static_cast<double>(num9));
    rResult.m[_M14] =
      static_cast<float>(-(static_cast<double>(num2) * static_cast<double>(num34) - static_cast<double>(num3) * static_cast<double>(num35) +
                           static_cast<double>(num4) * static_cast<double>(num36))) *
      num27;
    rResult.m[_M24] =
      static_cast<float>(static_cast<double>(num1) * static_cast<double>(num34) - static_cast<double>(num3) * static_cast<double>(num37) +
                         static_cast<double>(num4) * static_cast<double>(num38)) *
      num27;
    rResult.m[_M34] =
      static_cast<float>(-(static_cast<double>(num1) * static_cast<double>(num35) - static_cast<double>(num2) * static_cast<double>(num37) +
                           static_cast<double>(num4) * static_cast<double>(num39))) *
      num27;
    rResult.m[_M44] =
      static_cast<float>(static_cast<double>(num1) * static_cast<double>(num36) - static_cast<double>(num2) * static_cast<double>(num38) +
                         static_cast<double>(num3) * static_cast<double>(num39)) *
      num27;
  }


  Matrix Matrix::Lerp(const Matrix& value1, const Matrix& value2, const float amount)
  {
    const float* pMatrix1 = value1.m;
    const float* pMatrix2 = value2.m;
    return Matrix(pMatrix1[_M11] + ((pMatrix2[_M11] - pMatrix1[_M11]) * amount), pMatrix1[_M12] + ((pMatrix2[_M12] - pMatrix1[_M12]) * amount),
                  pMatrix1[_M13] + ((pMatrix2[_M13] - pMatrix1[_M13]) * amount), pMatrix1[_M14] + ((pMatrix2[_M14] - pMatrix1[_M14]) * amount),
                  pMatrix1[_M21] + ((pMatrix2[_M21] - pMatrix1[_M21]) * amount), pMatrix1[_M22] + ((pMatrix2[_M22] - pMatrix1[_M22]) * amount),
                  pMatrix1[_M23] + ((pMatrix2[_M23] - pMatrix1[_M23]) * amount), pMatrix1[_M24] + ((pMatrix2[_M24] - pMatrix1[_M24]) * amount),
                  pMatrix1[_M31] + ((pMatrix2[_M31] - pMatrix1[_M31]) * amount), pMatrix1[_M32] + ((pMatrix2[_M32] - pMatrix1[_M32]) * amount),
                  pMatrix1[_M33] + ((pMatrix2[_M33] - pMatrix1[_M33]) * amount), pMatrix1[_M34] + ((pMatrix2[_M34] - pMatrix1[_M34]) * amount),
                  pMatrix1[_M41] + ((pMatrix2[_M41] - pMatrix1[_M41]) * amount), pMatrix1[_M42] + ((pMatrix2[_M42] - pMatrix1[_M42]) * amount),
                  pMatrix1[_M43] + ((pMatrix2[_M43] - pMatrix1[_M43]) * amount), pMatrix1[_M44] + ((pMatrix2[_M44] - pMatrix1[_M44]) * amount));
  }


  void Matrix::Lerp(const Matrix& value1, const Matrix& value2, const float amount, Matrix& rResult)
  {
    const float* pMatrix1 = value1.m;
    const float* pMatrix2 = value2.m;
    float* pResult = rResult.m;

    // It's not recommended to write the result to the matrixes that are being used to interpolate between
    assert(pMatrix1 != pResult);
    assert(pMatrix2 != pResult);

    pResult[_M11] = pMatrix1[_M11] + ((pMatrix2[_M11] - pMatrix1[_M11]) * amount);
    pResult[_M12] = pMatrix1[_M12] + ((pMatrix2[_M12] - pMatrix1[_M12]) * amount);
    pResult[_M13] = pMatrix1[_M13] + ((pMatrix2[_M13] - pMatrix1[_M13]) * amount);
    pResult[_M14] = pMatrix1[_M14] + ((pMatrix2[_M14] - pMatrix1[_M14]) * amount);
    pResult[_M21] = pMatrix1[_M21] + ((pMatrix2[_M21] - pMatrix1[_M21]) * amount);
    pResult[_M22] = pMatrix1[_M22] + ((pMatrix2[_M22] - pMatrix1[_M22]) * amount);
    pResult[_M23] = pMatrix1[_M23] + ((pMatrix2[_M23] - pMatrix1[_M23]) * amount);
    pResult[_M24] = pMatrix1[_M24] + ((pMatrix2[_M24] - pMatrix1[_M24]) * amount);
    pResult[_M31] = pMatrix1[_M31] + ((pMatrix2[_M31] - pMatrix1[_M31]) * amount);
    pResult[_M32] = pMatrix1[_M32] + ((pMatrix2[_M32] - pMatrix1[_M32]) * amount);
    pResult[_M33] = pMatrix1[_M33] + ((pMatrix2[_M33] - pMatrix1[_M33]) * amount);
    pResult[_M34] = pMatrix1[_M34] + ((pMatrix2[_M34] - pMatrix1[_M34]) * amount);
    pResult[_M41] = pMatrix1[_M41] + ((pMatrix2[_M41] - pMatrix1[_M41]) * amount);
    pResult[_M42] = pMatrix1[_M42] + ((pMatrix2[_M42] - pMatrix1[_M42]) * amount);
    pResult[_M43] = pMatrix1[_M43] + ((pMatrix2[_M43] - pMatrix1[_M43]) * amount);
    pResult[_M44] = pMatrix1[_M44] + ((pMatrix2[_M44] - pMatrix1[_M44]) * amount);
  }

  void Matrix::Negate(const Matrix& matrix, Matrix& rResult)
  {
    rResult.m[_M11] = -matrix.m[_M11];
    rResult.m[_M12] = -matrix.m[_M12];
    rResult.m[_M13] = -matrix.m[_M13];
    rResult.m[_M14] = -matrix.m[_M14];
    rResult.m[_M21] = -matrix.m[_M21];
    rResult.m[_M22] = -matrix.m[_M22];
    rResult.m[_M23] = -matrix.m[_M23];
    rResult.m[_M24] = -matrix.m[_M24];
    rResult.m[_M31] = -matrix.m[_M31];
    rResult.m[_M32] = -matrix.m[_M32];
    rResult.m[_M33] = -matrix.m[_M33];
    rResult.m[_M34] = -matrix.m[_M34];
    rResult.m[_M41] = -matrix.m[_M41];
    rResult.m[_M42] = -matrix.m[_M42];
    rResult.m[_M43] = -matrix.m[_M43];
    rResult.m[_M44] = -matrix.m[_M44];
  }


  void Matrix::SetIdentity()
  {
    m[_M11] = 1.0f;
    m[_M12] = 0.0f;
    m[_M13] = 0.0f;
    m[_M14] = 0.0f;
    m[_M21] = 0.0f;
    m[_M22] = 1.0f;
    m[_M23] = 0.0f;
    m[_M24] = 0.0f;
    m[_M31] = 0.0f;
    m[_M32] = 0.0f;
    m[_M33] = 1.0f;
    m[_M34] = 0.0f;
    m[_M41] = 0.0f;
    m[_M42] = 0.0f;
    m[_M43] = 0.0f;
    m[_M44] = 1.0f;
  }


  void Matrix::Subtract(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult)
  {
    rResult.m[_M11] = matrix1.m[_M11] - matrix2.m[_M11];
    rResult.m[_M12] = matrix1.m[_M12] - matrix2.m[_M12];
    rResult.m[_M13] = matrix1.m[_M13] - matrix2.m[_M13];
    rResult.m[_M14] = matrix1.m[_M14] - matrix2.m[_M14];
    rResult.m[_M21] = matrix1.m[_M21] - matrix2.m[_M21];
    rResult.m[_M22] = matrix1.m[_M22] - matrix2.m[_M22];
    rResult.m[_M23] = matrix1.m[_M23] - matrix2.m[_M23];
    rResult.m[_M24] = matrix1.m[_M24] - matrix2.m[_M24];
    rResult.m[_M31] = matrix1.m[_M31] - matrix2.m[_M31];
    rResult.m[_M32] = matrix1.m[_M32] - matrix2.m[_M32];
    rResult.m[_M33] = matrix1.m[_M33] - matrix2.m[_M33];
    rResult.m[_M34] = matrix1.m[_M34] - matrix2.m[_M34];
    rResult.m[_M41] = matrix1.m[_M41] - matrix2.m[_M41];
    rResult.m[_M42] = matrix1.m[_M42] - matrix2.m[_M42];
    rResult.m[_M43] = matrix1.m[_M43] - matrix2.m[_M43];
    rResult.m[_M44] = matrix1.m[_M44] - matrix2.m[_M44];
  }


  void Matrix::Transpose(const Matrix& src, Matrix& rResult)
  {
    rResult = Transpose(src);
  }


  void MatrixInternals::Transform(Vector2& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + pMatrix[_M41];
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + pMatrix[_M42];
    rResult.X = x;
    rResult.Y = y;
  }


  void MatrixInternals::TransformNormal(Vector2& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]);
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]);
    rResult.X = x;
    rResult.Y = y;
  }


  void MatrixInternals::Transform(Vector3& rResult, const Vector3& position, const Matrix& matrix)
  {
    // rResult and Position can not be the same
    const float* pMatrix = matrix.DirectAccess();

    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + pMatrix[_M41];
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + pMatrix[_M42];
    const auto z = (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + pMatrix[_M43];
    rResult.X = x;
    rResult.Y = y;
    rResult.Z = z;
  }


  void MatrixInternals::TransformNormal(Vector3& rResult, const Vector3& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();

    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]);
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]);
    const auto z = (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]);
    rResult.X = x;
    rResult.Y = y;
    rResult.Z = z;
  }

  void MatrixInternals::Transform(Vector4& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + pMatrix[_M41];
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + pMatrix[_M42];
    const auto z = (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + pMatrix[_M43];
    const auto w = (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + pMatrix[_M44];
    rResult.X = x;
    rResult.Y = y;
    rResult.Z = z;
    rResult.W = w;
  }

  void MatrixInternals::Transform(Vector4& rResult, const Vector3& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + pMatrix[_M41];
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + pMatrix[_M42];
    const auto z = (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + pMatrix[_M43];
    const auto w = (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + (position.Z * pMatrix[_M34]) + pMatrix[_M44];
    rResult.X = x;
    rResult.Y = y;
    rResult.Z = z;
    rResult.W = w;
  }


  void MatrixInternals::Transform(Vector4& rResult, const Vector4& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    // Done like this to ensure that position and rResult can be the same.
    const auto x = (position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + (position.W * pMatrix[_M41]);
    const auto y = (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + (position.W * pMatrix[_M42]);
    const auto z = (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + (position.W * pMatrix[_M43]);
    const auto w = (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + (position.Z * pMatrix[_M34]) + (position.W * pMatrix[_M44]);
    rResult.X = x;
    rResult.Y = y;
    rResult.Z = z;
    rResult.W = w;
  }
}
