#ifndef FSLGRAPHICS_FONT_TEXTUREATLASBITMAPFONT_HPP
#define FSLGRAPHICS_FONT_TEXTUREATLASBITMAPFONT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Font/FontGlyphRange.hpp>
#include <FslGraphics/Font/FontGlyphPosition.hpp>
#include <FslGraphics/Font/TextureAtlasGlyphInfo.hpp>
#include <vector>

namespace Fsl
{
  class IFontBasicKerning;
  class ITextureAtlas;

  class TextureAtlasBitmapFont
  {
    std::vector<FontGlyphRange> m_ranges;
    int32_t m_minGlyphId{0};
    // Beware the maxGlyphId is considered excluded
    int32_t m_maxGlyphId{0};
    int32_t m_unknownGlyphIndex{0};
    std::vector<TextureAtlasGlyphInfo> m_glyphs;
    int32_t m_baseLine{0};
    int32_t m_lineSpacing{0};

  public:
    TextureAtlasBitmapFont();
    TextureAtlasBitmapFont(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);

    void Reset(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);

    int32_t BaseLine() const
    {
      return m_baseLine;
    }

    int32_t LineSpacing() const
    {
      return m_lineSpacing;
    }

    //! @brief
    Point2 MeasureString(const char* const psz) const;

    //! @brief
    Point2 MeasureString(const char* const pStr, const uint32_t startIndex, const std::size_t length) const;

    //! @brief Extract render rules for the supplied string.
    //! @param rDst a vector that can contain at least 'length' entries
    //! @param startIndex the index in pStr to start reading characters at.
    //! @param length the number of characters to read from pStr
    void ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const char* const pStr, const uint32_t startIndex, const std::size_t length) const;

  private:
    void DoConstruct(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);
  };
}

#endif
