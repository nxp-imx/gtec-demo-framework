#ifndef FSLBASE_MATH_PIXEL_PXVALUEF_HPP
#define FSLBASE_MATH_PIXEL_PXVALUEF_HPP
/****************************************************************************************************************************************************
 * Copyright 2022-2024 NXP
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
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <compare>

namespace Fsl
{
  struct PxValueF
  {
    using value_type = float;
    using raw_value_type = float;

    float Value{0};

    inline constexpr PxValueF() noexcept = default;

    inline explicit constexpr PxValueF(const value_type value) noexcept
      : Value(value)
    {
    }

    inline explicit constexpr PxValueF(const PxValue value) noexcept
      : Value(static_cast<float>(value.Value))
    {
    }

    inline explicit constexpr PxValueF(const PxSize1D value) noexcept
      : Value(static_cast<float>(value.RawValue()))
    {
    }

    inline constexpr PxValueF operator-() const noexcept
    {
      return PxValueF(-Value);
    }

    inline constexpr PxValueF& operator+=(const PxValueF arg) noexcept
    {
      Value += arg.Value;
      return *this;
    }

    inline constexpr PxValueF& operator-=(const PxValueF arg) noexcept
    {
      Value -= arg.Value;
      return *this;
    }

    inline constexpr PxValueF& operator*=(const PxValueF arg) noexcept
    {
      Value *= arg.Value;
      return *this;
    }

    // inline constexpr PxValueF& operator*=(const value_type arg) noexcept
    //{
    //   Value *= arg;
    //   return *this;
    // }

    inline constexpr PxValueF& operator/=(const PxValueF arg)
    {
      Value /= arg.Value;
      return *this;
    }

    // inline constexpr PxValueF& operator/=(const value_type arg)
    //{
    //   Value /= arg;
    //   return *this;
    // }

    //! equal to std::min but its both constexpr and noexcept
    static constexpr PxValueF Min(const PxValueF val0, const PxValueF val1) noexcept
    {
      return PxValueF(val0.Value <= val1.Value ? val0.Value : val1.Value);
    }

    //! equal to std::max but its both constexpr and noexcept
    static constexpr PxValueF Max(const PxValueF val0, const PxValueF val1) noexcept
    {
      return PxValueF(val0.Value >= val1.Value ? val0.Value : val1.Value);
    }


    // @brief Returns a PxValueF with all components being zero (0, 0)
    inline static constexpr PxValueF Zero() noexcept
    {
      return {};
    }

    inline static constexpr PxValueF Create(const PxValueF value) noexcept
    {
      return value;
    }

    inline static constexpr PxValueF Create(const value_type value) noexcept
    {
      return PxValueF((value));
    }

    // op<==>
    constexpr auto operator<=>(const PxValueF& other) const noexcept = default;
    // op==
    constexpr bool operator==(const PxValueF& other) const noexcept = default;
  };

  // op add

  inline constexpr PxValueF operator+(const PxValueF lhs, const PxValueF rhs) noexcept
  {
    return PxValueF(lhs.Value + rhs.Value);
  }

  // op sub

  inline constexpr PxValueF operator-(const PxValueF lhs, const PxValueF rhs) noexcept
  {
    return PxValueF(lhs.Value - rhs.Value);
  }

  // op multiply

  inline constexpr PxValueF operator*(const PxValueF lhs, const PxValueF rhs) noexcept
  {
    return PxValueF(lhs.Value * rhs.Value);
  }

  inline constexpr PxValueF operator*(const float lhs, const PxValueF rhs) noexcept
  {
    return PxValueF(lhs * rhs.Value);
  }

  inline constexpr PxValueF operator*(const PxValueF lhs, const float rhs) noexcept
  {
    return PxValueF(lhs.Value * rhs);
  }

  // op divide

  inline constexpr PxValueF operator/(const PxValueF lhs, const PxValueF rhs)
  {
    return PxValueF(lhs.Value / rhs.Value);
  }
}

#endif
