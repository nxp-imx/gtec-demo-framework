#ifndef FSLGRAPHICS_SPRITE_FONT_SPRITEFONTINFO_HPP
#define FSLGRAPHICS_SPRITE_FONT_SPRITEFONTINFO_HPP
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

#include <FslBase/Math/Dp/DpExtent.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Font/BitmapFontConfig.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>

namespace Fsl
{
  class StringViewLite;

  //! @brief Represents a atlas texture 2d image with no trim
  struct SpriteFontInfo
  {
    //! Render material information
    SpriteMaterialInfo MaterialInfo;

    //! The actual size of the source atlas texture in pixels
    PxExtent2D AtlasTextureExtentPx;

    //! The source atlas texture density extent (this is the dp size of the source atlas texture)
    DpExtent AtlasTextureExtentDp;

    //! This is the actual distance in pixels between each line of text.
    uint16_t LineSpacingPx{};

    //! The actual number of pixels from the absolute top of the line to the base of the characters.
    uint16_t BaseLinePx{};

    uint32_t ImageDpi{};

    bool IsSdfBased{false};

    float SdfScale{1.0f};

    // ---- Values below can be modified due to 'resize'

    BitmapFontConfig FontConfig;

    //! This is the distance in pixels between each line of text.
    uint16_t ScaledLineSpacingPx{};

    //! The number of pixels from the absolute top of the line to the base of the characters.
    uint16_t ScaledBaseLinePx{};

    SpriteFontInfo() = default;
    SpriteFontInfo(const SpriteMaterialInfo& spriteMaterialInfo, const uint16_t lineSpacingPx, const uint16_t baseLinePx, const uint32_t imageDpi,
                   const bool enableKerning, const bool isSdfBased, const float sdfScale, const StringViewLite& debugName);


    bool operator==(const SpriteFontInfo& rhs) const
    {
      return MaterialInfo == rhs.MaterialInfo && AtlasTextureExtentPx == rhs.AtlasTextureExtentPx &&
             AtlasTextureExtentDp == rhs.AtlasTextureExtentDp && LineSpacingPx == rhs.LineSpacingPx && BaseLinePx == rhs.BaseLinePx &&
             ImageDpi == rhs.ImageDpi && IsSdfBased && rhs.IsSdfBased && SdfScale == rhs.SdfScale && FontConfig == rhs.FontConfig &&
             ScaledLineSpacingPx == rhs.ScaledLineSpacingPx && ScaledBaseLinePx == rhs.ScaledBaseLinePx;
    }

    bool operator!=(const SpriteFontInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
