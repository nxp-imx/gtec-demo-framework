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

#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Font/IFontBasicKerning.hpp>
#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log.hpp>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

namespace Fsl
{
  namespace
  {
    struct TextureAtlasGlyphEntry
    {
      int32_t Id{0};
      AtlasTextureInfo TextureInfo;

      TextureAtlasGlyphEntry() = default;

      TextureAtlasGlyphEntry(const int32_t id, const AtlasTextureInfo& textureInfo)
        : Id(id)
        , TextureInfo(textureInfo)
      {
      }
    };


    //! @brief Run through the texture atlas and extract all glyphs found at the basicFontKerning path
    int32_t ExtractAndSortGlyphs(std::vector<TextureAtlasGlyphEntry>& rTextureAtlasGlyphs, const ITextureAtlas& textureAtlas,
                                 const IFontBasicKerning& basicFontKerning)
    {
      assert(rTextureAtlasGlyphs.size() == std::size_t(basicFontKerning.Count()));

      const int32_t atlasEntries = textureAtlas.Count();

      const IO::Path fontPath(basicFontKerning.GetPathName().ToUTF8String() + "/");
      const int32_t fontPathLength = static_cast<int32_t>(fontPath.ToUTF8String().size());
      const int32_t dstEndIndex = basicFontKerning.Count();
      int32_t dstIndex = 0;
      for (int32_t i = 0; i < atlasEntries; ++i)
      {
        NamedAtlasTexture namedEntry = textureAtlas.GetEntry(i);
        IO::Path str(namedEntry.Name);
        if (str.StartsWith(fontPath))
        {
          // std::stoi is not present on android :/
          // const int32_t glyphId = std::stoi(str.ToUTF8String().substr(fontPathLength), nullptr, 16);
          const int32_t glyphId = static_cast<int32_t>(std::strtol(str.ToUTF8String().substr(fontPathLength).c_str(), nullptr, 16));

          rTextureAtlasGlyphs[dstIndex] = TextureAtlasGlyphEntry(glyphId, namedEntry.TextureInfo);
          ++dstIndex;
          if (dstIndex > dstEndIndex)
          {
            throw std::invalid_argument("The texture atlas contains more glyphs than expected");
          }
        }
      }
      return dstIndex;
    }


    struct TextureAtlasGlyphEntryComparator
    {
      bool operator()(const TextureAtlasGlyphEntry& lhs, const TextureAtlasGlyphEntry& rhs)
      {
        return (lhs.Id < rhs.Id);
      }
    };


    int32_t ExtractAtlasGlyphs(std::vector<TextureAtlasGlyphEntry>& rTextureAtlasGlyphs, const ITextureAtlas& textureAtlas,
                               const IFontBasicKerning& basicFontKerning)
    {
      const int32_t atlasGlyphCount = ExtractAndSortGlyphs(rTextureAtlasGlyphs, textureAtlas, basicFontKerning);
      // Sort the glyphs by id
      std::sort(rTextureAtlasGlyphs.begin(), rTextureAtlasGlyphs.begin() + atlasGlyphCount, TextureAtlasGlyphEntryComparator());
      return atlasGlyphCount;
    }


    void ExtractRanges(std::vector<FontGlyphRange>& rRanges, const IFontBasicKerning& basicFontKerning)
    {
      const int32_t rangeCount = basicFontKerning.RangeCount();
      assert(rRanges.size() == std::size_t(rangeCount));

      int32_t totalGlyphs = 0;
      for (int32_t i = 0; i < rangeCount; ++i)
      {
        rRanges[i] = basicFontKerning.GetRange(i);
        totalGlyphs += rRanges[i].Length;
      }

      if (totalGlyphs != basicFontKerning.Count())
      {
        throw std::invalid_argument("The basicFontKerning object is invalid");
      }
    }


