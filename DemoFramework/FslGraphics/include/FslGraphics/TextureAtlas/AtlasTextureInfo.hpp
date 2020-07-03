#ifndef FSLGRAPHICS_TEXTUREATLAS_ATLASTEXTUREINFO_HPP
#define FSLGRAPHICS_TEXTUREATLAS_ATLASTEXTUREINFO_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>

namespace Fsl
{
  struct AtlasTextureInfo
  {
    PxExtent2D ExtentPx;
    PxPoint2 OffsetPx;
    PxRectangleU TrimmedRectPx;
    PxThicknessU TrimMarginPx;
    uint32_t Dpi{0};

    constexpr AtlasTextureInfo() = default;

    //! @param srcRect the untrimmed rect
    //! @param trimmedRect he trimmed rect
    constexpr AtlasTextureInfo(const PxRectangleU& trimmedRectPx, const PxThicknessU& trimPx, const uint32_t dpi) noexcept
      : ExtentPx(trimmedRectPx.Width + trimPx.SumX(), trimmedRectPx.Height + trimPx.SumY())
      , OffsetPx(trimmedRectPx.X - trimPx.Left, trimmedRectPx.Y - trimPx.Top)
      , TrimmedRectPx(trimmedRectPx)
      , TrimMarginPx(trimPx)
      , Dpi(dpi)
    {
    }

    constexpr bool operator==(const AtlasTextureInfo& rhs) const noexcept
    {
      return ExtentPx == rhs.ExtentPx && OffsetPx == rhs.OffsetPx && TrimmedRectPx == rhs.TrimmedRectPx && TrimMarginPx == rhs.TrimMarginPx &&
             Dpi == rhs.Dpi;
    }

    constexpr bool operator!=(const AtlasTextureInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
