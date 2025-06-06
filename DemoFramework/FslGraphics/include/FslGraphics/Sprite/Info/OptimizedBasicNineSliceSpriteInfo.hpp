#ifndef FSLGRAPHICS_SPRITE_INFO_OPTIMIZEDBASICNINESLICESPRITEINFO_HPP
#define FSLGRAPHICS_SPRITE_INFO_OPTIMIZEDBASICNINESLICESPRITEINFO_HPP
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

#include <FslBase/Math/Dp/DpExtent2D.hpp>
#include <FslBase/Math/Dp/DpThicknessU.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU16.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/NativeNineSliceTextureArea.hpp>
#include <FslGraphics/Sprite/Info/Core/CoreBasicNineSliceInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderOptimizedBasicNineSliceInfo.hpp>
#include <FslGraphics/Sprite/Info/ImageSpriteInfo.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSliceFlags.hpp>

namespace Fsl
{
  class SpriteNativeAreaCalc;
  class StringViewLite;

  //! @brief Represents a atlas texture 2d nine-slice image with no trim, furthermore its transparency aware and has a material for both opaque and
  //! transparent sections.
  struct OptimizedBasicNineSliceSpriteInfo
  {
    //! Render material information
    SpriteMaterialInfo OpaqueMaterialInfo;
    SpriteMaterialInfo TransparentMaterialInfo;

    //! Image information
    CoreBasicNineSliceInfo ImageInfo;

    uint32_t ImageDpi{SpriteDpConfig::BaseDpi};

    RenderOptimizedBasicNineSliceInfo RenderInfo;

    OptimizedBasicNineSliceSpriteInfo() = default;
    OptimizedBasicNineSliceSpriteInfo(const SpriteNativeAreaCalc& spriteNativeAreaCalc, const SpriteMaterialInfo& opaqueSpriteMaterialInfo,
                                      const SpriteMaterialInfo& transparentSpriteMaterialInfo, const PxRectangleU16& imageRectanglePx,
                                      const PxThicknessU& nineSlicePx, const PxThicknessU& contentMarginPx, const AtlasNineSliceFlags flags,
                                      const uint32_t imageDpi, const StringViewLite& debugName);


    bool operator==(const OptimizedBasicNineSliceSpriteInfo& rhs) const
    {
      return OpaqueMaterialInfo == rhs.OpaqueMaterialInfo && TransparentMaterialInfo == rhs.TransparentMaterialInfo && ImageDpi == rhs.ImageDpi &&
             RenderInfo == rhs.RenderInfo;
    }

    bool operator!=(const OptimizedBasicNineSliceSpriteInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
