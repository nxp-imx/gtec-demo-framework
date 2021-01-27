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

#include <FslGraphics/Sprite/Info/NineSliceSpriteInfo.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace
  {
    NativeNineSliceTextureArea CalcNativeNineSliceTextureArea(const SpriteMaterialInfo& spriteMaterialInfo, const PxRectangleU& imageRectanglePx,
                                                              const PxThicknessU& nineSlicePx, const StringViewLite& debugName)
    {
      if (nineSlicePx.SumX() > imageRectanglePx.Width || nineSlicePx.SumY() > imageRectanglePx.Height)
      {
        throw std::invalid_argument("not a valid nine slice texture");
      }

      // We calculate everything in integers so we are doing pixel perfect slicing
      const PxRectangleU imageRectangleInnerPx(imageRectanglePx.X + nineSlicePx.Left, imageRectanglePx.Y + nineSlicePx.Top,
                                               imageRectanglePx.Width - nineSlicePx.SumX(), imageRectanglePx.Height - nineSlicePx.SumY());

      // then we calculate the texture coordinates of the inner and outer rectangle
      const NativeTextureArea areaOuter = spriteMaterialInfo.CalcNativeTextureArea(imageRectanglePx, debugName);
      const NativeTextureArea areaInner = spriteMaterialInfo.CalcNativeTextureArea(imageRectangleInnerPx, debugName);

      return {areaOuter.X0, areaOuter.Y0, areaInner.X0, areaInner.Y0, areaInner.X1, areaInner.Y1, areaOuter.X1, areaOuter.Y1};
    }

    PxThicknessU CalcTrimmedNineSlice(const PxThicknessU& nineSlicePx, const PxThicknessU& imageTrimMarginPx)
    {
      if (nineSlicePx.Left < imageTrimMarginPx.Left || nineSlicePx.Top < imageTrimMarginPx.Top || nineSlicePx.Right < imageTrimMarginPx.Right ||
          nineSlicePx.Bottom < imageTrimMarginPx.Bottom)
      {
        throw std::invalid_argument("not a valid nine-slice texture. The trim is not allowed to exceed the nine slice border area");
      }
      return {nineSlicePx.Left - imageTrimMarginPx.Left, nineSlicePx.Top - imageTrimMarginPx.Top, nineSlicePx.Right - imageTrimMarginPx.Right,
              nineSlicePx.Bottom - imageTrimMarginPx.Bottom};
    }

    constexpr inline PxExtent2D CalcExtent(const PxThicknessU& imageTrimMarginPx, const PxRectangleU& imageTrimmedRectanglePx)
    {
      return imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
    }
  }

  NineSliceSpriteInfo::NineSliceSpriteInfo(const SpriteMaterialInfo& spriteMaterialInfo, const PxThicknessU& imageTrimMarginPx,
                                           const PxRectangleU& imageTrimmedRectanglePx, const PxThicknessU& nineSlicePx,
                                           const PxThicknessU& contentMarginPx, const uint32_t imageDpi, const StringViewLite& debugName)
    : MaterialInfo(spriteMaterialInfo)
    , ImageInfo(CalcExtent(imageTrimMarginPx, imageTrimmedRectanglePx), imageTrimMarginPx, imageTrimmedRectanglePx,
                CalcTrimmedNineSlice(nineSlicePx, imageTrimMarginPx), contentMarginPx,
                SpriteUnitConverter::CalcImageDpExtent(CalcExtent(imageTrimMarginPx, imageTrimmedRectanglePx), imageDpi),
                SpriteUnitConverter::CalcDpThicknessU(nineSlicePx, imageDpi), SpriteUnitConverter::CalcDpThicknessU(contentMarginPx, imageDpi))
    , ImageDpi(imageDpi)
    , RenderInfo(CalcNativeNineSliceTextureArea(spriteMaterialInfo, imageTrimmedRectanglePx, ImageInfo.TrimmedNineSlicePx, debugName),
                 PxTrimmedNineSlice(TypeConverter::To<PxSize2D>(ImageInfo.ExtentPx), TypeConverter::To<PxThicknessF>(ImageInfo.TrimMarginPx),
                                    TypeConverter::To<PxThicknessF>(ImageInfo.TrimmedNineSlicePx),
                                    TypeConverter::To<PxThickness>(ImageInfo.ContentMarginPx)))
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }
  }

}
