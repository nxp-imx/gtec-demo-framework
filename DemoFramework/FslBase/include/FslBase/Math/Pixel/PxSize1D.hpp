#ifndef FSLBASE_MATH_PIXEL_PXSIZE1D_HPP
#define FSLBASE_MATH_PIXEL_PXSIZE1D_HPP
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
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <compare>
#include <limits>

namespace Fsl
{
  //! A immutable PxValue based size that will never be negative!
  struct PxSize1D
  {
    using value_type = PxValue;
    using unsigned_value_type = PxValueU;
    using raw_value_type = value_type::raw_value_type;
    using raw_unsigned_value_type = unsigned_value_type::raw_value_type;

  private:
    PxValue m_value;

    explicit constexpr PxSize1D(const raw_value_type value) noexcept
      : m_value(value >= 0 ? value : 0)
    {
      assert(m_value >= value_type(0));
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr PxSize1D(const raw_value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(m_value >= value_type(0));
    }

  public:
    constexpr PxSize1D() noexcept = default;

    explicit constexpr PxSize1D(const value_type value) noexcept
      : m_value(value >= value_type() ? value : value_type())
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the value are positive.
    explicit constexpr PxSize1D(const value_type value, const OptimizationCheckFlag /*unused*/) noexcept
      : m_value(value)
    {
      assert(m_value >= value_type());
    }

    // inline constexpr operator value_type() const noexcept
    //{
    //   return m_value;
    // }

    constexpr inline value_type Value() const noexcept
    {
      return m_value;
    }

    constexpr inline unsigned_value_type UnsignedValue() const noexcept
    {
      return PxValueU(static_cast<raw_unsigned_value_type>(m_value.Value));
    }

    constexpr inline raw_value_type RawValue() const noexcept
    {
      return m_value.Value;
    }

    constexpr inline raw_unsigned_value_type RawUnsignedValue() const noexcept
    {
      return static_cast<raw_unsigned_value_type>(m_value.Value);
    }

    inline constexpr const PxSize1D& operator++() noexcept
    {
      ++m_value;
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxSize1D operator++(int) noexcept
    {
      PxSize1D temp(*this);
      ++m_value;
      return temp;
    }

    inline constexpr const PxSize1D& operator--() noexcept
    {
      m_value = m_value.Value > 0 ? --m_value : m_value;
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxSize1D operator--(int) noexcept
    {
      PxSize1D temp(*this);
      m_value = m_value.Value > 0 ? --m_value : m_value;
      return temp;
    }


    constexpr inline void Add(const PxSize1D valuePx) noexcept
    {
      assert(m_value.Value <= (std::numeric_limits<raw_value_type>::max() - valuePx.m_value.Value));

      // Both values are known to be >= 0
      m_value = m_value + valuePx.m_value;
    }

    constexpr inline void Add(const value_type value) noexcept
    {
      // value might be <= 0
      m_value = value_type::Max(m_value + value, value_type());
    }


    constexpr inline void SetMax(const PxSize1D value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value.m_value);
    }

    constexpr inline void SetMax(const value_type value) noexcept
    {
      // m_value is always >= 0
      m_value = value_type::Max(m_value, value);
    }

    static constexpr PxSize1D Add(const PxSize1D sizePx, const PxSize1D valuePx) noexcept
    {
      // Check for overflow
      assert(sizePx.m_value.Value <= (std::numeric_limits<raw_value_type>::max() - valuePx.m_value.Value));

      // Both values are known to be >= 0
      return PxSize1D(sizePx.m_value + valuePx.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Add(const value_type sizePx, const PxSize1D valuePx) noexcept
    {
      return value_type(sizePx + valuePx.m_value);
    }

    static constexpr value_type Add(const PxSize1D sizePx, const value_type valuePx) noexcept
    {
      return value_type(sizePx.m_value + valuePx);
    }

    static constexpr value_type Subtract(const PxSize1D sizePx, const PxSize1D valuePx) noexcept
    {
      return sizePx.m_value - valuePx.m_value;
    }

    static constexpr value_type Subtract(const value_type sizePx, const PxSize1D valuePx) noexcept
    {
      return sizePx - valuePx.m_value;
    }

    static constexpr value_type Subtract(const PxSize1D sizePx, const value_type valuePx) noexcept
    {
      return sizePx.m_value - valuePx;
    }


    static constexpr PxSize1D Multiply(const PxSize1D sizePx, const PxSize1D valuePx) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1D(sizePx.m_value * valuePx.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Multiply(const value_type sizePx, const PxSize1D valuePx) noexcept
    {
      // One of the values might be negative
      return sizePx * valuePx.m_value;
    }

    static constexpr value_type Multiply(const PxSize1D sizePx, const value_type valuePx) noexcept
    {
      // One of the values might be negative
      return sizePx.m_value * valuePx;
    }


    static constexpr PxSize1D Divide(const PxSize1D sizePx, const PxSize1D value)
    {
      // Both values are known to be >= 0
      assert(value.m_value != value_type());
      return PxSize1D(sizePx.m_value / value.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Divide(const value_type valuePx, const PxSize1D sizePx)
    {
      assert(sizePx.Value() != value_type());
      return valuePx / sizePx.m_value;
    }


    static constexpr PxSize1D Modulo(const PxSize1D sizePx, const PxSize1D value)
    {
      // Both values are known to be >= 0
      assert(value.m_value != value_type());
      return PxSize1D(sizePx.m_value % value.m_value, OptimizationCheckFlag::NoCheck);
    }

    static constexpr value_type Divide(const PxSize1D sizePx, const value_type value)
    {
      assert(value != value_type());
      return sizePx.m_value / value;
    }

    constexpr PxSize1D operator+=(const PxSize1D valuePx) noexcept
    {
      // m_value is >= 0 and valuePx >= 0
      assert(m_value.Value <= (std::numeric_limits<raw_value_type>::max() - valuePx.m_value.Value));
      m_value += valuePx.m_value;
      return *this;
    }

    constexpr PxSize1D operator+=(const value_type valuePx) noexcept
    {
      // m_value is >= 0 but valuePx can be negative
      m_value = value_type::Max(m_value + valuePx, value_type());
      return *this;
    }

    constexpr PxSize1D operator-=(const PxSize1D valuePx) noexcept
    {
      // m_value is >= 0 and valuePx >= 0
      m_value = value_type::Max(m_value - valuePx.m_value, value_type());
      return *this;
    }

    constexpr PxSize1D operator-=(const PxSize1D::value_type valuePx) noexcept
    {
      // m_value is >= 0 but valuePx can be negative
      m_value = value_type::Max(m_value - valuePx, value_type());
      return *this;
    }

    static constexpr PxSize1D Min(const PxSize1D val0, const PxSize1D val1) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1D(value_type::Min(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1D Min(const value_type val0, const PxSize1D val1) noexcept
    {
      // One of the values might be < 0
      return PxSize1D(value_type::Min(val0, val1.m_value));
    }

    static constexpr PxSize1D Min(const PxSize1D val0, const value_type val1) noexcept
    {
      // One of the values might be < 0
      return PxSize1D(value_type::Min(val0.m_value, val1));
    }

    static constexpr PxSize1D Min(const value_type val0, const value_type val1) noexcept
    {
      // Both values might be < 0
      return PxSize1D(value_type::Min(val0, val1));
    }

    static constexpr PxSize1D Max(const PxSize1D val0, const PxSize1D val1) noexcept
    {
      // Both values are known to be >= 0
      return PxSize1D(value_type::Max(val0.m_value, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1D Max(const value_type val0, const PxSize1D val1) noexcept
    {
      // One of the values are >= 0
      return PxSize1D(value_type::Max(val0, val1.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1D Max(const PxSize1D val0, const value_type val1) noexcept
    {
      // One of the values are >= 0
      return PxSize1D(value_type::Max(val0.m_value, val1), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1D Max(const value_type val0, const value_type val1) noexcept
    {
      // Both values might be < 0
      return PxSize1D(value_type::Max(val0, val1));
    }

    static constexpr PxSize1D Max(const PxSize1D val0, const PxSize1D val1, const PxSize1D val2) noexcept
    {
      // All values are known to be >= 0
      return PxSize1D(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), OptimizationCheckFlag::NoCheck);
    }

    static constexpr PxSize1D Max(const PxSize1D val0, const PxSize1D val1, const PxSize1D val2, const PxSize1D val3) noexcept
    {
      // All values are known to be >= 0
      return PxSize1D(value_type::Max(value_type::Max(value_type::Max(val0.m_value, val1.m_value), val2.m_value), val3.m_value),
                      OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1D Create(const PxSize1D value) noexcept
    {
      return value;
    }

    inline static constexpr PxSize1D Create(const value_type value) noexcept
    {
      return PxSize1D(value);
    }

    inline static constexpr PxSize1D Create(const raw_value_type value) noexcept
    {
      return PxSize1D(value);
    }

    inline static constexpr PxSize1D Create(const value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return PxSize1D(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1D Create(const raw_value_type value, const OptimizationCheckFlag /*flag*/) noexcept
    {
      return PxSize1D(value, OptimizationCheckFlag::NoCheck);
    }


    inline static constexpr PxSize1D UncheckedCreate(const value_type value) noexcept
    {
      return PxSize1D(value, OptimizationCheckFlag::NoCheck);
    }

    inline static constexpr PxSize1D UncheckedCreate(const raw_value_type value) noexcept
    {
      return PxSize1D(value, OptimizationCheckFlag::NoCheck);
    }

    // op<==>
    constexpr auto operator<=>(const PxSize1D& other) const noexcept = default;
    // op==
    constexpr bool operator==(const PxSize1D& other) const noexcept = default;
  };


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // PxSize1D::value_type
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const PxSize1D lhs, const PxSize1D::value_type rhs) noexcept
  {
    return lhs.Value() <=> rhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr bool operator==(const PxSize1D lhs, const PxSize1D::value_type rhs) noexcept
  {
    return lhs.Value() == rhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op add
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxSize1D operator+(const PxSize1D lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Add(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator+(const PxSize1D::value_type lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Add(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator+(const PxSize1D lhs, const PxSize1D::value_type rhs) noexcept
  {
    return PxSize1D::Add(lhs, rhs);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op sub
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxSize1D::value_type operator-(const PxSize1D lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Subtract(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator-(const PxSize1D::value_type lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Subtract(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator-(const PxSize1D lhs, const PxSize1D::value_type rhs) noexcept
  {
    return PxSize1D::Subtract(lhs, rhs);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op multiply
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxSize1D operator*(const PxSize1D lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Multiply(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator*(const PxSize1D::value_type lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::Multiply(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator*(const PxSize1D lhs, const PxSize1D::value_type rhs) noexcept
  {
    return PxSize1D::Multiply(lhs, rhs);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op divide
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxSize1D operator/(const PxSize1D lhs, const PxSize1D rhs)
  {
    return PxSize1D::Divide(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator/(const PxSize1D lhs, const PxSize1D::value_type rhs)
  {
    return PxSize1D::Divide(lhs, rhs);
  }

  inline constexpr PxSize1D::value_type operator/(const PxSize1D::value_type lhs, const PxSize1D rhs)
  {
    return PxSize1D::Divide(lhs, rhs);
  }


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op modulo
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxSize1D operator%(const PxSize1D lhs, const PxSize1D rhs)
  {
    return PxSize1D::Modulo(lhs, rhs);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op +=
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxValue operator+=(PxValue& rLhs, const PxSize1D rhs) noexcept
  {
    rLhs += rhs.Value();
    return rLhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op -=
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxValue operator-=(PxValue& rLhs, const PxSize1D rhs) noexcept
  {
    rLhs -= rhs.Value();
    return rLhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // op *=
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr PxValue operator*=(PxValue& rLhs, const PxSize1D rhs) noexcept
  {
    rLhs *= rhs.Value();
    return rLhs;
  }
}

#endif
