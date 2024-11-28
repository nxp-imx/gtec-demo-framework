#ifndef FSLBASE_MATH_PIXEL_PXSIZE2DF_HPP
#define FSLBASE_MATH_PIXEL_PXSIZE2DF_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/Pixel/PxSize1DF.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>

namespace Fsl
{
  //! A immutable float based size that will never be negative!
  struct PxSize2DF
  {
    using value_type = PxSize1DF;
    using raw_value_type = value_type::raw_value_type;

  private:
    PxSize1DF m_width;
    PxSize1DF m_height;

  public:
    constexpr PxSize2DF() noexcept = default;


    explicit constexpr PxSize2DF(const PxVector2 value) noexcept
      : m_width(PxSize1DF::Create(value.X))
      , m_height(PxSize1DF::Create(value.Y))
    {
    }

    constexpr PxSize2DF(const value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxSize2DF(const value_type::value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxSize2DF(const value_type width, const value_type::value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxSize2DF(const value_type::value_type width, const value_type::value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr inline PxVector2 Value() const noexcept
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


    constexpr inline raw_value_type RawWidth() const noexcept
    {
      return m_width.RawValue();
    }

    constexpr inline raw_value_type RawHeight() const noexcept
    {
      return m_height.RawValue();
    }

    constexpr inline void SetWidth(const value_type width) noexcept
    {
      m_width = width;
    }

    constexpr inline void SetHeight(const value_type height) noexcept
    {
      m_height = height;
    }

    constexpr inline void SetWidth(const value_type::value_type width) noexcept
    {
      m_width = value_type(width);
    }

    constexpr inline void SetHeight(const value_type::value_type height) noexcept
    {
      m_height = value_type(height);
    }


    constexpr inline void AddWidth(const value_type width)
    {
      m_width = m_width + width;
    }

    constexpr inline void AddHeight(const value_type height)
    {
      m_height = m_height + height;
    }


    constexpr inline void AddWidth(const value_type::value_type width)
    {
      m_width += width;
    }

    constexpr inline void AddHeight(const value_type::value_type height)
    {
      m_height += height;
    }

    constexpr inline void SetMax(const PxSize2DF value) noexcept
    {
      m_width = value_type::Max(m_width, value.m_width);
      m_height = value_type::Max(m_height, value.m_height);
    }

    //! @brief Calculates the length of the size squared.
    constexpr float LengthSquared() const
    {
      return (m_width.RawValue() * m_width.RawValue()) + (m_height.RawValue() * m_height.RawValue());
    }

    constexpr bool operator==(const PxSize2DF rhs) const noexcept
    {
      return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    constexpr bool operator!=(const PxSize2DF rhs) const noexcept
    {
      return m_width != rhs.m_width || m_height != rhs.m_height;
    }

    static constexpr PxSize2DF Add(const PxSize2DF sizePx, const PxSize2DF valuePx)
    {
      return {sizePx.m_width + valuePx.m_width, sizePx.m_height + valuePx.m_height};
    }

    static constexpr PxVector2 Subtract(const PxSize2DF sizePx, const PxSize2DF valuePx)
    {
      return {sizePx.m_width - valuePx.m_width, sizePx.m_height - valuePx.m_height};
    }

    static constexpr PxSize2DF Divide(const PxSize2DF sizePx, const value_type value)
    {
      return {sizePx.m_width / value, sizePx.m_height / value};
    }

    constexpr PxSize2DF& operator+=(const PxSize2DF valuePx) noexcept
    {
      m_width += valuePx.m_width;
      m_height += valuePx.m_height;
      return *this;
    }

    constexpr PxSize2DF& operator-=(const PxSize2DF valuePx) noexcept
    {
      m_width -= valuePx.m_width;
      m_height -= valuePx.m_height;
      return *this;
    }

    static constexpr PxSize2DF Min(const PxSize2DF val0, const PxSize2DF val1) noexcept
    {
      return {value_type::Min(val0.m_width, val1.m_width), value_type::Min(val0.m_height, val1.m_height)};
    }

    static constexpr PxSize2DF Max(const PxSize2DF val0, const PxSize2DF val1) noexcept
    {
      return {value_type::Max(val0.m_width, val1.m_width), value_type::Max(val0.m_height, val1.m_height)};
    }

    inline static constexpr PxSize2DF Create(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {PxSize1DF::Create(width), PxSize1DF::Create(height)};
    }

    //! If this is used is extremely important to be 100% sure the width and height are positive.
    inline static constexpr PxSize2DF UncheckedCreate(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {PxSize1DF::UncheckedCreate(width), PxSize1DF::UncheckedCreate(height)};
    }
  };

  inline constexpr PxSize2DF operator+(const PxSize2DF lhs, const PxSize2DF rhs) noexcept
  {
    return PxSize2DF::Add(lhs, rhs);
  }

  inline constexpr PxVector2 operator+(const PxVector2 lhs, const PxSize2DF rhs) noexcept
  {
    return {lhs.X + rhs.Width(), lhs.Y + rhs.Height()};
  }

  inline constexpr PxVector2 operator+(const PxSize2DF lhs, const PxVector2 rhs) noexcept
  {
    return {lhs.Width() + rhs.X, lhs.Height() + rhs.Y};
  }


  inline constexpr PxVector2 operator-(const PxSize2DF lhs, const PxSize2DF rhs) noexcept
  {
    return PxSize2DF::Subtract(lhs, rhs);
  }

  inline constexpr PxVector2 operator-(const PxVector2 lhs, const PxSize2DF rhs) noexcept
  {
    return {lhs.X - rhs.Width(), lhs.Y - rhs.Height()};
  }

  inline constexpr PxVector2 operator-(const PxSize2DF lhs, const PxVector2 rhs) noexcept
  {
    return {lhs.Width() - rhs.X, lhs.Height() - rhs.Y};
  }


  inline constexpr PxSize2DF operator/(const PxSize2DF lhs, const PxSize2DF::value_type rhs) noexcept
  {
    return PxSize2DF::Divide(lhs, rhs);
  }

  inline constexpr PxVector2 operator*(const PxVector2 lhs, const PxSize2DF rhs) noexcept
  {
    return {lhs.X * rhs.Width(), lhs.Y * rhs.Height()};
  }

  inline constexpr PxVector2 operator*(const PxSize2DF lhs, const PxVector2 rhs) noexcept
  {
    return {lhs.Width() * rhs.X, lhs.Height() * rhs.Y};
  }


  // op +=

  inline constexpr PxVector2 operator+=(PxVector2& rLhs, const PxSize2DF rhs) noexcept
  {
    rLhs += rhs.Value();
    return rLhs;
  }

  // op -=

  inline constexpr PxVector2 operator-=(PxVector2& rLhs, const PxSize2DF rhs) noexcept
  {
    rLhs -= rhs.Value();
    return rLhs;
  }

  // op *=

  inline constexpr PxVector2 operator*=(PxVector2& rLhs, const PxSize2DF rhs) noexcept
  {
    rLhs *= rhs.Value();
    return rLhs;
  }

}

#endif
