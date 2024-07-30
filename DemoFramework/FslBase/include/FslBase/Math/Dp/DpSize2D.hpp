#ifndef FSLBASE_MATH_DP_DPSIZE2D_HPP
#define FSLBASE_MATH_DP_DPSIZE2D_HPP
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
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/DpSize1D.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>
// #include <limits>

namespace Fsl
{
  //! A immutable int32_t based size that will never be negative!
  struct DpSize2D
  {
    using value_type = DpSize1D;
    using raw_value_type = value_type::raw_value_type;

  private:
    value_type m_width;
    value_type m_height;

  public:
    constexpr DpSize2D() noexcept = default;

    constexpr DpSize2D(const value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr DpSize2D(const value_type::value_type width, const value_type::value_type height) noexcept
      : m_width(width >= value_type::value_type(0) ? width : value_type::value_type(0))
      , m_height(height >= value_type::value_type(0) ? height : value_type::value_type(0))
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the width and height are positive.
    constexpr DpSize2D(const value_type::value_type width, const value_type::value_type height, const OptimizationCheckFlag /*unused*/) noexcept
      : m_width(width)
      , m_height(height)
    {
      assert(width >= value_type::value_type(0));
      assert(height >= value_type::value_type(0));
    }

    constexpr inline value_type Width() const noexcept
    {
      return m_width;
    }

    constexpr inline value_type Height() const noexcept
    {
      return m_height;
    }

    constexpr inline void SetWidth(const value_type width) noexcept
    {
      m_width = width;
    }

    constexpr inline void SetWidth(const value_type::value_type width) noexcept
    {
      m_width = DpSize1D(width);
    }

    constexpr inline void SetHeight(const value_type height) noexcept
    {
      m_height = height;
    }

    constexpr inline void SetHeight(const value_type::value_type height) noexcept
    {
      m_height = DpSize1D(height);
    }

    constexpr inline void SetWidth(const value_type::value_type width, const OptimizationCheckFlag /*unused*/) noexcept
    {
      m_width = DpSize1D(width, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void SetHeight(const value_type::value_type height, const OptimizationCheckFlag /*unused*/) noexcept
    {
      m_height = DpSize1D(height, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void AddWidth(const value_type width) noexcept
    {
      m_width = m_width + width;
    }

    constexpr inline void AddWidth(const value_type::value_type width) noexcept
    {
      m_width = DpSize1D(m_width + width);
    }


    constexpr inline void AddHeight(const value_type height) noexcept
    {
      m_height = m_height + height;
    }


    constexpr inline void AddHeight(const value_type::value_type height) noexcept
    {
      m_height = DpSize1D(m_height + height);
    }

    constexpr inline void SetMax(const DpSize2D value)
    {
      m_width = std::max(m_width, value.m_width);
      m_height = std::max(m_height, value.m_height);
    }


    constexpr bool operator==(const DpSize2D rhs) const noexcept
    {
      return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    constexpr bool operator!=(const DpSize2D rhs) const noexcept
    {
      return m_width != rhs.m_width || m_height != rhs.m_height;
    }

    static constexpr DpSize2D Add(const DpSize2D sizeDp, const DpSize2D valueDp) noexcept
    {
      return {sizeDp.m_width + valueDp.m_width, sizeDp.m_height + valueDp.m_height};
    }

    static constexpr DpPoint2 Subtract(const DpSize2D sizeDp, const DpSize2D valueDp) noexcept
    {
      return {sizeDp.m_width - valueDp.m_width, sizeDp.m_height - valueDp.m_height};
    }

    static constexpr DpSize2D Divide(const DpSize2D sizeDp, const value_type value)
    {
      assert(value.RawValue() != 0);
      return {sizeDp.m_width / value, sizeDp.m_height / value};
    }

    static constexpr DpPoint2 Divide(const DpSize2D sizeDp, const value_type::value_type value)
    {
      assert(value.Value != 0);
      return {sizeDp.m_width / value, sizeDp.m_height / value};
    }

    constexpr DpSize2D operator+=(const DpSize2D valueDp) noexcept
    {
      m_width += valueDp.m_width;
      m_height += valueDp.m_height;
      return *this;
    }

    constexpr DpSize2D operator-=(const DpSize2D valueDp) noexcept
    {
      m_width = DpSize1D(m_width - valueDp.m_width);
      m_height = DpSize1D(m_height - valueDp.m_height);
      return *this;
    }

    inline static constexpr DpSize2D Create(const value_type::value_type width, const value_type::value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }

    inline static constexpr DpSize2D Create(const raw_value_type width, raw_value_type height) noexcept
    {
      return {value_type::Create(width), value_type::Create(height)};
    }
  };

  inline constexpr DpSize2D operator+(const DpSize2D lhs, const DpSize2D rhs) noexcept
  {
    return DpSize2D::Add(lhs, rhs);
  }

  inline constexpr DpPoint2 operator+(const DpPoint2 lhs, const DpSize2D rhs) noexcept
  {
    return {lhs.X + rhs.Width(), lhs.Y + rhs.Height()};
  }

  inline constexpr DpPoint2 operator+(const DpSize2D lhs, const DpPoint2 rhs) noexcept
  {
    return {lhs.Width() + rhs.X, lhs.Height() + rhs.Y};
  }


  inline constexpr DpPoint2 operator-(const DpSize2D lhs, const DpSize2D rhs) noexcept
  {
    return DpSize2D::Subtract(lhs, rhs);
  }

  inline constexpr DpPoint2 operator-(const DpPoint2 lhs, const DpSize2D rhs) noexcept
  {
    return {lhs.X - rhs.Width(), lhs.Y - rhs.Height()};
  }

  inline constexpr DpPoint2 operator-(const DpSize2D lhs, const DpPoint2 rhs) noexcept
  {
    return {lhs.Width() - rhs.X, lhs.Height() - rhs.Y};
  }

  inline constexpr DpSize2D operator*(const DpSize2D lhs, const DpSize2D rhs) noexcept
  {
    return {lhs.Width() * rhs.Width(), lhs.Height() * rhs.Height()};
  }

  inline constexpr DpPoint2 operator*(const DpPoint2 lhs, const DpSize2D rhs) noexcept
  {
    return {lhs.X * rhs.Width(), lhs.Y * rhs.Height()};
  }

  inline constexpr DpPoint2 operator*(const DpSize2D lhs, const DpPoint2 rhs) noexcept
  {
    return {lhs.Width() * rhs.X, lhs.Height() * rhs.Y};
  }


  inline constexpr DpSize2D operator/(const DpSize2D lhs, const DpSize2D::value_type rhs) noexcept
  {
    return DpSize2D::Divide(lhs, rhs);
  }
}

#endif