    void BuildGlyphs(std::vector<TextureAtlasGlyphInfo>& rGlyphs, const std::vector<FontGlyphRange>& ranges,
                     const std::vector<TextureAtlasGlyphEntry>& textureAtlasGlyphs, const int32_t textureAtlasGlyphCount,
                     const IFontBasicKerning& basicFontKerning)
    {
      const auto endRangeIndex = static_cast<int32_t>(ranges.size());
      // const int32_t glyphEndIndex = static_cast<int32_t>(rGlyphs.size());

      int32_t glyphIndex = 0;
      int32_t atlasGlyphIndex = 0;
      for (int32_t rangeIndex = 0; rangeIndex < endRangeIndex; ++rangeIndex)
      {
        int32_t rangeGlyphId = ranges[rangeIndex].From;
        const int32_t rangeGlyphEndId = rangeGlyphId + ranges[rangeIndex].Length;

        // Skip unused glyphs
        while (atlasGlyphIndex < textureAtlasGlyphCount && rangeGlyphId > textureAtlasGlyphs[atlasGlyphIndex].Id)
        {
          FSLLOG_WARNING("The texture atlas contained a unexpected glyph: " << textureAtlasGlyphs[atlasGlyphIndex].Id);
          ++atlasGlyphIndex;
        }

        while (rangeGlyphId < rangeGlyphEndId && atlasGlyphIndex < textureAtlasGlyphCount)
        {
          if (textureAtlasGlyphs[atlasGlyphIndex].Id == rangeGlyphId)
          {
            // The texture atlas contained information about the glyph so lets use that
            // textureAtlasGlyphs[atlasGlyphIndex].TextureInfo.;
            const Rectangle atlasRect = textureAtlasGlyphs[atlasGlyphIndex].TextureInfo.TrimmedRect;
            FontGlyphBasicKerning basicKerning = basicFontKerning.Get(glyphIndex);

            rGlyphs[glyphIndex] = TextureAtlasGlyphInfo(atlasRect, basicKerning);
            ++atlasGlyphIndex;
          }
          else
          {
            // The texture atlas did not contain information about the glyph, so mark is as a unavailable glyph
            assert(rangeGlyphId < textureAtlasGlyphs[atlasGlyphIndex].Id);
            FontGlyphBasicKerning basicKerning = basicFontKerning.Get(glyphIndex);
            rGlyphs[glyphIndex] = TextureAtlasGlyphInfo(Rectangle(), basicKerning, TextureAtlasGlyphInfo::Flags::Unavailable);
          }
          ++glyphIndex;
          ++rangeGlyphId;
        }

        // Handle 'ending' glyphs not found in the atlas
        while (rangeGlyphId < rangeGlyphEndId)
        {
          rGlyphs[glyphIndex] = TextureAtlasGlyphInfo();
          ++glyphIndex;
          ++rangeGlyphId;
        }
      }
    }

    //! @brief Find the glyph index
    //! @param ranges the glyph ranges supported by the font.
    //! @param glyph the glyph to find a index for.
    //! @param unknownGlyphIndex the index that will be returned for glyphs outside of our supported ranges
    inline int32_t FindGlyphIndex(const std::vector<FontGlyphRange>& ranges, const int32_t glyph, const int32_t unknownGlyphIndex)
    {
      auto itr = ranges.begin();
      int index = 0;
      while (itr != ranges.end())
      {
        if (itr->Contains(glyph))
        {
          return index + (glyph - itr->From);
        }
        index += itr->Length;
        ++itr;
      }
      return unknownGlyphIndex;
    }
  }


  TextureAtlasBitmapFont::TextureAtlasBitmapFont()
    : m_glyphs(0)
  {
  }


  TextureAtlasBitmapFont::TextureAtlasBitmapFont(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning)
    : m_ranges(basicFontKerning.RangeCount())
    , m_glyphs(basicFontKerning.Count())

  {
    DoConstruct(textureAtlas, basicFontKerning);
  }


  void TextureAtlasBitmapFont::Reset(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning)
  {
    m_ranges.resize(basicFontKerning.RangeCount());
    m_minGlyphId = 0;
    m_maxGlyphId = 0;
    m_unknownGlyphIndex = 0;
    m_glyphs.resize(basicFontKerning.Count());
    m_baseLine = 0;
    m_lineSpacing = 0;
    DoConstruct(textureAtlas, basicFontKerning);
  }


