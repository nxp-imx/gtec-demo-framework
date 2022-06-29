#ifndef FSLGRAPHICS_SPRITE_INFO_NINESLICESPRITEINFOUTIL_HPP
#define FSLGRAPHICS_SPRITE_INFO_NINESLICESPRITEINFOUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/NativeNineSliceTextureArea.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>

namespace Fsl::NineSliceSpriteInfoUtil
{
  inline NativeNineSliceTextureArea CalcNativeNineSliceTextureArea(const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                                                   const SpriteMaterialInfo& spriteMaterialInfo,
                                                                   const PxRectangleU16& imageRectanglePx, const PxThicknessU& nineSlicePx,
                                                                   const StringViewLite& debugName)
  {
    FSL_PARAM_NOT_USED(debugName);
    if (nineSlicePx.SumX() > imageRectanglePx.Width || nineSlicePx.SumY() > imageRectanglePx.Height)
    {
      throw std::invalid_argument("not a valid nine slice texture");
    }

    // We calculate everything in integers so we are doing pixel perfect slicing
    const PxRectangleU16 imageRectangleInnerPx(imageRectanglePx.X + nineSlicePx.Left, imageRectanglePx.Y + nineSlicePx.Top,
                                               imageRectanglePx.Width - nineSlicePx.SumX(), imageRectanglePx.Height - nineSlicePx.SumY());

    // then we calculate the texture coordinates of the inner and outer rectangle
    const NativeTextureArea areaOuter = spriteNativeAreaCalc.CalcNativeTextureArea(imageRectanglePx, spriteMaterialInfo.ExtentPx);
    const NativeTextureArea areaInner = spriteNativeAreaCalc.CalcNativeTextureArea(imageRectangleInnerPx, spriteMaterialInfo.ExtentPx);

    return {areaOuter.X0, areaOuter.Y0, areaInner.X0, areaInner.Y0, areaInner.X1, areaInner.Y1, areaOuter.X1, areaOuter.Y1};
  }
}

#endif
