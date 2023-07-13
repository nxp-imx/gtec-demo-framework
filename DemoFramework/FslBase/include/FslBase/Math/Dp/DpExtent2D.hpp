#ifndef FSLBASE_MATH_DP_DPEXTENT2D_HPP
#define FSLBASE_MATH_DP_DPEXTENT2D_HPP
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
#include <FslBase/Math/Dp/DpPoint2U.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslBase/Math/Dp/DpValueU.hpp>
#include <cassert>

namespace Fsl
{
  struct DpExtent2D
  {
    using value_type = DpValueU;
    using raw_value_type = value_type::raw_value_type;

    value_type Width{0};
    value_type Height{0};

    constexpr DpExtent2D() noexcept = default;

    constexpr DpExtent2D(const value_type width, const value_type height) noexcept
      : Width(width)
      , Height(height)
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit DpExtent2D(const DpSize2D value) noexcept
      : DpExtent2D(static_cast<value_type>(value.Width().RawValue()), static_cast<value_type>(value.Height().RawValue()))
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit DpExtent2D(const DpPoint2U value) noexcept
      : DpExtent2D(value.X, value.Y)
    {
    }


    constexpr DpExtent2D& operator+=(const DpExtent2D arg) noexcept
    {
      Width += arg.Width;
      Height += arg.Height;
      return *this;
    }

    constexpr DpExtent2D& operator-=(const DpExtent2D arg) noexcept
    {
      assert(Width >= arg.Width);
      assert(Height >= arg.Height);
      Width -= arg.Width;
      Height -= arg.Height;
      return *this;
    }

    constexpr DpExtent2D& operator*=(const DpExtent2D arg) noexcept
    {
      Width *= arg.Width;
      Height *= arg.Height;
      return *this;
    }

    constexpr DpExtent2D& operator*=(const value_type arg) noexcept
    {
      Width *= arg;
      Height *= arg;
      return *this;
    }

    constexpr DpExtent2D& operator/=(const value_type arg)
    {
      Width /= arg;
      Height /= arg;
      return *this;
    }

    constexpr bool operator==(const DpExtent2D rhs) const noexcept
    {
      return Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const DpExtent2D rhs) const noexcept
    {
      return Width != rhs.Width || Height != rhs.Height;
    }

    // @brief Returns a DpExtent2D with all components being zero (0, 0)
    static constexpr DpExtent2D Zero() noexcept
    {
      return {};
    }

    inline static constexpr DpExtent2D Create(const value_type::value_type width, const value_type::value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }
  };

  inline constexpr DpExtent2D operator+(const DpExtent2D lhs, const DpExtent2D rhs) noexcept
  {
    return {lhs.Width + rhs.Width, lhs.Height + rhs.Height};
  }

  inline constexpr DpExtent2D operator-(const DpExtent2D lhs, const DpExtent2D rhs) noexcept
  {
    assert(lhs.Width >= rhs.Width);
    assert(lhs.Height >= rhs.Height);
    return {lhs.Width - rhs.Width, lhs.Height - rhs.Height};
  }

  inline constexpr DpExtent2D operator*(const DpExtent2D lhs, const DpExtent2D rhs) noexcept
  {
    return {lhs.Width * rhs.Width, lhs.Height * rhs.Height};
  }

  inline constexpr DpExtent2D operator*(const DpExtent2D lhs, const DpExtent2D::value_type rhs) noexcept
  {
    return {lhs.Width * rhs, lhs.Height * rhs};
  }

  inline constexpr DpExtent2D operator*(const DpExtent2D::value_type lhs, const DpExtent2D rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpExtent2D operator/(const DpExtent2D lhs, const DpExtent2D::value_type rhs)
  {
    return {lhs.Width / rhs, lhs.Height / rhs};
  }

}
#endif
