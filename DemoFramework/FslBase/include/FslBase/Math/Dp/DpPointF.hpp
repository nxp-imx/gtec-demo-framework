#ifndef FSLBASE_MATH_DP_DPPOINTF_HPP
#define FSLBASE_MATH_DP_DPPOINTF_HPP
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
#include <FslBase/Math/Dp/DpSizeF.hpp>

namespace Fsl
{
  struct DpPointF
  {
    using value_type = float;

    value_type X{0};
    value_type Y{0};

    constexpr DpPointF() noexcept = default;

    constexpr DpPointF(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    constexpr DpPointF& operator+=(const DpPointF& arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    constexpr DpPointF& operator+=(const DpSizeF& arg) noexcept
    {
      X += arg.Width();
      Y += arg.Height();
      return *this;
    }

    constexpr DpPointF& operator-=(const DpPointF& arg) noexcept
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    constexpr DpPointF& operator-=(const DpSizeF& arg) noexcept
    {
      X -= arg.Width();
      Y -= arg.Height();
      return *this;
    }

    constexpr DpPointF& operator*=(const DpPointF& arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    constexpr DpPointF& operator*=(const DpSizeF& arg) noexcept
    {
      X *= arg.Width();
      Y *= arg.Height();
      return *this;
    }

    constexpr DpPointF& operator*=(const value_type arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr DpPointF& operator/=(const value_type arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    constexpr bool operator==(const DpPointF& rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const DpPointF& rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a DpPointF with all components being zero (0, 0)
    static constexpr DpPointF Zero() noexcept
    {
      return {};
    }
  };


  inline constexpr DpPointF operator+(const DpPointF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }

  inline constexpr DpPointF operator+(const DpPointF& lhs, const DpSizeF& rhs) noexcept
  {
    return {lhs.X + rhs.Width(), lhs.Y + rhs.Height()};
  }

  inline constexpr DpPointF operator+(const DpSizeF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.Width() + rhs.X, lhs.Height() + rhs.Y};
  }

  inline constexpr DpPointF operator-(const DpPointF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }

  inline constexpr DpPointF operator-(const DpPointF& lhs, const DpSizeF& rhs) noexcept
  {
    return {lhs.X - rhs.Width(), lhs.Y - rhs.Height()};
  }

  inline constexpr DpPointF operator-(const DpSizeF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.Width() - rhs.X, lhs.Height() - rhs.Y};
  }


  inline constexpr DpPointF operator*(const DpPointF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }

  inline constexpr DpPointF operator*(const DpPointF& lhs, const DpSizeF& rhs) noexcept
  {
    return {lhs.X * rhs.Width(), lhs.Y * rhs.Height()};
  }

  inline constexpr DpPointF operator*(const DpSizeF& lhs, const DpPointF& rhs) noexcept
  {
    return {lhs.Width() * rhs.X, lhs.Height() * rhs.Y};
  }

  inline constexpr DpPointF operator*(const DpPointF& lhs, const DpPointF::value_type rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr DpPointF operator*(const DpPointF::value_type lhs, const DpPointF& rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpPointF operator/(const DpPointF& lhs, const DpPointF::value_type rhs)
  {
    return {lhs.X / rhs, lhs.Y / rhs};
  }

}

#endif
