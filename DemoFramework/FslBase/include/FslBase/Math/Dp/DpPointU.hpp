#ifndef FSLBASE_MATH_DP_DPPOINTU_HPP
#define FSLBASE_MATH_DP_DPPOINTU_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <cassert>

namespace Fsl
{
  struct DpPointU
  {
    using value_type = uint32_t;

    value_type X{0};
    value_type Y{0};

    constexpr DpPointU() noexcept = default;

    constexpr DpPointU(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    constexpr DpPointU& operator+=(const DpPointU& arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    constexpr DpPointU& operator-=(const DpPointU& arg) noexcept
    {
      assert(X >= arg.X);
      assert(Y >= arg.Y);
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    constexpr DpPointU& operator*=(const DpPointU& arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    constexpr DpPointU& operator*=(const value_type arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr DpPointU& operator/=(const value_type arg)
    {
      assert(arg > 0u);
      X /= arg;
      Y /= arg;
      return *this;
    }

    constexpr bool operator==(const DpPointU& rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const DpPointU& rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a DpPointU with all components being zero (0, 0)
    static constexpr DpPointU Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr DpPointU operator+(const DpPointU& lhs, const DpPointU& rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }

  inline constexpr DpPointU operator-(const DpPointU& lhs, const DpPointU& rhs) noexcept
  {
    assert(lhs.X >= rhs.X);
    assert(lhs.Y >= rhs.Y);
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }

  inline constexpr DpPointU operator*(const DpPointU& lhs, const DpPointU& rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }

  inline constexpr DpPointU operator*(const DpPointU& lhs, const DpPointU::value_type rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr DpPointU operator*(const DpPointU::value_type lhs, const DpPointU& rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpPointU operator/(const DpPointU& lhs, const DpPointU::value_type rhs)
  {
    assert(rhs > 0u);
    return {lhs.X / rhs, lhs.Y / rhs};
  }
}
#endif
