#ifndef FSLBASE_MATH_PIXEL_PXTRIMMEDNINESLICE_HPP
#define FSLBASE_MATH_PIXEL_PXTRIMMEDNINESLICE_HPP
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

#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>

namespace Fsl
{
  struct PxTrimmedNineSlice
  {
    //! The image size in pixels
    PxSize2D SizePx;

    //! The image trim in pixels
    PxThicknessF TrimMarginPxf;

    //! The scaled trimmed nine slice (basically the nine slice information minus the trim, scaled to the current density)
    PxThicknessF TrimmedNineSlicePxf;

    //! The scaled content margin
    PxThickness ContentMarginPx;

    constexpr PxTrimmedNineSlice() noexcept = default;

    constexpr PxTrimmedNineSlice(const PxSize2D sizePx, const PxThicknessF& trimMarginPxf, const PxThicknessF& trimmedNineSlicePxf,
                                 const PxThickness& contentMarginPx) noexcept
      : SizePx(sizePx)
      , TrimMarginPxf(trimMarginPxf)
      , TrimmedNineSlicePxf(trimmedNineSlicePxf)
      , ContentMarginPx(contentMarginPx)
    {
    }

    constexpr bool operator==(const PxTrimmedNineSlice& rhs) const noexcept
    {
      return SizePx == rhs.SizePx && TrimMarginPxf == rhs.TrimMarginPxf && TrimmedNineSlicePxf == rhs.TrimmedNineSlicePxf &&
             ContentMarginPx == rhs.ContentMarginPx;
    }

    constexpr bool operator!=(const PxTrimmedNineSlice& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
