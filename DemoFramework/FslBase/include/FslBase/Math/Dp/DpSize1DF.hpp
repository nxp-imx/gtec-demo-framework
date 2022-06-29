#ifndef FSLBASE_MATH_DP_DPSIZE1DF_HPP
#define FSLBASE_MATH_DP_DPSIZE1DF_HPP
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
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  //! A immutable int32_t based size that will never be negative!
  struct DpSize1DF
  {
    using value_type = DpValueF;
    using raw_value_type = float;

  private:
    value_type m_value{};

    explicit constexpr DpSize1DF(const value_type::value_type value) noexcept
      : m_value(value >= 0.0f ? value : 0.0f)
    {
      assert(value >= 0.0f);
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr DpSize1DF(const value_type::value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= 0);
    }

  public:
    constexpr DpSize1DF() noexcept = default;

    explicit constexpr DpSize1DF(const value_type value) noexcept
      : m_value(value >= value_type() ? value : value_type())
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr DpSize1DF(const value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= value_type());
    }

    constexpr inline value_type Value() const noexcept
    {
      return m_value;
    }

    constexpr inline raw_value_type RawValue() const noexcept
    {
      return m_value.Value;
    }

    constexpr inline void Add(const DpSize1DF valueDp) noexcept
    {
      assert(m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valueDp.m_value.Value));

      // Both values are known to be >= 0
      m_value = m_value + valueDp.m_value;
    }

    constexpr inline void Add(const value_type value) noexcept
    {
      // value might be <= 0
      m_value = value_type::Max(m_value + value, value_type());
    }


    constexpr inline void SetMax(const DpSize1DF value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value.m_value);
    }

    constexpr inline void SetMax(const value_type value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value);
    }

    static constexpr DpSize1DF Add(const DpSize1DF sizeDp, const DpSize1DF valueDp) noexcept
    {
      // Check for overflow
      assert(sizeDp.m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valueDp.m_value.Value));

      // Both values are known to be >= 0
      return DpSize1DF(sizeDp.m_value + valueDp.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Add(const value_type sizeDp, const DpSize1DF valueDp) noexcept
    {
      return DpSize1DF(sizeDp + valueDp.m_value);
    }

    static constexpr DpSize1DF Add(const DpSize1DF sizeDp, const value_type valueDp) noexcept
    {
      return DpSize1DF(sizeDp.m_value + valueDp);
    }

    static constexpr DpValueF Subtract(const DpSize1DF sizeDp, const DpSize1DF valueDp) noexcept
    {
      return sizeDp.m_value - valueDp.m_value;
    }

    static constexpr DpValueF Subtract(const DpSize1DF::value_type sizeDp, const DpSize1DF valueDp) noexcept
    {
      return sizeDp - valueDp.m_value;
    }

    static constexpr DpValueF Subtract(const DpSize1DF sizeDp, const DpSize1DF::value_type valueDp) noexcept
    {
      return sizeDp.m_value - valueDp;
    }


    static constexpr DpSize1DF Multiply(const DpSize1DF sizeDp, const DpSize1DF valueDp) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1DF(sizeDp.m_value * valueDp.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpValueF Multiply(const DpSize1DF::value_type sizeDp, const DpSize1DF valueDp) noexcept
    {
      // One of the values might be negative
      return sizeDp * valueDp.m_value;
    }

    static constexpr DpValueF Multiply(const DpSize1DF sizeDp, const DpSize1DF::value_type valueDp) noexcept
    {
      // One of the values might be negative
      return sizeDp.m_value * valueDp;
    }


    static constexpr DpSize1DF Divide(const DpSize1DF sizeDp, const DpSize1DF value)
    {
      // Both values are known to be >= 0
      assert(value.m_value != value_type());
      return DpSize1DF(sizeDp.m_value / value.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpValueF Divide(const DpSize1DF sizeDp, const value_type value)
    {
      // sizeDp are known to be >= 0
      assert(value != value_type());
      return sizeDp.m_value / value;
    }

    constexpr DpSize1DF operator+=(const DpSize1DF valueDp) noexcept
    {
      // m_value is >= 0 and valueDp >= 0
      assert(m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valueDp.m_value.Value));
      m_value += valueDp.m_value;
      return *this;
    }

    constexpr DpSize1DF operator+=(const DpSize1DF::value_type valueDp) noexcept
    {
      // m_value is >= 0 but valueDp can be negative as well
      m_value = value_type::Max(m_value + valueDp, value_type());
      return *this;
    }

    constexpr DpSize1DF operator-=(const DpSize1DF valueDp) noexcept
    {
      // m_value is >= 0 and valueDp >= 0
      m_value = value_type::Max(m_value - valueDp.m_value, value_type());
      return *this;
    }

    constexpr DpSize1DF operator-=(const DpSize1DF::value_type valueDp) noexcept
    {
      // m_value is >= 0 but valueDp can be negative as well
      m_value = value_type::Max(m_value - valueDp, value_type());
      return *this;
    }

    static constexpr DpSize1DF Min(const DpSize1DF val0, const DpSize1DF val1) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1DF(value_type::Min(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Min(const DpSize1DF::value_type val0, const DpSize1DF val1) noexcept
    {
      // One of the values might be < 0
      return DpSize1DF(value_type::Min(val0, val1.m_value));
    }

    static constexpr DpSize1DF Min(const DpSize1DF val0, const DpSize1DF::value_type val1) noexcept
    {
      // One of the values might be < 0
      return DpSize1DF(value_type::Min(val0.m_value, val1));
    }

    static constexpr DpSize1DF Min(const DpSize1DF::value_type val0, const DpSize1DF::value_type val1) noexcept
    {
      // Both values might be < 0
      return DpSize1DF(value_type::Min(val0, val1));
    }

    static constexpr DpSize1DF Max(const DpSize1DF val0, const DpSize1DF val1) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1DF(value_type::Max(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Max(const DpSize1DF::value_type val0, const DpSize1DF val1) noexcept
    {
      // One of the values are >= 0
      return DpSize1DF(value_type::Max(val0, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Max(const DpSize1DF val0, const DpSize1DF::value_type val1) noexcept
    {
      // One of the values are >= 0
      return DpSize1DF(value_type::Max(val0.m_value, val1), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Max(const DpSize1DF::value_type val0, const DpSize1DF::value_type val1) noexcept
    {
      // Both values might be < 0
      return DpSize1DF(value_type::Max(val0, val1));
    }

    static constexpr DpSize1DF Max(const DpSize1DF val0, const DpSize1DF val1, const DpSize1DF val2) noexcept
    {
      // All values are known to be >= 0
      return DpSize1DF(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1DF Max(const DpSize1DF val0, const DpSize1DF val1, const DpSize1DF val2, const DpSize1DF val3) noexcept
    {
      // All values are known to be >= 0
      return DpSize1DF(value_type::Max(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), val3.m_value),
                       OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr DpSize1DF Create(const DpSize1DF value) noexcept
    {
      return value;
    }

    inline static constexpr DpSize1DF Create(const value_type value) noexcept
    {
      return DpSize1DF(value);
    }

    inline static constexpr DpSize1DF Create(const value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return DpSize1DF(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr DpSize1DF Create(const value_type::value_type value) noexcept
    {
      return DpSize1DF(value_type(value));
    }

    inline static constexpr DpSize1DF Create(const value_type::value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return DpSize1DF(value, OptimizationCheckFlag::NoCheck);
    }
  };

  // op==

  constexpr bool operator==(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() == rhs.Value();
  }

  constexpr bool operator==(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs == rhs.Value();
  }

  constexpr bool operator==(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() == rhs;
  }

  // op!=

  constexpr bool operator!=(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() != rhs.Value();
  }

  constexpr bool operator!=(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs != rhs.Value();
  }

  constexpr bool operator!=(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() != rhs;
  }

  // op<

  inline constexpr bool operator<(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() < rhs.Value();
  }

  inline constexpr bool operator<(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs < rhs.Value();
  }

  inline constexpr bool operator<(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() < rhs;
  }

  // op<=

  inline constexpr bool operator<=(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() <= rhs.Value();
  }

  inline constexpr bool operator<=(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs <= rhs.Value();
  }

  inline constexpr bool operator<=(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() <= rhs;
  }

  // op>

  inline constexpr bool operator>(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() > rhs.Value();
  }

  inline constexpr bool operator>(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs > rhs.Value();
  }

  inline constexpr bool operator>(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() > rhs;
  }

  // op>=

  inline constexpr bool operator>=(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return lhs.Value() >= rhs.Value();
  }

  inline constexpr bool operator>=(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return lhs >= rhs.Value();
  }

  inline constexpr bool operator>=(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() >= rhs;
  }


  // op add

  inline constexpr DpSize1DF operator+(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Add(lhs, rhs);
  }

  inline constexpr DpSize1DF operator+(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Add(lhs, rhs);
  }

  inline constexpr DpSize1DF operator+(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return DpSize1DF::Add(lhs, rhs);
  }

  // op sub

  inline constexpr DpValueF operator-(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Subtract(lhs, rhs);
  }

  inline constexpr DpValueF operator-(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Subtract(lhs, rhs);
  }

  inline constexpr DpValueF operator-(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return DpSize1DF::Subtract(lhs, rhs);
  }

  // op multiply

  inline constexpr DpSize1DF operator*(const DpSize1DF lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Multiply(lhs, rhs);
  }

  inline constexpr DpValueF operator*(const DpSize1DF::value_type lhs, const DpSize1DF rhs) noexcept
  {
    return DpSize1DF::Multiply(lhs, rhs);
  }

  inline constexpr DpValueF operator*(const DpSize1DF lhs, const DpSize1DF::value_type rhs) noexcept
  {
    return DpSize1DF::Multiply(lhs, rhs);
  }

  // op divide

  inline constexpr DpSize1DF operator/(const DpSize1DF lhs, const DpSize1DF rhs)
  {
    return DpSize1DF::Divide(lhs, rhs);
  }

  inline constexpr DpValueF operator/(const DpSize1DF lhs, const DpSize1DF::value_type rhs)
  {
    return DpSize1DF::Divide(lhs, rhs);
  }

}

#endif
