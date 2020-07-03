#ifndef FSLGRAPHICS_SPRITE_INFO_CORE_COREBASICNINESLICEINFO_HPP
#define FSLGRAPHICS_SPRITE_INFO_CORE_COREBASICNINESLICEINFO_HPP
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

#include <FslBase/Math/Dp/DpExtent.hpp>
#include <FslBase/Math/Dp/DpThicknessU.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>

namespace Fsl
{
  //! @brief Represents a atlas texture 2d image with no trim
  struct CoreBasicNineSliceInfo
  {
    //! The actual size of the source image in pixels
    PxExtent2D ExtentPx;

    //! The rectangle on the source atlas texture
    PxRectangleU RectanglePx;

    //! The nine slice
    PxThicknessU NineSlicePx;

    //! The content margin
    PxThicknessU ContentMarginPx;

    //! The image density extent (this is the dp size of the image)
    DpExtent ExtentDp;

    //! The nineslice
    DpThicknessU NineSliceDp;

    //! The content margin
    DpThicknessU ContentMarginDp;

    constexpr CoreBasicNineSliceInfo() = default;
    constexpr CoreBasicNineSliceInfo(const PxExtent2D extentPx, const PxRectangleU& rectanglePx, const PxThicknessU& nineSlicePx,
                                     const PxThicknessU& contentMarginPx, const DpExtent imageExtentDp, const DpThicknessU& nineSliceDp,
                                     const DpThicknessU& contentMarginDp)
      : ExtentPx(extentPx)
      , RectanglePx(rectanglePx)
      , NineSlicePx(nineSlicePx)
      , ContentMarginPx(contentMarginPx)
      , ExtentDp(imageExtentDp)
      , NineSliceDp(nineSliceDp)
      , ContentMarginDp(contentMarginDp)
    {
    }


    constexpr bool operator==(const CoreBasicNineSliceInfo& rhs) const
    {
      return ExtentPx == rhs.ExtentPx && RectanglePx == rhs.RectanglePx && NineSlicePx == rhs.NineSlicePx && ContentMarginPx == rhs.ContentMarginPx &&
             ExtentDp == rhs.ExtentDp && NineSliceDp == rhs.NineSliceDp && ContentMarginDp == rhs.ContentMarginDp;
    }

    constexpr bool operator!=(const CoreBasicNineSliceInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
