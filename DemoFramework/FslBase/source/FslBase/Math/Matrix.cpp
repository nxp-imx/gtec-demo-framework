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
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include "MatrixFields.hpp"
#include "MatrixInternals.hpp"

// Workaround a issue with qnx signbit
using namespace std;

namespace Fsl
{

  Matrix::Matrix()
  {
    m[_M11] = 0.0f; m[_M12] = 0.0f; m[_M13] = 0.0f; m[_M14] = 0.0f;
    m[_M21] = 0.0f; m[_M22] = 0.0f; m[_M23] = 0.0f; m[_M24] = 0.0f;
    m[_M31] = 0.0f; m[_M32] = 0.0f; m[_M33] = 0.0f; m[_M34] = 0.0f;
    m[_M41] = 0.0f; m[_M42] = 0.0f; m[_M43] = 0.0f; m[_M44] = 0.0f;
  }


  Matrix::Matrix(const float m11, const float m12, const float m13, const float m14,
    const float m21, const float m22, const float m23, const float m24,
    const float m31, const float m32, const float m33, const float m34,
    const float m41, const float m42, const float m43, const float m44)
  {
    m[_M11] = m11;
    m[_M12] = m12;
    m[_M13] = m13;
    m[_M14] = m14;
    m[_M21] = m21;
    m[_M22] = m22;
    m[_M23] = m23;
    m[_M24] = m24;
    m[_M31] = m31;
    m[_M32] = m32;
    m[_M33] = m33;
    m[_M34] = m34;
    m[_M41] = m41;
    m[_M42] = m42;
    m[_M43] = m43;
    m[_M44] = m44;
  }


  Matrix Matrix::Add(const Matrix& matrix1, const Matrix& matrix2)
  {
    return Matrix(
      matrix1.m[_M11] + matrix2.m[_M11],
      matrix1.m[_M12] + matrix2.m[_M12],
      matrix1.m[_M13] + matrix2.m[_M13],
      matrix1.m[_M14] + matrix2.m[_M14],
      matrix1.m[_M21] + matrix2.m[_M21],
      matrix1.m[_M22] + matrix2.m[_M22],
      matrix1.m[_M23] + matrix2.m[_M23],
      matrix1.m[_M24] + matrix2.m[_M24],
      matrix1.m[_M31] + matrix2.m[_M31],
      matrix1.m[_M32] + matrix2.m[_M32],
      matrix1.m[_M33] + matrix2.m[_M33],
      matrix1.m[_M34] + matrix2.m[_M34],
      matrix1.m[_M41] + matrix2.m[_M41],
      matrix1.m[_M42] + matrix2.m[_M42],
      matrix1.m[_M43] + matrix2.m[_M43],
      matrix1.m[_M44] + matrix2.m[_M44]);
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
    m[_M11] = 0.0f; m[_M12] = 0.0f; m[_M13] = 0.0f; m[_M14] = 0.0f;
    m[_M21] = 0.0f; m[_M22] = 0.0f; m[_M23] = 0.0f; m[_M24] = 0.0f;
    m[_M31] = 0.0f; m[_M32] = 0.0f; m[_M33] = 0.0f; m[_M34] = 0.0f;
    m[_M41] = 0.0f; m[_M42] = 0.0f; m[_M43] = 0.0f; m[_M44] = 0.0f;
  }


