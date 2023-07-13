#ifndef FSLBASE_MATH_PIXEL_PXEXTENT2D_HPP
#define FSLBASE_MATH_PIXEL_PXEXTENT2D_HPP
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
#include <FslBase/Math/Pixel/PxPoint2U.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <cassert>

namespace Fsl
{
  struct PxExtent2D
  {
    using value_type = PxValueU;
    using raw_value_type = value_type::raw_value_type;

    value_type Width{0};
    value_type Height{0};

    constexpr PxExtent2D() noexcept = default;

    constexpr PxExtent2D(const value_type width, const value_type height) noexcept
      : Width(width)
      , Height(height)
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit PxExtent2D(const PxSize2D& value) noexcept
      : PxExtent2D(static_cast<value_type>(value.RawWidth()), static_cast<value_type>(value.RawHeight()))
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr explicit PxExtent2D(const PxPoint2U value) noexcept
      : PxExtent2D(value.X, value.Y)
    {
    }


    constexpr PxExtent2D& operator+=(const PxExtent2D arg) noexcept
    {
      Width += arg.Width;
      Height += arg.Height;
      return *this;
    }

    constexpr PxExtent2D& operator-=(const PxExtent2D arg) noexcept
    {
      assert(Width >= arg.Width);
      assert(Height >= arg.Height);
      Width -= arg.Width;
      Height -= arg.Height;
      return *this;
    }

    constexpr PxExtent2D& operator*=(const PxExtent2D arg) noexcept
    {
      Width *= arg.Width;
      Height *= arg.Height;
      return *this;
    }

    constexpr PxExtent2D& operator*=(const value_type arg) noexcept
    {
      Width *= arg;
      Height *= arg;
      return *this;
    }

    constexpr PxExtent2D& operator/=(const value_type arg)
    {
      Width /= arg;
      Height /= arg;
      return *this;
    }

    constexpr bool operator==(const PxExtent2D rhs) const noexcept
    {
      return Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const PxExtent2D rhs) const noexcept
    {
      return Width != rhs.Width || Height != rhs.Height;
    }

    // @brief Returns a PxExtent2D with all components being zero (0, 0)
    static constexpr PxExtent2D Zero() noexcept
    {
      return {};
    }

    static constexpr PxExtent2D Create(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {value_type(width), value_type(height)};
    }
  };

  inline constexpr PxExtent2D operator+(const PxExtent2D lhs, const PxExtent2D rhs) noexcept
  {
    return {lhs.Width + rhs.Width, lhs.Height + rhs.Height};
  }

  inline constexpr PxExtent2D operator-(const PxExtent2D lhs, const PxExtent2D rhs) noexcept
  {
    assert(lhs.Width >= rhs.Width);
    assert(lhs.Height >= rhs.Height);
    return {lhs.Width - rhs.Width, lhs.Height - rhs.Height};
  }

  inline constexpr PxExtent2D operator*(const PxExtent2D lhs, const PxExtent2D rhs) noexcept
  {
    return {lhs.Width * rhs.Width, lhs.Height * rhs.Height};
  }

  inline constexpr PxExtent2D operator*(const PxExtent2D lhs, const PxExtent2D::value_type rhs) noexcept
  {
    return {lhs.Width * rhs, lhs.Height * rhs};
  }

  inline constexpr PxExtent2D operator*(const PxExtent2D::value_type lhs, const PxExtent2D rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr PxExtent2D operator/(const PxExtent2D lhs, const PxExtent2D::value_type rhs)
  {
    return {lhs.Width / rhs, lhs.Height / rhs};
  }
}
#endif
