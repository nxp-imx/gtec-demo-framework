/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxAreaRectangleF.hpp>
#include <FslGraphics/Render/Batch2DUtil.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <algorithm>
#include <array>

using namespace Fsl;

namespace
{
  using TestRender_Batch2DUtil = TestFixtureFslGraphics;

  PxAreaRectangleF CalcClippedSrcRect(const PxAreaRectangleF startSrc, const PxAreaRectangleF clippedSrc, PxAreaRectangleF srcRect)
  {
    assert(startSrc.Left() <= clippedSrc.Left());
    assert(startSrc.Top() <= clippedSrc.Top());
    assert(startSrc.Right() >= clippedSrc.Right());
    assert(startSrc.Bottom() >= clippedSrc.Bottom());

    auto left = PxValueF(static_cast<float>(srcRect.RawLeft()));
    auto top = PxValueF(static_cast<float>(srcRect.RawTop()));
    auto right = PxValueF(static_cast<float>(srcRect.RawRight()));
    auto bottom = PxValueF(static_cast<float>(srcRect.RawBottom()));

    if (startSrc.Width() > PxValueF())
    {
      const auto leftChanged = (clippedSrc.Left() - startSrc.Left()) / startSrc.Width();
      const auto rightChanged = (startSrc.Right() - clippedSrc.Right()) / startSrc.Width();

      left += (leftChanged * srcRect.Width());
      right -= (rightChanged * srcRect.Width());
    }
    else
    {
      left = PxValueF();
      right = PxValueF();
    }

    if (startSrc.Height() > PxValueF())
    {
      const auto topChanged = (clippedSrc.Top() - startSrc.Top()) / startSrc.Height();
      const auto bottomChanged = (startSrc.Bottom() - clippedSrc.Bottom()) / startSrc.Height();
      top += (topChanged * srcRect.Height());
      bottom -= (bottomChanged * srcRect.Height());
    }
    else
    {
      top = PxValueF();
      bottom = PxValueF();
    }
    if (bottom < top || right < left)
    {
      throw std::runtime_error("internal clip error");
    }
    return PxAreaRectangleF::FromLeftTopRightBottom(left, top, right, bottom);
  }
}

TEST(TestRender_Batch2DUtil, Clip_BruteForce)
{
  // Basically brute force test all combinations by moving A from 0,0 to 10,8

  //   012345678901        0123456789012
  // 0                    0
  // 1                    1
  // 2                    2
  // 3     *----*         3    *----*
  // 4 *--*|B   |         4    |B   |*-*
  // 5 | A||    |  -->    5    |    ||A|
  // 6 *--*|    |         6    |    |*-*
  // 7     *----*         7    *----*
  // 8                    8
  {
    std::array<uint8_t, 11 * 10> result = {
      // 0    1      2     3     4     5     6     7     8     9    10
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 1
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 2
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 3
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 4
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 5
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 6
      0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    // 7
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 8
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 9
    };
    constexpr auto RectB = PxAreaRectangleF::Create(4, 3, 6, 5);

    auto srcRect = PxAreaRectangleF::Create(10, 20, 40, 80);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        auto rectA = PxAreaRectangleF::Create(static_cast<float>(x), static_cast<float>(y), 4, 3);
        auto resA = rectA;
        auto resSrcA = srcRect;
        auto resB = RectB;
        auto resSrcB = srcRect;

        const bool clippedA = Batch2DUtil::Clip(resA, resSrcA, RectB);
        const bool clippedB = Batch2DUtil::Clip(resB, resSrcB, rectA);
        EXPECT_EQ(result[x + yOffset] != 0u, clippedA);
        EXPECT_EQ(result[x + yOffset] != 0u, clippedB);
        EXPECT_EQ(clippedA, clippedB);
        // check for intersection
        if (result[x + yOffset] != 0u && clippedA)
        {
          EXPECT_EQ(resA, resB);
          // check result
          ASSERT_EQ(std::max(rectA.Left(), RectB.Left()), resA.Left());
          ASSERT_EQ(std::max(rectA.Top(), RectB.Top()), resA.Top());
          ASSERT_EQ(std::min(rectA.Right(), RectB.Right()), resA.Right());
          ASSERT_EQ(std::min(rectA.Bottom(), RectB.Bottom()), resA.Bottom());

          auto expectedSrcA = CalcClippedSrcRect(rectA, resA, srcRect);
          auto expectedSrcB = CalcClippedSrcRect(RectB, resB, srcRect);

          EXPECT_FLOAT_EQ(expectedSrcA.RawLeft(), resSrcA.RawLeft());
          EXPECT_FLOAT_EQ(expectedSrcA.RawTop(), resSrcA.RawTop());
          EXPECT_FLOAT_EQ(expectedSrcA.RawRight(), resSrcA.RawRight());
          EXPECT_FLOAT_EQ(expectedSrcA.RawBottom(), resSrcA.RawBottom());

          EXPECT_FLOAT_EQ(expectedSrcB.RawLeft(), resSrcB.RawLeft());
          EXPECT_FLOAT_EQ(expectedSrcB.RawTop(), resSrcB.RawTop());
          EXPECT_FLOAT_EQ(expectedSrcB.RawRight(), resSrcB.RawRight());
          EXPECT_FLOAT_EQ(expectedSrcB.RawBottom(), resSrcB.RawBottom());
        }
      }
    }
  }
}

TEST(TestRender_Batch2DUtil, Clip_Now)
{
  PxAreaRectangleF toClipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(170, 799, 181, 813);
  NativeTextureArea textureArea(0.921875000f, 0.662109375f, 0.943359375f, 0.634765625f);
  const PxAreaRectangleF clipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(161, 805, 1916, 1068);

  EXPECT_TRUE(Batch2DUtil::Clip(toClipRect, textureArea, clipRect));
}

TEST(TestRender_Batch2DUtil, Clip_Top)
{
  PxAreaRectangleF toClipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(10, 20, 50, 120);
  NativeTextureArea textureArea(100, 200, 900, 10200);
  const PxAreaRectangleF clipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(0, 40, 1000, 1000);

  EXPECT_TRUE(Batch2DUtil::Clip(toClipRect, textureArea, clipRect));

  // 20%
  // 20000

  EXPECT_FLOAT_EQ(100.0f, textureArea.X0);
  EXPECT_FLOAT_EQ(2200.0f, textureArea.Y0);
  EXPECT_FLOAT_EQ(900.0f, textureArea.X1);
  EXPECT_FLOAT_EQ(10200.0f, textureArea.Y1);
}

TEST(TestRender_Batch2DUtil, Clip_Top_ReversedTexCoord)
{
  PxAreaRectangleF toClipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(10, 20, 50, 120);
  NativeTextureArea textureArea(100, 10200, 900, 200);
  const PxAreaRectangleF clipRect = PxAreaRectangleF::CreateFromLeftTopRightBottom(0, 40, 1000, 1000);

  EXPECT_TRUE(Batch2DUtil::Clip(toClipRect, textureArea, clipRect));

  // 20%
  // 20000

  EXPECT_FLOAT_EQ(100.0f, textureArea.X0);
  EXPECT_FLOAT_EQ(8200.0f, textureArea.Y0);
  EXPECT_FLOAT_EQ(900.0f, textureArea.X1);
  EXPECT_FLOAT_EQ(200.0f, textureArea.Y1);
}
