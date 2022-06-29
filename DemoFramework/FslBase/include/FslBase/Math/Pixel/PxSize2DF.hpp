#ifndef FSLBASE_MATH_PIXEL_PXSIZE2DF_HPP
#define FSLBASE_MATH_PIXEL_PXSIZE2DF_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  //! A immutable float based size that will never be negative!
  struct PxSize2DF
  {
    using value_type = float;

  private:
    value_type m_width{};
    value_type m_height{};

  public:
    constexpr PxSize2DF() noexcept = default;

    explicit constexpr PxSize2DF(const PxVector2 value) noexcept
      : m_width(value.X >= 0 ? value.X : 0)
      , m_height(value.Y >= 0 ? value.Y : 0)
    {
    }

    constexpr PxSize2DF(const value_type width, const value_type height) noexcept
      : m_width(width >= 0.0f ? width : 0.0f)
      , m_height(height >= 0.0f ? height : 0.0f)
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the width and height are positive.
    constexpr PxSize2DF(const value_type width, const value_type height, const OptimizationCheckFlag /*unused*/) noexcept
      : m_width(width)
      , m_height(height)
    {
      assert(width >= 0);
      assert(height >= 0);
    }

    constexpr inline value_type Width() const
    {
      return m_width;
    }

    constexpr inline value_type Height() const
    {
      return m_height;
    }

    constexpr inline void SetWidth(const value_type width)
    {
      m_width = std::max(width, 0.0f);
    }

    constexpr inline void SetHeight(const value_type height)
    {
      m_height = std::max(height, 0.0f);
    }

    constexpr inline void SetWidth(const value_type width, const OptimizationCheckFlag /*unused*/)
    {
      assert(width >= 0.0f);
      m_width = width;
    }

    constexpr inline void SetHeight(const value_type height, const OptimizationCheckFlag /*unused*/)
    {
      assert(height >= 0.0f);
      m_height = height;
    }

    constexpr inline void AddWidth(const value_type width)
    {
      m_width = std::max(m_width + width, 0.0f);
    }

    constexpr inline void AddHeight(const value_type height)
    {
      m_height = std::max(m_height + height, 0.0f);
    }

    constexpr inline void SetMax(const PxSize2DF value)
    {
      m_width = std::max(m_width, value.m_width);
      m_height = std::max(m_height, value.m_height);
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
      // check for overflow
      assert(sizePx.m_width <= (std::numeric_limits<value_type>::max() - valuePx.m_width));
      assert(sizePx.m_height <= (std::numeric_limits<value_type>::max() - valuePx.m_height));
      return {sizePx.m_width + valuePx.m_width, sizePx.m_height + valuePx.m_height, OptimizationCheckFlag::NoCheck};
    }

    static constexpr PxVector2 Subtract(const PxSize2DF sizePx, const PxSize2DF valuePx)
    {
      return {sizePx.m_width - valuePx.m_width, sizePx.m_height - valuePx.m_height};
    }

    static constexpr PxSize2DF Divide(const PxSize2DF sizePx, const PxSize1DF value)
    {
      assert(value.Value().Value != 0.0f);
      return {sizePx.m_width / value.RawValue(), sizePx.m_height / value.RawValue(), OptimizationCheckFlag::NoCheck};
    }

    static constexpr PxSize2DF Divide(const PxSize2DF sizePx, const value_type value)
    {
      assert(value != 0.0f);
      return {sizePx.m_width / value, sizePx.m_height / value, OptimizationCheckFlag::NoCheck};
    }

    constexpr PxSize2DF& operator+=(const PxSize2DF valuePx) noexcept
    {
      assert(m_width <= (std::numeric_limits<value_type>::max() - valuePx.m_width));
      assert(m_height <= (std::numeric_limits<value_type>::max() - valuePx.m_height));
      m_width += valuePx.m_width;
      m_height += valuePx.m_height;
      return *this;
    }

    constexpr PxSize2DF& operator-=(const PxSize2DF valuePx) noexcept
    {
      m_width = std::max(m_width - valuePx.m_width, 0.0f);
      m_height = std::max(m_height - valuePx.m_height, 0.0f);
      return *this;
    }

    static constexpr PxSize2DF Min(const PxSize2DF val0, const PxSize2DF val1)
    {
      return {std::min(val0.m_width, val1.m_width), std::min(val0.m_height, val1.m_height)};
    }

    static constexpr PxSize2DF Max(const PxSize2DF val0, const PxSize2DF val1)
    {
      return {std::max(val0.m_width, val1.m_width), std::max(val0.m_height, val1.m_height)};
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
}

#endif