  Matrix Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector, const Vector3* pCameraForwardVector)
  {
    Matrix result;

    // Delegate to the other overload of the function to do the work
    CreateBillboard(objectPosition, cameraPosition, cameraUpVector, pCameraForwardVector, result);
    return result;
  }


  void Matrix::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector, const Vector3* pCameraForwardVector, Matrix& rResult)
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


  Matrix Matrix::CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotateAxis, const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector)
  {
    Matrix result;
    CreateConstrainedBillboard(objectPosition, cameraPosition, rotateAxis, pCameraForwardVector, pObjectForwardVector, result);
    return result;
  }


  void Matrix::CreateConstrainedBillboard(const Vector3 objectPosition, const Vector3 cameraPosition, const Vector3 rotateAxis, const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector, Matrix& rResult)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateFromAxisAngle(result, axis, angle);
    return result;
  }


  void Matrix::CreateFromAxisAngle(Matrix& rResult, const Vector3& axis, const float angle)
  {
    const float x = axis.X;
    const float y = axis.Y;
    const float z = axis.Z;
    const float num2 = (float)std::sin(angle);
    const float num = (float)std::cos(angle);
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateFromYawPitchRoll(result, yaw, pitch, roll);
    return result;
  }


  Matrix Matrix::CreateFromQuaternion(const Quaternion& quaternion)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateFromQuaternion(result, quaternion);
    return result;
  }


  void Matrix::CreateFromQuaternion(Matrix& rResult, const Quaternion& quaternion)
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


  void Matrix::CreateFromYawPitchRoll(Matrix& rResult, const float yaw, const float pitch, const float roll)
  {
    Quaternion quaternion(OptimizationFlag::NoInitialization);
    Quaternion::CreateFromYawPitchRoll(quaternion, yaw, pitch, roll);
    CreateFromQuaternion(rResult, quaternion);
  }


  Matrix Matrix::CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateLookAt(result, cameraPosition, cameraTarget, cameraUpVector);
    return result;
  }


  void Matrix::CreateLookAt(Matrix& rResult, const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateOrthographic(result, width, height, zNearPlane, zFarPlane);
    return result;
  }


  void Matrix::CreateOrthographic(Matrix& rResult, const float width, const float height, const float zNearPlane, const float zFarPlane)
  {
    if (width <= 0.0f)
      throw std::invalid_argument("width <= 0.0f");
    if (height <= 0.0f)
      throw std::invalid_argument("height <= 0.0f");

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


  Matrix Matrix::CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane, const float zFarPlane)
  {
    Matrix matrix;
    CreateOrthographicOffCenter(left, right, bottom, top, zNearPlane, zFarPlane, matrix);
    return matrix;
  }


  Matrix Matrix::CreateOrthographicOffCenter(const Rectangle& viewingVolume, const float zNearPlane, const float zFarPlane)
  {
    Matrix matrix;
    CreateOrthographicOffCenter(static_cast<float>(viewingVolume.Left()), static_cast<float>(viewingVolume.Right()), static_cast<float>(viewingVolume.Bottom()), static_cast<float>(viewingVolume.Top()), zNearPlane, zFarPlane, matrix);
    return matrix;
  }


  void Matrix::CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane, const float zFarPlane, Matrix& rResult)
  {
    rResult.m[_M11] = (float)(2.0 / ((double)right - (double)left));
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = (float)(2.0 / ((double)top - (double)bottom));
    rResult.m[_M23] = 0.0f;
    rResult.m[_M24] = 0.0f;
    rResult.m[_M31] = 0.0f;
    rResult.m[_M32] = 0.0f;
    rResult.m[_M33] = (float)(1.0 / ((double)zNearPlane - (double)zFarPlane));
    rResult.m[_M34] = 0.0f;
    rResult.m[_M41] = (float)(((double)left + (double)right) / ((double)left - (double)right));
    rResult.m[_M42] = (float)(((double)top + (double)bottom) / ((double)bottom - (double)top));
    rResult.m[_M43] = (float)((double)zNearPlane / ((double)zNearPlane - (double)zFarPlane));
    rResult.m[_M44] = 1.0f;
  }


  Matrix Matrix::CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreatePerspective(result, width, height, nearPlaneDistance, farPlaneDistance);
    return result;
  }


  void Matrix::CreatePerspective(Matrix& rResult, const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance)
  {
    if (width <= 0.0f)
      throw std::invalid_argument("width <= 0.0f");
    if (height <= 0.0f)
      throw std::invalid_argument("height <= 0.0f");
    if (nearPlaneDistance <= 0.0f)
      throw std::invalid_argument("nearPlaneDistance <= 0.0f");
    if (farPlaneDistance <= 0.0f)
      throw std::invalid_argument("farPlaneDistance <= 0.0f");
    if (nearPlaneDistance >= farPlaneDistance)
      throw std::invalid_argument("nearPlaneDistance >= farPlaneDistance");

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


  Matrix Matrix::CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance, const float farPlaneDistance)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreatePerspectiveFieldOfView(result, fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
    return result;
  }


  void Matrix::CreatePerspectiveFieldOfView(Matrix& rResult, const float fieldOfView, const float aspectRatio, const float nearPlaneDistance, const float farPlaneDistance)
  {
    if ((fieldOfView <= 0.0f) || (fieldOfView >= 3.141593f))
      throw std::invalid_argument("fieldOfView <= 0 or >= PI");
    if (nearPlaneDistance <= 0.0f)
      throw std::invalid_argument("nearPlaneDistance <= 0");
    if (farPlaneDistance <= 0.0f)
      throw std::invalid_argument("farPlaneDistance <= 0");
    if (nearPlaneDistance >= farPlaneDistance)
      throw std::invalid_argument("nearPlaneDistance >= farPlaneDistance");

    const float num = 1.0f / ((float)std::tan(fieldOfView * 0.5));
    const float num9 = num / aspectRatio;
    rResult.m[_M11] = num9;
    rResult.m[_M12] = 0.0f;
    rResult.m[_M13] = 0.0f;
    rResult.m[_M14] = 0.0f;
    rResult.m[_M21] = 0.0f;
    rResult.m[_M22] = num;
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


  Matrix Matrix::CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance, const float farPlaneDistance)
  {
    Matrix result;
    CreatePerspectiveOffCenter(left, right, bottom, top, nearPlaneDistance, farPlaneDistance, result);
    return result;
  }


  Matrix Matrix::CreatePerspectiveOffCenter(const Rectangle& viewingVolume, const float nearPlaneDistance, const float farPlaneDistance)
  {
    Matrix result;
    CreatePerspectiveOffCenter(static_cast<float>(viewingVolume.Left()), static_cast<float>(viewingVolume.Right()), static_cast<float>(viewingVolume.Bottom()), static_cast<float>(viewingVolume.Top()), nearPlaneDistance, farPlaneDistance, result);
    return result;
  }


  void Matrix::CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance, const float farPlaneDistance, Matrix& rResult)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateRotationX(result, radians);
    return result;
  }


  void Matrix::CreateRotationX(Matrix& rResult, const float radians)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateRotationY(result, radians);
    return result;
  }


  void Matrix::CreateRotationY(Matrix& rResult, const float radians)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateRotationZ(result, radians);
    return result;
  }


  void Matrix::CreateRotationZ(Matrix& rResult, const float radians)
  {
    rResult.SetIdentity();

    const float val1 = std::cos(radians);
    const float val2 = std::sin(radians);

    rResult.m[_M11] = val1;
    rResult.m[_M12] = val2;
    rResult.m[_M21] = -val2;
    rResult.m[_M22] = val1;
  }


  Matrix Matrix::CreateScale(const float scale)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateScale(result, scale, scale, scale);
    return result;
  }

  void Matrix::CreateScale(Matrix& rResult, const float scale)
  {
    CreateScale(rResult, scale, scale, scale);
  }

  Matrix Matrix::CreateScale(const float scaleX, const float scaleY, const float scaleZ)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateScale(result, scaleX, scaleY, scaleZ);
    return result;
  }

  void Matrix::CreateScale(Matrix& rResult, const float scaleX, const float scaleY, const float scaleZ)
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

  Matrix Matrix::CreateScale(const Vector3& scale)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateScale(result, scale);
    return result;
  }

  void Matrix::CreateScale(Matrix& rResult, const Vector3& scale)
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


  Matrix Matrix::CreateTranslation(const float x, const float y, const float z)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateTranslation(result, x, y, z);
    return result;
  }


  void Matrix::CreateTranslation(Matrix& rResult, const float x, const float y, const float z)
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


  Matrix Matrix::CreateTranslation(const Vector3& value)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    CreateTranslation(result, value);
    return result;
  }


  void Matrix::CreateTranslation(Matrix& rResult, const Vector3& value)
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
    Matrix result(OptimizationFlag::NoInitialization);
    CreateWorld(result, position, forward, up);
    return result;
  }


  void Matrix::CreateWorld(Matrix& rResult, const Vector3& position, const Vector3& forward, const Vector3& up)
  {
    Vector3 x, y, z;
    Vector3::Normalize(z, forward);
    Vector3::Cross(x, forward, up);
    Vector3::Cross(y, x, forward);
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

    const Matrix m1(
      m[_M11] / rScale.X, m[_M12] / rScale.X, m[_M13] / rScale.X, 0,
      m[_M21] / rScale.Y, m[_M22] / rScale.Y, m[_M23] / rScale.Y, 0,
      m[_M31] / rScale.Z, m[_M32] / rScale.Z, m[_M33] / rScale.Z, 0,
      0.0f, 0.0f, 0.0f, 1.0f);
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
    return ((((num22 * (((num11 * num18) - (num10 * num17)) + (num9 * num16))) - (num21 * (((num12 * num18) - (num10 * num15)) + (num9 * num14)))) + (num20 * (((num12 * num17) - (num11 * num15)) + (num9 * num13)))) - (num19 * (((num12 * num16) - (num11 * num14)) + (num10 * num13))));
  }


  Matrix Matrix::Divide(const Matrix& matrix1, const Matrix& matrix2)
  {
    return Matrix(
      matrix1.m[_M11] / matrix2.m[_M11],
      matrix1.m[_M12] / matrix2.m[_M12],
      matrix1.m[_M13] / matrix2.m[_M13],
      matrix1.m[_M14] / matrix2.m[_M14],
      matrix1.m[_M21] / matrix2.m[_M21],
      matrix1.m[_M22] / matrix2.m[_M22],
      matrix1.m[_M23] / matrix2.m[_M23],
      matrix1.m[_M24] / matrix2.m[_M24],
      matrix1.m[_M31] / matrix2.m[_M31],
      matrix1.m[_M32] / matrix2.m[_M32],
      matrix1.m[_M33] / matrix2.m[_M33],
      matrix1.m[_M34] / matrix2.m[_M34],
      matrix1.m[_M41] / matrix2.m[_M41],
      matrix1.m[_M42] / matrix2.m[_M42],
      matrix1.m[_M43] / matrix2.m[_M43],
      matrix1.m[_M44] / matrix2.m[_M44]);
  }


  void Matrix::Divide(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
  {
    rResult.m[_M11] = matrix1.m[_M11] / matrix2.m[_M11];
    rResult.m[_M12] = matrix1.m[_M12] / matrix2.m[_M12];
    rResult.m[_M13] = matrix1.m[_M13] / matrix2.m[_M13];
    rResult.m[_M14] = matrix1.m[_M14] / matrix2.m[_M14];
    rResult.m[_M21] = matrix1.m[_M21] / matrix2.m[_M21];
    rResult.m[_M22] = matrix1.m[_M22] / matrix2.m[_M22];
    rResult.m[_M23] = matrix1.m[_M23] / matrix2.m[_M23];
    rResult.m[_M24] = matrix1.m[_M24] / matrix2.m[_M24];
    rResult.m[_M31] = matrix1.m[_M31] / matrix2.m[_M31];
    rResult.m[_M32] = matrix1.m[_M32] / matrix2.m[_M32];
    rResult.m[_M33] = matrix1.m[_M33] / matrix2.m[_M33];
    rResult.m[_M34] = matrix1.m[_M34] / matrix2.m[_M34];
    rResult.m[_M41] = matrix1.m[_M41] / matrix2.m[_M41];
    rResult.m[_M42] = matrix1.m[_M42] / matrix2.m[_M42];
    rResult.m[_M43] = matrix1.m[_M43] / matrix2.m[_M43];
    rResult.m[_M44] = matrix1.m[_M44] / matrix2.m[_M44];
  }


  Matrix Matrix::Divide(const Matrix& matrix1, const float divider)
  {
    // Multiply usually has better performance
    const float num = 1.0f / divider;

    return Matrix(
      matrix1.m[_M11] * num, matrix1.m[_M12] * num, matrix1.m[_M13] * num, matrix1.m[_M14] * num,
      matrix1.m[_M21] * num, matrix1.m[_M22] * num, matrix1.m[_M23] * num, matrix1.m[_M24] * num,
      matrix1.m[_M31] * num, matrix1.m[_M32] * num, matrix1.m[_M33] * num, matrix1.m[_M34] * num,
      matrix1.m[_M41] * num, matrix1.m[_M42] * num, matrix1.m[_M43] * num, matrix1.m[_M44] * num);
  }


  void Matrix::Divide(Matrix& rResult, const Matrix& matrix1, const float divider)
  {
    // Multiply usually has better performance
    const float num = 1.0f / divider;
    rResult.m[_M11] = matrix1.m[_M11] * num;
    rResult.m[_M12] = matrix1.m[_M12] * num;
    rResult.m[_M13] = matrix1.m[_M13] * num;
    rResult.m[_M14] = matrix1.m[_M14] * num;
    rResult.m[_M21] = matrix1.m[_M21] * num;
    rResult.m[_M22] = matrix1.m[_M22] * num;
    rResult.m[_M23] = matrix1.m[_M23] * num;
    rResult.m[_M24] = matrix1.m[_M24] * num;
    rResult.m[_M31] = matrix1.m[_M31] * num;
    rResult.m[_M32] = matrix1.m[_M32] * num;
    rResult.m[_M33] = matrix1.m[_M33] * num;
    rResult.m[_M34] = matrix1.m[_M34] * num;
    rResult.m[_M41] = matrix1.m[_M41] * num;
    rResult.m[_M42] = matrix1.m[_M42] * num;
    rResult.m[_M43] = matrix1.m[_M43] * num;
    rResult.m[_M44] = matrix1.m[_M44] * num;
  }


  Matrix Matrix::Invert(const Matrix& matrix)
  {
    Matrix result(OptimizationFlag::NoInitialization);
    Invert(result, matrix);
    return result;
  }


  void Matrix::Invert(Matrix& rResult, const Matrix& matrix)
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
    const float num17 = (float)((double)num11 * (double)num16 - (double)num12 * (double)num15);
    const float num18 = (float)((double)num10 * (double)num16 - (double)num12 * (double)num14);
    const float num19 = (float)((double)num10 * (double)num15 - (double)num11 * (double)num14);
    const float num20 = (float)((double)num9 * (double)num16 - (double)num12 * (double)num13);
    const float num21 = (float)((double)num9 * (double)num15 - (double)num11 * (double)num13);
    const float num22 = (float)((double)num9 * (double)num14 - (double)num10 * (double)num13);
    const float num23 = (float)((double)num6 * (double)num17 - (double)num7 * (double)num18 + (double)num8 * (double)num19);
    const float num24 = (float)-((double)num5 * (double)num17 - (double)num7 * (double)num20 + (double)num8 * (double)num21);
    const float num25 = (float)((double)num5 * (double)num18 - (double)num6 * (double)num20 + (double)num8 * (double)num22);
    const float num26 = (float)-((double)num5 * (double)num19 - (double)num6 * (double)num21 + (double)num7 * (double)num22);
    const float num27 = (float)(1.0 / ((double)num1 * (double)num23 + (double)num2 * (double)num24 + (double)num3 * (double)num25 + (double)num4 * (double)num26));

    rResult.m[_M11] = num23 * num27;
    rResult.m[_M21] = num24 * num27;
    rResult.m[_M31] = num25 * num27;
    rResult.m[_M41] = num26 * num27;
    rResult.m[_M12] = (float)-((double)num2 * (double)num17 - (double)num3 * (double)num18 + (double)num4 * (double)num19) * num27;
    rResult.m[_M22] = (float)((double)num1 * (double)num17 - (double)num3 * (double)num20 + (double)num4 * (double)num21) * num27;
    rResult.m[_M32] = (float)-((double)num1 * (double)num18 - (double)num2 * (double)num20 + (double)num4 * (double)num22) * num27;
    rResult.m[_M42] = (float)((double)num1 * (double)num19 - (double)num2 * (double)num21 + (double)num3 * (double)num22) * num27;
    const float num28 = (float)((double)num7 * (double)num16 - (double)num8 * (double)num15);
    const float num29 = (float)((double)num6 * (double)num16 - (double)num8 * (double)num14);
    const float num30 = (float)((double)num6 * (double)num15 - (double)num7 * (double)num14);
    const float num31 = (float)((double)num5 * (double)num16 - (double)num8 * (double)num13);
    const float num32 = (float)((double)num5 * (double)num15 - (double)num7 * (double)num13);
    const float num33 = (float)((double)num5 * (double)num14 - (double)num6 * (double)num13);
    rResult.m[_M13] = (float)((double)num2 * (double)num28 - (double)num3 * (double)num29 + (double)num4 * (double)num30) * num27;
    rResult.m[_M23] = (float)-((double)num1 * (double)num28 - (double)num3 * (double)num31 + (double)num4 * (double)num32) * num27;
    rResult.m[_M33] = (float)((double)num1 * (double)num29 - (double)num2 * (double)num31 + (double)num4 * (double)num33) * num27;
    rResult.m[_M43] = (float)-((double)num1 * (double)num30 - (double)num2 * (double)num32 + (double)num3 * (double)num33) * num27;
    const float num34 = (float)((double)num7 * (double)num12 - (double)num8 * (double)num11);
    const float num35 = (float)((double)num6 * (double)num12 - (double)num8 * (double)num10);
    const float num36 = (float)((double)num6 * (double)num11 - (double)num7 * (double)num10);
    const float num37 = (float)((double)num5 * (double)num12 - (double)num8 * (double)num9);
    const float num38 = (float)((double)num5 * (double)num11 - (double)num7 * (double)num9);
    const float num39 = (float)((double)num5 * (double)num10 - (double)num6 * (double)num9);
    rResult.m[_M14] = (float)-((double)num2 * (double)num34 - (double)num3 * (double)num35 + (double)num4 * (double)num36) * num27;
    rResult.m[_M24] = (float)((double)num1 * (double)num34 - (double)num3 * (double)num37 + (double)num4 * (double)num38) * num27;
    rResult.m[_M34] = (float)-((double)num1 * (double)num35 - (double)num2 * (double)num37 + (double)num4 * (double)num39) * num27;
    rResult.m[_M44] = (float)((double)num1 * (double)num36 - (double)num2 * (double)num38 + (double)num3 * (double)num39) * num27;
  }


  Matrix Matrix::Lerp(const Matrix& value1, const Matrix& value2, const float amount)
  {
    const float* pMatrix1 = value1.m;
    const float* pMatrix2 = value2.m;
    return Matrix(
      pMatrix1[_M11] + ((pMatrix2[_M11] - pMatrix1[_M11]) * amount),
      pMatrix1[_M12] + ((pMatrix2[_M12] - pMatrix1[_M12]) * amount),
      pMatrix1[_M13] + ((pMatrix2[_M13] - pMatrix1[_M13]) * amount),
      pMatrix1[_M14] + ((pMatrix2[_M14] - pMatrix1[_M14]) * amount),
      pMatrix1[_M21] + ((pMatrix2[_M21] - pMatrix1[_M21]) * amount),
      pMatrix1[_M22] + ((pMatrix2[_M22] - pMatrix1[_M22]) * amount),
      pMatrix1[_M23] + ((pMatrix2[_M23] - pMatrix1[_M23]) * amount),
      pMatrix1[_M24] + ((pMatrix2[_M24] - pMatrix1[_M24]) * amount),
      pMatrix1[_M31] + ((pMatrix2[_M31] - pMatrix1[_M31]) * amount),
      pMatrix1[_M32] + ((pMatrix2[_M32] - pMatrix1[_M32]) * amount),
      pMatrix1[_M33] + ((pMatrix2[_M33] - pMatrix1[_M33]) * amount),
      pMatrix1[_M34] + ((pMatrix2[_M34] - pMatrix1[_M34]) * amount),
      pMatrix1[_M41] + ((pMatrix2[_M41] - pMatrix1[_M41]) * amount),
      pMatrix1[_M42] + ((pMatrix2[_M42] - pMatrix1[_M42]) * amount),
      pMatrix1[_M43] + ((pMatrix2[_M43] - pMatrix1[_M43]) * amount),
      pMatrix1[_M44] + ((pMatrix2[_M44] - pMatrix1[_M44]) * amount));
  }


  void Matrix::Lerp(Matrix& rResult, const Matrix& value1, const Matrix& value2, const float amount)
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

  Matrix Matrix::Multiply(const Matrix& matrix1, const Matrix& matrix2)
  {
    return Matrix(
      (((matrix1.m[_M11] * matrix2.m[_M11]) + (matrix1.m[_M12] * matrix2.m[_M21])) + (matrix1.m[_M13] * matrix2.m[_M31])) + (matrix1.m[_M14] * matrix2.m[_M41]),
      (((matrix1.m[_M11] * matrix2.m[_M12]) + (matrix1.m[_M12] * matrix2.m[_M22])) + (matrix1.m[_M13] * matrix2.m[_M32])) + (matrix1.m[_M14] * matrix2.m[_M42]),
      (((matrix1.m[_M11] * matrix2.m[_M13]) + (matrix1.m[_M12] * matrix2.m[_M23])) + (matrix1.m[_M13] * matrix2.m[_M33])) + (matrix1.m[_M14] * matrix2.m[_M43]),
      (((matrix1.m[_M11] * matrix2.m[_M14]) + (matrix1.m[_M12] * matrix2.m[_M24])) + (matrix1.m[_M13] * matrix2.m[_M34])) + (matrix1.m[_M14] * matrix2.m[_M44]),
      (((matrix1.m[_M21] * matrix2.m[_M11]) + (matrix1.m[_M22] * matrix2.m[_M21])) + (matrix1.m[_M23] * matrix2.m[_M31])) + (matrix1.m[_M24] * matrix2.m[_M41]),
      (((matrix1.m[_M21] * matrix2.m[_M12]) + (matrix1.m[_M22] * matrix2.m[_M22])) + (matrix1.m[_M23] * matrix2.m[_M32])) + (matrix1.m[_M24] * matrix2.m[_M42]),
      (((matrix1.m[_M21] * matrix2.m[_M13]) + (matrix1.m[_M22] * matrix2.m[_M23])) + (matrix1.m[_M23] * matrix2.m[_M33])) + (matrix1.m[_M24] * matrix2.m[_M43]),
      (((matrix1.m[_M21] * matrix2.m[_M14]) + (matrix1.m[_M22] * matrix2.m[_M24])) + (matrix1.m[_M23] * matrix2.m[_M34])) + (matrix1.m[_M24] * matrix2.m[_M44]),
      (((matrix1.m[_M31] * matrix2.m[_M11]) + (matrix1.m[_M32] * matrix2.m[_M21])) + (matrix1.m[_M33] * matrix2.m[_M31])) + (matrix1.m[_M34] * matrix2.m[_M41]),
      (((matrix1.m[_M31] * matrix2.m[_M12]) + (matrix1.m[_M32] * matrix2.m[_M22])) + (matrix1.m[_M33] * matrix2.m[_M32])) + (matrix1.m[_M34] * matrix2.m[_M42]),
      (((matrix1.m[_M31] * matrix2.m[_M13]) + (matrix1.m[_M32] * matrix2.m[_M23])) + (matrix1.m[_M33] * matrix2.m[_M33])) + (matrix1.m[_M34] * matrix2.m[_M43]),
      (((matrix1.m[_M31] * matrix2.m[_M14]) + (matrix1.m[_M32] * matrix2.m[_M24])) + (matrix1.m[_M33] * matrix2.m[_M34])) + (matrix1.m[_M34] * matrix2.m[_M44]),
      (((matrix1.m[_M41] * matrix2.m[_M11]) + (matrix1.m[_M42] * matrix2.m[_M21])) + (matrix1.m[_M43] * matrix2.m[_M31])) + (matrix1.m[_M44] * matrix2.m[_M41]),
      (((matrix1.m[_M41] * matrix2.m[_M12]) + (matrix1.m[_M42] * matrix2.m[_M22])) + (matrix1.m[_M43] * matrix2.m[_M32])) + (matrix1.m[_M44] * matrix2.m[_M42]),
      (((matrix1.m[_M41] * matrix2.m[_M13]) + (matrix1.m[_M42] * matrix2.m[_M23])) + (matrix1.m[_M43] * matrix2.m[_M33])) + (matrix1.m[_M44] * matrix2.m[_M43]),
      (((matrix1.m[_M41] * matrix2.m[_M14]) + (matrix1.m[_M42] * matrix2.m[_M24])) + (matrix1.m[_M43] * matrix2.m[_M34])) + (matrix1.m[_M44] * matrix2.m[_M44]));
  }


  void Matrix::Multiply(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
  {
    const float* pMatrix1 = matrix1.m;
    const float* pMatrix2 = matrix2.m;

    const float m11 = (((pMatrix1[_M11] * pMatrix2[_M11]) + (pMatrix1[_M12] * pMatrix2[_M21])) + (pMatrix1[_M13] * pMatrix2[_M31])) + (pMatrix1[_M14] * pMatrix2[_M41]);
    const float m12 = (((pMatrix1[_M11] * pMatrix2[_M12]) + (pMatrix1[_M12] * pMatrix2[_M22])) + (pMatrix1[_M13] * pMatrix2[_M32])) + (pMatrix1[_M14] * pMatrix2[_M42]);
    const float m13 = (((pMatrix1[_M11] * pMatrix2[_M13]) + (pMatrix1[_M12] * pMatrix2[_M23])) + (pMatrix1[_M13] * pMatrix2[_M33])) + (pMatrix1[_M14] * pMatrix2[_M43]);
    const float m14 = (((pMatrix1[_M11] * pMatrix2[_M14]) + (pMatrix1[_M12] * pMatrix2[_M24])) + (pMatrix1[_M13] * pMatrix2[_M34])) + (pMatrix1[_M14] * pMatrix2[_M44]);
    const float m21 = (((pMatrix1[_M21] * pMatrix2[_M11]) + (pMatrix1[_M22] * pMatrix2[_M21])) + (pMatrix1[_M23] * pMatrix2[_M31])) + (pMatrix1[_M24] * pMatrix2[_M41]);
    const float m22 = (((pMatrix1[_M21] * pMatrix2[_M12]) + (pMatrix1[_M22] * pMatrix2[_M22])) + (pMatrix1[_M23] * pMatrix2[_M32])) + (pMatrix1[_M24] * pMatrix2[_M42]);
    const float m23 = (((pMatrix1[_M21] * pMatrix2[_M13]) + (pMatrix1[_M22] * pMatrix2[_M23])) + (pMatrix1[_M23] * pMatrix2[_M33])) + (pMatrix1[_M24] * pMatrix2[_M43]);
    const float m24 = (((pMatrix1[_M21] * pMatrix2[_M14]) + (pMatrix1[_M22] * pMatrix2[_M24])) + (pMatrix1[_M23] * pMatrix2[_M34])) + (pMatrix1[_M24] * pMatrix2[_M44]);
    const float m31 = (((pMatrix1[_M31] * pMatrix2[_M11]) + (pMatrix1[_M32] * pMatrix2[_M21])) + (pMatrix1[_M33] * pMatrix2[_M31])) + (pMatrix1[_M34] * pMatrix2[_M41]);
    const float m32 = (((pMatrix1[_M31] * pMatrix2[_M12]) + (pMatrix1[_M32] * pMatrix2[_M22])) + (pMatrix1[_M33] * pMatrix2[_M32])) + (pMatrix1[_M34] * pMatrix2[_M42]);
    const float m33 = (((pMatrix1[_M31] * pMatrix2[_M13]) + (pMatrix1[_M32] * pMatrix2[_M23])) + (pMatrix1[_M33] * pMatrix2[_M33])) + (pMatrix1[_M34] * pMatrix2[_M43]);
    const float m34 = (((pMatrix1[_M31] * pMatrix2[_M14]) + (pMatrix1[_M32] * pMatrix2[_M24])) + (pMatrix1[_M33] * pMatrix2[_M34])) + (pMatrix1[_M34] * pMatrix2[_M44]);
    const float m41 = (((pMatrix1[_M41] * pMatrix2[_M11]) + (pMatrix1[_M42] * pMatrix2[_M21])) + (pMatrix1[_M43] * pMatrix2[_M31])) + (pMatrix1[_M44] * pMatrix2[_M41]);
    const float m42 = (((pMatrix1[_M41] * pMatrix2[_M12]) + (pMatrix1[_M42] * pMatrix2[_M22])) + (pMatrix1[_M43] * pMatrix2[_M32])) + (pMatrix1[_M44] * pMatrix2[_M42]);
    const float m43 = (((pMatrix1[_M41] * pMatrix2[_M13]) + (pMatrix1[_M42] * pMatrix2[_M23])) + (pMatrix1[_M43] * pMatrix2[_M33])) + (pMatrix1[_M44] * pMatrix2[_M43]);
    const float m44 = (((pMatrix1[_M41] * pMatrix2[_M14]) + (pMatrix1[_M42] * pMatrix2[_M24])) + (pMatrix1[_M43] * pMatrix2[_M34])) + (pMatrix1[_M44] * pMatrix2[_M44]);

    rResult.m[_M11] = m11;
    rResult.m[_M12] = m12;
    rResult.m[_M13] = m13;
    rResult.m[_M14] = m14;
    rResult.m[_M21] = m21;
    rResult.m[_M22] = m22;
    rResult.m[_M23] = m23;
    rResult.m[_M24] = m24;
    rResult.m[_M31] = m31;
    rResult.m[_M32] = m32;
    rResult.m[_M33] = m33;
    rResult.m[_M34] = m34;
    rResult.m[_M41] = m41;
    rResult.m[_M42] = m42;
    rResult.m[_M43] = m43;
    rResult.m[_M44] = m44;
  }


  Matrix Matrix::Multiply(const Matrix& matrix1, const float factor)
  {
    return Matrix(
      matrix1.m[_M11] * factor, matrix1.m[_M12] * factor, matrix1.m[_M13] * factor, matrix1.m[_M14] * factor,
      matrix1.m[_M21] * factor, matrix1.m[_M22] * factor, matrix1.m[_M23] * factor, matrix1.m[_M24] * factor,
      matrix1.m[_M31] * factor, matrix1.m[_M32] * factor, matrix1.m[_M33] * factor, matrix1.m[_M34] * factor,
      matrix1.m[_M41] * factor, matrix1.m[_M42] * factor, matrix1.m[_M43] * factor, matrix1.m[_M44] * factor);
  }

  void Matrix::Multiply(Matrix& rResult, const Matrix& matrix1, const float factor)
  {
    rResult.m[_M11] = matrix1.m[_M11] * factor;
    rResult.m[_M12] = matrix1.m[_M12] * factor;
    rResult.m[_M13] = matrix1.m[_M13] * factor;
    rResult.m[_M14] = matrix1.m[_M14] * factor;
    rResult.m[_M21] = matrix1.m[_M21] * factor;
    rResult.m[_M22] = matrix1.m[_M22] * factor;
    rResult.m[_M23] = matrix1.m[_M23] * factor;
    rResult.m[_M24] = matrix1.m[_M24] * factor;
    rResult.m[_M31] = matrix1.m[_M31] * factor;
    rResult.m[_M32] = matrix1.m[_M32] * factor;
    rResult.m[_M33] = matrix1.m[_M33] * factor;
    rResult.m[_M34] = matrix1.m[_M34] * factor;
    rResult.m[_M41] = matrix1.m[_M41] * factor;
    rResult.m[_M42] = matrix1.m[_M42] * factor;
    rResult.m[_M43] = matrix1.m[_M43] * factor;
    rResult.m[_M44] = matrix1.m[_M44] * factor;
  }


  Matrix Matrix::Negate(const Matrix& matrix) const
  {
    return Matrix(
      -matrix.m[_M11], -matrix.m[_M12], -matrix.m[_M13], -matrix.m[_M14],
      -matrix.m[_M21], -matrix.m[_M22], -matrix.m[_M23], -matrix.m[_M24],
      -matrix.m[_M31], -matrix.m[_M32], -matrix.m[_M33], -matrix.m[_M34],
      -matrix.m[_M41], -matrix.m[_M42], -matrix.m[_M43], -matrix.m[_M44]
      );
  }


  void Matrix::Negate(Matrix& rResult, const Matrix& matrix)
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
    m[_M11] = 1.0f; m[_M12] = 0.0f; m[_M13] = 0.0f; m[_M14] = 0.0f;
    m[_M21] = 0.0f; m[_M22] = 1.0f; m[_M23] = 0.0f; m[_M24] = 0.0f;
    m[_M31] = 0.0f; m[_M32] = 0.0f; m[_M33] = 1.0f; m[_M34] = 0.0f;
    m[_M41] = 0.0f; m[_M42] = 0.0f; m[_M43] = 0.0f; m[_M44] = 1.0f;
  }

  Matrix Matrix::Subtract(const Matrix& matrix1, const Matrix& matrix2)
  {
    return Matrix(
      matrix1.m[_M11] - matrix2.m[_M11],
      matrix1.m[_M12] - matrix2.m[_M12],
      matrix1.m[_M13] - matrix2.m[_M13],
      matrix1.m[_M14] - matrix2.m[_M14],
      matrix1.m[_M21] - matrix2.m[_M21],
      matrix1.m[_M22] - matrix2.m[_M22],
      matrix1.m[_M23] - matrix2.m[_M23],
      matrix1.m[_M24] - matrix2.m[_M24],
      matrix1.m[_M31] - matrix2.m[_M31],
      matrix1.m[_M32] - matrix2.m[_M32],
      matrix1.m[_M33] - matrix2.m[_M33],
      matrix1.m[_M34] - matrix2.m[_M34],
      matrix1.m[_M41] - matrix2.m[_M41],
      matrix1.m[_M42] - matrix2.m[_M42],
      matrix1.m[_M43] - matrix2.m[_M43],
      matrix1.m[_M44] - matrix2.m[_M44]);
  }


  void Matrix::Subtract(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
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


  Matrix Matrix::Transpose(const Matrix& matrix)
  {
    return Matrix(matrix.m[_M11], matrix.m[_M21], matrix.m[_M31], matrix.m[_M41],
      matrix.m[_M12], matrix.m[_M22], matrix.m[_M32], matrix.m[_M42],
      matrix.m[_M13], matrix.m[_M23], matrix.m[_M33], matrix.m[_M43],
      matrix.m[_M14], matrix.m[_M24], matrix.m[_M34], matrix.m[_M44]);
  }


  void Matrix::Transpose(Matrix& rResult, const Matrix& src)
  {
    rResult = Transpose(src);
  }


  void MatrixInternals::Transform(Vector2& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector2((position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M21]) + pMatrix[_M41],
      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M22]) + pMatrix[_M42]);
  }


  void MatrixInternals::TransformNormal(Vector2& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector2((position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M21]),
      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M22]));
  }

  void MatrixInternals::Transform(Vector3& rResult, const Vector3& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector3((position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + pMatrix[_M41],
                      (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + pMatrix[_M42],
                      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + pMatrix[_M43]);
  }


  void MatrixInternals::TransformNormal(Vector3& rResult, const Vector3& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector3((position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]),
                      (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]),
                      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]));
  }

  void MatrixInternals::Transform(Vector4& rResult, const Vector2& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector4((position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + pMatrix[_M41],
                      (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + pMatrix[_M42],
                      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + pMatrix[_M43],
                      (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + pMatrix[_M44]);
  }

  void MatrixInternals::Transform(Vector4& rResult, const Vector3& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector4((position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + pMatrix[_M41],
                      (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + pMatrix[_M42],
                      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + pMatrix[_M43],
                      (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + (position.Z * pMatrix[_M34]) + pMatrix[_M44]);
  }


  void MatrixInternals::Transform(Vector4& rResult, const Vector4& position, const Matrix& matrix)
  {
    const float* pMatrix = matrix.DirectAccess();
    rResult = Vector4((position.X * pMatrix[_M11]) + (position.Y * pMatrix[_M21]) + (position.Z * pMatrix[_M31]) + (position.W * pMatrix[_M41]),
                      (position.X * pMatrix[_M12]) + (position.Y * pMatrix[_M22]) + (position.Z * pMatrix[_M32]) + (position.W * pMatrix[_M42]),
                      (position.X * pMatrix[_M13]) + (position.Y * pMatrix[_M23]) + (position.Z * pMatrix[_M33]) + (position.W * pMatrix[_M43]),
                      (position.X * pMatrix[_M14]) + (position.Y * pMatrix[_M24]) + (position.Z * pMatrix[_M34]) + (position.W * pMatrix[_M44]));
  }

  Matrix Matrix::operator-() const
  {
    return Negate(*this);
  }


  Matrix& Matrix::operator*= (const Matrix& rhs)
  {
    const float* pRhs = rhs.DirectAccess();

    const float m11 = (((m[_M11] * pRhs[_M11]) + (m[_M12] * pRhs[_M21]) + (m[_M13] * pRhs[_M31])) + (m[_M14] * pRhs[_M41]));
    const float m12 = (((m[_M11] * pRhs[_M12]) + (m[_M12] * pRhs[_M22]) + (m[_M13] * pRhs[_M32])) + (m[_M14] * pRhs[_M42]));
    const float m13 = (((m[_M11] * pRhs[_M13]) + (m[_M12] * pRhs[_M23]) + (m[_M13] * pRhs[_M33])) + (m[_M14] * pRhs[_M43]));
    const float m14 = (((m[_M11] * pRhs[_M14]) + (m[_M12] * pRhs[_M24]) + (m[_M13] * pRhs[_M34])) + (m[_M14] * pRhs[_M44]));
    const float m21 = (((m[_M21] * pRhs[_M11]) + (m[_M22] * pRhs[_M21]) + (m[_M23] * pRhs[_M31])) + (m[_M24] * pRhs[_M41]));
    const float m22 = (((m[_M21] * pRhs[_M12]) + (m[_M22] * pRhs[_M22]) + (m[_M23] * pRhs[_M32])) + (m[_M24] * pRhs[_M42]));
    const float m23 = (((m[_M21] * pRhs[_M13]) + (m[_M22] * pRhs[_M23]) + (m[_M23] * pRhs[_M33])) + (m[_M24] * pRhs[_M43]));
    const float m24 = (((m[_M21] * pRhs[_M14]) + (m[_M22] * pRhs[_M24]) + (m[_M23] * pRhs[_M34])) + (m[_M24] * pRhs[_M44]));
    const float m31 = (((m[_M31] * pRhs[_M11]) + (m[_M32] * pRhs[_M21]) + (m[_M33] * pRhs[_M31])) + (m[_M34] * pRhs[_M41]));
    const float m32 = (((m[_M31] * pRhs[_M12]) + (m[_M32] * pRhs[_M22]) + (m[_M33] * pRhs[_M32])) + (m[_M34] * pRhs[_M42]));
    const float m33 = (((m[_M31] * pRhs[_M13]) + (m[_M32] * pRhs[_M23]) + (m[_M33] * pRhs[_M33])) + (m[_M34] * pRhs[_M43]));
    const float m34 = (((m[_M31] * pRhs[_M14]) + (m[_M32] * pRhs[_M24]) + (m[_M33] * pRhs[_M34])) + (m[_M34] * pRhs[_M44]));
    const float m41 = (((m[_M41] * pRhs[_M11]) + (m[_M42] * pRhs[_M21]) + (m[_M43] * pRhs[_M31])) + (m[_M44] * pRhs[_M41]));
    const float m42 = (((m[_M41] * pRhs[_M12]) + (m[_M42] * pRhs[_M22]) + (m[_M43] * pRhs[_M32])) + (m[_M44] * pRhs[_M42]));
    const float m43 = (((m[_M41] * pRhs[_M13]) + (m[_M42] * pRhs[_M23]) + (m[_M43] * pRhs[_M33])) + (m[_M44] * pRhs[_M43]));
    const float m44 = (((m[_M41] * pRhs[_M14]) + (m[_M42] * pRhs[_M24]) + (m[_M43] * pRhs[_M34])) + (m[_M44] * pRhs[_M44]));

    m[_M11] = m11;
    m[_M12] = m12;
    m[_M13] = m13;
    m[_M14] = m14;
    m[_M21] = m21;
    m[_M22] = m22;
    m[_M23] = m23;
    m[_M24] = m24;
    m[_M31] = m31;
    m[_M32] = m32;
    m[_M33] = m33;
    m[_M34] = m34;
    m[_M41] = m41;
    m[_M42] = m42;
    m[_M43] = m43;
    m[_M44] = m44;
    return *this;
  }


  Matrix& Matrix::operator += (const Matrix& rhs)
  {
    *this = Add(*this, rhs);
    return *this;
  }


  Matrix& Matrix::operator -= (const Matrix& rhs)
  {
    *this = Subtract(*this, rhs);
    return *this;
  }


  bool Matrix::operator==(const Matrix &rhs) const
  {
    return (m[_M11] == rhs.m[_M11] && m[_M12] == rhs.m[_M12] && m[_M13] == rhs.m[_M13] && m[_M14] == rhs.m[_M14] &&
      m[_M21] == rhs.m[_M21] && m[_M22] == rhs.m[_M22] && m[_M23] == rhs.m[_M23] && m[_M24] == rhs.m[_M24] &&
      m[_M31] == rhs.m[_M31] && m[_M32] == rhs.m[_M32] && m[_M33] == rhs.m[_M33] && m[_M34] == rhs.m[_M34] &&
      m[_M41] == rhs.m[_M41] && m[_M42] == rhs.m[_M42] && m[_M43] == rhs.m[_M43] && m[_M44] == rhs.m[_M44]);
  }


  bool Matrix::operator!=(const Matrix &rhs) const
  {
    return (m[_M11] != rhs.m[_M11] || m[_M12] != rhs.m[_M12] || m[_M13] != rhs.m[_M13] || m[_M14] != rhs.m[_M14] ||
      m[_M21] != rhs.m[_M21] || m[_M22] != rhs.m[_M22] || m[_M23] != rhs.m[_M23] || m[_M24] != rhs.m[_M24] ||
      m[_M31] != rhs.m[_M31] || m[_M32] != rhs.m[_M32] || m[_M33] != rhs.m[_M33] || m[_M34] != rhs.m[_M34] ||
      m[_M41] != rhs.m[_M41] || m[_M42] != rhs.m[_M42] || m[_M43] != rhs.m[_M43] || m[_M44] != rhs.m[_M44]);
  }
}


const Fsl::Matrix operator*(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  const float* pLhs = lhs.DirectAccess();
  const float* pRhs = rhs.DirectAccess();
  return Fsl::Matrix(
    (((pLhs[_M11] * pRhs[_M11]) + (pLhs[_M12] * pRhs[_M21])) + (pLhs[_M13] * pRhs[_M31])) + (pLhs[_M14] * pRhs[_M41]),
    (((pLhs[_M11] * pRhs[_M12]) + (pLhs[_M12] * pRhs[_M22])) + (pLhs[_M13] * pRhs[_M32])) + (pLhs[_M14] * pRhs[_M42]),
    (((pLhs[_M11] * pRhs[_M13]) + (pLhs[_M12] * pRhs[_M23])) + (pLhs[_M13] * pRhs[_M33])) + (pLhs[_M14] * pRhs[_M43]),
    (((pLhs[_M11] * pRhs[_M14]) + (pLhs[_M12] * pRhs[_M24])) + (pLhs[_M13] * pRhs[_M34])) + (pLhs[_M14] * pRhs[_M44]),
    (((pLhs[_M21] * pRhs[_M11]) + (pLhs[_M22] * pRhs[_M21])) + (pLhs[_M23] * pRhs[_M31])) + (pLhs[_M24] * pRhs[_M41]),
    (((pLhs[_M21] * pRhs[_M12]) + (pLhs[_M22] * pRhs[_M22])) + (pLhs[_M23] * pRhs[_M32])) + (pLhs[_M24] * pRhs[_M42]),
    (((pLhs[_M21] * pRhs[_M13]) + (pLhs[_M22] * pRhs[_M23])) + (pLhs[_M23] * pRhs[_M33])) + (pLhs[_M24] * pRhs[_M43]),
    (((pLhs[_M21] * pRhs[_M14]) + (pLhs[_M22] * pRhs[_M24])) + (pLhs[_M23] * pRhs[_M34])) + (pLhs[_M24] * pRhs[_M44]),
    (((pLhs[_M31] * pRhs[_M11]) + (pLhs[_M32] * pRhs[_M21])) + (pLhs[_M33] * pRhs[_M31])) + (pLhs[_M34] * pRhs[_M41]),
    (((pLhs[_M31] * pRhs[_M12]) + (pLhs[_M32] * pRhs[_M22])) + (pLhs[_M33] * pRhs[_M32])) + (pLhs[_M34] * pRhs[_M42]),
    (((pLhs[_M31] * pRhs[_M13]) + (pLhs[_M32] * pRhs[_M23])) + (pLhs[_M33] * pRhs[_M33])) + (pLhs[_M34] * pRhs[_M43]),
    (((pLhs[_M31] * pRhs[_M14]) + (pLhs[_M32] * pRhs[_M24])) + (pLhs[_M33] * pRhs[_M34])) + (pLhs[_M34] * pRhs[_M44]),
    (((pLhs[_M41] * pRhs[_M11]) + (pLhs[_M42] * pRhs[_M21])) + (pLhs[_M43] * pRhs[_M31])) + (pLhs[_M44] * pRhs[_M41]),
    (((pLhs[_M41] * pRhs[_M12]) + (pLhs[_M42] * pRhs[_M22])) + (pLhs[_M43] * pRhs[_M32])) + (pLhs[_M44] * pRhs[_M42]),
    (((pLhs[_M41] * pRhs[_M13]) + (pLhs[_M42] * pRhs[_M23])) + (pLhs[_M43] * pRhs[_M33])) + (pLhs[_M44] * pRhs[_M43]),
    (((pLhs[_M41] * pRhs[_M14]) + (pLhs[_M42] * pRhs[_M24])) + (pLhs[_M43] * pRhs[_M34])) + (pLhs[_M44] * pRhs[_M44]));
}


const Fsl::Matrix operator+(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  return Fsl::Matrix::Add(lhs, rhs);
}


const Fsl::Matrix operator-(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  return Fsl::Matrix::Subtract(lhs, rhs);
}
