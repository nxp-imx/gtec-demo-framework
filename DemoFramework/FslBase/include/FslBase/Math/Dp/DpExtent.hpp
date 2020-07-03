#ifndef FSLBASE_MATH_DP_DPEXTENT_HPP
#define FSLBASE_MATH_DP_DPEXTENT_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Math/Dp/DpPointU.hpp>
#include <FslBase/Math/Dp/DpSize.hpp>
#include <cassert>

namespace Fsl
{
  struct DpExtent
  {
    using value_type = uint32_t;

    value_type Width{0};
    value_type Height{0};

    constexpr DpExtent() noexcept = default;

    constexpr DpExtent(const value_type width, const value_type height) noexcept
      : Width(width)
      , Height(height)
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit DpExtent(const DpSize& value) noexcept
      : DpExtent(static_cast<value_type>(value.Width()), static_cast<value_type>(value.Height()))
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit DpExtent(const DpPointU& value) noexcept
      : DpExtent(value.X, value.Y)
    {
    }


    constexpr DpExtent& operator+=(const DpExtent& arg) noexcept
    {
      Width += arg.Width;
      Height += arg.Height;
      return *this;
    }

    constexpr DpExtent& operator-=(const DpExtent& arg) noexcept
    {
      assert(Width >= arg.Width);
      assert(Height >= arg.Height);
      Width -= arg.Width;
      Height -= arg.Height;
      return *this;
    }

    constexpr DpExtent& operator*=(const DpExtent& arg) noexcept
    {
      Width *= arg.Width;
      Height *= arg.Height;
      return *this;
    }

    constexpr DpExtent& operator*=(const value_type arg) noexcept
    {
      Width *= arg;
      Height *= arg;
      return *this;
    }

    constexpr DpExtent& operator/=(const value_type arg)
    {
      assert(arg > 0u);
      Width /= arg;
      Height /= arg;
      return *this;
    }

    constexpr bool operator==(const DpExtent& rhs) const noexcept
    {
      return Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const DpExtent& rhs) const noexcept
    {
      return Width != rhs.Width || Height != rhs.Height;
    }

    // @brief Returns a DpExtent with all components being zero (0, 0)
    static constexpr DpExtent Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr DpExtent operator+(const DpExtent& lhs, const DpExtent& rhs) noexcept
  {
    return {lhs.Width + rhs.Width, lhs.Height + rhs.Height};
  }

  inline constexpr DpExtent operator-(const DpExtent& lhs, const DpExtent& rhs) noexcept
  {
    assert(lhs.Width >= rhs.Width);
    assert(lhs.Height >= rhs.Height);
    return {lhs.Width - rhs.Width, lhs.Height - rhs.Height};
  }

  inline constexpr DpExtent operator*(const DpExtent& lhs, const DpExtent& rhs) noexcept
  {
    return {lhs.Width * rhs.Width, lhs.Height * rhs.Height};
  }

  inline constexpr DpExtent operator*(const DpExtent& lhs, const DpExtent::value_type rhs) noexcept
  {
    return {lhs.Width * rhs, lhs.Height * rhs};
  }

  inline constexpr DpExtent operator*(const DpExtent::value_type lhs, const DpExtent& rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr DpExtent operator/(const DpExtent& lhs, const DpExtent::value_type rhs)
  {
    assert(rhs > 0u);
    return {lhs.Width / rhs, lhs.Height / rhs};
  }

}
#endif
