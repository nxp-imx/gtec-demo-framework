#ifndef FSLBASE_MATH_PIXEL_PXSIZE1DF_HPP
#define FSLBASE_MATH_PIXEL_PXSIZE1DF_HPP
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
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValueF.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  //! A immutable PxValueF based size that will never be negative!
  struct PxSize1DF
  {
    using value_type = PxValueF;
    using raw_value_type = value_type::raw_value_type;

  private:
    value_type m_value;

    explicit constexpr PxSize1DF(const value_type::value_type value) noexcept
      : m_value(value >= 0.0f ? value : 0.0f)
    {
      assert(value >= 0.0f);
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr PxSize1DF(const value_type::value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= 0.0f);
    }

  public:
    constexpr PxSize1DF() noexcept = default;

    explicit constexpr PxSize1DF(const value_type value) noexcept
      : m_value(value >= value_type() ? value : value_type())
    {
    }

    explicit constexpr PxSize1DF(const PxSize1D value) noexcept
      : m_value(static_cast<float>(value.RawValue()))
    {
    }

    explicit constexpr PxSize1DF(const PxValueU value) noexcept
      : m_value(static_cast<float>(value.Value))
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr PxSize1DF(const value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(value >= value_type());
    }

    // inline constexpr operator value_type() const noexcept
    //{
    //   return m_value;
    // }

    constexpr inline value_type Value() const noexcept
    {
      return m_value;
    }

    constexpr inline value_type::value_type RawValue() const noexcept
    {
      return m_value.Value;
    }

    constexpr inline void Add(const PxSize1DF valuePx) noexcept
    {
      assert(m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valuePx.m_value.Value));

      // Both values are known to be >= 0
      m_value = m_value + valuePx.m_value;
    }

    constexpr inline void Add(const value_type value) noexcept
    {
      // value might be <= 0
      m_value = value_type::Max(m_value + value, value_type());
    }


    constexpr inline void SetMax(const PxSize1DF value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value.m_value);
    }

    constexpr inline void SetMax(const value_type value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value);
    }

    static constexpr PxSize1DF Add(const PxSize1DF sizePx, const PxSize1DF valuePx) noexcept
    {
      // Check for overflow
      assert(sizePx.m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valuePx.m_value.Value));

      // Both values are known to be >= 0
      return PxSize1DF(sizePx.m_value + valuePx.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Add(const value_type sizePx, const PxSize1DF valuePx) noexcept
    {
      return sizePx + valuePx.m_value;
    }

    static constexpr value_type Add(const PxSize1DF sizePx, const value_type valuePx) noexcept
    {
      return sizePx.m_value + valuePx;
    }

    static constexpr value_type Subtract(const PxSize1DF sizePx, const PxSize1DF valuePx) noexcept
    {
      return sizePx.m_value - valuePx.m_value;
    }

    static constexpr value_type Subtract(const value_type sizePx, const PxSize1DF valuePx) noexcept
    {
      return sizePx - valuePx.m_value;
    }

    static constexpr value_type Subtract(const PxSize1DF sizePx, const value_type valuePx) noexcept
    {
      return sizePx.m_value - valuePx;
    }


    static constexpr PxSize1DF Multiply(const PxSize1DF sizePx, const PxSize1DF valuePx) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1DF(sizePx.m_value * valuePx.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Multiply(const value_type sizePx, const PxSize1DF valuePx) noexcept
    {
      return sizePx * valuePx.m_value;
    }

    static constexpr value_type Multiply(const PxSize1DF sizePx, const value_type valuePx) noexcept
    {
      // One of the values might be negative
      return sizePx.m_value * valuePx;
    }


    static constexpr PxSize1DF Divide(const PxSize1DF sizePx, const PxSize1DF value)
    {
      // Both values are known to be >= 0
      assert(value.m_value != value_type());
      return PxSize1DF(sizePx.m_value / value.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Divide(const PxSize1DF sizePx, const value_type value)
    {
      // sizePx are known to be >= 0
      assert(value != value_type());
      return sizePx.m_value / value;
    }

    static constexpr value_type Divide(const value_type sizePx, const PxSize1DF value)
    {
      assert(value.m_value.Value != 0.0f);
      return sizePx / value.m_value;
    }

    constexpr PxSize1DF operator+=(const PxSize1DF valuePx) noexcept
    {
      // m_value is >= 0 and valuePx >= 0
      assert(m_value.Value <= (std::numeric_limits<value_type::value_type>::max() - valuePx.m_value.Value));
      m_value += valuePx.m_value;
      return *this;
    }

    constexpr PxSize1DF operator*=(const PxSize1DF valuePx) noexcept
    {
      m_value *= valuePx.m_value;
      assert(m_value.Value >= 0.0f);
      return *this;
    }

    constexpr PxSize1DF operator+=(const value_type valuePx) noexcept
    {
      // m_value is >= 0 but valuePx can be negative as well
      m_value = value_type::Max(m_value + valuePx, value_type());
      return *this;
    }

    constexpr PxSize1DF operator-=(const PxSize1DF valuePx) noexcept
    {
      // m_value is >= 0 and valuePx >= 0
      m_value = value_type::Max(m_value - valuePx.m_value, value_type());
      return *this;
    }

    constexpr PxSize1DF operator-=(const value_type valuePx) noexcept
    {
      // m_value is >= 0 but valuePx can be negative as well
      m_value = value_type::Max(m_value - valuePx, value_type());
      return *this;
    }

    static constexpr PxSize1DF Min(const PxSize1DF val0, const PxSize1DF val1) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1DF(value_type::Min(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1DF Min(const value_type val0, const PxSize1DF val1) noexcept
    {
      // One of the values might be < 0
      return PxSize1DF(value_type::Min(val0, val1.m_value));
    }

    static constexpr PxSize1DF Min(const PxSize1DF val0, const value_type val1) noexcept
    {
      // One of the values might be < 0
      return PxSize1DF(value_type::Min(val0.m_value, val1));
    }

    static constexpr PxSize1DF Min(const value_type val0, const value_type val1) noexcept
    {
      // Both values might be < 0
      return PxSize1DF(value_type::Min(val0, val1));
    }

    static constexpr PxSize1DF Max(const PxSize1DF val0, const PxSize1DF val1) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1DF(value_type::Max(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1DF Max(const value_type val0, const PxSize1DF val1) noexcept
    {
      // One of the values are >= 0
      return PxSize1DF(value_type::Max(val0, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1DF Max(const PxSize1DF val0, const value_type val1) noexcept
    {
      // One of the values are >= 0
      return PxSize1DF(value_type::Max(val0.m_value, val1), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1DF Max(const value_type val0, const value_type val1) noexcept
    {
      // Both values might be < 0
      return PxSize1DF(value_type::Max(val0, val1));
    }

    static constexpr PxSize1DF Max(const PxSize1DF val0, const PxSize1DF val1, const PxSize1DF val2) noexcept
    {
      // All values are known to be >= 0
      return PxSize1DF(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1DF Max(const PxSize1DF val0, const PxSize1DF val1, const PxSize1DF val2, const PxSize1DF val3) noexcept
    {
      // All values are known to be >= 0
      return PxSize1DF(value_type::Max(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), val3.m_value),
                       OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1DF Create(const PxSize1DF value) noexcept
    {
      return value;
    }

    inline static constexpr PxSize1DF Create(const value_type value) noexcept
    {
      return PxSize1DF(value);
    }

    inline static constexpr PxSize1DF Create(const value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return PxSize1DF(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1DF Create(const value_type::value_type value) noexcept
    {
      return PxSize1DF(value_type(value));
    }

    inline static constexpr PxSize1DF UncheckedCreate(const value_type::value_type value) noexcept
    {
      return PxSize1DF(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1DF UncheckedCreate(const value_type value) noexcept
    {
      return PxSize1DF(value, OptimizationCheckFlag::NoCheck);
    }
  };

  // op==

  constexpr bool operator==(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() == rhs.Value();
  }

  constexpr bool operator==(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs == rhs.Value();
  }

  constexpr bool operator==(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() == rhs;
  }

  // op!=

  constexpr bool operator!=(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() != rhs.Value();
  }

  constexpr bool operator!=(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs != rhs.Value();
  }

  constexpr bool operator!=(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() != rhs;
  }

  // op<

  inline constexpr bool operator<(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() < rhs.Value();
  }

  inline constexpr bool operator<(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs < rhs.Value();
  }

  inline constexpr bool operator<(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() < rhs;
  }

  // op<=

  inline constexpr bool operator<=(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() <= rhs.Value();
  }

  inline constexpr bool operator<=(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs <= rhs.Value();
  }

  inline constexpr bool operator<=(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() <= rhs;
  }

  // op>

  inline constexpr bool operator>(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() > rhs.Value();
  }

  inline constexpr bool operator>(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs > rhs.Value();
  }

  inline constexpr bool operator>(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() > rhs;
  }

  // op>=

  inline constexpr bool operator>=(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return lhs.Value() >= rhs.Value();
  }

  inline constexpr bool operator>=(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return lhs >= rhs.Value();
  }

  inline constexpr bool operator>=(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return lhs.Value() >= rhs;
  }


  // op add

  inline constexpr PxSize1DF operator+(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Add(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator+(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Add(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator+(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return PxSize1DF::Add(lhs, rhs);
  }

  // op sub

  inline constexpr PxSize1DF::value_type operator-(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Subtract(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator-(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Subtract(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator-(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return PxSize1DF::Subtract(lhs, rhs);
  }

  // op multiply

  inline constexpr PxSize1DF operator*(const PxSize1DF lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Multiply(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator*(const PxSize1DF::value_type lhs, const PxSize1DF rhs) noexcept
  {
    return PxSize1DF::Multiply(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator*(const PxSize1DF lhs, const PxSize1DF::value_type rhs) noexcept
  {
    return PxSize1DF::Multiply(lhs, rhs);
  }

  // op divide

  inline constexpr PxSize1DF operator/(const PxSize1DF lhs, const PxSize1DF rhs)
  {
    return PxSize1DF::Divide(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator/(const PxSize1DF lhs, const PxSize1DF::value_type rhs)
  {
    return PxSize1DF::Divide(lhs, rhs);
  }

  inline constexpr PxSize1DF::value_type operator/(const PxSize1DF::value_type lhs, const PxSize1DF rhs)
  {
    return PxSize1DF::Divide(lhs, rhs);
  }

}

#endif
