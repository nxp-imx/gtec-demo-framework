#ifndef FSLBASE_MATH_EXTENT2D_HPP
#define FSLBASE_MATH_EXTENT2D_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

namespace Fsl
{
  struct Point2;

  struct Extent2D
  {
    using value_type = uint32_t;

    value_type Width{0};
    value_type Height{0};

    constexpr Extent2D() noexcept = default;

    constexpr Extent2D(const value_type width, const value_type height) noexcept
      : Width(width)
      , Height(height)
    {
    }

    //! @brief Create a Extent
    //! @note  width and height is expected to be positive and able to fit in a value_type, if not a exception is thrown
    Extent2D(int32_t width, int32_t height);

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a value_type, if not a exception is thrown
    explicit Extent2D(const Point2& value);


    constexpr Extent2D& operator+=(const Extent2D& arg) noexcept
    {
      Width += arg.Width;
      Height += arg.Height;
      return *this;
    }

    constexpr Extent2D& operator-=(const Extent2D& arg) noexcept
    {
      Width -= arg.Width;
      Height -= arg.Height;
      return *this;
    }

    constexpr Extent2D& operator*=(const Extent2D& arg) noexcept
    {
      Width *= arg.Width;
      Height *= arg.Height;
      return *this;
    }

    constexpr Extent2D& operator*=(const int arg) noexcept
    {
      Width *= arg;
      Height *= arg;
      return *this;
    }

    constexpr bool operator==(const Extent2D& rhs) const noexcept
    {
      return Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const Extent2D& rhs) const noexcept
    {
      return Width != rhs.Width || Height != rhs.Height;
    }

    // @brief Returns a Extent2D with all components being zero (0, 0)
    static constexpr Extent2D Zero() noexcept
    {
      return {};
    }
  };

  inline constexpr Extent2D operator+(const Extent2D& lhs, const Extent2D& rhs) noexcept
  {
    return {lhs.Width + rhs.Width, lhs.Height + rhs.Height};
  }

  inline constexpr Extent2D operator-(const Extent2D& lhs, const Extent2D& rhs) noexcept
  {
    return {lhs.Width - rhs.Width, lhs.Height - rhs.Height};
  }

  inline constexpr Extent2D operator*(const Extent2D& lhs, const Extent2D& rhs) noexcept
  {
    return {lhs.Width * rhs.Width, lhs.Height * rhs.Height};
  }

  inline constexpr Extent2D operator*(const Extent2D& lhs, const int rhs)
  {
    return {lhs.Width * rhs, lhs.Height * rhs};
  }

  inline constexpr Extent2D operator*(const int lhs, const Extent2D& rhs)
  {
    return rhs * lhs;
  }

}

#endif
