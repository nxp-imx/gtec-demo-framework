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
#include <FslBase/NumericCast.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslGraphics/Font/BinaryFontBasicKerningLoader.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/BinaryTextureAtlasLoader.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphicsContent.hpp>
#include <fmt/format.h>

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr IO::PathView FontAtlasPath("Font/NormalAtlas/FontAtlas.bta");
    constexpr IO::PathView FontBasicPath("Font/NormalAtlas/FontAtlas_Font.fbk");
  }

  class TestFont_BitmapFontConverter : public TestFixtureFslGraphicsContent
  {
  protected:
    IO::Path m_fontAtlasPath;
    IO::Path m_fontBasicPath;

  public:
    TestFont_BitmapFontConverter()
      : m_fontAtlasPath(IO::Path::Combine(GetContentPath(), LocalConfig::FontAtlasPath))
      , m_fontBasicPath(IO::Path::Combine(GetContentPath(), LocalConfig::FontBasicPath))
    {
    }
  };

  struct AtlasCharInfo
  {
    PxPoint2 OffsetPx;
    PxRectangleU SrcTextureRectPx;
  };

  BitmapFontChar Get(const ReadOnlySpan<BitmapFontChar>& charSpan, const int32_t id)
  {
    const auto orgId = NumericCast<uint32_t>(id);
    for (std::size_t i = 0; i < charSpan.size(); ++i)
    {
      if (charSpan[i].Id == orgId)
      {
        return charSpan[i];
      }
    }
    throw NotFoundException("Could not find the expected char id");
  }


  AtlasCharInfo FindAtlasEntry(const StringViewLite strPath, BasicTextureAtlas& atlas, const int32_t charId)
  {
    IO::Path glyphPath(fmt::format("{}/{:X}", strPath, charId));

    for (uint32_t i = 0; i < atlas.Count(); ++i)
    {
      const auto& entry = atlas.GetEntry(i);
      if (entry.Name == glyphPath)
      {
        return {PxPoint2(entry.TextureInfo.TrimMarginPx.Left, entry.TextureInfo.TrimMarginPx.Top), entry.TextureInfo.TrimmedRectPx};
      }
    }
    return {};
  }
}


TEST_F(TestFont_BitmapFontConverter, Convert)
{
  BasicTextureAtlas atlas;
  BinaryTextureAtlasLoader::Load(atlas, m_fontAtlasPath);

  BasicFontKerning font;
  BinaryFontBasicKerningLoader::Load(font, m_fontBasicPath);

  auto bitmapFont = BitmapFontConverter::ToBitmapFont(atlas, font);

  EXPECT_EQ(font.GetName(), bitmapFont.GetName());
  EXPECT_EQ(font.GetPathName(), bitmapFont.GetTextureName());
  EXPECT_EQ(font.GetDesc().LineSpacing, bitmapFont.GetSize());
  EXPECT_EQ(font.GetDesc().LineSpacing, bitmapFont.GetLineSpacingPx());
  EXPECT_EQ(font.GetDesc().BaseLine, bitmapFont.GetBaseLinePx());
  EXPECT_EQ(BitmapFontType::Bitmap, bitmapFont.GetFontType());
  EXPECT_EQ(NumericCast<std::size_t>(font.Count()), bitmapFont.GetCharCount());

  auto charSpan = bitmapFont.GetChars();
  EXPECT_EQ(bitmapFont.GetCharCount(), charSpan.size());

  const StringViewLite atlasPath = font.GetPathName().AsStringViewLite();

  int32_t charIndex = 0;
  for (int32_t rangeIndex = 0; rangeIndex < font.RangeCount(); ++rangeIndex)
  {
    auto range = font.GetRange(rangeIndex);
    auto rangeCharId = range.From;
    const auto rangeCharIdEnd = rangeCharId + range.Length;
    while (rangeCharId < rangeCharIdEnd)
    {
      auto entry = font.Get(charIndex);
      auto convertedEntry = Get(charSpan, rangeCharId);
      // Compare the parts we can lookup directly in the kerning
      EXPECT_EQ(entry.LayoutWidthPx, convertedEntry.XAdvancePx);

      // Find the atlas entry and use that to compare the rest

      AtlasCharInfo atlasInfo = FindAtlasEntry(atlasPath, atlas, rangeCharId);
      atlasInfo.OffsetPx.X += entry.OffsetXPx;
      atlasInfo.OffsetPx.Y += entry.OffsetYPx;

      EXPECT_EQ(atlasInfo.OffsetPx, convertedEntry.OffsetPx);
      EXPECT_EQ(atlasInfo.SrcTextureRectPx, convertedEntry.SrcTextureRectPx);

      ++rangeCharId;
      ++charIndex;
    }
  }
}
