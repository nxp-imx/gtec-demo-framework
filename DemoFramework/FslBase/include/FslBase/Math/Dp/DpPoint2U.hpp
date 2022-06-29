#ifndef FSLBASE_MATH_DP_DPPOINT2U_HPP
#define FSLBASE_MATH_DP_DPPOINT2U_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Dp/DpValueU.hpp>

namespace Fsl
{
  struct DpPoint2U
  {
    using value_type = DpValueU;

    value_type X{0};
    value_type Y{0};

    constexpr DpPoint2U() noexcept = default;

    constexpr DpPoint2U(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    constexpr DpPoint2U& operator+=(const DpPoint2U arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    constexpr DpPoint2U& operator-=(const DpPoint2U arg) noexcept
    {
      assert(X >= arg.X);
      assert(Y >= arg.Y);
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    constexpr DpPoint2U& operator*=(const DpPoint2U arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    constexpr DpPoint2U& operator*=(const value_type arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr DpPoint2U& operator/=(const value_type arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    constexpr bool operator==(const DpPoint2U rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const DpPoint2U rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a DpPoint2U with all components being zero (0, 0)
    static constexpr DpPoint2U Zero() noexcept
    {
      return {};
    }

    inline static constexpr DpPoint2U Create(const value_type::value_type width, const value_type::value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }
  };

  inline constexpr DpPoint2U operator+(const DpPoint2U lhs, const DpPoint2U rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }

  inline constexpr DpPoint2U operator-(const DpPoint2U lhs, const DpPoint2U rhs) noexcept
  {
    assert(lhs.X >= rhs.X);
    assert(lhs.Y >= rhs.Y);
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }

  inline constexpr DpPoint2U operator*(const DpPoint2U lhs, const DpPoint2U rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }

  inline constexpr DpPoint2U operator*(const DpPoint2U lhs, const DpPoint2U::value_type rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr DpPoint2U operator*(const DpPoint2U::value_type lhs, const DpPoint2U rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpPoint2U operator/(const DpPoint2U lhs, const DpPoint2U::value_type rhs)
  {
    return {lhs.X / rhs, lhs.Y / rhs};
  }
}
#endif
