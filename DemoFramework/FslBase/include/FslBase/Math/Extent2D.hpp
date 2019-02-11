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
  public:
    using element_type = uint32_t;

    element_type Width{0};
    element_type Height{0};

    constexpr Extent2D() = default;

    constexpr Extent2D(const element_type width, const element_type height)
      : Width(width)
      , Height(height)
    {
    }

    //! @brief Create a Extent
    //! @note  width and height is expected to be positive and able to fit in a element_type, if not a exception is thrown
    Extent2D(int32_t width, int32_t height);

    //! @brief Create a Extent
    //! @note  Value is expected to be positive and able to fit in a element_type, if not a exception is thrown
    explicit Extent2D(const Point2& value);


    Extent2D& operator+=(const Extent2D& arg)
    {
      Width += arg.Width;
      Height += arg.Height;
      return *this;
    }

    Extent2D& operator-=(const Extent2D& arg)
    {
      Width -= arg.Width;
      Height -= arg.Height;
      return *this;
    }

    Extent2D& operator*=(const Extent2D& arg)
    {
      Width *= arg.Width;
      Height *= arg.Height;
      return *this;
    }

    Extent2D& operator*=(const int arg)
    {
      Width *= arg;
      Height *= arg;
      return *this;
    }

    constexpr bool operator==(const Extent2D& rhs) const
    {
      return Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const Extent2D& rhs) const
    {
      return Width != rhs.Width || Height != rhs.Height;
    }

    // @brief Returns a Extent2D with all components being zero (0, 0)
    static constexpr Extent2D Zero()
    {
      return {};
    }
  };
}

inline Fsl::Extent2D operator+(const Fsl::Extent2D& lhs, const Fsl::Extent2D& rhs)
{
  return Fsl::Extent2D(lhs.Width + rhs.Width, lhs.Height + rhs.Height);
}

inline Fsl::Extent2D operator-(const Fsl::Extent2D& lhs, const Fsl::Extent2D& rhs)
{
  return Fsl::Extent2D(lhs.Width - rhs.Width, lhs.Height - rhs.Height);
}

inline Fsl::Extent2D operator*(const Fsl::Extent2D& lhs, const Fsl::Extent2D& rhs)
{
  return Fsl::Extent2D(lhs.Width * rhs.Width, lhs.Height * rhs.Height);
}

inline Fsl::Extent2D operator*(const Fsl::Extent2D& lhs, const int rhs)
{
  return Fsl::Extent2D(lhs.Width * rhs, lhs.Height * rhs);
}

inline Fsl::Extent2D operator*(const int lhs, const Fsl::Extent2D& rhs)
{
  return rhs * lhs;
}

#endif
