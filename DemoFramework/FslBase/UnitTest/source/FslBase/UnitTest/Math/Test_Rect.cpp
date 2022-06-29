/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogRect.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Rect = TestFixtureFslBase;
}


TEST(TestMath_Rect, Empty)
{
  Rect value;

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(Rect(), Rect::Empty());
  EXPECT_EQ(0.0f, value.Left());
  EXPECT_EQ(0.0f, value.Top());
  EXPECT_EQ(0.0f, value.Right());
  EXPECT_EQ(0.0f, value.Bottom());
  EXPECT_FLOAT_EQ(0.0f, value.Width());
  EXPECT_FLOAT_EQ(0.0f, value.Height());
  EXPECT_EQ(0.0f, value.TopLeft().X);
  EXPECT_EQ(0.0f, value.TopLeft().Y);
  EXPECT_EQ(0.0f, value.TopRight().X);
  EXPECT_EQ(0.0f, value.TopRight().Y);
  EXPECT_EQ(0.0f, value.BottomLeft().X);
  EXPECT_EQ(0.0f, value.BottomLeft().Y);
  EXPECT_EQ(0.0f, value.BottomRight().X);
  EXPECT_EQ(0.0f, value.BottomRight().Y);
}


TEST(TestMath_Rect, Construct1)
{
  float offsetX = 1.0f;
  float offsetY = 2.0f;
  float width = 10.0f;
  float height = 20.0f;
  Rect value(offsetX, offsetY, width, height);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_FLOAT_EQ(offsetX + width, value.Right());
  EXPECT_FLOAT_EQ(offsetY + height, value.Bottom());

  EXPECT_FLOAT_EQ(width, value.Width());
  EXPECT_FLOAT_EQ(height, value.Height());

  EXPECT_EQ(offsetX, value.TopLeft().X);
  EXPECT_EQ(offsetY, value.TopLeft().Y);

  EXPECT_FLOAT_EQ(offsetX + width, value.TopRight().X);
  EXPECT_EQ(offsetY, value.TopRight().Y);

  EXPECT_EQ(offsetX, value.BottomLeft().X);
  EXPECT_FLOAT_EQ(offsetY + height, value.BottomLeft().Y);

  EXPECT_FLOAT_EQ(offsetX + width, value.BottomRight().X);
  EXPECT_FLOAT_EQ(offsetY + height, value.BottomRight().Y);
}

TEST(TestMath_Rect, FromLeftTopRightBottom)
{
  float left = 1.0f;
  float top = 2.0f;
  float right = 10.0f;
  float bottom = 20.0f;
  auto value = Rect::FromLeftTopRightBottom(left, top, right, bottom);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_FLOAT_EQ(right - left, value.Width());
  EXPECT_FLOAT_EQ(bottom - top, value.Height());

  EXPECT_EQ(left, value.TopLeft().X);
  EXPECT_EQ(top, value.TopLeft().Y);
  EXPECT_EQ(right, value.TopRight().X);
  EXPECT_EQ(top, value.TopRight().Y);
  EXPECT_EQ(left, value.BottomLeft().X);
  EXPECT_EQ(bottom, value.BottomLeft().Y);
  EXPECT_EQ(right, value.BottomRight().X);
  EXPECT_EQ(bottom, value.BottomRight().Y);
}

TEST(TestMath_Rect, SetX)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, width, height);

  const float newX = 10.0f;
  value.SetX(newX);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(newX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_FLOAT_EQ(newX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_FLOAT_EQ(width, value.Width());
  EXPECT_FLOAT_EQ(height, value.Height());
}

TEST(TestMath_Rect, SetY)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, width, height);

  const float newY = 10.0f;
  value.SetY(newY);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(newY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_FLOAT_EQ(newY + height, value.Bottom());
  EXPECT_FLOAT_EQ(width, value.Width());
  EXPECT_FLOAT_EQ(height, value.Height());
}


TEST(TestMath_Rect, SetLeftRight)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, width, height);

  const float newLeft = 10.0f;
  const float newRight = 15.0f;
  value.SetLeftRight(newLeft, newRight);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(newLeft, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(newRight, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newRight - newLeft, value.Width());
  EXPECT_FLOAT_EQ(height, value.Height());
}

TEST(TestMath_Rect, SetTopBottom)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, width, height);

  const float newTop = 10.0f;
  const float newBottom = 15.0f;
  value.SetTopBottom(newTop, newBottom);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(newTop, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(newBottom, value.Bottom());
  EXPECT_FLOAT_EQ(width, value.Width());
  EXPECT_EQ(newBottom - newTop, value.Height());
}


