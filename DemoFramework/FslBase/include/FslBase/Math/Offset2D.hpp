#ifndef FSLBASE_MATH_OFFSET2D_HPP
#define FSLBASE_MATH_OFFSET2D_HPP
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
  struct Offset2D
  {
    using value_type = int32_t;

    value_type X{0};
    value_type Y{0};

    constexpr Offset2D() noexcept = default;

    constexpr Offset2D(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    constexpr Offset2D& operator+=(const Offset2D& arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    constexpr Offset2D& operator-=(const Offset2D& arg) noexcept
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    constexpr Offset2D& operator*=(const Offset2D& arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    constexpr Offset2D& operator*=(const int arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr bool operator==(const Offset2D& rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const Offset2D& rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a Offset2D with all components being zero (0, 0)
    static constexpr Offset2D Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr Offset2D operator+(const Offset2D& lhs, const Offset2D& rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }

  inline constexpr Offset2D operator-(const Offset2D& lhs, const Offset2D& rhs) noexcept
  {
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }

  inline constexpr Offset2D operator*(const Offset2D& lhs, const Offset2D& rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }

  inline constexpr Offset2D operator*(const Offset2D& lhs, const int32_t rhs)
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr Offset2D operator*(const int32_t lhs, const Offset2D& rhs)
  {
    return rhs * lhs;
  }

}

#endif
