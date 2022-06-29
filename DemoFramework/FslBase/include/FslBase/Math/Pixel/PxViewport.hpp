#ifndef FSLBASE_MATH_PIXEL_PXVIEWPORT_HPP
#define FSLBASE_MATH_PIXEL_PXVIEWPORT_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>

namespace Fsl
{
  struct PxViewport
  {
    int32_t X{0};
    int32_t Y{0};
    int32_t Width{0};
    int32_t Height{0};

    constexpr PxViewport() noexcept = default;


    constexpr explicit PxViewport(const PxRectangle& bounds) noexcept
      : PxViewport(bounds.X(), bounds.Y(), bounds.Width(), bounds.Height())
    {
    }

    constexpr PxViewport(const int32_t x, const int32_t y, const int32_t width, const int32_t height) noexcept
      : X(x)
      , Y(y)
      , Width(width >= 0 ? width : 0)
      , Height(height >= 0 ? height : 0)
    {
    }

    float GetAspectRatio() const
    {
      if ((Height != 0) && (Width != 0))
      {
        return static_cast<float>(Width) / static_cast<float>(Height);
      }
      return 0.0f;
    }

    constexpr int32_t Left() const
    {
      return X;
    }

    constexpr int32_t Top() const
    {
      return Y;
    }

    constexpr int32_t Right() const
    {
      return X + Width;
    }

    constexpr int32_t Bottom() const
    {
      return Y + Height;
    }

    constexpr PxPoint2 Location() const
    {
      return {X, Y};
    }

    constexpr PxSize2D GetSize() const
    {
      return {Width, Height};
    }

    constexpr PxRectangle GetBounds() const
    {
      return {X, Y, Width, Height};
    }

    constexpr void SetBounds(const PxRectangle& value)
    {
      X = value.X();
      Y = value.Y();
      Width = value.Width();
      Height = value.Height();
    }

    constexpr bool operator==(const PxViewport& rhs) const noexcept
    {
      return X == rhs.X && Y == rhs.Y && Width == rhs.Width && Height == rhs.Height;
    }

    constexpr bool operator!=(const PxViewport& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}


#endif
