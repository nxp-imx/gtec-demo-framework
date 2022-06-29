#ifndef FSLBASE_MATH_BASICWINDOWMETRICS_HPP
#define FSLBASE_MATH_BASICWINDOWMETRICS_HPP
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
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <cassert>

namespace Fsl
{
  struct BasicWindowMetrics
  {
    //! The extent of the window in pixels
    PxExtent2D ExtentPx;

    //! The size of the window in dp
    Vector2 SizeDp;

    //! the exact physical DPI (as reported by the native window)
    //! Only need this if you really really need the exact DPI.
    Vector2 ExactDpi{160, 160};

    //! The density bucket the window was put in.
    //! This is the DPI that most apps/services should use
    uint32_t DensityDpi{160};

    //! This is the density scale factor for a device independent pixel unit.
    //! For a DensityDPI of
    //! -  80 its 0.5
    //! - 160 its 1.0
    //! - 240 its 1.5
    //! - 320 its 2.0
    float DensityScaleFactor{1.0f};

    constexpr BasicWindowMetrics() = default;
    constexpr BasicWindowMetrics(const PxExtent2D& extentPx, const Vector2& exactDpi, const uint32_t densityDpi)
      : ExtentPx(extentPx)
      , SizeDp(static_cast<float>(extentPx.Width) * (160.0f / static_cast<float>(densityDpi)),
               static_cast<float>(extentPx.Height) * (160.0f / static_cast<float>(densityDpi)))
      , ExactDpi(exactDpi)
      , DensityDpi(densityDpi)
      , DensityScaleFactor(static_cast<float>(densityDpi) / 160.0f)
    {
      assert(densityDpi > 0);
    }

    //! The size of the window in pixels
    PxSize2D GetSizePx() const;

    //! The aspect ratio of the window
    constexpr float AspectRatio() const
    {
      return ExtentPx.Height > 0 ? (static_cast<float>(ExtentPx.Width) / static_cast<float>(ExtentPx.Height)) : 1.0f;
    }

    constexpr bool IsEqualDpi(const BasicWindowMetrics& rhs) const
    {
      return ExactDpi == rhs.ExactDpi && DensityDpi == rhs.DensityDpi;
    }

    constexpr bool operator==(const BasicWindowMetrics& rhs) const
    {
      return ExtentPx == rhs.ExtentPx && SizeDp == rhs.SizeDp && ExactDpi == rhs.ExactDpi && DensityDpi == rhs.DensityDpi &&
             DensityScaleFactor == rhs.DensityScaleFactor;
    }

    constexpr bool operator!=(const BasicWindowMetrics& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
