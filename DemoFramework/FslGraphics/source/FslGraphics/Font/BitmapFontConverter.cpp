#ifndef FSLGRAPHICS_FONT_TYPECONVERTER_HPP
#define FSLGRAPHICS_FONT_TYPECONVERTER_HPP
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

#include <FslGraphics/Font/BitmapFontConverter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Font/IFontBasicKerning.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/NamedAtlasTexture.hpp>
#include <vector>

namespace Fsl
{
  namespace BitmapFontConverter
  {
    namespace
    {
      namespace LocalPath
      {
        constexpr const IO::PathView Slash("/");
      }

      struct TextureAtlasCharEntry
      {
        int32_t Id{0};
        AtlasTextureInfo TextureInfo;

        constexpr TextureAtlasCharEntry() = default;

        constexpr TextureAtlasCharEntry(const int32_t id, const AtlasTextureInfo& textureInfo)
          : Id(id)
          , TextureInfo(textureInfo)
        {
        }
      };

      bool IsGlyphInRange(const IFontBasicKerning& basicFontKerning, const int32_t id)
      {
        for (int32_t i = 0; i < basicFontKerning.RangeCount(); ++i)
        {
          if (basicFontKerning.GetRange(i).Contains(id))
          {
            return true;
          }
        }
        return false;
      }


      uint32_t CountGlyphs(const IFontBasicKerning& basicFontKerning)
      {
        const auto rangeCount = NumericCast<uint32_t>(basicFontKerning.RangeCount());

        uint32_t totalGlyphs = 0;
        for (uint32_t i = 0; i < rangeCount; ++i)
        {
          auto range = basicFontKerning.GetRange(i);
          totalGlyphs += range.Length;
        }

        if (totalGlyphs != NumericCast<uint32_t>(basicFontKerning.Count()))
        {
          throw std::invalid_argument("The basicFontKerning object is invalid");
        }
        return totalGlyphs;
      }


      //! @brief Run through the texture atlas and extract all glyphs found at the basicFontKerning path
      void ExtractChars(std::vector<TextureAtlasCharEntry>& rAtlasChars, const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning,
                        uint16_t& rDefaultDpi)
      {
        rDefaultDpi = 0u;
        uint16_t defaultDpi = 0u;
        assert(rAtlasChars.size() == UncheckedNumericCast<std::size_t>(basicFontKerning.Count()));

        const int32_t atlasEntries = textureAtlas.Count();

        const IO::Path fontPath(basicFontKerning.GetPathName() + LocalPath::Slash);
        const auto fontPathLength = fontPath.GetByteSize();
        const auto dstEndIndex = NumericCast<uint32_t>(basicFontKerning.Count());
        uint32_t dstIndex = 0;
        for (int32_t i = 0; i < atlasEntries; ++i)
        {
          const auto& namedEntry = textureAtlas.GetEntry(i);
          if (namedEntry.Name.StartsWith(fontPath))
          {
            int32_t glyphId = 0;
            if (StringParseUtil::TryParse(glyphId, namedEntry.Name.AsStringViewLite().substr(fontPathLength), 16) &&
                IsGlyphInRange(basicFontKerning, glyphId))
            {
              if (dstIndex >= dstEndIndex)
              {
                throw std::invalid_argument("The texture atlas contains more glyphs than expected");
              }

              rAtlasChars[dstIndex] = TextureAtlasCharEntry(glyphId, namedEntry.TextureInfo);

              ++dstIndex;
              // Extract the default dpi
              if (namedEntry.TextureInfo.Dpi != defaultDpi)
              {
                if (defaultDpi != 0)
                {
                  throw std::invalid_argument("fonts glyphs has different 'dpi' settings");
                }
                defaultDpi = namedEntry.TextureInfo.Dpi;
              }
            }
          }
        }
        if (dstIndex < rAtlasChars.size())
        {
          // resize the vector to fit the exact number of chars we extracted from the atlas
          rAtlasChars.resize(dstIndex);
        }
        rDefaultDpi = defaultDpi;
      }


      void ExtractAtlasChars(std::vector<TextureAtlasCharEntry>& rAtlasChars, const ITextureAtlas& textureAtlas,
                             const IFontBasicKerning& basicFontKerning, uint16_t& rDefaultDpi)
      {
        ExtractChars(rAtlasChars, textureAtlas, basicFontKerning, rDefaultDpi);
        // Sort the glyphs by id
        std::sort(rAtlasChars.begin(), rAtlasChars.end(),
                  [](const TextureAtlasCharEntry& lhs, const TextureAtlasCharEntry& rhs) { return (lhs.Id < rhs.Id); });
      }

      // std::size_t IndexOf(const std::vector<BitmapFontChar>& fontChars, const uint32_t id)
      //{
      //  for (std::size_t i = 0; i < fontChars.size(); ++i)
      //  {
      //    if (fontChars[i].Id == id)
      //    {
      //      return i;
      //    }
      //  }
      //  return fontChars.size();
      //}

