#ifndef FSLBASE_MATH_PIXEL_PXEXTENT3D_HPP
#define FSLBASE_MATH_PIXEL_PXEXTENT3D_HPP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2U.hpp>
#include <cassert>

namespace Fsl
{
  struct PxExtent3D
  {
    using value_type = uint32_t;

    value_type Width{0};
    value_type Height{0};
    value_type Depth{0};

    constexpr PxExtent3D() noexcept = default;

    constexpr PxExtent3D(const value_type width, const value_type height, const value_type depth) noexcept
      : Width(width)
      , Height(height)
      , Depth(depth)
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr PxExtent3D(const PxPoint2U& value, const value_type depth) noexcept
      : PxExtent3D(value.X, value.Y, depth)
    {
    }

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    constexpr PxExtent3D(const PxExtent2D& value, const value_type depth) noexcept
      : PxExtent3D(value.Width, value.Height, depth)
    {
    }


    constexpr PxExtent3D& operator+=(const PxExtent3D& arg) noexcept
    {
      Width += arg.Width;
      Height += arg.Height;
      Depth += arg.Depth;
      return *this;
    }

    constexpr PxExtent3D& operator-=(const PxExtent3D& arg) noexcept
    {
      assert(Width >= arg.Width);
      assert(Height >= arg.Height);
      assert(Depth >= arg.Depth);

      Width -= arg.Width;
      Height -= arg.Height;
      Depth -= arg.Depth;
      return *this;
    }

    constexpr PxExtent3D& operator*=(const PxExtent3D& arg) noexcept
    {
      Width *= arg.Width;
      Height *= arg.Height;
      Depth *= arg.Depth;
      return *this;
    }

    constexpr PxExtent3D& operator*=(const value_type arg) noexcept
    {
      Width *= arg;
      Height *= arg;
      Depth *= arg;
      return *this;
    }

    constexpr PxExtent3D& operator/=(const value_type arg)
    {
      assert(arg > 0u);
      Width /= arg;
      Height /= arg;
      Depth /= arg;
      return *this;
    }

    constexpr bool operator==(const PxExtent3D& rhs) const noexcept
    {
      return Width == rhs.Width && Height == rhs.Height && Depth == rhs.Depth;
    }

    constexpr bool operator!=(const PxExtent3D& rhs) const noexcept
    {
      return Width != rhs.Width || Height != rhs.Height || Depth != rhs.Depth;
    }

    // @brief Returns a PxExtent3D with all components being zero (0, 0)
    static constexpr PxExtent3D Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr PxExtent3D operator+(const PxExtent3D& lhs, const PxExtent3D& rhs) noexcept
  {
    return {lhs.Width + rhs.Width, lhs.Height + rhs.Height, lhs.Depth + rhs.Depth};
  }

  inline constexpr PxExtent3D operator-(const PxExtent3D& lhs, const PxExtent3D& rhs) noexcept
  {
    assert(lhs.Width >= rhs.Width);
    assert(lhs.Height >= rhs.Height);
    assert(lhs.Depth >= rhs.Depth);
    return {lhs.Width - rhs.Width, lhs.Height - rhs.Height, lhs.Depth - rhs.Depth};
  }

  inline constexpr PxExtent3D operator*(const PxExtent3D& lhs, const PxExtent3D& rhs) noexcept
  {
    return {lhs.Width * rhs.Width, lhs.Height * rhs.Height, lhs.Depth * rhs.Depth};
  }

  inline constexpr PxExtent3D operator*(const PxExtent3D& lhs, const PxExtent3D::value_type rhs) noexcept
  {
    return {lhs.Width * rhs, lhs.Height * rhs, lhs.Depth * rhs};
  }

  inline constexpr PxExtent3D operator*(const PxExtent3D::value_type lhs, const PxExtent3D& rhs) noexcept
  {
    return rhs * lhs;
  }

  inline constexpr PxExtent3D operator/(const PxExtent3D& lhs, const PxExtent3D::value_type rhs)
  {
    assert(rhs > 0u);
    return {lhs.Width / rhs, lhs.Height / rhs, lhs.Depth / rhs};
  }
}

#endif
