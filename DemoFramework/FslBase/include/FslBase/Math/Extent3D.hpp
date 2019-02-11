#ifndef FSLBASE_MATH_EXTENT3D_HPP
#define FSLBASE_MATH_EXTENT3D_HPP
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
  struct Extent2D;
  struct Point2;

  struct Extent3D
  {
  public:
    using element_type = uint32_t;

    element_type Width{0};
    element_type Height{0};
    element_type Depth{0};

    constexpr Extent3D() = default;

    // constexpr Extent3D(const element_type width, const element_type height)
    //  : Extent3D(width, height, 1)
    //{
    //}

    Extent3D(const int32_t width, const int32_t height, const int32_t depth);

    constexpr Extent3D(const element_type width, const element_type height, const element_type depth)
      : Width(width)
      , Height(height)
      , Depth(depth)
    {
    }

    Extent3D(const Point2& value, const element_type depth);
    Extent3D(const Extent2D& extent, const element_type depth);


    Extent3D& operator+=(const Extent3D& arg)
    {
      Width += arg.Width;
      Height += arg.Height;
      Depth += arg.Depth;
      return *this;
    }

    Extent3D& operator-=(const Extent3D& arg)
    {
      Width -= arg.Width;
      Height -= arg.Height;
      Depth -= arg.Depth;
      return *this;
    }

    Extent3D& operator*=(const Extent3D& arg)
    {
      Width *= arg.Width;
      Height *= arg.Height;
      Depth *= arg.Depth;
      return *this;
    }

    Extent3D& operator*=(const int arg)
    {
      Width *= arg;
      Height *= arg;
      Depth *= arg;
      return *this;
    }

    constexpr bool operator==(const Extent3D& rhs) const
    {
      return Width == rhs.Width && Height == rhs.Height && Depth == rhs.Depth;
    }

    constexpr bool operator!=(const Extent3D& rhs) const
    {
      return Width != rhs.Width || Height != rhs.Height || Depth != rhs.Depth;
    }

    // @brief Returns a Extent3D with all components being zero (0, 0)
    static constexpr Extent3D Zero()
    {
      return {};
    }
  };
}

inline Fsl::Extent3D operator+(const Fsl::Extent3D& lhs, const Fsl::Extent3D& rhs)
{
  return Fsl::Extent3D(lhs.Width + rhs.Width, lhs.Height + rhs.Height, lhs.Depth + rhs.Depth);
}

inline Fsl::Extent3D operator-(const Fsl::Extent3D& lhs, const Fsl::Extent3D& rhs)
{
  return Fsl::Extent3D(lhs.Width - rhs.Width, lhs.Height - rhs.Height, lhs.Depth - rhs.Depth);
}

inline Fsl::Extent3D operator*(const Fsl::Extent3D& lhs, const Fsl::Extent3D& rhs)
{
  return Fsl::Extent3D(lhs.Width * rhs.Width, lhs.Height * rhs.Height, lhs.Depth * rhs.Depth);
}

inline Fsl::Extent3D operator*(const Fsl::Extent3D& lhs, const int rhs)
{
  return Fsl::Extent3D(lhs.Width * rhs, lhs.Height * rhs, lhs.Depth + rhs);
}

inline Fsl::Extent3D operator*(const int lhs, const Fsl::Extent3D& rhs)
{
  return rhs * lhs;
}

#endif