      void FinalizeChars(std::vector<BitmapFontChar>& rFontChars, std::vector<TextureAtlasCharEntry>& atlasChars,
                         const IFontBasicKerning& basicFontKerning)
      {
        const int32_t endRangeIndex = basicFontKerning.RangeCount();
        uint32_t dstGlyphIndex = 0;
        std::size_t atlasCharIndex = 0;
        int32_t rangeGlyphEndId = 0;
        int32_t rangeCharOffsetMod = 0;
        for (int32_t rangeIndex = 0; rangeIndex < endRangeIndex; ++rangeIndex)
        {
          auto range = basicFontKerning.GetRange(rangeIndex);
          int32_t rangeGlyphId = range.From;
          // If this assert fires then the ranges are not sorted low to high or they overlap
          assert(rangeGlyphId >= rangeGlyphEndId);
          rangeGlyphEndId = rangeGlyphId + range.Length;
          rangeCharOffsetMod += rangeGlyphId;

          // Skip unused glyphs
          // Since
          // - the atlasChars are sorted low to high
          // - the ranges are sorted low to high
          // we can just skip entries in the range until we hit the first match
          while (atlasCharIndex < atlasChars.size() && rangeGlyphId > atlasChars[atlasCharIndex].Id)
          {
            FSLLOG3_WARNING("The texture atlas contained a unexpected glyph '{}', which was skipped", atlasChars[atlasCharIndex].Id);
            ++atlasCharIndex;
          }

          while (rangeGlyphId < rangeGlyphEndId && atlasCharIndex < atlasChars.size())
          {
            FontGlyphBasicKerning basicKerning = basicFontKerning.Get(rangeGlyphId - rangeCharOffsetMod);
            if (atlasChars[atlasCharIndex].Id == rangeGlyphId)
            {
              // The texture atlas contained information about the glyph so lets use that
              const auto atlasRect = atlasChars[atlasCharIndex].TextureInfo.TrimmedRectPx;
              assert(atlasChars[atlasCharIndex].TextureInfo.TrimMarginPx.Left <= uint32_t(std::numeric_limits<int16_t>::max() / 2));
              assert(atlasChars[atlasCharIndex].TextureInfo.TrimMarginPx.Top <= uint32_t(std::numeric_limits<int16_t>::max() / 2));
              basicKerning.OffsetXPx += uint16_t(atlasChars[atlasCharIndex].TextureInfo.TrimMarginPx.Left);
              basicKerning.OffsetYPx += uint16_t(atlasChars[atlasCharIndex].TextureInfo.TrimMarginPx.Top);

              rFontChars[dstGlyphIndex] =
                BitmapFontChar(rangeGlyphId, atlasRect, PxPoint2(basicKerning.OffsetXPx, basicKerning.OffsetYPx), basicKerning.LayoutWidthPx);
              ++atlasCharIndex;
            }
            else
            {
              // The texture atlas did not contain information about the glyph, so mark is as a unavailable glyph
              assert(rangeGlyphId < atlasChars[atlasCharIndex].Id);
              rFontChars[dstGlyphIndex] = BitmapFontChar(rangeGlyphId, PxRectangleU32(), PxPoint2(), basicKerning.LayoutWidthPx);
            }
            ++dstGlyphIndex;
            ++rangeGlyphId;
          }

          // Handle 'ending' glyphs not found in the atlas
          while (rangeGlyphId < rangeGlyphEndId)
          {
            FontGlyphBasicKerning basicKerning = basicFontKerning.Get(rangeGlyphId - rangeCharOffsetMod);
            rFontChars[dstGlyphIndex] = BitmapFontChar(rangeGlyphId, PxRectangleU32(), PxPoint2(), basicKerning.LayoutWidthPx);
            ++dstGlyphIndex;
            ++rangeGlyphId;
          }
        }

        if (dstGlyphIndex < rFontChars.size())
        {
          // resize the vector to fit the exact number of chars we extracted from the atlas
          rFontChars.resize(dstGlyphIndex);
        }
      }


      std::vector<BitmapFontChar> ExtractChars(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning, uint16_t& rDefaultDpi)
      {
        const uint32_t numGlyphs = CountGlyphs(basicFontKerning);
        assert(numGlyphs == UncheckedNumericCast<uint32_t>(basicFontKerning.Count()));

        // Extract the glyph entries from the texture atlas
        std::vector<TextureAtlasCharEntry> atlasChars(basicFontKerning.Count());
        ExtractAtlasChars(atlasChars, textureAtlas, basicFontKerning, rDefaultDpi);

        std::vector<BitmapFontChar> bitmapFontChars(numGlyphs);

        FinalizeChars(bitmapFontChars, atlasChars, basicFontKerning);

        return bitmapFontChars;
      }

      inline std::vector<BitmapFontKerning> ExtractKernings(const ITextureAtlas& /*unused*/, const IFontBasicKerning& /*unused*/)
      {
        //! The basic font does not include per char kernings
        return {};
      }
    }

    BitmapFont ToBitmapFont(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning, const BitmapFontType fontType)
    {
      auto desc = basicFontKerning.GetDesc();
      const uint16_t size = desc.LineSpacing;    // We dont have a better number for this parameter
      const uint16_t lineSpacingPx = desc.LineSpacing;
      const uint16_t baseLinePx = desc.BaseLine;

      uint16_t defaultDpi = 0;
      auto chars = ExtractChars(textureAtlas, basicFontKerning, defaultDpi);
      auto kernings = ExtractKernings(textureAtlas, basicFontKerning);
      return {basicFontKerning.GetName().AsStringViewLite(),
              defaultDpi,
              size,
              lineSpacingPx,
              baseLinePx,
              PxThicknessU16(),
              basicFontKerning.GetPathName().AsStringViewLite(),
              fontType,
              BitmapFont::SdfParams(),
              chars,
              kernings};
    }
  };
}

#endif
