#ifndef FSLBASE_MATH_DP_DPSIZE2DF_HPP
#define FSLBASE_MATH_DP_DPSIZE2DF_HPP
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
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>
//#include <limits>

namespace Fsl
{
  //! A immutable float based size that will never be negative!
  struct DpSize2DF
  {
    using value_type = DpSize1DF;
    using raw_value_type = float;

  private:
    value_type m_width{};
    value_type m_height{};

  public:
    constexpr DpSize2DF() noexcept = default;

    constexpr DpSize2DF(const value_type width, const value_type height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr DpSize2DF(const value_type::value_type width, const value_type::value_type height) noexcept
      : m_width(width.Value >= static_cast<raw_value_type>(0.0f) ? width : value_type::value_type(0.0f))
      , m_height(height.Value >= static_cast<raw_value_type>(0.0f) ? height : value_type::value_type(0.0f))
    {
    }

    //! If this constructor is used is extremely important to be 100% sure the width and height are positive.
    constexpr DpSize2DF(const value_type width, const value_type height, const OptimizationCheckFlag /*unused*/) noexcept
      : m_width(width)
      , m_height(height)
    {
      assert(width.Value() >= value_type::value_type(0.0f));
      assert(height.Value() >= value_type::value_type(0.0f));
    }

    //! If this constructor is used is extremely important to be 100% sure the width and height are positive.
    constexpr DpSize2DF(const value_type::value_type width, const value_type::value_type height, const OptimizationCheckFlag /*unused*/) noexcept
      : m_width(width)
      , m_height(height)
    {
      assert(width >= value_type::value_type(0.0f));
      assert(height >= value_type::value_type(0.0f));
    }

    constexpr inline value_type Width() const
    {
      return m_width;
    }

    constexpr inline value_type Height() const
    {
      return m_height;
    }

    constexpr inline void SetWidth(const value_type width) noexcept
    {
      m_width = width;
    }

    constexpr inline void SetWidth(const value_type::value_type width) noexcept
    {
      m_width = value_type(width);
    }

    constexpr inline void SetHeight(const value_type height) noexcept
    {
      m_height = height;
    }

    constexpr inline void SetHeight(const value_type::value_type height) noexcept
    {
      m_height = value_type(height);
    }


    constexpr inline void SetWidth(const value_type::value_type width, const OptimizationCheckFlag /*unused*/) noexcept
    {
      m_width = value_type::Create(width, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void SetHeight(const value_type::value_type height, const OptimizationCheckFlag /*unused*/) noexcept
    {
      m_height = value_type::Create(height, OptimizationCheckFlag::NoCheck);
    }

    constexpr inline void AddWidth(const value_type width)
    {
      m_width = m_width + width;
    }

    constexpr inline void AddWidth(const value_type::value_type width)
    {
      m_width = m_width + width;
    }

    constexpr inline void AddHeight(const value_type height)
    {
      m_height = m_height + height;
    }

    constexpr inline void AddHeight(const value_type::value_type height)
    {
      m_height = m_height + height;
    }

    constexpr inline void SetMax(const DpSize2DF value)
    {
      m_width = std::max(m_width, value.m_width);
      m_height = std::max(m_height, value.m_height);
    }


    constexpr bool operator==(const DpSize2DF rhs) const noexcept
    {
      return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    constexpr bool operator!=(const DpSize2DF rhs) const noexcept
    {
      return m_width != rhs.m_width || m_height != rhs.m_height;
    }

    static constexpr DpSize2DF Add(const DpSize2DF sizeDp, const DpSize2DF valueDp)
    {
      // check for overflow
      // assert(sizeDp.m_width <= (std::numeric_limits<value_type>::max() - valueDp.m_width));
      // assert(sizeDp.m_height <= (std::numeric_limits<value_type>::max() - valueDp.m_height));
      return {sizeDp.m_width + valueDp.m_width, sizeDp.m_height + valueDp.m_height, OptimizationCheckFlag::NoCheck};
    }

    static constexpr DpPoint2F Subtract(const DpSize2DF sizeDp, const DpSize2DF valueDp)
    {
      return {sizeDp.m_width - valueDp.m_width, sizeDp.m_height - valueDp.m_height};
    }

    static constexpr DpSize2DF Divide(const DpSize2DF sizeDp, const value_type value)
    {
      return {sizeDp.m_width / value, sizeDp.m_height / value};
    }

    static constexpr DpPoint2F Divide(const DpSize2DF sizeDp, const value_type::value_type value)
    {
      return {sizeDp.m_width / value, sizeDp.m_height / value};
    }

    constexpr DpSize2DF& operator+=(const DpSize2DF valueDp) noexcept
    {
      // assert(m_width <= (std::numeric_limits<value_type>::max() - valueDp.m_width));
      // assert(m_height <= (std::numeric_limits<value_type>::max() - valueDp.m_height));
      m_width += valueDp.m_width;
      m_height += valueDp.m_height;
      return *this;
    }

    constexpr DpSize2DF& operator-=(const DpSize2DF valueDp) noexcept
    {
      m_width = DpSize1DF(m_width - valueDp.m_width);
      m_height = DpSize1DF(m_height - valueDp.m_height);
      return *this;
    }


    inline static constexpr DpSize2DF Create(const value_type::value_type width, const value_type::value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }

    inline static constexpr DpSize2DF Create(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {value_type::Create(width), value_type::Create(height)};
    }
  };

  inline constexpr DpSize2DF operator+(const DpSize2DF lhs, const DpSize2DF rhs) noexcept
  {
    return DpSize2DF::Add(lhs, rhs);
  }

  inline constexpr DpPoint2F operator+(const DpPoint2F lhs, const DpSize2DF rhs) noexcept
  {
    return {lhs.X + rhs.Width().Value(), lhs.Y + rhs.Height().Value()};
  }

  inline constexpr DpPoint2F operator+(const DpSize2DF& lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.Width().Value() + rhs.X, lhs.Height().Value() + rhs.Y};
  }


  inline constexpr DpPoint2F operator-(const DpSize2DF lhs, const DpSize2DF rhs) noexcept
  {
    return DpSize2DF::Subtract(lhs, rhs);
  }

  inline constexpr DpPoint2F operator-(const DpPoint2F lhs, const DpSize2DF rhs) noexcept
  {
    return {lhs.X - rhs.Width(), lhs.Y - rhs.Height()};
  }

  inline constexpr DpPoint2F operator-(const DpSize2DF lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.Width() - rhs.X, lhs.Height() - rhs.Y};
  }

  inline constexpr DpSize2DF operator/(const DpSize2DF lhs, const DpSize2DF::value_type rhs) noexcept
  {
    return DpSize2DF::Divide(lhs, rhs);
  }

  inline constexpr DpPoint2F operator/(const DpSize2DF lhs, const DpSize2DF::value_type::value_type rhs) noexcept
  {
    return DpSize2DF::Divide(lhs, rhs);
  }

  inline constexpr DpSize2DF operator*(const DpSize2DF lhs, const DpSize2DF rhs) noexcept
  {
    return {lhs.Width() * rhs.Width(), lhs.Height() * rhs.Height()};
  }

  inline constexpr DpPoint2F operator*(const DpPoint2F lhs, const DpSize2DF rhs) noexcept
  {
    return {lhs.X * rhs.Width(), lhs.Y * rhs.Height()};
  }

  inline constexpr DpPoint2F operator*(const DpSize2DF lhs, const DpPoint2F rhs) noexcept
  {
    return {lhs.Width() * rhs.X, lhs.Height() * rhs.Y};
  }


}

#endif
