#ifndef FSLBASE_MATH_MATRIX_HPP
#define FSLBASE_MATH_MATRIX_HPP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <algorithm>
#include <cstddef>
//#include <FslBase/OptimizationFlag.hpp>

namespace Fsl
{
  struct Plane;
  struct Quaternion;
  struct Rectangle;

  // Simple matrix class inspired by XNA.
  // Designed for 3D graphics.
  //
  // Format:
  //   m11, m12, m13, m14,
  //   m21, m22, m23, m24,
  //   m31, m32, m33, m34,
  //   m41, m42, m43, m44
  struct Matrix
  {
    using value_type = float;
    using size_type = std::size_t;

    static constexpr size_type NumElements = 4 * 4;

    constexpr size_type size() const
    {
      return NumElements;
    }

  private:
    // Anonymous structs are part of C11 but not currently allowed in C++
    // however most compilers to support them (GCC, Apple LLVM, Visual Studio).

    // union
    //{
    //  struct
    //  {
    //    float m11;
    //    float m12;
    //    float m13;
    //    float m14;
    //    float m21;
    //    float m22;
    //    float m23;

    //    float m24;
    //    float m31;
    //    float m32;
    //    float m33;
    //    float m34;
    //    float m41;
    //    float m42;
    //    float m43;
    //    float m44;
    //  };
    //  float m[4 * 4];
    //};
    float m[NumElements]{};

  public:
    //! @brief Creates a empty matrix (all components are set to zero)
    constexpr Matrix() = default;

    constexpr Matrix(const float m11, const float m12, const float m13, const float m14, const float m21, const float m22, const float m23,
                     const float m24, const float m31, const float m32, const float m33, const float m34, const float m41, const float m42,
                     const float m43, const float m44)
      : m{m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}
    {
    }

    ////! @brief A optimization constructor that doesn't initialize the matrix so when this is called the content of the matrix in undefined!!!
    //// coverity[uninit_member]
    // Matrix(const OptimizationFlag flag){};

    //! @brief Creates a new Matrix which contains sum of two matrices.
    //! @param matrix1 The first matrix to add.
    //! @param matrix2 The second matrix to add.
    //! @return The result of the matrix addition
    static Matrix Add(const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Creates a new Matrix which contains sum of two matrices.
    //! @param matrix1 The first matrix to add.
    //! @param matrix2 The second matrix to add.
    //! @param result The result of the matrix addition as an output parameter.
    static void Add(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult);

    //! @brief Direct access to the matrix array
    constexpr const float* DirectAccess() const
    {
      return m;
    }

    //! @brief Direct access to the matrix array
    float* DirectAccess()
    {
      return m;
    }

    //! @brief Get the backward vector of the Matrix.
    Vector3 GetBackward() const;

    //! @brief Set the backward vector of the Matrix.
    void SetBackward(const Vector3& value);

    //! @brief Get the down vector of the Matrix.
    Vector3 GetDown() const;

    //! @brief Set the Down vector of the Matrix.
    void SetDown(const Vector3& value);

    //! @brief Get the forward vector of the Matrix.
    Vector3 GetForward() const;

    //! @brief Set the forward vector of the Matrix.
    void SetForward(const Vector3& value);

    //! @brief Return a instance of the identity matrix
    static constexpr Matrix GetIdentity()
    {
      return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    //! @brief Get the left vector of the Matrix.
    Vector3 GetLeft() const;

    //! @brief Set the left vector of the Matrix.
    void SetLeft(const Vector3& value);

    //! @brief Get the right vector of the Matrix.
    Vector3 GetRight() const;

    //! @brief Set the right vector of the Matrix.
    void SetRight(const Vector3& value);

    //! @brief Get the translation part of the matrix
    Vector3 GetTranslation() const;

    //! @brief Set the translation part of the matrix
    void SetTranslation(const Vector3& value);

    //! @brief Get the up part of the matrix
    Vector3 GetUp() const;

    //! @brief Set the up part of the matrix
    void SetUp(const Vector3& value);

    //! @brief Fill the matrix with zero
    void Clear();

    //! @brief Creates a new Matrix for spherical billboarding that rotates around specified object position.
    //! @param objectPosition Position of billboard object. It will rotate around that vector.
    //! @param cameraPosition The camera position.
    //! @param cameraUpVector The camera up vector.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @return The Matrix for spherical billboarding
    static Matrix CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector,
                                  const Vector3* pCameraForwardVector);

    //! @brief Creates a new Matrix for spherical billboarding that rotates around specified object position.
    //! @param objectPosition Position of billboard object. It will rotate around that vector.
    //! @param cameraPosition The camera position.
    //! @param cameraUpVector The camera up vector.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param result The Matrix for spherical billboarding as an output parameter.
    static void CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector,
                                const Vector3* pCameraForwardVector, Matrix& rResult);

    //! @brief Creates a new Matrix for cylindrical billboarding that rotates around specified axis.
    //! @param objectPosition Object position the billboard will rotate around.
    //! @param cameraPosition Camera position.
    //! @param rotateAxis Axis of billboard for rotation.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param objectForwardVector Optional object forward vector.
    //! @return The Matrix for cylindrical billboarding
    static Matrix CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotateAxis,
                                             const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector);

    //! @brief Creates a new Matrix for cylindrical billboarding that rotates around specified axis.
    //! @param objectPosition Object position the billboard will rotate around.
    //! @param cameraPosition Camera position.
    //! @param rotateAxis Axis of billboard for rotation.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param objectForwardVector Optional object forward vector.
    //! @param result The Matrix for cylindrical billboarding as an output parameter.
    static void CreateConstrainedBillboard(const Vector3 objectPosition, const Vector3 cameraPosition, const Vector3 rotateAxis,
                                           const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector, Matrix& rResult);

