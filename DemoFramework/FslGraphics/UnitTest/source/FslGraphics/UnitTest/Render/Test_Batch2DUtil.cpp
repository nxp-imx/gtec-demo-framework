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

#include <FslGraphics/Render/Batch2DUtil.hpp>
#include <FslBase/Log/Math/Pixel/LogPxAreaRectangleF.hpp>
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

    auto left = static_cast<float>(srcRect.Left());
    auto top = static_cast<float>(srcRect.Top());
    auto right = static_cast<float>(srcRect.Right());
    auto bottom = static_cast<float>(srcRect.Bottom());

    if (startSrc.Width() > 0)
    {
      const auto leftChanged = (clippedSrc.Left() - startSrc.Left()) / startSrc.Width();
      const auto rightChanged = (startSrc.Right() - clippedSrc.Right()) / startSrc.Width();

      left += (leftChanged * srcRect.Width());
      right -= (rightChanged * srcRect.Width());
    }
    else
    {
      left = 0;
      right = 0;
    }

    if (startSrc.Height() > 0)
    {
      const auto topChanged = (clippedSrc.Top() - startSrc.Top()) / startSrc.Height();
      const auto bottomChanged = (startSrc.Bottom() - clippedSrc.Bottom()) / startSrc.Height();
      top += (topChanged * srcRect.Height());
      bottom -= (bottomChanged * srcRect.Height());
    }
    else
    {
      top = 0;
      bottom = 0;
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
    constexpr PxAreaRectangleF rectB(4, 3, 6, 5);

    PxAreaRectangleF srcRect(10, 20, 40, 80);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        PxAreaRectangleF rectA(float(x), float(y), 4, 3);
        auto resA = rectA;
        auto resSrcA = srcRect;
        auto resB = rectB;
        auto resSrcB = srcRect;

        const bool clippedA = Batch2DUtil::Clip(resA, resSrcA, rectB);
        const bool clippedB = Batch2DUtil::Clip(resB, resSrcB, rectA);
        EXPECT_EQ(result[x + yOffset] != 0u, clippedA);
        EXPECT_EQ(result[x + yOffset] != 0u, clippedB);
        EXPECT_EQ(clippedA, clippedB);
        // check for intersection
        if (result[x + yOffset] != 0u && clippedA)
        {
          EXPECT_EQ(resA, resB);
          // check result
          ASSERT_EQ(std::max(rectA.Left(), rectB.Left()), resA.Left());
          ASSERT_EQ(std::max(rectA.Top(), rectB.Top()), resA.Top());
          ASSERT_EQ(std::min(rectA.Right(), rectB.Right()), resA.Right());
          ASSERT_EQ(std::min(rectA.Bottom(), rectB.Bottom()), resA.Bottom());

          auto expectedSrcA = CalcClippedSrcRect(rectA, resA, srcRect);
          auto expectedSrcB = CalcClippedSrcRect(rectB, resB, srcRect);

          EXPECT_FLOAT_EQ(expectedSrcA.Left(), resSrcA.Left());
          EXPECT_FLOAT_EQ(expectedSrcA.Top(), resSrcA.Top());
          EXPECT_FLOAT_EQ(expectedSrcA.Right(), resSrcA.Right());
          EXPECT_FLOAT_EQ(expectedSrcA.Bottom(), resSrcA.Bottom());

          EXPECT_FLOAT_EQ(expectedSrcB.Left(), resSrcB.Left());
          EXPECT_FLOAT_EQ(expectedSrcB.Top(), resSrcB.Top());
          EXPECT_FLOAT_EQ(expectedSrcB.Right(), resSrcB.Right());
          EXPECT_FLOAT_EQ(expectedSrcB.Bottom(), resSrcB.Bottom());
        }
      }
    }
  }
}
