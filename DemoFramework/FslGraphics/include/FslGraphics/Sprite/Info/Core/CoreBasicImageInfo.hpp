#ifndef FSLGRAPHICS_SPRITE_INFO_CORE_COREBASICIMAGEINFO_HPP
#define FSLGRAPHICS_SPRITE_INFO_CORE_COREBASICIMAGEINFO_HPP
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

#include <FslBase/Math/Dp/DpExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU16.hpp>

namespace Fsl
{
  //! @brief Represents a atlas texture 2d image with no trim
  struct CoreBasicImageInfo
  {
    //! The actual size of the source image in pixels
    PxExtent2D ExtentPx;

    //! The rectangle on the source atlas texture
    PxRectangleU16 RectanglePx;

    //! The image density extent (this is the dp size of the image)
    DpExtent2D ExtentDp;

    constexpr CoreBasicImageInfo() = default;
    constexpr CoreBasicImageInfo(const PxExtent2D extentPx, const PxRectangleU16 rectanglePx, const DpExtent2D extentDp)
      : ExtentPx(extentPx)
      , RectanglePx(rectanglePx)
      , ExtentDp(extentDp)
    {
    }


    constexpr bool operator==(const CoreBasicImageInfo& rhs) const
    {
      return ExtentPx == rhs.ExtentPx && RectanglePx == rhs.RectanglePx && ExtentDp == rhs.ExtentDp;
    }

    constexpr bool operator!=(const CoreBasicImageInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
