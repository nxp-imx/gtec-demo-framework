#ifndef FSLBASE_MATH_PIXEL_PXVALUE_HPP
#define FSLBASE_MATH_PIXEL_PXVALUE_HPP
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
#include <compare>

namespace Fsl
{
  struct PxValue
  {
    using value_type = int32_t;
    using raw_value_type = int32_t;

    int32_t Value{0};

    inline constexpr PxValue() noexcept = default;

    inline explicit constexpr PxValue(const value_type value) noexcept
      : Value(value)
    {
    }

    inline constexpr PxValue operator-() const noexcept
    {
      return PxValue(-Value);
    }

    inline constexpr PxValue& operator+=(const PxValue arg) noexcept
    {
      Value += arg.Value;
      return *this;
    }

    inline constexpr PxValue& operator-=(const PxValue arg) noexcept
    {
      Value -= arg.Value;
      return *this;
    }

    inline constexpr PxValue& operator*=(const PxValue arg) noexcept
    {
      Value *= arg.Value;
      return *this;
    }

    // inline constexpr PxValue& operator*=(const value_type arg) noexcept
    //{
    //   Value *= arg;
    //   return *this;
    // }

    inline constexpr PxValue& operator/=(const PxValue arg)
    {
      Value /= arg.Value;
      return *this;
    }

    // inline constexpr PxValue& operator/=(const value_type arg)
    //{
    //   Value /= arg;
    //   return *this;
    // }

    inline constexpr const PxValue& operator++() noexcept
    {
      ++Value;
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxValue operator++(int) noexcept
    {
      PxValue temp(*this);
      ++Value;
      return temp;
    }

    inline constexpr const PxValue& operator--() noexcept
    {
      --Value;
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxValue operator--(int) noexcept
    {
      PxValue temp(*this);
      --Value;
      return temp;
    }

    //! equal to std::min but its both constexpr and noexcept
    static constexpr PxValue Min(const PxValue val0, const PxValue val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValue(val0.Value <= val1.Value ? val0.Value : val1.Value);
    }

    //! equal to std::max but its both constexpr and noexcept
    static constexpr PxValue Max(const PxValue val0, const PxValue val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValue(val0.Value >= val1.Value ? val0.Value : val1.Value);
    }


    // @brief Returns a PxValue with all components being zero (0, 0)
    inline static constexpr PxValue Zero() noexcept
    {
      return {};
    }

    inline static constexpr PxValue Create(const PxValue value) noexcept
    {
      return value;
    }

    inline static constexpr PxValue Create(const value_type value) noexcept
    {
      return PxValue((value));
    }

    // op<==>
    constexpr auto operator<=>(const PxValue& other) const noexcept = default;
    // op==
    constexpr bool operator==(const PxValue& other) const noexcept = default;
  };

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // PxValue::value_type
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const PxValue lhs, const PxValue::value_type rhs) noexcept
  {
    return lhs.Value <=> rhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr bool operator==(const PxValue lhs, const PxValue::value_type rhs) noexcept
  {
    return lhs.Value == rhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op add
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxValue operator+(const PxValue lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value + rhs.Value);
  }

  // op sub

  inline constexpr PxValue operator-(const PxValue lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value - rhs.Value);
  }

  // op multiply

  inline constexpr PxValue operator*(const PxValue lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value * rhs.Value);
  }

  // op divide

  inline constexpr PxValue operator/(const PxValue lhs, const PxValue rhs)
  {
    return PxValue(lhs.Value / rhs.Value);
  }

  // op modulo

  inline constexpr PxValue operator%(const PxValue lhs, const PxValue rhs)
  {
    return PxValue(lhs.Value % rhs.Value);
  }

}

#endif
