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

#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Font/BitmapFontConfig.hpp>
#include <FslGraphics/Font/BitmapFontFastLookup.hpp>
#include <FslGraphics/Font/FontGlyphPosition.hpp>
#include <FslGraphics/Font/FontGlyphRange.hpp>
#include <vector>

namespace Fsl
{
  class BitmapFont;
  class IFontBasicKerning;
  class ITextureAtlas;

  class TextureAtlasBitmapFont final
  {
    BitmapFontFastLookup m_lookup;
    BitmapFontChar m_unknownChar{};

  public:
    // move assignment operator
    TextureAtlasBitmapFont& operator=(TextureAtlasBitmapFont&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_lookup = std::move(other.m_lookup);
        m_unknownChar = other.m_unknownChar;

        // Remove the data from other
        other.m_unknownChar = {};
      }
      return *this;
    }

    // move constructor
    TextureAtlasBitmapFont(TextureAtlasBitmapFont&& other) noexcept
      : m_lookup(std::move(other.m_lookup))
      , m_unknownChar(other.m_unknownChar)
    {
      other.m_unknownChar = {};
    }

    TextureAtlasBitmapFont(const TextureAtlasBitmapFont&) = delete;
    TextureAtlasBitmapFont& operator=(const TextureAtlasBitmapFont&) = delete;

    TextureAtlasBitmapFont() = default;
    TextureAtlasBitmapFont(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);
    explicit TextureAtlasBitmapFont(const BitmapFont& bitmapFont);

    void Reset() noexcept
    {
      m_lookup.Clear();
      m_unknownChar = {};
    }

    void Reset(const BitmapFont& bitmapFont);
    void Reset(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);

    int32_t BaseLinePx() const
    {
      return m_lookup.GetBaseLinePx();
    }

    //! @brief Get the baseline in pixels for the given font config
    int32_t BaseLinePx(const BitmapFontConfig& fontConfig) const;

    int32_t LineSpacingPx() const
    {
      return m_lookup.GetLineSpacingPx();
    }

    //! @brief Get the line spacing in pixels for the given font config
    int32_t LineSpacingPx(const BitmapFontConfig& fontConfig) const;


    //! @brief Measure the string size in pixels
    PxSize2D MeasureString(const char* const psz) const
    {
      return MeasureString(StringViewLite(psz));
    }

    //! @brief Measure the string size in pixels
    PxSize2D MeasureString(const StringViewLite& strView) const;

    //! @brief Measure the string size in pixels taking into account the font config
    PxSize2D MeasureString(const char* const psz, const BitmapFontConfig& fontConfig) const
    {
      return MeasureString(StringViewLite(psz), fontConfig);
    }

    //! @brief Measure the string size in pixels taking into account the font config
    PxSize2D MeasureString(const StringViewLite& strView, const BitmapFontConfig& fontConfig) const;

    //! @brief Extract render rules for the supplied string.
    //! @param rDst a vector that can contain at least 'length' entries
    //! @param startIndex the index in pStr to start reading characters at.
    //! @param length the number of characters to read from pStr
    //! @return true if the rules were extracted, false if it was determined the string would be empty.
    //          If this returns false then rDst was unmodified, if true strView.size() entries were written to rDst.
    bool ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const StringViewLite& strView) const FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT;

    //! @brief Extract render rules for the supplied string with offsets for the baseline.
    //! @param rDst a vector that can contain at least 'length' entries
    //! @param startIndex the index in pStr to start reading characters at.
    //! @param length the number of characters to read from pStr
    //! @return true if the rules were extracted, false if it was determined the string would be empty.
    //          If this returns false then rDst was unmodified, if true strView.size() entries were written to rDst.
    bool ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const StringViewLite& strView,
                            const BitmapFontConfig& fontConfig) const FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT;

    //! @brief Do a glyph info lookup
    const BitmapFontChar* TryGetChar(const uint32_t charId) const
    {
      return m_lookup.TryGetChar(charId);
    }

    //! @brief Do a glyph info lookup
    const BitmapFontChar& GetBitmapFontChar(const uint32_t charId) const
    {
      return m_lookup.GetChar(charId, m_unknownChar);
    }


  private:
    // void DoConstruct(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning);
  };
}

#endif
