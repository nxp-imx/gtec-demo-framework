#ifndef FSLBASE_MATH_PIXEL_PXVALUEU_HPP
#define FSLBASE_MATH_PIXEL_PXVALUEU_HPP
/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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
// #include <cassert>
// #include <limits>

namespace Fsl
{
  struct PxValueU
  {
    using value_type = uint32_t;
    using raw_value_type = uint32_t;

    value_type Value{0};

    inline constexpr PxValueU() noexcept = default;

    inline explicit constexpr PxValueU(const value_type value) noexcept
      : Value(value)
    {
    }

    inline constexpr PxValueU& operator+=(const PxValueU arg) noexcept
    {
      Value += arg.Value;
      return *this;
    }

    inline constexpr PxValueU& operator-=(const PxValueU arg) noexcept
    {
      Value -= arg.Value;
      return *this;
    }

    inline constexpr PxValueU& operator*=(const PxValueU arg) noexcept
    {
      Value *= arg.Value;
      return *this;
    }

    // inline constexpr PxValueU& operator*=(const value_type arg) noexcept
    //{
    //   Value *= arg;
    //   return *this;
    // }

    inline constexpr PxValueU& operator/=(const PxValueU arg)
    {
      Value /= arg.Value;
      return *this;
    }

    // inline constexpr PxValueU& operator/=(const value_type arg)
    //{
    //   Value /= arg;
    //   return *this;
    // }

    inline constexpr bool operator==(const PxValueU rhs) const noexcept
    {
      return Value == rhs.Value;
    }

    inline constexpr bool operator!=(const PxValueU rhs) const noexcept
    {
      return Value != rhs.Value;
    }


    inline constexpr bool operator<(const PxValueU rhs) const noexcept
    {
      return Value < rhs.Value;
    }

    inline constexpr bool operator<=(const PxValueU rhs) const noexcept
    {
      return Value <= rhs.Value;
    }

    inline constexpr bool operator>(const PxValueU rhs) const noexcept
    {
      return Value > rhs.Value;
    }

    inline constexpr bool operator>=(const PxValueU rhs) const noexcept
    {
      return Value >= rhs.Value;
    }


    // @brief Returns a PxValueU with all components being zero (0, 0)
    inline static constexpr PxValueU Zero() noexcept
    {
      return {};
    }

    //! equal to std::min but its both constexpr and noexcept
    static constexpr PxValueU Min(const PxValueU val0, const PxValueU val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValueU(val0.Value <= val1.Value ? val0.Value : val1.Value);
    }

    //! equal to std::max but its both constexpr and noexcept
    static constexpr PxValueU Max(const PxValueU val0, const PxValueU val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValueU(val0.Value >= val1.Value ? val0.Value : val1.Value);
    }

    inline static constexpr PxValueU Create(const raw_value_type value) noexcept
    {
      return PxValueU(value);
    }
  };


  inline constexpr PxValueU operator+(const PxValueU lhs, const PxValueU rhs) noexcept
  {
    // assert(lhs.Value <= (std::numeric_limits<PxValueU::value_type>::max() - rhs.Value));
    return PxValueU(lhs.Value + rhs.Value);
  }

  inline constexpr PxValueU operator-(const PxValueU lhs, const PxValueU rhs) noexcept
  {
    return PxValueU(lhs.Value - rhs.Value);
  }

  inline constexpr PxValueU operator*(const PxValueU lhs, const PxValueU rhs) noexcept
  {
    return PxValueU(lhs.Value * rhs.Value);
  }

  // inline constexpr PxValueU operator*(const PxValueU lhs, const PxValueU::value_type rhs) noexcept
  //{
  //   return PxValueU(lhs.Value * rhs);
  // }

  // inline constexpr PxValueU operator*(const PxValueU::value_type lhs, const PxValueU rhs) noexcept
  //{
  //   return PxValueU(lhs * rhs.Value);
  // }

  inline constexpr PxValueU operator/(const PxValueU lhs, const PxValueU rhs)
  {
    return PxValueU(lhs.Value / rhs.Value);
  }

  // inline constexpr PxValueU operator/(const PxValueU lhs, const PxValueU::value_type rhs)
  //{
  //   return PxValueU(lhs.Value / rhs);
  // }


}

#endif
