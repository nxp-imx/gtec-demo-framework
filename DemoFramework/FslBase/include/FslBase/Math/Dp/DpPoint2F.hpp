#ifndef FSLBASE_MATH_DP_DPPOINT2F_HPP
#define FSLBASE_MATH_DP_DPPOINT2F_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <cmath>

namespace Fsl
{
  struct DpSize2DF;

  struct DpPoint2F
  {
    using value_type = DpValueF;
    using raw_value_type = value_type::raw_value_type;

    value_type X{0};
    value_type Y{0};

    constexpr DpPoint2F() noexcept = default;

    constexpr DpPoint2F(const value_type x, const value_type y) noexcept
      : X(x)
      , Y(y)
    {
    }

    //! @brief Calculates the length of the vector squared.
    constexpr DpValueF LengthSquared() const noexcept
    {
      return (X * X) + (Y * Y);
    }

    //! @brief Calculates the length of the vector squared.
    constexpr DpValueF Length() const
    {
      return DpValueF(std::sqrt((X.Value * X.Value) + (Y.Value * Y.Value)));
    }

    constexpr DpPoint2F& operator+=(const DpPoint2F arg) noexcept
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    DpPoint2F& operator+=(const DpSize2DF& arg) noexcept;

    constexpr DpPoint2F& operator-=(const DpPoint2F arg) noexcept
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    DpPoint2F& operator-=(const DpSize2DF& arg) noexcept;

    constexpr DpPoint2F& operator*=(const DpPoint2F arg) noexcept
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    DpPoint2F& operator*=(const DpSize2DF& arg) noexcept;

    constexpr DpPoint2F& operator*=(const value_type arg) noexcept
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr DpPoint2F& operator/=(const value_type arg)
    {
      X /= arg;
      Y /= arg;
      return *this;
    }

    constexpr bool operator==(const DpPoint2F rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const DpPoint2F rhs) const noexcept
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a DpPoint2F with all components being zero (0, 0)
    static constexpr DpPoint2F Zero() noexcept
    {
      return {};
    }

    inline static constexpr DpPoint2F Create(const value_type::value_type width, const value_type::value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }
  };


  inline constexpr DpPoint2F operator+(const DpPoint2F lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.X + rhs.X, lhs.Y + rhs.Y};
  }


  inline constexpr DpPoint2F operator-(const DpPoint2F lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.X - rhs.X, lhs.Y - rhs.Y};
  }


  inline constexpr DpPoint2F operator*(const DpPoint2F lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.X * rhs.X, lhs.Y * rhs.Y};
  }

  inline constexpr DpPoint2F operator*(const DpPoint2F lhs, const DpPoint2F::value_type rhs) noexcept
  {
    return {lhs.X * rhs, lhs.Y * rhs};
  }

  inline constexpr DpPoint2F operator*(const DpPoint2F::value_type lhs, const DpPoint2F rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpPoint2F operator/(const DpPoint2F lhs, const DpPoint2F::value_type rhs)
  {
    return {lhs.X / rhs, lhs.Y / rhs};
  }

}

#endif
