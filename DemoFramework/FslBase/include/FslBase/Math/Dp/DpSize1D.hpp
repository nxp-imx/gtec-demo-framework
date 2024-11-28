#ifndef FSLBASE_MATH_DP_DPSIZE1D_HPP
#define FSLBASE_MATH_DP_DPSIZE1D_HPP
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
#include <FslBase/Math/Dp/DpValue.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  //! A immutable DpValue based size that will never be negative!
  struct DpSize1D
  {
    using value_type = DpValue;
    using raw_value_type = int32_t;

  private:
    DpValue m_value;

    explicit constexpr DpSize1D(const value_type::value_type value) noexcept
      : m_value(value >= 0 ? value : 0)
    {
      assert(value >= 0);
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr DpSize1D(const value_type::value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= 0);
    }

  public:
    constexpr DpSize1D() noexcept = default;

    explicit constexpr DpSize1D(const value_type value) noexcept
      : m_value(value >= value_type() ? value : value_type())
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr DpSize1D(const value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= value_type());
    }

    constexpr inline value_type Value() const noexcept
    {
      return m_value;
    }

    constexpr inline value_type::value_type RawValue() const noexcept
    {
      return m_value.Value;
    }

    constexpr inline void Add(const DpSize1D valueDp) noexcept
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


    constexpr inline void SetMax(const DpSize1D value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value.m_value);
    }

    constexpr inline void SetMax(const value_type value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value);
    }

    static constexpr DpSize1D Add(const DpSize1D sizeDp, const DpSize1D valueDp) noexcept
    {
      // Check for overflow
      assert(sizeDp.m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valueDp.m_value.Value));

      // Both values are known to be >= 0
      return DpSize1D(sizeDp.m_value + valueDp.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpValue Add(const value_type sizeDp, const DpSize1D valueDp) noexcept
    {
      return sizeDp + valueDp.m_value;
    }

    static constexpr DpValue Add(const DpSize1D sizeDp, const value_type valueDp) noexcept
    {
      return sizeDp.m_value + valueDp;
    }

    static constexpr DpValue Subtract(const DpSize1D sizeDp, const DpSize1D valueDp) noexcept
    {
      return sizeDp.m_value - valueDp.m_value;
    }

    static constexpr DpValue Subtract(const DpSize1D::value_type sizeDp, const DpSize1D valueDp) noexcept
    {
      return sizeDp - valueDp.m_value;
    }

    static constexpr DpValue Subtract(const DpSize1D sizeDp, const DpSize1D::value_type valueDp) noexcept
    {
      return sizeDp.m_value - valueDp;
    }


    static constexpr DpSize1D Multiply(const DpSize1D sizeDp, const DpSize1D valueDp) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1D(sizeDp.m_value * valueDp.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpValue Multiply(const DpSize1D::value_type sizeDp, const DpSize1D valueDp) noexcept
    {
      // One of the values might be negative
      return sizeDp * valueDp.m_value;
    }

    static constexpr DpValue Multiply(const DpSize1D sizeDp, const DpSize1D::value_type valueDp) noexcept
    {
      // One of the values might be negative
      return sizeDp.m_value * valueDp;
    }


    static constexpr DpSize1D Divide(const DpSize1D sizeDp, const DpSize1D value)
    {
      // Both values are known to be >= 0
      assert(value.m_value != value_type());
      return DpSize1D(sizeDp.m_value / value.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpValue Divide(const DpSize1D sizeDp, const value_type value)
    {
      // sizeDp are known to be >= 0
      assert(value != value_type());
      return sizeDp.m_value / value;
    }

    constexpr DpSize1D operator+=(const DpSize1D valueDp) noexcept
    {
      // m_value is >= 0 and valueDp >= 0
      assert(m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valueDp.m_value.Value));
      m_value += valueDp.m_value;
      return *this;
    }

    constexpr DpSize1D operator+=(const DpSize1D::value_type valueDp) noexcept
    {
      // m_value is >= 0 but valueDp can be negative as well
      m_value = value_type::Max(m_value + valueDp, value_type());
      return *this;
    }

    constexpr DpSize1D operator-=(const DpSize1D valueDp) noexcept
    {
      // m_value is >= 0 and valueDp >= 0
      m_value = value_type::Max(m_value - valueDp.m_value, value_type());
      return *this;
    }

    constexpr DpSize1D operator-=(const DpSize1D::value_type valueDp) noexcept
    {
      // m_value is >= 0 but valueDp can be negative as well
      m_value = value_type::Max(m_value - valueDp, value_type());
      return *this;
    }

    static constexpr DpSize1D Min(const DpSize1D val0, const DpSize1D val1) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1D(value_type::Min(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1D Min(const DpSize1D::value_type val0, const DpSize1D val1) noexcept
    {
      // One of the values might be < 0
      return DpSize1D(value_type::Min(val0, val1.m_value));
    }

    static constexpr DpSize1D Min(const DpSize1D val0, const DpSize1D::value_type val1) noexcept
    {
      // One of the values might be < 0
      return DpSize1D(value_type::Min(val0.m_value, val1));
    }

    static constexpr DpSize1D Min(const DpSize1D::value_type val0, const DpSize1D::value_type val1) noexcept
    {
      // Both values might be < 0
      return DpSize1D(value_type::Min(val0, val1));
    }

    static constexpr DpSize1D Max(const DpSize1D val0, const DpSize1D val1) noexcept
    {
      // Both values are known to be >= 0
      return DpSize1D(value_type::Max(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1D Max(const DpSize1D::value_type val0, const DpSize1D val1) noexcept
    {
      // One of the values are >= 0
      return DpSize1D(value_type::Max(val0, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1D Max(const DpSize1D val0, const DpSize1D::value_type val1) noexcept
    {
      // One of the values are >= 0
      return DpSize1D(value_type::Max(val0.m_value, val1), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1D Max(const DpSize1D::value_type val0, const DpSize1D::value_type val1) noexcept
    {
      // Both values might be < 0
      return DpSize1D(value_type::Max(val0, val1));
    }

    static constexpr DpSize1D Max(const DpSize1D val0, const DpSize1D val1, const DpSize1D val2) noexcept
    {
      // All values are known to be >= 0
      return DpSize1D(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr DpSize1D Max(const DpSize1D val0, const DpSize1D val1, const DpSize1D val2, const DpSize1D val3) noexcept
    {
      // All values are known to be >= 0
      return DpSize1D(value_type::Max(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), val3.m_value),
                      OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr DpSize1D Create(const DpSize1D value) noexcept
    {
      return value;
    }

    inline static constexpr DpSize1D Create(const value_type value) noexcept
    {
      return DpSize1D(value);
    }

    inline static constexpr DpSize1D Create(const value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return DpSize1D(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr DpSize1D Create(const value_type::value_type value) noexcept
    {
      return DpSize1D(value_type(value));
    }

    inline static constexpr DpSize1D Create(const value_type::value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return DpSize1D(value, OptimizationCheckFlag::NoCheck);
    }
  };

  // op==

  constexpr bool operator==(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() == rhs.Value();
  }

  constexpr bool operator==(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs == rhs.Value();
  }

  constexpr bool operator==(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() == rhs;
  }

  // op!=

  constexpr bool operator!=(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() != rhs.Value();
  }

  constexpr bool operator!=(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs != rhs.Value();
  }

  constexpr bool operator!=(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() != rhs;
  }

  // op<

  inline constexpr bool operator<(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() < rhs.Value();
  }

  inline constexpr bool operator<(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs < rhs.Value();
  }

  inline constexpr bool operator<(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() < rhs;
  }

  // op<=

  inline constexpr bool operator<=(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() <= rhs.Value();
  }

  inline constexpr bool operator<=(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs <= rhs.Value();
  }

  inline constexpr bool operator<=(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() <= rhs;
  }

  // op>

  inline constexpr bool operator>(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() > rhs.Value();
  }

  inline constexpr bool operator>(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs > rhs.Value();
  }

  inline constexpr bool operator>(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() > rhs;
  }

  // op>=

  inline constexpr bool operator>=(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return lhs.Value() >= rhs.Value();
  }

  inline constexpr bool operator>=(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return lhs >= rhs.Value();
  }

  inline constexpr bool operator>=(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return lhs.Value() >= rhs;
  }


  // op add

  inline constexpr DpSize1D operator+(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Add(lhs, rhs);
  }

  inline constexpr DpValue operator+(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Add(lhs, rhs);
  }

  inline constexpr DpValue operator+(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return DpSize1D::Add(lhs, rhs);
  }

  // op sub

  inline constexpr DpValue operator-(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Subtract(lhs, rhs);
  }

  inline constexpr DpValue operator-(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Subtract(lhs, rhs);
  }

  inline constexpr DpValue operator-(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return DpSize1D::Subtract(lhs, rhs);
  }

  // op multiply

  inline constexpr DpSize1D operator*(const DpSize1D lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Multiply(lhs, rhs);
  }

  inline constexpr DpValue operator*(const DpSize1D::value_type lhs, const DpSize1D rhs) noexcept
  {
    return DpSize1D::Multiply(lhs, rhs);
  }

  inline constexpr DpValue operator*(const DpSize1D lhs, const DpSize1D::value_type rhs) noexcept
  {
    return DpSize1D::Multiply(lhs, rhs);
  }

  // op divide

  inline constexpr DpSize1D operator/(const DpSize1D lhs, const DpSize1D rhs)
  {
    return DpSize1D::Divide(lhs, rhs);
  }

  inline constexpr DpValue operator/(const DpSize1D lhs, const DpSize1D::value_type rhs)
  {
    return DpSize1D::Divide(lhs, rhs);
  }

}

#endif
