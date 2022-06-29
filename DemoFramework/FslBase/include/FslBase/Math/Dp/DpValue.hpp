#ifndef FSLBASE_MATH_DP_DPVALUE_HPP
#define FSLBASE_MATH_DP_DPVALUE_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

namespace Fsl
{
  struct DpValue
  {
    using value_type = int32_t;

    value_type Value{0};

    inline constexpr DpValue() noexcept = default;

    inline explicit constexpr DpValue(const value_type value) noexcept
      : Value(value)
    {
    }

    DpValue operator-() noexcept
    {
      return DpValue(-Value);
    }

    inline constexpr DpValue& operator+=(const DpValue arg) noexcept
    {
      Value += arg.Value;
      return *this;
    }

    inline constexpr DpValue& operator-=(const DpValue arg) noexcept
    {
      Value -= arg.Value;
      return *this;
    }

    inline constexpr DpValue& operator*=(const DpValue arg) noexcept
    {
      Value *= arg.Value;
      return *this;
    }

    // inline constexpr DpValue& operator*=(const value_type arg) noexcept
    //{
    //   Value *= arg;
    //   return *this;
    // }

    inline constexpr DpValue& operator/=(const DpValue arg)
    {
      Value /= arg.Value;
      return *this;
    }

    // inline constexpr DpValue& operator/=(const value_type arg)
    //{
    //   Value /= arg;
    //   return *this;
    // }

    //! equal to std::min but its both constexpr and noexcept
    static constexpr DpValue Min(const DpValue val0, const DpValue val1) noexcept
    {
      // Both values are known to be >= 0
      return DpValue(val0.Value <= val1.Value ? val0.Value : val1.Value);
    }

    //! equal to std::max but its both constexpr and noexcept
    static constexpr DpValue Max(const DpValue val0, const DpValue val1) noexcept
    {
      // Both values are known to be >= 0
      return DpValue(val0.Value >= val1.Value ? val0.Value : val1.Value);
    }


    // @brief Returns a DpValue with all components being zero (0, 0)
    inline static constexpr DpValue Zero() noexcept
    {
      return {};
    }

    inline static constexpr DpValue Create(const DpValue value) noexcept
    {
      return value;
    }

    inline static constexpr DpValue Create(const value_type value) noexcept
    {
      return DpValue((value));
    }
  };

  // op==

  inline constexpr bool operator==(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }

  // op!=

  inline constexpr bool operator!=(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }

  // op<

  inline constexpr bool operator<(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }

  // op<=

  inline constexpr bool operator<=(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }

  // op>

  inline constexpr bool operator>(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }

  // op>=

  inline constexpr bool operator>=(const DpValue lhs, const DpValue rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }

  // op add

  inline constexpr DpValue operator+(const DpValue lhs, const DpValue rhs) noexcept
  {
    return DpValue(lhs.Value + rhs.Value);
  }

  // op sub

  inline constexpr DpValue operator-(const DpValue lhs, const DpValue rhs) noexcept
  {
    return DpValue(lhs.Value - rhs.Value);
  }

  // op multiply

  inline constexpr DpValue operator*(const DpValue lhs, const DpValue rhs) noexcept
  {
    return DpValue(lhs.Value * rhs.Value);
  }

  // op divide

  inline constexpr DpValue operator/(const DpValue lhs, const DpValue rhs)
  {
    return DpValue(lhs.Value / rhs.Value);
  }
}

#endif
