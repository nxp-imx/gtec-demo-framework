/****************************************************************************************************************************************************
 * Copyright 2021, 2023 NXP
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

#include <FslGraphics/Sprite/Info/SpriteFontCharInfo.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestFont_SpriteFontCharInfo = TestFixtureFslGraphics;
}


TEST(TestFont_SpriteFontCharInfo, Construct_Default)
{
  SpriteFontCharInfo value;
  EXPECT_EQ(0u, value.CharInfo.Id);
  EXPECT_EQ(PxRectangleU16(), value.CharInfo.SrcTextureRectPx);
  EXPECT_EQ(PxVector2(), value.RenderInfo.OffsetPxf);
  EXPECT_EQ(0.0f, value.RenderInfo.XAdvancePxf);
}


TEST(TestFont_SpriteFontCharInfo, Construct)
{
  const uint32_t id = 10;
  const auto srcTextureRectPx = PxRectangleU16::Create(1, 2, 3, 4);
  const auto offsetPx = PxPoint2::Create(5, 6);
  const auto xAdvancePx = PxValueU16(43);
  const NativeTextureArea nativeTex(0.1f, 0.2f, 0.3f, 0.4f);
  const auto scaledSizePxf = PxSize2DF::Create(3, 4);
  const auto offsetPxf = PxVector2::Create(5.1f, 6.2f);
  const float xAdvancePxf = 42.6f;

  SpriteFontCharInfo value(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                           RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf));
  EXPECT_EQ(id, value.CharInfo.Id);
  EXPECT_EQ(srcTextureRectPx, value.CharInfo.SrcTextureRectPx);
  EXPECT_EQ(nativeTex, value.RenderInfo.TextureArea);
  EXPECT_EQ(offsetPxf, value.RenderInfo.OffsetPxf);
  EXPECT_EQ(xAdvancePxf, value.RenderInfo.XAdvancePxf);
}

TEST(TestFont_SpriteFontCharInfo, OpEqual)
{
  const uint32_t id = 10;
  const auto srcTextureRectPx = PxRectangleU16::Create(1, 2, 3, 4);
  const auto offsetPx = PxPoint2::Create(5, 6);
  const auto xAdvancePx = PxValueU16(43);
  const NativeTextureArea nativeTex(0.1f, 0.2f, 0.3f, 0.4f);
  const auto scaledSizePxf = PxSize2DF::Create(3, 4);
  const auto offsetPxf = PxVector2::Create(5.1f, 6.2f);
  const float xAdvancePxf = 42.6f;

  EXPECT_EQ(SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)),
            SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)));
}

TEST(TestFont_SpriteFontCharInfo, OpNotEqual)
{
  const uint32_t id = 10;
  const auto srcTextureRectPx = PxRectangleU16::Create(1, 2, 3, 4);
  const auto offsetPx = PxPoint2::Create(5, 6);
  const auto xAdvancePx = PxValueU16(43);
  const NativeTextureArea nativeTex(0.1f, 0.2f, 0.3f, 0.4f);
  const auto scaledSizePxf = PxSize2DF::Create(3, 4);
  const auto offsetPxf = PxVector2::Create(5.1f, 6.2f);
  const float xAdvancePxf = 42.6f;


  auto almostSrcTextureRectPx = srcTextureRectPx;
  almostSrcTextureRectPx.Add(PxPoint2U::Create(1, 1));

  EXPECT_NE(SpriteFontCharInfo(CoreFontCharInfo(id + 1, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)),
            SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)));
  EXPECT_NE(SpriteFontCharInfo(CoreFontCharInfo(id, almostSrcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)),
            SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)));
  EXPECT_NE(SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, PxVector2(offsetPxf.X + PxValueF(1.0f), offsetPxf.Y), xAdvancePxf)),
            SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)));
  EXPECT_NE(SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf + 1.0f)),
            SpriteFontCharInfo(CoreFontCharInfo(id, srcTextureRectPx, offsetPx, xAdvancePx),
                               RenderFontCharInfo(nativeTex, scaledSizePxf, offsetPxf, xAdvancePxf)));
}
