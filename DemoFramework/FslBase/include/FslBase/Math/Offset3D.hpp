#ifndef FSLBASE_MATH_OFFSET3D_HPP
#define FSLBASE_MATH_OFFSET3D_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  struct Offset3D
  {
    using value_type = int32_t;

    value_type X{0};
    value_type Y{0};
    value_type Z{0};

    constexpr Offset3D() noexcept = default;

    constexpr Offset3D(const value_type x, const value_type y, const value_type z) noexcept
      : X(x)
      , Y(y)
      , Z(z)
    {
    }

    constexpr Offset3D& operator+=(const Offset3D& arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      Z += arg.Z;
      return *this;
    }

    constexpr Offset3D& operator-=(const Offset3D& arg) noexcept
    {
      X -= arg.X;
      Y -= arg.Y;
      Z -= arg.Z;
      return *this;
    }

    constexpr Offset3D& operator*=(const Offset3D& arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      Z *= arg.Z;
      return *this;
    }

    constexpr Offset3D& operator*=(const int32_t arg) noexcept
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      return *this;
    }

    constexpr bool operator==(const Offset3D& rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
    }

    constexpr bool operator!=(const Offset3D& rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z;
    }

    // @brief Returns a Offset3D with all components being zero (0, 0)
    static constexpr Offset3D Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr Offset3D operator+(const Offset3D& lhs, const Offset3D& rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z};
  }

  inline constexpr Offset3D operator-(const Offset3D& lhs, const Offset3D& rhs) noexcept
  {
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z};
  }

  inline constexpr Offset3D operator*(const Offset3D& lhs, const Offset3D& rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z};
  }

  inline constexpr Offset3D operator*(const Offset3D& lhs, const int32_t rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs};
  }

  inline constexpr Offset3D operator*(const int32_t lhs, const Offset3D& rhs) noexcept
  {
    return rhs * lhs;
  }

}

#endif
