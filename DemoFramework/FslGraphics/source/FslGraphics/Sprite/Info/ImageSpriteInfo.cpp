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

#include <FslGraphics/Sprite/Info/ImageSpriteInfo.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace
  {
    constexpr inline PxExtent2D CalcExtent(const PxThicknessU& imageTrimMarginPx, const PxRectangleU& imageTrimmedRectanglePx)
    {
      return imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
    }
  }

  ImageSpriteInfo::ImageSpriteInfo(const SpriteMaterialInfo& spriteMaterialInfo, const PxThicknessU& imageTrimMarginPx,
                                   const PxRectangleU& imageTrimmedRectanglePx, const uint32_t imageDpi, const StringViewLite& debugName)
    : MaterialInfo(spriteMaterialInfo)
    , ImageInfo(CalcExtent(imageTrimMarginPx, imageTrimmedRectanglePx), imageTrimMarginPx, imageTrimmedRectanglePx,
                SpriteUnitConverter::CalcImageDpExtent(CalcExtent(imageTrimMarginPx, imageTrimmedRectanglePx), imageDpi))
    , ImageDpi(imageDpi)
    , RenderInfo(spriteMaterialInfo.CalcNativeTextureArea(imageTrimmedRectanglePx, debugName),
                 PxTrimmedImage(TypeConverter::To<PxSize2D>(ImageInfo.ExtentPx), TypeConverter::To<PxThicknessF>(imageTrimMarginPx),
                                TypeConverter::To<PxSize2DF>(ImageInfo.TrimmedRectanglePx.GetExtent())))
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }
  }
}