TEST(TestMath_Rect, SetWidth)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, 3.0f, height);

  const float newWidth = 10.0f;
  value.SetWidth(newWidth);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_FLOAT_EQ(newWidth, value.Width());
  EXPECT_FLOAT_EQ(height, value.Height());
}


TEST(TestMath_Rect, SetHeight)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  Rect value(offsetX, offsetY, width, 4.0f);

  const float newHeight = 10.0f;
  value.SetHeight(newHeight);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_FLOAT_EQ(width, value.Width());
  EXPECT_FLOAT_EQ(newHeight, value.Height());
}


TEST(TestMath_Rect, SetWidth_Invalid)
{
  Rect value(0.0f, 0.0f, 10.0f, 10.0f);

  value.SetWidth(-10.0f);
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0.0f, value.Left());
  EXPECT_EQ(0.0f, value.Top());
  EXPECT_EQ(0.0f, value.Right());
  EXPECT_EQ(10.0f, value.Bottom());
  EXPECT_FLOAT_EQ(0.0f, value.Width());
  EXPECT_FLOAT_EQ(10.0f, value.Height());
}


TEST(TestMath_Rect, SetHeight_Invalid)
{
  Rect value(0.0f, 0.0f, 10.0f, 10.0f);

  value.SetHeight(-10.0f);
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0.0f, value.Left());
  EXPECT_EQ(0.0f, value.Top());
  EXPECT_EQ(10.0f, value.Right());
  EXPECT_EQ(0.0f, value.Bottom());
  EXPECT_FLOAT_EQ(10.0f, value.Width());
  EXPECT_FLOAT_EQ(0.0f, value.Height());
}


TEST(TestMath_Rect, GetCenter)
{
  const float offsetX = 1.0f;
  const float offsetY = 2.0f;
  const float width = 3.0f;
  const float height = 4.0f;
  Rect value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + (width / 2.0f), value.GetCenter().X);
  EXPECT_EQ(offsetY + (height / 2.0f), value.GetCenter().Y);
}

TEST(TestMath_Rect, IntersectsSelf)
{
  {
    constexpr Rect rectA(1, 2, 5, 4);
    constexpr Rect rectB(7, 1, 7, 6);
    constexpr Rect rectC(6, 1, 7, 6);
    constexpr Rect rectD(5, 1, 7, 6);
    // constexpr Rect rectE(4, 1, 7, 6);

    EXPECT_TRUE(rectA.Intersects(rectA));
    EXPECT_TRUE(rectB.Intersects(rectB));
    EXPECT_TRUE(rectC.Intersects(rectC));
    EXPECT_TRUE(rectD.Intersects(rectD));
  }
}

TEST(TestMath_Rect, Intersects_BruteForce)
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
    constexpr Rect rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        Rect rectA(static_cast<float>(x), static_cast<float>(y), 4, 3);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(rectB));
        EXPECT_EQ(result[x + yOffset] != 0u, rectB.Intersects(rectA));
      }
    }
  }
}

TEST(TestMath_Rect, Intersect_BruteForce)
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
    constexpr Rect rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        Rect rectA(static_cast<float>(x), static_cast<float>(y), 4, 3);

        auto res1 = Rect::Intersect(rectA, rectB);
        auto res2 = Rect::Intersect(rectB, rectA);
        EXPECT_EQ(res1, res2);
        // check for intersection
        EXPECT_EQ(result[x + yOffset] != 0u, !res1.IsEmpty());
        // check for intersection
        EXPECT_EQ(result[x + yOffset] != 0u, !res1.IsEmpty());
        // check result
        if (result[x + yOffset] != 0u)
        {
          EXPECT_EQ(std::max(rectA.Left(), rectB.Left()), res1.Left());
          EXPECT_EQ(std::max(rectA.Top(), rectB.Top()), res1.Top());
          EXPECT_EQ(std::min(rectA.Right(), rectB.Right()), res1.Right());
          EXPECT_EQ(std::min(rectA.Bottom(), rectB.Bottom()), res1.Bottom());
        }
      }
    }
  }
}

TEST(TestMath_Rect, Union_BruteForce)
{
  constexpr Rect rectB(4, 3, 6, 5);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      Rect rectA(static_cast<float>(x), static_cast<float>(y), 4, 3);

      auto res1 = Rect::Union(rectA, rectB);
      auto res2 = Rect::Union(rectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), rectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), rectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), rectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), rectB.Bottom()), res1.Bottom());
    }
  }
}
