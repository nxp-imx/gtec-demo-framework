/****************************************************************************************************************************************************
 * Copyright 2020, 2023-2024 NXP
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

#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestFont_BitmapFont = TestFixtureFslGraphics;
}


TEST(TestFont_BitmapFont, Construct_Default)
{
  BitmapFont font;
  EXPECT_EQ("", font.GetName());
  EXPECT_EQ("", font.GetTextureName());
  EXPECT_EQ(0u, font.GetSize());
  EXPECT_EQ(0u, font.GetLineSpacingPx().Value);
  EXPECT_EQ(0u, font.GetBaseLinePx().Value);
  EXPECT_EQ(BitmapFontType::Bitmap, font.GetFontType());
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<BitmapFontChar>(), font.GetChars()));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<BitmapFontKerning>(), font.GetKernings()));
}


TEST(TestFont_BitmapFont, Construct)
{
  const StringViewLite name("font name");
  const uint16_t dpi = 160;
  const uint16_t size = 42;
  const auto lineSpacingPx = PxValueU16(21);
  const auto baseLinePx = PxValueU16(18);
  const auto paddingPx = PxThicknessU16::Create(1, 2, 3, 4);
  const StringViewLite textureName("font texture name");
  const BitmapFontType fontType = BitmapFontType::SDF;
  const BitmapFont::SdfParams sdfParams(2, 1.0f);

  constexpr const BitmapFontChar Char0(0u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontKerning Kerning0(0u, 1u, PxValue::Create(-1));

  std::vector<BitmapFontChar> chars = {Char0};
  std::vector<BitmapFontKerning> kernings = {Kerning0};

  BitmapFont font(name, dpi, size, lineSpacingPx, baseLinePx, paddingPx, textureName, fontType, sdfParams, chars, kernings);

  EXPECT_EQ(name, font.GetName());
  EXPECT_EQ(textureName, font.GetTextureName());
  EXPECT_EQ(dpi, font.GetDpi());
  EXPECT_EQ(size, font.GetSize());
  EXPECT_EQ(lineSpacingPx, font.GetLineSpacingPx());
  EXPECT_EQ(baseLinePx, font.GetBaseLinePx());
  EXPECT_EQ(paddingPx, font.GetPaddingPx());
  EXPECT_EQ(fontType, font.GetFontType());
  EXPECT_EQ(sdfParams, font.GetSdfParams());
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(chars), font.GetChars()));
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(kernings), font.GetKernings()));
}


TEST(TestFont_BitmapFont, Construct_SortChars)
{
  const StringViewLite name("font name");
  const uint16_t dpi = 160;
  const uint16_t size = 42;
  const auto lineSpacingPx = PxValueU16(21);
  const auto baseLinePx = PxValueU16(18);
  const auto paddingPx = PxThicknessU16::Create(1, 2, 3, 4);
  const StringViewLite textureName("font texture name");
  const BitmapFontType fontType = BitmapFontType::SDF;
  const BitmapFont::SdfParams sdfParams(2, 1.0f);

  constexpr const BitmapFontChar Char0(3u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontChar Char1(2u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontChar Char2(1u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontKerning Kerning0(0u, 1u, PxValue::Create(-1));

  std::vector<BitmapFontChar> chars = {Char0, Char1, Char2};
  std::vector<BitmapFontChar> sortedChars = {Char2, Char1, Char0};
  std::vector<BitmapFontKerning> kernings = {Kerning0};

  BitmapFont font(name, dpi, size, lineSpacingPx, baseLinePx, paddingPx, textureName, fontType, sdfParams, chars, kernings);

  EXPECT_EQ(name, font.GetName());
  EXPECT_EQ(textureName, font.GetTextureName());
  EXPECT_EQ(dpi, font.GetDpi());
  EXPECT_EQ(size, font.GetSize());
  EXPECT_EQ(lineSpacingPx, font.GetLineSpacingPx());
  EXPECT_EQ(baseLinePx, font.GetBaseLinePx());
  EXPECT_EQ(paddingPx, font.GetPaddingPx());
  EXPECT_EQ(fontType, font.GetFontType());
  EXPECT_EQ(sdfParams, font.GetSdfParams());
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(sortedChars), font.GetChars()));
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(kernings), font.GetKernings()));
}


TEST(TestFont_BitmapFont, SortedKernings)
{
  const StringViewLite name("font name");
  const uint16_t dpi = 160;
  const uint16_t size = 42;
  const auto lineSpacingPx = PxValueU16(21);
  const auto baseLinePx = PxValueU16(18);
  const auto paddingPx = PxThicknessU16::Create(1, 2, 3, 4);
  const StringViewLite textureName("font texture name");
  const BitmapFontType fontType = BitmapFontType::SDF;
  const BitmapFont::SdfParams sdfParams(2, 1.0f);

  constexpr const BitmapFontChar Char0(0u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));

  constexpr const BitmapFontKerning Kerning0(3u, 0u, PxValue::Create(-1));
  constexpr const BitmapFontKerning Kerning1(0u, 0u, PxValue::Create(-2));
  constexpr const BitmapFontKerning Kerning2(2u, 1u, PxValue::Create(-3));
  constexpr const BitmapFontKerning Kerning3(2u, 0u, PxValue::Create(-4));
  constexpr const BitmapFontKerning Kerning4(2u, 5u, PxValue::Create(-4));

  std::vector<BitmapFontChar> chars = {Char0};
  std::vector<BitmapFontKerning> kernings = {Kerning0, Kerning1, Kerning2, Kerning3, Kerning4};
  std::vector<BitmapFontKerning> sortedKernings = {Kerning1, Kerning3, Kerning2, Kerning4, Kerning0};

  BitmapFont font(name, dpi, size, lineSpacingPx, baseLinePx, paddingPx, textureName, fontType, sdfParams, chars, kernings);

  EXPECT_EQ(name, font.GetName());
  EXPECT_EQ(textureName, font.GetTextureName());
  EXPECT_EQ(dpi, font.GetDpi());
  EXPECT_EQ(size, font.GetSize());
  EXPECT_EQ(lineSpacingPx, font.GetLineSpacingPx());
  EXPECT_EQ(baseLinePx, font.GetBaseLinePx());
  EXPECT_EQ(paddingPx, font.GetPaddingPx());
  EXPECT_EQ(fontType, font.GetFontType());
  EXPECT_EQ(sdfParams, font.GetSdfParams());
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(chars), font.GetChars()));
  EXPECT_TRUE(SpanUtil::ValueEquals(SpanUtil::AsReadOnlySpan(sortedKernings), font.GetKernings()));
}

TEST(TestFont_BitmapFont, Construct_DuplicatedChar)
{
  const StringViewLite name("font name");
  const uint16_t dpi = 160;
  const uint16_t size = 42;
  const auto lineSpacingPx = PxValueU16(21);
  const auto baseLinePx = PxValueU16(18);
  const auto paddingPx = PxThicknessU16::Create(1, 2, 3, 4);
  const StringViewLite textureName("font texture name");
  const BitmapFontType fontType = BitmapFontType::SDF;
  const BitmapFont::SdfParams sdfParams(2, 1.0f);

  constexpr const BitmapFontChar Char0(0u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontChar Char1(1u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontKerning Kerning0(0u, 1u, PxValue(-1));

  std::vector<BitmapFontChar> chars = {Char0, Char1, Char0};
  std::vector<BitmapFontKerning> kernings = {Kerning0};

  EXPECT_THROW(BitmapFont(name, dpi, size, lineSpacingPx, baseLinePx, paddingPx, textureName, fontType, sdfParams, chars, kernings),
               std::invalid_argument);
}


TEST(TestFont_BitmapFont, Construct_DuplicatedKerning)
{
  const StringViewLite name("font name");
  const uint16_t dpi = 160;
  const uint16_t size = 42;
  const auto lineSpacingPx = PxValueU16(21);
  const auto baseLinePx = PxValueU16(18);
  const auto paddingPx = PxThicknessU16::Create(1, 2, 3, 4);
  const StringViewLite textureName("font texture name");
  const BitmapFontType fontType = BitmapFontType::SDF;
  const BitmapFont::SdfParams sdfParams(2, 1.0f);

  constexpr const BitmapFontChar Char0(0u, PxRectangleU32::Create(1, 2, 3, 4), PxPoint2::Create(5, 6), PxValueU16(7));
  constexpr const BitmapFontKerning Kerning0(0u, 1u, PxValue(-1));
  constexpr const BitmapFontKerning Kerning1(1u, 0u, PxValue(-2));

  std::vector<BitmapFontChar> chars = {Char0};
  std::vector<BitmapFontKerning> kernings = {Kerning0, Kerning1, Kerning0};

  EXPECT_THROW(BitmapFont(name, dpi, size, lineSpacingPx, baseLinePx, paddingPx, textureName, fontType, sdfParams, chars, kernings),
               std::invalid_argument);
}
