#ifndef FSLBASE_MATH_MATRIX3STD140_HPP
#define FSLBASE_MATH_MATRIX3STD140_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Math/Matrix3Std140Fields.hpp>
#include <cstddef>

namespace Fsl
{
  struct Matrix;

  // Designed for 3D graphics and to be comptaible with the opengl and vulkan std140 memory layout.
  // Format:
  //   m11, m12, m13, padding,
  //   m21, m22, m23, padding,
  //   m31, m32, m33, padding
  struct Matrix3Std140
  {
    using value_type = float;
    using size_type = std::size_t;

    static constexpr size_type NumElements = 3 * 4;

    constexpr size_type size() const    // NOLINT(readability-convert-member-functions-to-static,readability-identifier-naming)
    {
      return NumElements;
    }

  private:
    // const int _M11 = ((4 * 0) + 0);
    // const int _M12 = ((4 * 0) + 1);
    // const int _M13 = ((4 * 0) + 2);
    // const int _M21 = ((4 * 1) + 0);
    // const int _M22 = ((4 * 1) + 1);
    // const int _M23 = ((4 * 1) + 2);
    // const int _M31 = ((4 * 2) + 0);
    // const int _M32 = ((4 * 2) + 1);
    // const int _M33 = ((4 * 2) + 2);
    float m[NumElements]{};    // NOLINT(modernize-avoid-c-arrays,readability-identifier-naming)

  public:
    //! @brief Creates a empty matrix (all components are set to zero)
    constexpr Matrix3Std140() = default;
    constexpr Matrix3Std140(const float m11, const float m12, const float m13, const float m21, const float m22, const float m23, const float m31,
                            const float m32, const float m33)
      : m{m11, m12, m13, 0, m21, m22, m23, 0, m31, m32, m33, 0}
    {
    }

    //! @brief A optimization constructor that doesn't initialize the matrix so when this is called the content of the matrix in undefined!!!
    // coverity[uninit_member]
    // Matrix3Std140(const OptimizationFlag flag){};

    //! @brief Direct access to the matrix array
    const float* DirectAccess() const
    {
      return m;
    }

    //! @brief Direct access to the matrix array
    float* DirectAccess()
    {
      return m;
    }

    //! @brief Return a instance of the identity matrix
    static constexpr Matrix3Std140 GetIdentity()
    {
      return {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    }

    //! @brief Fill the matrix with zero
    constexpr void Clear()
    {
      using namespace Matrix3Std140Fields;
      m[_M11] = 0.0f;
      m[_M12] = 0.0f;
      m[_M13] = 0.0f;
      m[_M21] = 0.0f;
      m[_M22] = 0.0f;
      m[_M23] = 0.0f;
      m[_M31] = 0.0f;
      m[_M32] = 0.0f;
      m[_M33] = 0.0f;
    }

    //! @brief Set the matrix to the identity matrix
    constexpr void SetIdentity()
    {
      using namespace Matrix3Std140Fields;
      m[_M11] = 1.0f;
      m[_M12] = 0.0f;
      m[_M13] = 0.0f;
      m[_M21] = 0.0f;
      m[_M22] = 1.0f;
      m[_M23] = 0.0f;
      m[_M31] = 0.0f;
      m[_M32] = 0.0f;
      m[_M33] = 1.0f;
    }

    constexpr float Determinant() const
    {
      using namespace Matrix3Std140Fields;
      return (m[_M11] * (m[_M22] * m[_M33] - m[_M23] * m[_M32])) - (m[_M12] * (m[_M21] * m[_M33] - m[_M23] * m[_M31])) +
             (m[_M13] * (m[_M21] * m[_M32] - m[_M22] * m[_M31]));
    }

    static constexpr Matrix3Std140 Transpose(const Matrix3Std140& matrix)
    {
      using namespace Matrix3Std140Fields;
      return {matrix.m[_M11], matrix.m[_M21], matrix.m[_M31], matrix.m[_M12], matrix.m[_M22],
              matrix.m[_M32], matrix.m[_M13], matrix.m[_M23], matrix.m[_M33]};
    }

    static Matrix3Std140 Invert(const Matrix3Std140& matrix);

    constexpr bool operator==(const Matrix3Std140& rhs) const noexcept
    {
      using namespace Matrix3Std140Fields;
      return (m[_M11] == rhs.m[_M11] && m[_M12] == rhs.m[_M12] && m[_M13] == rhs.m[_M13] && m[_M21] == rhs.m[_M21] && m[_M22] == rhs.m[_M22] &&
              m[_M23] == rhs.m[_M23] && m[_M31] == rhs.m[_M31] && m[_M32] == rhs.m[_M32] && m[_M33] == rhs.m[_M33]);
    }

    constexpr bool operator!=(const Matrix3Std140& rhs) const noexcept
    {
      using namespace Matrix3Std140Fields;
      return (m[_M11] != rhs.m[_M11] || m[_M12] != rhs.m[_M12] || m[_M13] != rhs.m[_M13] || m[_M21] != rhs.m[_M21] || m[_M22] != rhs.m[_M22] ||
              m[_M23] != rhs.m[_M23] || m[_M31] != rhs.m[_M31] || m[_M32] != rhs.m[_M32] || m[_M33] != rhs.m[_M33]);
    }
  };
}

#endif
