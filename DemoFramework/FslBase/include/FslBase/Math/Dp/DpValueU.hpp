#ifndef FSLBASE_MATH_DP_DPVALUEU_HPP
#define FSLBASE_MATH_DP_DPVALUEU_HPP
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
#include <cassert>
// #include <limits>

namespace Fsl
{
  struct DpValueU
  {
    using value_type = uint32_t;

    value_type Value{0};

    inline constexpr DpValueU() noexcept = default;

    inline explicit constexpr DpValueU(const value_type value) noexcept
      : Value(value)
    {
    }

    inline constexpr DpValueU& operator+=(const DpValueU arg) noexcept
    {
      Value += arg.Value;
      return *this;
    }

    inline constexpr DpValueU& operator-=(const DpValueU arg) noexcept
    {
      Value -= arg.Value;
      return *this;
    }

    inline constexpr DpValueU& operator*=(const DpValueU arg) noexcept
    {
      Value *= arg.Value;
      return *this;
    }

    // inline constexpr DpValueU& operator*=(const value_type arg) noexcept
    //{
    //   Value *= arg;
    //   return *this;
    // }

    inline constexpr DpValueU& operator/=(const DpValueU arg)
    {
      assert(arg.Value != 0);
      Value /= arg.Value;
      return *this;
    }

    // inline constexpr DpValueU& operator/=(const value_type arg)
    //{
    //   Value /= arg;
    //   return *this;
    // }

    inline constexpr bool operator==(const DpValueU rhs) const noexcept
    {
      return Value == rhs.Value;
    }

    inline constexpr bool operator!=(const DpValueU rhs) const noexcept
    {
      return Value != rhs.Value;
    }


    inline constexpr bool operator<(const DpValueU rhs) const noexcept
    {
      return Value < rhs.Value;
    }

    inline constexpr bool operator<=(const DpValueU rhs) const noexcept
    {
      return Value <= rhs.Value;
    }

    inline constexpr bool operator>(const DpValueU rhs) const noexcept
    {
      return Value > rhs.Value;
    }

    inline constexpr bool operator>=(const DpValueU rhs) const noexcept
    {
      return Value >= rhs.Value;
    }


    // @brief Returns a DpValueU with all components being zero (0, 0)
    inline static constexpr DpValueU Zero() noexcept
    {
      return {};
    }
  };


  inline constexpr DpValueU operator+(const DpValueU lhs, const DpValueU rhs) noexcept
  {
    // assert(lhs.Value <= (std::numeric_limits<DpValueU::value_type>::max() - rhs.Value));
    return DpValueU(lhs.Value + rhs.Value);
  }

  inline constexpr DpValueU operator-(const DpValueU lhs, const DpValueU rhs) noexcept
  {
    return DpValueU(lhs.Value - rhs.Value);
  }

  inline constexpr DpValueU operator*(const DpValueU lhs, const DpValueU rhs) noexcept
  {
    return DpValueU(lhs.Value * rhs.Value);
  }

  // inline constexpr DpValueU operator*(const DpValueU lhs, const DpValueU::value_type rhs) noexcept
  //{
  //   return DpValueU(lhs.Value * rhs);
  // }

  // inline constexpr DpValueU operator*(const DpValueU::value_type lhs, const DpValueU rhs) noexcept
  //{
  //   return DpValueU(lhs * rhs.Value);
  // }

  inline constexpr DpValueU operator/(const DpValueU lhs, const DpValueU rhs)
  {
    assert(rhs.Value != 0);
    return DpValueU(lhs.Value / rhs.Value);
  }

  // inline constexpr DpValueU operator/(const DpValueU lhs, const DpValueU::value_type rhs)
  //{
  //   return DpValueU(lhs.Value / rhs);
  // }
}

#endif
