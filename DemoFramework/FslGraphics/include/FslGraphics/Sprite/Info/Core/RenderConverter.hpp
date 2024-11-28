#ifndef FSLGRAPHICS_SPRITE_INFO_CORE_RENDERCONVERTER_HPP
#define FSLGRAPHICS_SPRITE_INFO_CORE_RENDERCONVERTER_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Pixel/PxTrimmedImage.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderBasicImageInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderBasicNineSliceInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderImageInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderNineSliceInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderOptimizedBasicNineSliceInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderOptimizedNineSliceInfo.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSliceFlags.hpp>
#include <cassert>

namespace Fsl::RenderConverter
{
  inline constexpr RenderImageInfo ToRenderImageInfo(const RenderBasicImageInfo& renderInfo) noexcept
  {
    return {renderInfo.TextureArea, PxTrimmedImage(renderInfo.ScaledSizePx, PxThicknessF(), TypeConverter::To<PxSize2DF>(renderInfo.ScaledSizePx))};
  }

  //--------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr RenderBasicImageInfo ToRenderBasicImageInfo(const RenderImageInfo& renderInfo) noexcept
  {
    assert(renderInfo.ScaledTrimMarginPxf.SumX().RawValue() <= 0.0f);
    assert(renderInfo.ScaledTrimMarginPxf.SumY().RawValue() <= 0.0f);
    return {renderInfo.TextureArea, renderInfo.ScaledSizePx};
  }

  //--------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr AtlasNineSliceFlags ToAtlasNineSliceFlags(const bool isOpaque) noexcept
  {
    return isOpaque ? AtlasNineSliceFlags::Opaque : AtlasNineSliceFlags::Transparent;
  }

  //--------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr RenderOptimizedNineSliceInfo ToRenderOptimizedNineSliceInfo(const RenderBasicNineSliceInfo& renderInfo,
                                                                               const bool isOpaque) noexcept
  {
    const auto flags = ToAtlasNineSliceFlags(isOpaque);
    PxTrimmedNineSlice trimmed(renderInfo.ScaledSizePx, PxThicknessF(), TypeConverter::To<PxThicknessF>(renderInfo.ScaledNineSlicePx),
                               renderInfo.ScaledContentMarginPx);
    return {renderInfo.TextureArea, flags, trimmed};
  }

  //--------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr RenderOptimizedNineSliceInfo ToRenderOptimizedNineSliceInfo(const RenderNineSliceInfo& renderInfo, const bool isOpaque) noexcept
  {
    const auto flags = ToAtlasNineSliceFlags(isOpaque);
    PxTrimmedNineSlice trimmed(renderInfo.ScaledSizePx, renderInfo.ScaledTrimMarginPxf, renderInfo.ScaledTrimmedNineSlicePxf,
                               renderInfo.ScaledContentMarginPx);
    return {renderInfo.TextureArea, flags, trimmed};
  }

  //--------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr RenderOptimizedNineSliceInfo ToRenderOptimizedNineSliceInfo(const RenderOptimizedBasicNineSliceInfo& renderInfo) noexcept
  {
    PxTrimmedNineSlice trimmed(renderInfo.ScaledSizePx, PxThicknessF(), TypeConverter::To<PxThicknessF>(renderInfo.ScaledNineSlicePx),
                               renderInfo.ScaledContentMarginPx);
    return {renderInfo.TextureArea, renderInfo.Flags, trimmed};
  }
}

#endif
