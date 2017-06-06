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

#include <algorithm>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Math/Vector3.hpp>

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
  private:
    // Anonymous structs are part of C11 but not currently allowed in C++
    // however most compilers to support them (GCC, Apple LLVM, Visual Studio).

    //union
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
    float m[4 * 4];
  public:
    //! @brief Creates a empty matrix (all components are set to zero)
    Matrix();
    Matrix(const float m11, const float m12, const float m13, const float m14,
      const float m21, const float m22, const float m23, const float m24,
      const float m31, const float m32, const float m33, const float m34,
      const float m41, const float m42, const float m43, const float m44);

    //! @brief A optimization constructor that doesn't initialize the matrix so when this is called the content of the matrix in undefined!!!
    Matrix(const OptimizationFlag flag) {};

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
    const float* DirectAccess() const { return m; }

    //! @brief Direct access to the matrix array
    float* DirectAccess() { return m; }

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
    static Matrix GetIdentity()
    {
      return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
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
    static Matrix CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector, const Vector3* pCameraForwardVector);

    //! @brief Creates a new Matrix for spherical billboarding that rotates around specified object position.
    //! @param objectPosition Position of billboard object. It will rotate around that vector.
    //! @param cameraPosition The camera position.
    //! @param cameraUpVector The camera up vector.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param result The Matrix for spherical billboarding as an output parameter.
    static void CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& cameraUpVector, const Vector3* pCameraForwardVector, Matrix& rResult);

    //! @brief Creates a new Matrix for cylindrical billboarding that rotates around specified axis.
    //! @param objectPosition Object position the billboard will rotate around.
    //! @param cameraPosition Camera position.
    //! @param rotateAxis Axis of billboard for rotation.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param objectForwardVector Optional object forward vector.
    //! @return The Matrix for cylindrical billboarding
    static Matrix CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotateAxis, const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector);

    //! @brief Creates a new Matrix for cylindrical billboarding that rotates around specified axis.
    //! @param objectPosition Object position the billboard will rotate around.
    //! @param cameraPosition Camera position.
    //! @param rotateAxis Axis of billboard for rotation.
    //! @param cameraForwardVector Optional camera forward vector.
    //! @param objectForwardVector Optional object forward vector.
    //! @param result The Matrix for cylindrical billboarding as an output parameter.
    static void CreateConstrainedBillboard(const Vector3 objectPosition, const Vector3 cameraPosition, const Vector3 rotateAxis, const Vector3* pCameraForwardVector, const Vector3* pObjectForwardVector, Matrix& rResult);

    //! @brief Creates a new Matrix which contains the rotation moment around specified axis.
    //! @param The axis of rotation.
    //! @param The angle of rotation in radians.
    //! @return The rotation Matrix.
    static Matrix CreateFromAxisAngle(const Vector3& axis, const float angle);

    //! @brief Creates a new Matrix which contains the rotation moment around specified axis.
    //! @param The axis of rotation.
    //! @param The angle of rotation in radians.
    //! @param The rotation Matrix as a out parameter.
    static void CreateFromAxisAngle(Matrix& rResult, const Vector3& axis, const float angle);

    //! @brief Creates a new rotation Matrix from a Quaternion.
    //! @param quaternion Quaternion of rotation moment.
    //! @return The rotation Matrix
    static Matrix CreateFromQuaternion(const Quaternion& quaternion);

    //! @brief Creates a new rotation Matrix from a Quaternion.
    //! @param rRotation The rotation Matrix as an output parameter.
    //! @param quaternion Quaternion of rotation moment.
    static void CreateFromQuaternion(Matrix& rResult, const Quaternion& quaternion);

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
    static void CreateFromYawPitchRoll(Matrix& rResult, const float yaw, const float pitch, const float roll);

    //! @brief Creates a view matrix
    static Matrix CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector);

    //! @brief Creates a view matrix
    static void CreateLookAt(Matrix& rResult, const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector);

    //! @brief Builds an orthogonal projection matrix.
    static Matrix CreateOrthographic(const float width, const float height, const float zNearPlane, const float zFarPlane);

    //! @brief Builds an orthogonal projection matrix.
    static void CreateOrthographic(Matrix& rResult, const float width, const float height, const float zNearPlane, const float zFarPlane);

    //! @brief Creates a new projection Matrix for customized orthographic view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param zNearPlane Depth of the near plane.
    //! @param zFarPlane Depth of the far plane.
    //! @return The new projection Matrix for customized orthographic view
    static Matrix CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane, const float zFarPlane);

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
    static void CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float zNearPlane, const float zFarPlane, Matrix& rResult);

    //! @brief Builds a perspective projection matrix.
    static Matrix CreatePerspective(const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Builds a perspective projection matrix.
    static void CreatePerspective(Matrix& rResult, const float width, const float height, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Builds a perspective projection matrix based on a field of view and returns by value.
    //! @param fieldOfView Field of view in the y direction, in radians.
    //! @param aspectRatio Aspect ratio, defined as view space width divided by height.
    //! @param nearPlaneDistance Distance to the near view plane.
    //! @param farPlaneDistance Distance to the far view plane.
    //! @return The perspective projection matrix.
    static Matrix CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Builds a perspective projection matrix based on a field of view and returns by value.
    //! @param rResult The perspective projection matrix.
    //! @param fieldOfView Field of view in the y direction, in radians.
    //! @param aspectRatio Aspect ratio, defined as view space width divided by height.
    //! @param nearPlaneDistance Distance to the near view plane.
    //! @param farPlaneDistance Distance to the far view plane.
    static void CreatePerspectiveFieldOfView(Matrix& rResult, const float fieldOfView, const float aspectRatio, const float nearPlaneDistance, const float farPlaneDistance);

    //! @brief Creates a new projection Matrix for customized perspective view.
    //! @param left Lower x-value at the near plane.
    //! @param right Upper x-value at the near plane.
    //! @param bottom Lower y-coordinate at the near plane.
    //! @param top Upper y-value at the near plane.
    //! @param nearPlaneDistance Distance to the near plane.
    //! @param farPlaneDistance Distance to the far plane.
    //! @return The new Matrix for customized perspective view
    static Matrix CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance, const float farPlaneDistance);

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
    static void CreatePerspectiveOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlaneDistance, const float farPlaneDistance, Matrix& rResult);

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
    static void CreateRotationX(Matrix& rResult, const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the y-axis
    static Matrix CreateRotationY(const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the y-axis
    static void CreateRotationY(Matrix& rResult, const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the z-axis
    static Matrix CreateRotationZ(const float radians);

    //! @brief Creates a matrix that can be used to rotate a set of vertices around the z-axis
    static void CreateRotationZ(Matrix& rResult, const float radians);

    //! @brief Creates a scaling matrix
    static Matrix CreateScale(const float scale);

    //! @brief Creates a scaling matrix
    static void CreateScale(Matrix& rResult, const float scale);

    //! @brief Creates a scaling matrix
    static Matrix CreateScale(const float scaleX, const float scaleY, const float scaleZ);

    //! @brief Creates a scaling matrix
    static void CreateScale(Matrix& rResult, const float scaleX, const float scaleY, const float scaleZ);

    //! @brief Creates a scaling matrix
    static Matrix CreateScale(const Vector3& scale);

    //! @brief Creates a scaling matrix
    static void CreateScale(Matrix& rResult, const Vector3& scale);

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
    static Matrix CreateTranslation(const float x, const float y, const float z);

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param x Value to translate by on the x-axis
    //! @param y Value to translate by on the y-axis
    //! @param z Value to translate by on the z-axis
    static void CreateTranslation(Matrix& rResult, const float x, const float y, const float z);

    //! @brief Creates a translation matrix
    //! @param value Amounts to translate by on the x, y, and z axes.
    //! @return The created translation Matrix.
    static Matrix CreateTranslation(const Vector3& value);

    //! @brief Creates a translation matrix
    //! @param rResult The created translation Matrix.
    //! @param value Amounts to translate by on the x, y, and z axes.
    static void CreateTranslation(Matrix& rResult, const Vector3& value);

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
    static void CreateWorld(Matrix& rResult, const Vector3& position, const Vector3& forward, const Vector3& up);

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
    static Matrix Divide(const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Divides the components of a matrix by the corresponding components of another matrix.
    static void Divide(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Divides the components of a matrix by a scalar.
    static Matrix Divide(const Matrix& matrix1, const float divider);

    //! @brief Divides the components of a matrix by a scalar.
    static void Divide(Matrix& rResult, const Matrix& matrix1, const float divider);

    //! @brief Creates a new Matrix which contains inversion of the specified matrix.
    //! @param matrix Source Matrix.
    //! @return The inverted matrix.
    static Matrix Invert(const Matrix& matrix);

    //! @brief Creates a new Matrix which contains inversion of the specified matrix.
    //! @param rResult The inverted matrix as output parameter.
    //! @param matrix Source Matrix.
    static void Invert(Matrix& rResult, const Matrix& matrix);

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
    static void Lerp(Matrix& rResult, const Matrix& value1, const Matrix& value2, const float amount);

    //! @brief Multiplies a matrix by another matrix.
    static Matrix Multiply(const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Multiplies a matrix by another matrix.
    static void Multiply(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Multiplies a matrix by a scalar.
    static Matrix Multiply(const Matrix& matrix1, const float factor);

    //! @brief Multiplies a matrix by a scalar.
    static void Multiply(Matrix& rResult, const Matrix& matrix1, const float factor);

    //! @brief Negate the matrix
    Matrix Negate(const Matrix& matrix) const;

    //! @brief Negate the matrix
    void Negate(Matrix& rResult, const Matrix& matrix);

    //! @brief Set the matrix to the identity matrix
    void SetIdentity();

    //! @brief Subtracts matrices.
    static Matrix Subtract(const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Subtracts matrices.
    static void Subtract(Matrix& rResult, const Matrix& matrix1, const Matrix& matrix2);

    //! @brief Transposes the rows and columns of a matrix.
    static Matrix Transpose(const Matrix& matrix);

    //! @brief  Transposes the rows and columns of a matrix.
    //! @note matrix can be equal to rResult.
    static void Transpose(Matrix& rResult, const Matrix& matrix);

    // Negate the matrix
    Matrix operator-() const;

    Matrix& operator*=(const Matrix& rhs);
    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);

    bool operator==(const Matrix &rhs) const;
    bool operator!=(const Matrix &rhs) const;
  };
}

//! @brief Matrix multiply
const Fsl::Matrix operator*(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs);

//! @brief Matrix addition
const Fsl::Matrix operator+(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs);

//! @brief Matrix subtraction
const Fsl::Matrix operator-(const Fsl::Matrix& lhs, const Fsl::Matrix& rhs);


#endif
