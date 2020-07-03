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

#include <FslGraphics/Font/BitmapFontChar.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestFont_BitmapFontChar = TestFixtureFslGraphics;
}


TEST(TestFont_BitmapFontChar, Construct_Default)
{
  BitmapFontChar value;
  EXPECT_EQ(0u, value.Id);
  EXPECT_EQ(PxRectangleU(), value.SrcTextureRectPx);
  EXPECT_EQ(PxPoint2(), value.OffsetPx);
  EXPECT_EQ(0u, value.XAdvancePx);
}


TEST(TestFont_BitmapFontChar, Construct)
{
  const uint32_t id = 10;
  const PxRectangleU srcTextureRectPx(1, 2, 3, 4);
  const PxPoint2 offsetPx(5, 6);
  uint16_t xAdvance = 42;

  BitmapFontChar value(id, srcTextureRectPx, offsetPx, xAdvance);
  EXPECT_EQ(id, value.Id);
  EXPECT_EQ(srcTextureRectPx, value.SrcTextureRectPx);
  EXPECT_EQ(offsetPx, value.OffsetPx);
  EXPECT_EQ(xAdvance, value.XAdvancePx);
}

TEST(TestFont_BitmapFontChar, OpEqual)
{
  const uint32_t id = 10;
  const PxRectangleU srcTextureRectPx(1, 2, 3, 4);
  const PxPoint2 offsetPx(5, 6);
  uint16_t xAdvance = 42;

  EXPECT_EQ(BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance), BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance));
}

TEST(TestFont_BitmapFontChar, OpNotEqual)
{
  const uint32_t id = 10;
  const PxRectangleU srcTextureRectPx(1, 2, 3, 4);
  const PxPoint2 offsetPx(5, 6);
  uint16_t xAdvance = 42;

  auto almostSrcTextureRectPx = srcTextureRectPx;
  almostSrcTextureRectPx.Add(PxPoint2U(1, 1));

  EXPECT_NE(BitmapFontChar(id + 1, srcTextureRectPx, offsetPx, xAdvance), BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance));
  EXPECT_NE(BitmapFontChar(id, almostSrcTextureRectPx, offsetPx, xAdvance), BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance));
  EXPECT_NE(BitmapFontChar(id, srcTextureRectPx, PxPoint2(offsetPx.X + 1, offsetPx.Y), xAdvance),
            BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance));
  EXPECT_NE(BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance + 1), BitmapFontChar(id, srcTextureRectPx, offsetPx, xAdvance));
}