    //! @brief Creates a new Matrix which contains the rotation moment around specified axis.
    //! @param The axis of rotation.
    //! @param The angle of rotation in radians.
    //! @return The rotation Matrix.
    static Matrix CreateFromAxisAngle(const Vector3& axis, const float angle);

    //! @brief Creates a new Matrix which contains the rotation moment around specified axis.
    //! @param The axis of rotation.
    //! @param The angle of rotation in radians.
    //! @param The rotation Matrix as a out parameter.
    FSL_ATTR_DEPRECATED static void CreateFromAxisAngle(Matrix& rResult, const Vector3& axis, const float angle)
    {
      CreateFromAxisAngle(axis, angle, rResult);
    }

    //! @brief Creates a new Matrix which contains the rotation moment around specified axis.
    //! @param The axis of rotation.
    //! @param The angle of rotation in radians.
    //! @param The rotation Matrix as a out parameter.
    static void CreateFromAxisAngle(const Vector3& axis, const float angle, Matrix& rResult);

    //! @brief Creates a new rotation Matrix from a Quaternion.
    //! @param quaternion Quaternion of rotation moment.
    //! @return The rotation Matrix
    static Matrix CreateFromQuaternion(const Quaternion& quaternion);

    //! @brief Creates a new rotation Matrix from a Quaternion.
    //! @param rRotation The rotation Matrix as an output parameter.
    //! @param quaternion Quaternion of rotation moment.
    FSL_ATTR_DEPRECATED static void CreateFromQuaternion(Matrix& rResult, const Quaternion& quaternion)
    {
      CreateFromQuaternion(quaternion, rResult);
    }

    //! @brief Creates a new rotation Matrix from a Quaternion.
    //! @param rRotation The rotation Matrix as an output parameter.
    //! @param quaternion Quaternion of rotation moment.
    static void CreateFromQuaternion(const Quaternion& quaternion, Matrix& rResult);

