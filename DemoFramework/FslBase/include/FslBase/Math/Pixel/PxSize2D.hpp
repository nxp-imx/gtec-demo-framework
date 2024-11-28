#ifndef FSLBASE_MATH_PIXEL_PXSIZE2D_HPP
#define FSLBASE_MATH_PIXEL_PXSIZE2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>

namespace Fsl
{
  //! A immutable int32_t based size that will never be negative!
  struct PxSize2D
  {
    using value_type = PxSize1D;
    using unsigned_value_type = PxValueU;
    using raw_value_type = value_type::raw_value_type;
    using raw_unsigned_value_type = unsigned_value_type::raw_value_type;

  private:
    PxSize1D m_width;
    PxSize1D m_height;

  public:
    constexpr PxSize2D() noexcept = default;


    explicit constexpr PxSize2D(const PxPoint2 value) noexcept
      : m_width(value_type::Create(value.X))
      , m_height(value_type::Create(value.Y))
    {
    }

    constexpr PxSize2D(const value_type::value_type width, const value_type::value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxSize2D(const value_type::value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxSize2D(const value_type width, const value_type::value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }


    constexpr PxSize2D(const value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr inline PxPoint2 Value() const noexcept
    {
      return {m_width, m_height};
    }

    constexpr inline value_type Width() const noexcept
    {
      return m_width;
    }

    constexpr inline value_type Height() const noexcept
    {
      return m_height;
    }

    constexpr inline unsigned_value_type UnsignedWidth() const noexcept
    {
      return m_width.UnsignedValue();
    }

    constexpr inline unsigned_value_type UnsignedHeight() const noexcept
    {
      return m_height.UnsignedValue();
    }

    constexpr inline raw_value_type RawWidth() const noexcept
    {
      return m_width.RawValue();
    }

    constexpr inline raw_value_type RawHeight() const noexcept
    {
      return m_height.RawValue();
    }

    constexpr inline raw_unsigned_value_type RawUnsignedWidth() const noexcept
    {
      return m_width.RawUnsignedValue();
    }

    constexpr inline raw_unsigned_value_type RawUnsignedHeight() const noexcept
    {
      return m_height.RawUnsignedValue();
    }

    constexpr inline void SetWidth(const value_type width)
    {
      m_width = width;
    }


    constexpr inline void SetHeight(const value_type height)
    {
      m_height = height;
    }

    constexpr inline void SetWidth(const PxValue width)
    {
      m_width = value_type(width);
    }

    constexpr inline void SetHeight(const PxValue height)
    {
      m_height = value_type(height);
    }

    constexpr inline void SetWidth(const raw_value_type width)
    {
      m_width = value_type::Create(width);
    }

    constexpr inline void SetHeight(const raw_value_type height)
    {
      m_height = value_type::Create(height);
    }

    constexpr inline void SetWidth(const raw_value_type width, const OptimizationCheckFlag /*unused*/)
    {
      m_width = value_type::Create(width, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void SetHeight(const raw_value_type height, const OptimizationCheckFlag /*unused*/)
    {
      m_height = value_type::Create(height, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void AddWidth(const value_type width)
    {
      m_width = value_type::Add(m_width, width);
    }

    constexpr inline void AddWidth(const PxValue width)
    {
      m_width = value_type(value_type::Add(m_width, width));
    }

    constexpr inline void AddHeight(const value_type height)
    {
      m_height = value_type::Add(m_height, height);
    }

    constexpr inline void AddHeight(const PxValue height)
    {
      m_height = value_type(value_type::Add(m_height, height));
    }

    constexpr inline void SetMax(const PxSize2D value)
    {
      m_width.SetMax(value.m_width);
      m_height.SetMax(value.m_height);
    }

    constexpr inline void AddWidth(const raw_value_type width)
    {
      m_width += PxValue::Create(width);
    }

    constexpr inline void AddHeight(const raw_value_type height)
    {
      m_height += PxValue::Create(height);
    }

    static constexpr PxSize2D Add(const PxSize2D sizePx, const PxSize2D valuePx)
    {
      return {value_type::Add(sizePx.m_width, valuePx.m_width), value_type::Add(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxPoint2 Add(const PxPoint2 pointPx, const PxSize2D valuePx)
    {
      return {value_type::Add(PxValue(pointPx.X), valuePx.m_width), value_type::Add(PxValue(pointPx.Y), valuePx.m_height)};
    }

    static constexpr PxPoint2 Add(const PxSize2D valuePx, const PxPoint2 pointPx)
    {
      return {value_type::Add(valuePx.m_width, PxValue(pointPx.X)), value_type::Add(valuePx.m_height, PxValue(pointPx.Y))};
    }

    static constexpr PxPoint2 Subtract(const PxSize2D sizePx, const PxSize2D valuePx)
    {
      return {value_type::Subtract(sizePx.m_width, valuePx.m_width), value_type::Subtract(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxPoint2 Subtract(const PxPoint2 sizePx, const PxSize2D valuePx)
    {
      return {value_type::Subtract(PxValue(sizePx.X), valuePx.m_width), value_type::Subtract(PxValue(sizePx.Y), valuePx.m_height)};
    }

    static constexpr PxPoint2 Subtract(const PxSize2D sizePx, const PxPoint2 valuePx)
    {
      return {value_type::Subtract(sizePx.m_width, PxValue(valuePx.X)), value_type::Subtract(sizePx.m_height, PxValue(valuePx.Y))};
    }

    static constexpr PxSize2D Multiply(const PxSize2D sizePx, const PxSize2D size2Px)
    {
      return {value_type::Multiply(sizePx.m_width, size2Px.m_width), value_type::Multiply(sizePx.m_height, size2Px.m_height)};
    }

    static constexpr PxSize2D Multiply(const PxSize2D sizePx, const PxSize1D size2Px)
    {
      return {sizePx.Width() * size2Px, sizePx.Height() * size2Px};
    }

    static constexpr PxPoint2 Multiply(const PxPoint2 pointPx, const PxSize2D sizePx)
    {
      return {value_type::Multiply(PxValue(pointPx.X), sizePx.m_width), value_type::Multiply(PxValue(pointPx.Y), sizePx.m_height)};
    }

    static constexpr PxPoint2 Multiply(const PxSize2D sizePx, const PxPoint2 pointPx)
    {
      return {value_type::Multiply(sizePx.m_width, PxValue(pointPx.X)), value_type::Multiply(sizePx.m_height, PxValue(pointPx.Y))};
    }

    static constexpr PxSize2D Divide(const PxSize2D sizePx, const PxSize2D valuePx)
    {
      return {value_type::Divide(sizePx.m_width, valuePx.m_width), value_type::Divide(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxPoint2 Divide(const PxPoint2 value, const PxSize2D sizePx)
    {
      return {value_type::Divide(PxValue(value.X), sizePx.m_width), value_type::Divide(PxValue(value.Y), sizePx.m_height)};
    }

    static constexpr PxPoint2 Divide(const PxSize2D sizePx, const PxPoint2 valuePx)
    {
      return {value_type::Divide(sizePx.m_width, PxValue(valuePx.X)), value_type::Divide(sizePx.m_height, PxValue(valuePx.Y))};
    }

    static constexpr PxSize2D Divide(const PxSize2D sizePx, const value_type value)
    {
      return {value_type::Divide(sizePx.m_width, value), value_type::Divide(sizePx.m_height, value)};
    }

    static constexpr PxSize2D Divide(const PxSize2D sizePx, const value_type::value_type value)
    {
      return {value_type::Divide(sizePx.m_width, value), value_type::Divide(sizePx.m_height, value)};
    }

    constexpr PxSize2D& operator+=(const PxSize2D valuePx) noexcept
    {
      m_width += valuePx.m_width;
      m_height += valuePx.m_height;
      return *this;
    }

    constexpr PxSize2D& operator-=(const PxSize2D valuePx) noexcept
    {
      m_width -= valuePx.m_width;
      m_height -= valuePx.m_height;
      return *this;
    }

    static constexpr PxSize2D Min(const PxSize2D val0, const PxSize2D val1)
    {
      return {value_type::Min(val0.m_width, val1.m_width), value_type::Min(val0.m_height, val1.m_height)};
    }

    static constexpr PxSize2D Max(const PxSize2D val0, const PxSize2D val1)
    {
      return {value_type::Max(val0.m_width, val1.m_width), value_type::Max(val0.m_height, val1.m_height)};
    }

    static constexpr PxSize2D Max(const PxSize2D val0, const PxSize2D val1, const PxSize2D val2)
    {
      return {value_type::Max(val0.m_width, val1.m_width, val2.m_width), value_type::Max(val0.m_height, val1.m_height, val2.m_height)};
    }

    static constexpr PxSize2D Max(const PxSize2D val0, const PxSize2D val1, const PxSize2D val2, const PxSize2D val3)
    {
      return {value_type::Max(val0.m_width, val1.m_width, val2.m_width, val3.m_width),
              value_type::Max(val0.m_height, val1.m_height, val2.m_height, val3.m_height)};
    }


    static constexpr PxSize2D Flip(const PxSize2D val)
    {
      // coverity[swapped_arguments]
      return {val.Height(), val.Width()};
    }

    inline static constexpr PxSize2D Create(const raw_value_type width, const value_type height) noexcept
    {
      return {value_type::Create(width), height};
    }

    inline static constexpr PxSize2D Create(const value_type width, const raw_value_type height) noexcept
    {
      return {width, value_type::Create(height)};
    }

    inline static constexpr PxSize2D Create(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {value_type::Create(width), value_type::Create(height)};
    }

    //! If this constructor is used is extremely important to be 100% sure the width and height are positive.
    inline static constexpr PxSize2D UncheckedCreate(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {value_type::UncheckedCreate(width), value_type::UncheckedCreate(height)};
    }
  };

  // op==

  constexpr bool operator==(const PxSize2D lhs, const PxSize2D rhs) noexcept
  {
    return lhs.Width() == rhs.Width() && lhs.Height() == rhs.Height();
  }

  // PxPoint2
  constexpr bool operator==(const PxPoint2 lhs, const PxSize2D rhs) noexcept
  {
    return lhs.X == rhs.Width() && lhs.Y == rhs.Height();
  }

  constexpr bool operator==(const PxSize2D lhs, const PxPoint2 rhs) noexcept
  {
    return lhs.Width() == rhs.X && lhs.Height() == rhs.Y;
  }

  // op!=

  constexpr bool operator!=(const PxSize2D lhs, const PxSize2D rhs) noexcept
  {
    return lhs.Width() != rhs.Width() || lhs.Height() != rhs.Height();
  }

  // PxPoint2
  constexpr bool operator!=(const PxPoint2 lhs, const PxSize2D rhs) noexcept
  {
    return lhs.X != rhs.Width() || lhs.Y != rhs.Height();
  }

  constexpr bool operator!=(const PxSize2D lhs, const PxPoint2 rhs) noexcept
  {
    return lhs.Width() != rhs.X || lhs.Height() != rhs.Y;
  }

  // op add

  inline constexpr PxSize2D operator+(const PxSize2D lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Add(lhs, rhs);
  }

  // PxPoint2
  inline constexpr PxPoint2 operator+(const PxPoint2 lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Add(lhs, rhs);
  }

  inline constexpr PxPoint2 operator+(const PxSize2D lhs, const PxPoint2 rhs) noexcept
  {
    return PxSize2D::Add(lhs, rhs);
  }

  // op sub

  inline constexpr PxPoint2 operator-(const PxSize2D lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Subtract(lhs, rhs);
  }

  // PxPoint2
  inline constexpr PxPoint2 operator-(const PxPoint2 lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Subtract(lhs, rhs);
  }

  inline constexpr PxPoint2 operator-(const PxSize2D lhs, const PxPoint2 rhs) noexcept
  {
    return PxSize2D::Subtract(lhs, rhs);
  }

  // op multiply

  inline constexpr PxSize2D operator*(const PxSize2D lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Multiply(lhs, rhs);
  }

  inline constexpr PxSize2D operator*(const PxSize2D lhs, const PxSize1D rhs) noexcept
  {
    return PxSize2D::Multiply(lhs, rhs);
  }

  // PxPoint2
  inline constexpr PxPoint2 operator*(const PxPoint2 lhs, const PxSize2D rhs) noexcept
  {
    return PxSize2D::Multiply(lhs, rhs);
  }

  inline constexpr PxPoint2 operator*(const PxSize2D lhs, const PxPoint2 rhs) noexcept
  {
    return PxSize2D::Multiply(lhs, rhs);
  }

  // op divide
  inline constexpr PxSize2D operator/(const PxSize2D lhs, const PxSize2D rhs)
  {
    return PxSize2D::Divide(lhs, rhs);
  }

  // PxPoint2
  inline constexpr PxPoint2 operator/(const PxPoint2 lhs, const PxSize2D rhs)
  {
    return PxSize2D::Divide(lhs, rhs);
  }

  inline constexpr PxPoint2 operator/(const PxSize2D lhs, const PxPoint2 rhs)
  {
    return PxSize2D::Divide(lhs, rhs);
  }


  // PxSize2D::value_type
  inline constexpr PxSize2D operator/(const PxSize2D lhs, const PxSize2D::value_type rhs) noexcept
  {
    return PxSize2D::Divide(lhs, rhs);
  }

  // PxPoint2::value_type
  inline constexpr PxSize2D operator/(const PxSize2D lhs, const PxPoint2::value_type rhs) noexcept
  {
    return PxSize2D::Divide(lhs, rhs);
  }

  // op +=

  inline constexpr PxPoint2 operator+=(PxPoint2& rLhs, const PxSize2D rhs) noexcept
  {
    rLhs += rhs.Value();
    return rLhs;
  }

  // op -=

  inline constexpr PxPoint2 operator-=(PxPoint2& rLhs, const PxSize2D rhs) noexcept
  {
    rLhs -= rhs.Value();
    return rLhs;
  }

  // op *=

  inline constexpr PxSize2D operator*=(PxSize2D& rLhs, const PxSize2D rhs) noexcept
  {
    rLhs = rLhs * rhs;
    return rLhs;
  }

  inline constexpr PxSize2D operator*=(PxSize2D& rLhs, const PxSize1D rhs) noexcept
  {
    rLhs = PxSize2D(rLhs.Width() * rhs, rLhs.Height() * rhs);
    return rLhs;
  }

  inline constexpr PxPoint2 operator*=(PxPoint2& rLhs, const PxSize2D rhs) noexcept
  {
    rLhs *= rhs.Value();
    return rLhs;
  }
}

#endif