  Point2 TextureAtlasBitmapFont::MeasureString(const char* const psz) const
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not null");
    }

    return MeasureString(psz, 0, std::strlen(psz));
  }


  Point2 TextureAtlasBitmapFont::MeasureString(const char* const pStr, const uint32_t startIndex, const std::size_t length) const
  {
    if (pStr == nullptr)
    {
      throw std::invalid_argument("pStr can not null");
    }
    if (length > std::numeric_limits<uint32_t>::max())
    {
      throw std::invalid_argument("length can not be larger than a uint32");
    }

    if (m_minGlyphId == m_maxGlyphId)
    {
      return Point2(0, 0);
    }

    // extract information about the characters we are rendering
    int32_t renderRight = 0;
    int32_t renderBottom = 0;

    int32_t layoutOffsetX = 0;
    int32_t dstIndex = 0;
    for (uint32_t index = startIndex; index < length; ++index)
    {
      const auto currentGlyph = static_cast<int32_t>(pStr[index]);
      int32_t currentGlyphIndex;
      if (currentGlyph >= m_minGlyphId && currentGlyph < m_maxGlyphId)
      {
        // a glyph that we might have information about
        currentGlyphIndex = FindGlyphIndex(m_ranges, currentGlyph, m_unknownGlyphIndex);
      }
      else
      {
        // A unknown glyph
        currentGlyphIndex = m_unknownGlyphIndex;
      }

      const int32_t currentRight = layoutOffsetX + m_glyphs[currentGlyphIndex].Kerning.OffsetX + m_glyphs[currentGlyphIndex].SrcRect.Width();
      const int32_t currentBottom = m_glyphs[currentGlyphIndex].Kerning.OffsetY + m_glyphs[currentGlyphIndex].SrcRect.Height();
      if (currentRight > renderRight)
      {
        renderRight = currentRight;
      }
      if (currentBottom > renderBottom)
      {
        renderBottom = currentBottom;
      }

      layoutOffsetX += m_glyphs[currentGlyphIndex].Kerning.LayoutWidth;
      ++dstIndex;
    }
    return Point2(renderRight, renderBottom);
  }


  void TextureAtlasBitmapFont::ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const char* const pStr, const uint32_t startIndex,
                                                  const std::size_t length) const
  {
    if (pStr == nullptr)
    {
      throw std::invalid_argument("pStr can not null");
    }
    if (length > std::numeric_limits<uint32_t>::max())
    {
      throw std::invalid_argument("length can not be larger than a uint32");
    }
    if (rDst.size() < std::size_t(length))
    {
      throw std::invalid_argument("rDst is too small");
    }

    if (m_minGlyphId == m_maxGlyphId)
    {
      uint32_t dstIndex = 0;
      for (uint32_t index = startIndex; index < length; ++index)
      {
        rDst[dstIndex] = FontGlyphPosition(Vector2(0.0f, 0.0f), Rectangle(), 0);
        ++dstIndex;
      }
      return;
    }

    // extract information about the characters we are rendering
    int32_t layoutOffsetX = 0;
    uint32_t dstIndex = 0;
    for (uint32_t index = startIndex; index < length; ++index)
    {
      const auto currentGlyph = static_cast<int32_t>(pStr[index]);
      int32_t currentGlyphIndex;
      if (currentGlyph >= m_minGlyphId && currentGlyph < m_maxGlyphId)
      {
        // a glyph that we might have information about
        currentGlyphIndex = FindGlyphIndex(m_ranges, currentGlyph, m_unknownGlyphIndex);
      }
      else
      {
        // A unknown glyph
        currentGlyphIndex = m_unknownGlyphIndex;
      }

      const Vector2 dstOffset = Vector2(static_cast<float>(layoutOffsetX + m_glyphs[currentGlyphIndex].Kerning.OffsetX),
                                        static_cast<float>(m_glyphs[currentGlyphIndex].Kerning.OffsetY));
      rDst[dstIndex] = FontGlyphPosition(dstOffset, m_glyphs[currentGlyphIndex].SrcRect, m_glyphs[currentGlyphIndex].Kerning.LayoutWidth);
      layoutOffsetX += m_glyphs[currentGlyphIndex].Kerning.LayoutWidth;
      ++dstIndex;
    }
  }


  void TextureAtlasBitmapFont::DoConstruct(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning)
  {
    // Setup everything
    const FontDesc fontDesc = basicFontKerning.GetDesc();
    m_baseLine = fontDesc.BaseLine;
    m_lineSpacing = fontDesc.LineSpacing;

    // Extract the ranges and validate the glyph count
    ExtractRanges(m_ranges, basicFontKerning);

    if (!m_ranges.empty())
    {
      m_minGlyphId = m_ranges.front().From;
      m_maxGlyphId = (m_ranges.back().From + std::max(m_ranges.back().Length, static_cast<int32_t>(0)));
    }

    // Extract the glyph entries from the texture atlas
    std::vector<TextureAtlasGlyphEntry> textureAtlasGlyphs(basicFontKerning.Count());
    const int32_t textureAtlasGlyphCount = ExtractAtlasGlyphs(textureAtlasGlyphs, textureAtlas, basicFontKerning);

    // Use the extracted information to build the glyphs
    BuildGlyphs(m_glyphs, m_ranges, textureAtlasGlyphs, textureAtlasGlyphCount, basicFontKerning);
  }
}