    //! @brief Creates a new rotation matrix from the specified yaw, pitch and roll values.
    //! @param yaw The yaw rotation value in radians.
    //! @param pitch The pitch rotation value in radians.
    //! @param roll The roll rotation value in radians.
    //! @return The rotation matrix
    //! @note For more information about yaw, pitch and roll visit http://en.wikipedia.org/wiki/Euler_angles.
    static Matrix CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll);

    //! @brief Creates a new rotation matrix from the specified yaw, pitch and roll values.
    //! @param rResult The rotation Matrix as an output parameter.
    //! @param yaw The yaw rotation value in radians.
    //! @param pitch The pitch rotation value in radians.
    //! @param roll The roll rotation value in radians.
    //! @return The rotation matrix
    //! Creates a new rotation Matrix from the specified yaw, pitch and roll values.
    FSL_ATTR_DEPRECATED static void CreateFromYawPitchRoll(Matrix& rResult, const float yaw, const float pitch, const float roll)
    {
      CreateFromYawPitchRoll(yaw, pitch, roll, rResult);
    }

    //! @brief Creates a new rotation matrix from the specified yaw, pitch and roll values.
    //! @param rResult The rotation Matrix as an output parameter.
    //! @param yaw The yaw rotation value in radians.
    //! @param pitch The pitch rotation value in radians.
    //! @param roll The roll rotation value in radians.
    //! @return The rotation matrix
    //! Creates a new rotation Matrix from the specified yaw, pitch and roll values.
    static void CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll, Matrix& rResult);

    //! @brief Creates a view matrix
    static Matrix CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector);

    //! @brief Creates a view matrix
    FSL_ATTR_DEPRECATED static void CreateLookAt(Matrix& rResult, const Vector3& cameraPosition, const Vector3& cameraTarget,
                                                 const Vector3& cameraUpVector)
    {
      CreateLookAt(cameraPosition, cameraTarget, cameraUpVector, rResult);
    }

    //! @brief Creates a view matrix
    static void CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector, Matrix& rResult);

    //! @brief Builds an orthogonal projection matrix.
    static Matrix CreateOrthographic(const float width, const float height, const float zNearPlane, const float zFarPlane);

    //! @brief Builds an orthogonal projection matrix.
    FSL_ATTR_DEPRECATED static void CreateOrthographic(Matrix& rResult, const float width, const float height, const float zNearPlane,
                                                       const float zFarPlane)
    {
      CreateOrthographic(width, height, zNearPlane, zFarPlane, rResult);
    }

    //! @brief Builds an orthogonal projection matrix.
    static void CreateOrthographic(const float width, const float height, const float zNearPlane, const float zFarPlane, Matrix& rResult);

    //! @brief Creates a new projection Matrix for customized orthographic view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param zNearPlane Depth of the near plane.
    //! @param zFarPlane Depth of the far plane.
    //! @return The new projection Matrix for customized orthographic view
    static Matrix CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane,
                                              const float zFarPlane);

    //! @brief Creates a new projection Matrix for customized orthographic view.
    //! @param viewingVolume The viewing volume.
    //! @param zNearPlane Depth of the near plane.
    //! @param zFarPlane Depth of the far plane.
    //! @return The new projection Matrix for customized orthographic view
    static Matrix CreateOrthographicOffCenter(const Rectangle& viewingVolume, const float zNearPlane, const float zFarPlane);

    //! @brief Creates a new projection Matrix for customized orthographic view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param zNearPlane Depth of the near plane.
    //! @param zFarPlane Depth of the far plane.
    //! @param result The new projection Matrix for customized orthographic view as an output parameter.
    static void CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane,
                                            const float zFarPlane, Matrix& rResult);

    //! @brief Builds a perspective projection matrix.
    static Matrix CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Builds a perspective projection matrix.
    FSL_ATTR_DEPRECATED static void CreatePerspective(Matrix& rResult, const float width, const float height, const float nearPlaneDistance,
                                                      const float farPlaneDistance)
    {
      CreatePerspective(width, height, nearPlaneDistance, farPlaneDistance, rResult);
    }

    //! @brief Builds a perspective projection matrix.
    static void CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance,
                                  Matrix& rResult);

    //! @brief Builds a perspective projection matrix based on a field of view and returns by value.
    //! @param fieldOfView Field of view in the y direction, in radians.
    //! @param aspectRatio Aspect ratio, defined as view space width divided by height.
    //! @param nearPlaneDistance Distance to the near view plane.
    //! @param farPlaneDistance Distance to the far view plane.
    //! @return The perspective projection matrix.
    //! @note   This produces a similar matrix to GLM::perspectiveRH with GLM_DEPTH_ZERO_TO_ONE
    static Matrix CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance,
                                               const float farPlaneDistance);

    //! @brief Builds a perspective projection matrix based on a field of view and returns by value.
    //! @param rResult The perspective projection matrix.
    //! @param fieldOfView Field of view in the y direction, in radians.
    //! @param aspectRatio Aspect ratio, defined as view space width divided by height.
    //! @param nearPlaneDistance Distance to the near view plane.
    //! @param farPlaneDistance Distance to the far view plane.
    //! @note   This produces a similar matrix to GLM::perspectiveRH with GLM_DEPTH_ZERO_TO_ONE
    FSL_ATTR_DEPRECATED static void CreatePerspectiveFieldOfView(Matrix& rResult, const float fieldOfView, const float aspectRatio,
                                                                 const float nearPlaneDistance, const float farPlaneDistance)
    {
      CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance, rResult);
    }

    //! @brief Builds a perspective projection matrix based on a field of view and returns by value.
    //! @param rResult The perspective projection matrix.
    //! @param fieldOfView Field of view in the y direction, in radians.
    //! @param aspectRatio Aspect ratio, defined as view space width divided by height.
    //! @param nearPlaneDistance Distance to the near view plane.
    //! @param farPlaneDistance Distance to the far view plane.
    //! @note   This produces a similar matrix to GLM::perspectiveRH with GLM_DEPTH_ZERO_TO_ONE
    static void CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance,
                                             const float farPlaneDistance, Matrix& rResult);

    //! @brief Creates a new projection Matrix for customized perspective view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param nearPlaneDistance Distance to the near plane.
    //! @param farPlaneDistance Distance to the far plane.
    //! @return The new Matrix for customized perspective view
    static Matrix CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance,
                                             const float farPlaneDistance);

    //! @brief Creates a new projection Matrix for customized perspective view.
    //! @param viewingVolume The viewing volume.
    //! @param nearPlaneDistance Distance to the near plane.
    //! @param farPlaneDistance Distance to the far plane.
    //! @return The new Matrix for customized perspective view
    static Matrix CreatePerspectiveOffCenter(const Rectangle& viewingVolume, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Creates a new projection Matrix for customized perspective view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param nearPlaneDistance Distance to the near plane.
    //! @param farPlaneDistance Distance to the far plane.
    //! @param result The new Matrix for customized perspective view as an output parameter.
    static void CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance,
                                           const float farPlaneDistance, Matrix& rResult);

    //! @brief Creates a new reflection Matrix.
    //! @param value The plane that used for reflection calculation.
    //! @return The reflection Matrix
    static Matrix CreateReflection(const Plane& value);

    //! @brief Creates a new reflection Matrix.
    //! @param value The plane that used for reflection calculation.
    //! @param result The reflection Matrix as an output parameter.
    static void CreateReflection(const Plane& value, Matrix& rResult);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the x-axis
    static Matrix CreateRotationX(const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the x-axis
    FSL_ATTR_DEPRECATED static void CreateRotationX(Matrix& rResult, const float radians)
    {
      CreateRotationX(radians, rResult);
    }

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the x-axis
    static void CreateRotationX(const float radians, Matrix& rResult);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the y-axis
    static Matrix CreateRotationY(const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the y-axis
    FSL_ATTR_DEPRECATED static void CreateRotationY(Matrix& rResult, const float radians)
    {
      CreateRotationY(radians, rResult);
    }

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the y-axis
    static void CreateRotationY(const float radians, Matrix& rResult);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the z-axis
    static Matrix CreateRotationZ(const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the z-axis
    FSL_ATTR_DEPRECATED static void CreateRotationZ(Matrix& rResult, const float radians)
    {
      CreateRotationZ(radians, rResult);
    }

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the z-axis
    static void CreateRotationZ(const float radians, Matrix& rResult);

    //! @brief Creates a scaling matrix
    static constexpr Matrix CreateScale(const float scale)
    {
      // -----------M11,   M12,  M13,  M14,  M21,  M22,   M23,  M24,  M31,  M32,  M33,   M34,  M41,  M42,  M43,  M44
      return Matrix(scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    //! @brief Creates a scaling matrix
    FSL_ATTR_DEPRECATED static void CreateScale(Matrix& rResult, const float scale)
    {
      CreateScale(scale, rResult);
    }

    //! @brief Creates a scaling matrix
    static void CreateScale(const float scale, Matrix& rResult);

    //! @brief Creates a scaling matrix
    static constexpr Matrix CreateScale(const float scaleX, const float scaleY, const float scaleZ)
    {
      // -----------M11,    M12,  M13,  M14,  M21,  M22,    M23,  M24,  M31,  M32,  M33,    M34,  M41,  M42,  M43,  M44
      return Matrix(scaleX, 0.0f, 0.0f, 0.0f, 0.0f, scaleY, 0.0f, 0.0f, 0.0f, 0.0f, scaleZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    //! @brief Creates a scaling matrix
    FSL_ATTR_DEPRECATED static void CreateScale(Matrix& rResult, const float scaleX, const float scaleY, const float scaleZ)
    {
      CreateScale(scaleX, scaleY, scaleZ, rResult);
    }

    //! @brief Creates a scaling matrix
    static void CreateScale(const float scaleX, const float scaleY, const float scaleZ, Matrix& rResult);

    //! @brief Creates a scaling matrix
    static constexpr Matrix CreateScale(const Vector3& scale)
    {
      // -----------M11,     M12,  M13,  M14,  M21,  M22,     M23,  M24,  M31,  M32,  M33,     M34,  M41,  M42,  M43,  M44
      return Matrix(scale.X, 0.0f, 0.0f, 0.0f, 0.0f, scale.Y, 0.0f, 0.0f, 0.0f, 0.0f, scale.Z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    //! @brief Creates a scaling matrix
    FSL_ATTR_DEPRECATED static void CreateScale(Matrix& rResult, const Vector3& scale)
    {
      CreateScale(scale, rResult);
    }

    //! @brief Creates a scaling matrix
    static void CreateScale(const Vector3& scale, Matrix& rResult);

    //! @brief Creates a Matrix that flattens geometry into a specified Plane as if casting a shadow from a specified light source.
    //! @param lightDirection A Vector3 specifying the direction from which the light that will cast the shadow is coming.
    //! @param plane The Plane onto which the new matrix should flatten geometry so as to cast a shadow.
    //! @return A new Matrix that can be used to flatten geometry onto the specified plane from the specified direction.
    static Matrix CreateShadow(const Vector3& lightDirection, const Plane& plane);

    //! @brief Fills in a Matrix to flatten geometry into a specified Plane as if casting a shadow from a specified light source.
    //! @param rResult A Matrix that can be used to flatten geometry onto the specified plane from the specified direction.
    //! @param lightDirection A Vector3 specifying the direction from which the light that will cast the shadow is coming.
    //! @param plane The Plane onto which the new matrix should flatten geometry so as to cast a shadow.
    static void CreateShadow(const Vector3& lightDirection, const Plane& plane, Matrix& rResult);

    //! @brief Creates a translation matrix
    //! @param x Value to translate by on the x-axis
    //! @param y Value to translate by on the y-axis
    //! @param z Value to translate by on the z-axis
    //! @return The created translation Matrix.
    static constexpr Matrix CreateTranslation(const float x, const float y, const float z)
    {
      // -----------M11,  M12,  M13,  M14,  M21,  M22,  M23,  M24,  M31,  M32,  M33, M34,   41,42,43,44
      return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, x, y, z, 1.0f);
    }

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param x Value to translate by on the x-axis
    //! @param y Value to translate by on the y-axis
    //! @param z Value to translate by on the z-axis
    FSL_ATTR_DEPRECATED static void CreateTranslation(Matrix& rResult, const float x, const float y, const float z)
    {
      CreateTranslation(x, y, z, rResult);
    }

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param x Value to translate by on the x-axis
    //! @param y Value to translate by on the y-axis
    //! @param z Value to translate by on the z-axis
    static void CreateTranslation(const float x, const float y, const float z, Matrix& rResult);

    //! @brief Creates a translation matrix
    //! @param value Amounts to translate by on the x, y, and z axes.
    //! @return The created translation Matrix.
    static constexpr Matrix CreateTranslation(const Vector3& value)
    {
      // -----------M11,  M12,  M13,  M14,  M21,  M22,  M23,  M24,  M31,  M32,  M33, M34,   M41,     M42,     M43,     M44
      return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, value.X, value.Y, value.Z, 1.0f);
    }

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param value Amounts to translate by on the x, y, and z axes.
    FSL_ATTR_DEPRECATED static void CreateTranslation(Matrix& rResult, const Vector3& value)
    {
      CreateTranslation(value, rResult);
    }

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param value Amounts to translate by on the x, y, and z axes.
    static void CreateTranslation(const Vector3& value, Matrix& rResult);

    //! @brief Creates a world matrix with the specified parameters.
    //! @param position Position of the object. This value is used in translation operations.
    //! @param forward Forward direction of the object.
    //! @param up Upward direction of the object. Usually [0, 1, 0]
    //! @return The created world matrix.
    static Matrix CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up);

    //! @brief Creates a world matrix with the specified parameters.
    //! @param rResult The created world matrix.
    //! @param position Position of the object. This value is used in translation operations.
    //! @param forward Forward direction of the object.
    //! @param up Upward direction of the object. Usually [0, 1, 0]
    FSL_ATTR_DEPRECATED static void CreateWorld(Matrix& rResult, const Vector3& position, const Vector3& forward, const Vector3& up)
    {
      CreateWorld(position, forward, up, rResult);
    }

    //! @brief Creates a world matrix with the specified parameters.
    //! @param rResult The created world matrix.
    //! @param position Position of the object. This value is used in translation operations.
    //! @param forward Forward direction of the object.
    //! @param up Upward direction of the object. Usually [0, 1, 0]
    static void CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up, Matrix& rResult);

    //! @brief Decomposes this matrix to translation, rotation and scale elements. Returns true if matrix can be decomposed; false otherwise.
    //! @param scale Scale vector as an output parameter.
    //! @param rotation Rotation quaternion as an output parameter.
    //! @param translation Translation vector as an output parameter.
    //! @return true if matrix can be decomposed; false otherwise
    bool Decompose(Vector3& rScale, Quaternion& rRotation, Vector3& rTranslation);

    //! @brief Calculates the determinant of the matrix.
    //! @return The determinant of the matrix.
    float Determinant() const;

    //! @brief Divides the components of a matrix by the corresponding components of another matrix.
    static constexpr Matrix Divide(const Matrix& matrix1, const Matrix& matrix2)
    {
      using namespace MatrixFields;
      return Matrix(
        matrix1.m[_M11] / matrix2.m[_M11], matrix1.m[_M12] / matrix2.m[_M12], matrix1.m[_M13] / matrix2.m[_M13], matrix1.m[_M14] / matrix2.m[_M14],
        matrix1.m[_M21] / matrix2.m[_M21], matrix1.m[_M22] / matrix2.m[_M22], matrix1.m[_M23] / matrix2.m[_M23], matrix1.m[_M24] / matrix2.m[_M24],
        matrix1.m[_M31] / matrix2.m[_M31], matrix1.m[_M32] / matrix2.m[_M32], matrix1.m[_M33] / matrix2.m[_M33], matrix1.m[_M34] / matrix2.m[_M34],
        matrix1.m[_M41] / matrix2.m[_M41], matrix1.m[_M42] / matrix2.m[_M42], matrix1.m[_M43] / matrix2.m[_M43], matrix1.m[_M44] / matrix2.m[_M44]);
    }

    //! @brief Divides the components of a matrix by the corresponding components of another matrix.
    FSL_ATTR_DEPRECATED static void Divide(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
    {
      Divide(matrix1, matrix2, rResult);
    }

    //! @brief Divides the components of a matrix by the corresponding components of another matrix.
    static void Divide(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult)
    {
      using namespace MatrixFields;
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

    //! @brief Divides the components of a matrix by a scalar.
    static Matrix Divide(const Matrix& matrix1, const float divider)
    {
      using namespace MatrixFields;
      // Multiply usually has better performance
      const float num = 1.0f / divider;

      return Matrix(matrix1.m[_M11] * num, matrix1.m[_M12] * num, matrix1.m[_M13] * num, matrix1.m[_M14] * num, matrix1.m[_M21] * num,
                    matrix1.m[_M22] * num, matrix1.m[_M23] * num, matrix1.m[_M24] * num, matrix1.m[_M31] * num, matrix1.m[_M32] * num,
                    matrix1.m[_M33] * num, matrix1.m[_M34] * num, matrix1.m[_M41] * num, matrix1.m[_M42] * num, matrix1.m[_M43] * num,
                    matrix1.m[_M44] * num);
    }

    //! @brief Divides the components of a matrix by a scalar.
    FSL_ATTR_DEPRECATED static void Divide(Matrix& rResult, const Matrix& matrix1, const float divider)
    {
      Divide(matrix1, divider, rResult);
    }

    //! @brief Divides the components of a matrix by a scalar.
    static void Divide(const Matrix& matrix1, const float divider, Matrix& rResult)
    {
      using namespace MatrixFields;
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

    //! @brief Creates a new Matrix which contains inversion of the specified matrix.
    //! @param matrix Source Matrix.
    //! @return The inverted matrix.
    static Matrix Invert(const Matrix& matrix);

    //! @brief Creates a new Matrix which contains inversion of the specified matrix.
    //! @param rResult The inverted matrix as output parameter.
    //! @param matrix Source Matrix.
    FSL_ATTR_DEPRECATED static void Invert(Matrix& rResult, const Matrix& matrix)
    {
      Invert(matrix, rResult);
    }

    //! @brief Creates a new Matrix which contains inversion of the specified matrix.
    //! @param rResult The inverted matrix as output parameter.
    //! @param matrix Source Matrix.
    static void Invert(const Matrix& matrix, Matrix& rResult);

    //! @brief Linearly interpolates between the corresponding values of two matrices.
    //! @param value1 source matrix1
    //! @param value2 source matrix2
    //! @param amount interpolation value (0 means full value1, 1 = full value2)
    //! @return Resulting matrix.
    static Matrix Lerp(const Matrix& value1, const Matrix& value2, const float amount);

    //! @brief Linearly interpolates between the corresponding values of two matrices.
    //! @param rResult Resulting matrix.
    //! @param value1 source matrix1
    //! @param value2 source matrix2
    //! @param amount interpolation value (0 means full value1, 1 = full value2)
    FSL_ATTR_DEPRECATED static void Lerp(Matrix& rResult, const Matrix& value1, const Matrix& value2, const float amount)
    {
      Lerp(value1, value2, amount, rResult);
    }

    //! @brief Linearly interpolates between the corresponding values of two matrices.
    //! @param rResult Resulting matrix.
    //! @param value1 source matrix1
    //! @param value2 source matrix2
    //! @param amount interpolation value (0 means full value1, 1 = full value2)
    static void Lerp(const Matrix& value1, const Matrix& value2, const float amount, Matrix& rResult);

    //! @brief Multiplies a matrix by another matrix.
    static constexpr Matrix Multiply(const Matrix& matrix1, const Matrix& matrix2)
    {
      using namespace MatrixFields;
      return Matrix((((matrix1.m[_M11] * matrix2.m[_M11]) + (matrix1.m[_M12] * matrix2.m[_M21])) + (matrix1.m[_M13] * matrix2.m[_M31])) +
                      (matrix1.m[_M14] * matrix2.m[_M41]),
                    (((matrix1.m[_M11] * matrix2.m[_M12]) + (matrix1.m[_M12] * matrix2.m[_M22])) + (matrix1.m[_M13] * matrix2.m[_M32])) +
                      (matrix1.m[_M14] * matrix2.m[_M42]),
                    (((matrix1.m[_M11] * matrix2.m[_M13]) + (matrix1.m[_M12] * matrix2.m[_M23])) + (matrix1.m[_M13] * matrix2.m[_M33])) +
                      (matrix1.m[_M14] * matrix2.m[_M43]),
                    (((matrix1.m[_M11] * matrix2.m[_M14]) + (matrix1.m[_M12] * matrix2.m[_M24])) + (matrix1.m[_M13] * matrix2.m[_M34])) +
                      (matrix1.m[_M14] * matrix2.m[_M44]),
                    (((matrix1.m[_M21] * matrix2.m[_M11]) + (matrix1.m[_M22] * matrix2.m[_M21])) + (matrix1.m[_M23] * matrix2.m[_M31])) +
                      (matrix1.m[_M24] * matrix2.m[_M41]),
                    (((matrix1.m[_M21] * matrix2.m[_M12]) + (matrix1.m[_M22] * matrix2.m[_M22])) + (matrix1.m[_M23] * matrix2.m[_M32])) +
                      (matrix1.m[_M24] * matrix2.m[_M42]),
                    (((matrix1.m[_M21] * matrix2.m[_M13]) + (matrix1.m[_M22] * matrix2.m[_M23])) + (matrix1.m[_M23] * matrix2.m[_M33])) +
                      (matrix1.m[_M24] * matrix2.m[_M43]),
                    (((matrix1.m[_M21] * matrix2.m[_M14]) + (matrix1.m[_M22] * matrix2.m[_M24])) + (matrix1.m[_M23] * matrix2.m[_M34])) +
                      (matrix1.m[_M24] * matrix2.m[_M44]),
                    (((matrix1.m[_M31] * matrix2.m[_M11]) + (matrix1.m[_M32] * matrix2.m[_M21])) + (matrix1.m[_M33] * matrix2.m[_M31])) +
                      (matrix1.m[_M34] * matrix2.m[_M41]),
                    (((matrix1.m[_M31] * matrix2.m[_M12]) + (matrix1.m[_M32] * matrix2.m[_M22])) + (matrix1.m[_M33] * matrix2.m[_M32])) +
                      (matrix1.m[_M34] * matrix2.m[_M42]),
                    (((matrix1.m[_M31] * matrix2.m[_M13]) + (matrix1.m[_M32] * matrix2.m[_M23])) + (matrix1.m[_M33] * matrix2.m[_M33])) +
                      (matrix1.m[_M34] * matrix2.m[_M43]),
                    (((matrix1.m[_M31] * matrix2.m[_M14]) + (matrix1.m[_M32] * matrix2.m[_M24])) + (matrix1.m[_M33] * matrix2.m[_M34])) +
                      (matrix1.m[_M34] * matrix2.m[_M44]),
                    (((matrix1.m[_M41] * matrix2.m[_M11]) + (matrix1.m[_M42] * matrix2.m[_M21])) + (matrix1.m[_M43] * matrix2.m[_M31])) +
                      (matrix1.m[_M44] * matrix2.m[_M41]),
                    (((matrix1.m[_M41] * matrix2.m[_M12]) + (matrix1.m[_M42] * matrix2.m[_M22])) + (matrix1.m[_M43] * matrix2.m[_M32])) +
                      (matrix1.m[_M44] * matrix2.m[_M42]),
                    (((matrix1.m[_M41] * matrix2.m[_M13]) + (matrix1.m[_M42] * matrix2.m[_M23])) + (matrix1.m[_M43] * matrix2.m[_M33])) +
                      (matrix1.m[_M44] * matrix2.m[_M43]),
                    (((matrix1.m[_M41] * matrix2.m[_M14]) + (matrix1.m[_M42] * matrix2.m[_M24])) + (matrix1.m[_M43] * matrix2.m[_M34])) +
                      (matrix1.m[_M44] * matrix2.m[_M44]));
    }

    //! @brief Multiplies a matrix by another matrix.
    FSL_ATTR_DEPRECATED static void Multiply(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
    {
      Multiply(matrix1, matrix2, rResult);
    }

    //! @brief Multiplies a matrix by another matrix.
    static void Multiply(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult)
    {
      using namespace MatrixFields;
      const float* pMatrix1 = matrix1.m;
      const float* pMatrix2 = matrix2.m;

      const float m11 = (((pMatrix1[_M11] * pMatrix2[_M11]) + (pMatrix1[_M12] * pMatrix2[_M21])) + (pMatrix1[_M13] * pMatrix2[_M31])) +
                        (pMatrix1[_M14] * pMatrix2[_M41]);
      const float m12 = (((pMatrix1[_M11] * pMatrix2[_M12]) + (pMatrix1[_M12] * pMatrix2[_M22])) + (pMatrix1[_M13] * pMatrix2[_M32])) +
                        (pMatrix1[_M14] * pMatrix2[_M42]);
      const float m13 = (((pMatrix1[_M11] * pMatrix2[_M13]) + (pMatrix1[_M12] * pMatrix2[_M23])) + (pMatrix1[_M13] * pMatrix2[_M33])) +
                        (pMatrix1[_M14] * pMatrix2[_M43]);
      const float m14 = (((pMatrix1[_M11] * pMatrix2[_M14]) + (pMatrix1[_M12] * pMatrix2[_M24])) + (pMatrix1[_M13] * pMatrix2[_M34])) +
                        (pMatrix1[_M14] * pMatrix2[_M44]);
      const float m21 = (((pMatrix1[_M21] * pMatrix2[_M11]) + (pMatrix1[_M22] * pMatrix2[_M21])) + (pMatrix1[_M23] * pMatrix2[_M31])) +
                        (pMatrix1[_M24] * pMatrix2[_M41]);
      const float m22 = (((pMatrix1[_M21] * pMatrix2[_M12]) + (pMatrix1[_M22] * pMatrix2[_M22])) + (pMatrix1[_M23] * pMatrix2[_M32])) +
                        (pMatrix1[_M24] * pMatrix2[_M42]);
      const float m23 = (((pMatrix1[_M21] * pMatrix2[_M13]) + (pMatrix1[_M22] * pMatrix2[_M23])) + (pMatrix1[_M23] * pMatrix2[_M33])) +
                        (pMatrix1[_M24] * pMatrix2[_M43]);
      const float m24 = (((pMatrix1[_M21] * pMatrix2[_M14]) + (pMatrix1[_M22] * pMatrix2[_M24])) + (pMatrix1[_M23] * pMatrix2[_M34])) +
                        (pMatrix1[_M24] * pMatrix2[_M44]);
      const float m31 = (((pMatrix1[_M31] * pMatrix2[_M11]) + (pMatrix1[_M32] * pMatrix2[_M21])) + (pMatrix1[_M33] * pMatrix2[_M31])) +
                        (pMatrix1[_M34] * pMatrix2[_M41]);
      const float m32 = (((pMatrix1[_M31] * pMatrix2[_M12]) + (pMatrix1[_M32] * pMatrix2[_M22])) + (pMatrix1[_M33] * pMatrix2[_M32])) +
                        (pMatrix1[_M34] * pMatrix2[_M42]);
      const float m33 = (((pMatrix1[_M31] * pMatrix2[_M13]) + (pMatrix1[_M32] * pMatrix2[_M23])) + (pMatrix1[_M33] * pMatrix2[_M33])) +
                        (pMatrix1[_M34] * pMatrix2[_M43]);
      const float m34 = (((pMatrix1[_M31] * pMatrix2[_M14]) + (pMatrix1[_M32] * pMatrix2[_M24])) + (pMatrix1[_M33] * pMatrix2[_M34])) +
                        (pMatrix1[_M34] * pMatrix2[_M44]);
      const float m41 = (((pMatrix1[_M41] * pMatrix2[_M11]) + (pMatrix1[_M42] * pMatrix2[_M21])) + (pMatrix1[_M43] * pMatrix2[_M31])) +
                        (pMatrix1[_M44] * pMatrix2[_M41]);
      const float m42 = (((pMatrix1[_M41] * pMatrix2[_M12]) + (pMatrix1[_M42] * pMatrix2[_M22])) + (pMatrix1[_M43] * pMatrix2[_M32])) +
                        (pMatrix1[_M44] * pMatrix2[_M42]);
      const float m43 = (((pMatrix1[_M41] * pMatrix2[_M13]) + (pMatrix1[_M42] * pMatrix2[_M23])) + (pMatrix1[_M43] * pMatrix2[_M33])) +
                        (pMatrix1[_M44] * pMatrix2[_M43]);
      const float m44 = (((pMatrix1[_M41] * pMatrix2[_M14]) + (pMatrix1[_M42] * pMatrix2[_M24])) + (pMatrix1[_M43] * pMatrix2[_M34])) +
                        (pMatrix1[_M44] * pMatrix2[_M44]);

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

    //! @brief Multiplies a matrix by a scalar.
    static constexpr Matrix Multiply(const Matrix& matrix1, const float factor)
    {
      using namespace MatrixFields;
      return Matrix(matrix1.m[_M11] * factor, matrix1.m[_M12] * factor, matrix1.m[_M13] * factor, matrix1.m[_M14] * factor, matrix1.m[_M21] * factor,
                    matrix1.m[_M22] * factor, matrix1.m[_M23] * factor, matrix1.m[_M24] * factor, matrix1.m[_M31] * factor, matrix1.m[_M32] * factor,
                    matrix1.m[_M33] * factor, matrix1.m[_M34] * factor, matrix1.m[_M41] * factor, matrix1.m[_M42] * factor, matrix1.m[_M43] * factor,
                    matrix1.m[_M44] * factor);
    }

    //! @brief Multiplies a matrix by a scalar.
    FSL_ATTR_DEPRECATED static void Multiply(Matrix& rResult, const Matrix& matrix1, const float factor)
    {
      Multiply(matrix1, factor, rResult);
    }

    //! @brief Multiplies a matrix by a scalar.
    static void Multiply(const Matrix& matrix1, const float factor, Matrix& rResult)
    {
      using namespace MatrixFields;
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

    //! @brief Negate the matrix
    static constexpr Matrix Negate(const Matrix& matrix)
    {
      using namespace MatrixFields;
      return Matrix(-matrix.m[_M11], -matrix.m[_M12], -matrix.m[_M13], -matrix.m[_M14], -matrix.m[_M21], -matrix.m[_M22], -matrix.m[_M23],
                    -matrix.m[_M24], -matrix.m[_M31], -matrix.m[_M32], -matrix.m[_M33], -matrix.m[_M34], -matrix.m[_M41], -matrix.m[_M42],
                    -matrix.m[_M43], -matrix.m[_M44]);
    }

    //! @brief Negate the matrix
    FSL_ATTR_DEPRECATED static void Negate(Matrix& rResult, const Matrix& matrix)
    {
      Negate(matrix, rResult);
    }

    //! @brief Negate the matrix
    static void Negate(const Matrix& matrix, Matrix& rResult);

    //! @brief Set the matrix to the identity matrix
    void SetIdentity();

    //! @brief Subtracts matrices.
    static constexpr Matrix Subtract(const Matrix& matrix1, const Matrix& matrix2)
    {
      using namespace MatrixFields;
      return Matrix(
        matrix1.m[_M11] - matrix2.m[_M11], matrix1.m[_M12] - matrix2.m[_M12], matrix1.m[_M13] - matrix2.m[_M13], matrix1.m[_M14] - matrix2.m[_M14],
        matrix1.m[_M21] - matrix2.m[_M21], matrix1.m[_M22] - matrix2.m[_M22], matrix1.m[_M23] - matrix2.m[_M23], matrix1.m[_M24] - matrix2.m[_M24],
        matrix1.m[_M31] - matrix2.m[_M31], matrix1.m[_M32] - matrix2.m[_M32], matrix1.m[_M33] - matrix2.m[_M33], matrix1.m[_M34] - matrix2.m[_M34],
        matrix1.m[_M41] - matrix2.m[_M41], matrix1.m[_M42] - matrix2.m[_M42], matrix1.m[_M43] - matrix2.m[_M43], matrix1.m[_M44] - matrix2.m[_M44]);
    }

    //! @brief Subtracts matrices.
    FSL_ATTR_DEPRECATED static void Subtract(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2)
    {
      Subtract(matrix1, matrix2, rResult);
    }

    //! @brief Subtracts matrices.
    static void Subtract(const Matrix& matrix1, const Matrix& matrix2, Matrix& rResult);

    //! @brief Transposes the rows and columns of a matrix.
    static constexpr Matrix Transpose(const Matrix& matrix)
    {
      using namespace MatrixFields;
      return Matrix(matrix.m[_M11], matrix.m[_M21], matrix.m[_M31], matrix.m[_M41], matrix.m[_M12], matrix.m[_M22], matrix.m[_M32], matrix.m[_M42],
                    matrix.m[_M13], matrix.m[_M23], matrix.m[_M33], matrix.m[_M43], matrix.m[_M14], matrix.m[_M24], matrix.m[_M34], matrix.m[_M44]);
    }

    //! @brief  Transposes the rows and columns of a matrix.
    //! @note matrix can be equal to rResult.
    FSL_ATTR_DEPRECATED static void Transpose(Matrix& rResult, const Matrix& src)
    {
      Transpose(src, rResult);
    }

    //! @brief  Transposes the rows and columns of a matrix.
    //! @note matrix can be equal to rResult.
    static void Transpose(const Matrix& src, Matrix& rResult);

    // Negate the matrix
    Matrix operator-() const
    {
      return Negate(*this);
    }

    Matrix& operator*=(const Matrix& rhs)
    {
      using namespace MatrixFields;
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

    Matrix& operator+=(const Matrix& rhs)
    {
      Add(*this, rhs, *this);
      return *this;
    }

    Matrix& operator-=(const Matrix& rhs)
    {
      // dumb cast due to deprecated static overloads
      Subtract(static_cast<const Matrix>(*this), rhs, *this);
      return *this;
    }

    constexpr bool operator==(const Matrix& rhs) const
    {
      using namespace MatrixFields;
      return (m[_M11] == rhs.m[_M11] && m[_M12] == rhs.m[_M12] && m[_M13] == rhs.m[_M13] && m[_M14] == rhs.m[_M14] && m[_M21] == rhs.m[_M21] &&
              m[_M22] == rhs.m[_M22] && m[_M23] == rhs.m[_M23] && m[_M24] == rhs.m[_M24] && m[_M31] == rhs.m[_M31] && m[_M32] == rhs.m[_M32] &&
              m[_M33] == rhs.m[_M33] && m[_M34] == rhs.m[_M34] && m[_M41] == rhs.m[_M41] && m[_M42] == rhs.m[_M42] && m[_M43] == rhs.m[_M43] &&
              m[_M44] == rhs.m[_M44]);
    }

    constexpr bool operator!=(const Matrix& rhs) const
    {
      using namespace MatrixFields;
      return (m[_M11] != rhs.m[_M11] || m[_M12] != rhs.m[_M12] || m[_M13] != rhs.m[_M13] || m[_M14] != rhs.m[_M14] || m[_M21] != rhs.m[_M21] ||
              m[_M22] != rhs.m[_M22] || m[_M23] != rhs.m[_M23] || m[_M24] != rhs.m[_M24] || m[_M31] != rhs.m[_M31] || m[_M32] != rhs.m[_M32] ||
              m[_M33] != rhs.m[_M33] || m[_M34] != rhs.m[_M34] || m[_M41] != rhs.m[_M41] || m[_M42] != rhs.m[_M42] || m[_M43] != rhs.m[_M43] ||
              m[_M44] != rhs.m[_M44]);
    }
  };
}

//! @brief Matrix multiply
inline const Fsl::Matrix operator*(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  using namespace Fsl::MatrixFields;

  const float* pLhs = lhs.DirectAccess();
  const float* pRhs = rhs.DirectAccess();
  return Fsl::Matrix((((pLhs[_M11] * pRhs[_M11]) + (pLhs[_M12] * pRhs[_M21])) + (pLhs[_M13] * pRhs[_M31])) + (pLhs[_M14] * pRhs[_M41]),
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

//! @brief Matrix addition
inline const Fsl::Matrix operator+(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  return Fsl::Matrix::Add(lhs, rhs);
}

//! @brief Matrix subtraction
inline const Fsl::Matrix operator-(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs)
{
  return Fsl::Matrix::Subtract(lhs, rhs);
}


#endif
