#ifndef FSLBASE_MATH_PIXEL_PXPOINT2_HPP
#define FSLBASE_MATH_PIXEL_PXPOINT2_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>

namespace Fsl
{
  struct PxPoint2
  {
    using value_type = PxValue;
    using raw_value_type = value_type::raw_value_type;

    value_type X{0};
    value_type Y{0};

    constexpr PxPoint2() noexcept = default;

    constexpr PxPoint2(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    constexpr PxPoint2(const value_type x, const PxSize1D y) noexcept
      : X(x)
      , Y(y.Value())
    {
    }

    constexpr PxPoint2(const PxSize1D x, const value_type y) noexcept
      : X(x.Value())
      , Y(y)
    {
    }

    constexpr PxPoint2(const PxSize1D x, const PxSize1D y) noexcept
      : X(x.Value())
      , Y(y.Value())
    {
    }

    constexpr PxPoint2& operator+=(const PxPoint2 arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }


    constexpr PxPoint2& operator-=(const PxPoint2 arg) noexcept
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    constexpr PxPoint2& operator*=(const PxPoint2 arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    constexpr PxPoint2& operator*=(const value_type arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr PxPoint2& operator/=(const value_type arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    constexpr bool operator==(const PxPoint2 rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const PxPoint2 rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a PxPoint2 with all components being zero (0, 0)
    static constexpr PxPoint2 Zero() noexcept
    {
      return {};
    }


    static constexpr PxPoint2 Min(const PxPoint2 val0, const PxPoint2 val1) noexcept
    {
      return {MathHelper::Min(val0.X, val1.X), MathHelper::Min(val0.Y, val1.Y)};
    }

    static constexpr PxPoint2 Max(const PxPoint2 val0, const PxPoint2 val1) noexcept
    {
      return {MathHelper::Max(val0.X, val1.X), MathHelper::Max(val0.Y, val1.Y)};
    }

    inline static constexpr PxPoint2 Create(const raw_value_type x, const raw_value_type y) noexcept
    {
      return {value_type(x), value_type(y)};
    }
  };


  inline constexpr PxPoint2 operator+(const PxPoint2 lhs, const PxPoint2 rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }


  inline constexpr PxPoint2 operator-(const PxPoint2 lhs, const PxPoint2 rhs) noexcept
  {
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }


  inline constexpr PxPoint2 operator*(const PxPoint2 lhs, const PxPoint2 rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }


  inline constexpr PxPoint2 operator*(const PxPoint2 lhs, const PxPoint2::value_type rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr PxPoint2 operator*(const PxPoint2::value_type lhs, const PxPoint2 rhs) noexcept
  {
    return rhs * lhs;
  }


  inline constexpr PxPoint2 operator/(const PxPoint2 lhs, const PxPoint2::value_type rhs)
  {
    return {lhs.X / rhs, lhs.Y / rhs};
  }
}

#endif
