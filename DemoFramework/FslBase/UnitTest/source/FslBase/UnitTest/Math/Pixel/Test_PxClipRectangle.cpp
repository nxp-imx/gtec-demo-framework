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

#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Log/Math/Pixel/LogPxClipRectangle.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxClipRectangle = TestFixtureFslBase;
}


TEST(TestMathPixel_PxClipRectangle, ConstructDefault)
{
  PxClipRectangle value;

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(0, value.Right());
  EXPECT_EQ(0, value.Bottom());
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
  EXPECT_EQ(0, value.TopLeft().X);
  EXPECT_EQ(0, value.TopLeft().Y);
  EXPECT_EQ(0, value.TopRight().X);
  EXPECT_EQ(0, value.TopRight().Y);
  EXPECT_EQ(0, value.BottomLeft().X);
  EXPECT_EQ(0, value.BottomLeft().Y);
  EXPECT_EQ(0, value.BottomRight().X);
  EXPECT_EQ(0, value.BottomRight().Y);
}


TEST(TestMathPixel_PxClipRectangle, Construct1)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
  PxClipRectangle value(offsetX, offsetY, width, height);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());

  EXPECT_EQ(offsetX, value.TopLeft().X);
  EXPECT_EQ(offsetY, value.TopLeft().Y);

  EXPECT_EQ(offsetX + width, value.TopRight().X);
  EXPECT_EQ(offsetY, value.TopRight().Y);

  EXPECT_EQ(offsetX, value.BottomLeft().X);
  EXPECT_EQ(offsetY + height, value.BottomLeft().Y);

  EXPECT_EQ(offsetX + width, value.BottomRight().X);
  EXPECT_EQ(offsetY + height, value.BottomRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t right = 10;
  int32_t bottom = 20;
  auto value = PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());

  EXPECT_EQ(left, value.TopLeft().X);
  EXPECT_EQ(top, value.TopLeft().Y);
  EXPECT_EQ(right, value.TopRight().X);
  EXPECT_EQ(top, value.TopRight().Y);
  EXPECT_EQ(left, value.BottomLeft().X);
  EXPECT_EQ(bottom, value.BottomLeft().Y);
  EXPECT_EQ(right, value.BottomRight().X);
  EXPECT_EQ(bottom, value.BottomRight().Y);
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowX)
{
  constexpr int32_t left = std::numeric_limits<int32_t>::min();
  constexpr int32_t top = 2;
  constexpr int32_t right = std::numeric_limits<int32_t>::max();
  constexpr int32_t bottom = 20;
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}

TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowX2)
{
  constexpr int32_t left = -1;
  constexpr int32_t top = 2;
  constexpr int32_t right = std::numeric_limits<int32_t>::max();
  constexpr int32_t bottom = 20;
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowY)
{
  constexpr int32_t left = 1;
  constexpr int32_t top = std::numeric_limits<int32_t>::min();
  constexpr int32_t right = 10;
  constexpr int32_t bottom = std::numeric_limits<int32_t>::max();
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowY2)
{
  constexpr int32_t left = 1;
  constexpr int32_t top = -1;
  constexpr int32_t right = 10;
  constexpr int32_t bottom = std::numeric_limits<int32_t>::max();
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OptimizationCheckFlag)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t right = 10;
  int32_t bottom = 20;
  auto value = PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom, OptimizationCheckFlag::NoCheck);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());

  EXPECT_EQ(left, value.TopLeft().X);
  EXPECT_EQ(top, value.TopLeft().Y);
  EXPECT_EQ(right, value.TopRight().X);
  EXPECT_EQ(top, value.TopRight().Y);
  EXPECT_EQ(left, value.BottomLeft().X);
  EXPECT_EQ(bottom, value.BottomLeft().Y);
  EXPECT_EQ(right, value.BottomRight().X);
  EXPECT_EQ(bottom, value.BottomRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, GetSize)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(width, value.GetSize().X);
  EXPECT_EQ(height, value.GetSize().Y);
}

TEST(TestMathPixel_PxClipRectangle, TopLeft)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.TopLeft().X);
  EXPECT_EQ(offsetY, value.TopLeft().Y);
}

TEST(TestMathPixel_PxClipRectangle, TopRight)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + width, value.TopRight().X);
  EXPECT_EQ(offsetY, value.TopRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, BottomLeft)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.BottomLeft().X);
  EXPECT_EQ(offsetY + height, value.BottomLeft().Y);
}

TEST(TestMathPixel_PxClipRectangle, BottomRight)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + width, value.BottomRight().X);
  EXPECT_EQ(offsetY + height, value.BottomRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, Location)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.Location().X);
  EXPECT_EQ(offsetY, value.Location().Y);
}

TEST(TestMathPixel_PxClipRectangle, Center)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + (width / 2), value.Center().X);
  EXPECT_EQ(offsetY + (height / 2), value.Center().Y);
}

TEST(TestMathPixel_PxClipRectangle, IsEmpty)
{
  EXPECT_TRUE(PxClipRectangle().IsEmpty());
  EXPECT_TRUE(PxClipRectangle(0, 0, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle(1, 0, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle(0, 1, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle(0, 0, 1, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle(0, 0, 0, 1).IsEmpty());
  EXPECT_FALSE(PxClipRectangle(1, 2, 3, 4).IsEmpty());
}

TEST(TestMathPixel_PxClipRectangle, MoveLocation)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  const PxPoint2 newLoc(20, 30);
  value.MoveLocation(newLoc);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(newLoc.X, value.Left());
  EXPECT_EQ(newLoc.Y, value.Top());
  EXPECT_EQ(newLoc.X + width, value.Right());
  EXPECT_EQ(newLoc.Y + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxClipRectangle, MoveLeft)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  const int32_t newX = 20;
  value.MoveLeft(newX);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(newX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(newX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxClipRectangle, MoveTop)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  const int32_t newY = 20;
  value.MoveTop(newY);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(newY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(newY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetWidth)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, 3, height);

  const int32_t newWidth = 10;
  value.SetWidth(newWidth);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetHeight)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  PxClipRectangle value(offsetX, offsetY, width, 4);

  const int32_t newHeight = 10;
  value.SetHeight(newHeight);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetWidth_Invalid)
{
  PxClipRectangle value(0, 0, 10, 10);

  value.SetWidth(-10);
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(0, value.Right());
  EXPECT_EQ(10, value.Bottom());
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(10, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetHeight_Invalid)
{
  PxClipRectangle value(0, 0, 10, 10);

  value.SetHeight(-10);
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(10, value.Right());
  EXPECT_EQ(0, value.Bottom());
  EXPECT_EQ(10, value.Width());
  EXPECT_EQ(0, value.Height());
}

TEST(TestMathPixel_PxClipRectangle, Contains_int32_int32)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  const int32_t height = 4;
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_TRUE(value.Contains(offsetX, offsetY));
  EXPECT_TRUE(value.Contains(offsetX + width - 1, offsetY));
  EXPECT_TRUE(value.Contains(offsetX, offsetY + height - 1));
  EXPECT_TRUE(value.Contains(offsetX + width - 1, offsetY + height - 1));

  EXPECT_TRUE(value.Contains(offsetX + 1, offsetY + 2));

  EXPECT_FALSE(value.Contains(offsetX + width, offsetY));
  EXPECT_FALSE(value.Contains(offsetX, offsetY + height));
  EXPECT_FALSE(value.Contains(offsetX + height, offsetY + height));

  EXPECT_FALSE(value.Contains(std::numeric_limits<int32_t>::min(), offsetY));
  EXPECT_FALSE(value.Contains(std::numeric_limits<int32_t>::max(), offsetY));
  EXPECT_FALSE(value.Contains(offsetX, std::numeric_limits<int32_t>::min()));
  EXPECT_FALSE(value.Contains(offsetY, std::numeric_limits<int32_t>::max()));
}

TEST(TestMathPixel_PxClipRectangle, IntersectsSelf)
{
  {
    constexpr PxClipRectangle rectA(1, 2, 5, 4);
    constexpr PxClipRectangle rectB(7, 1, 7, 6);
    constexpr PxClipRectangle rectC(6, 1, 7, 6);
    constexpr PxClipRectangle rectD(5, 1, 7, 6);
    // constexpr PxClipRectangle rectE(4, 1, 7, 6);

    EXPECT_TRUE(rectA.Intersects(rectA));
    EXPECT_TRUE(rectB.Intersects(rectB));
    EXPECT_TRUE(rectC.Intersects(rectC));
    EXPECT_TRUE(rectD.Intersects(rectD));
  }
}

TEST(TestMathPixel_PxClipRectangle, Intersects_BruteForce)
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
    constexpr PxClipRectangle rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        PxClipRectangle rectA(x, y, 4, 3);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(rectB));
        EXPECT_EQ(result[x + yOffset] != 0u, rectB.Intersects(rectA));
      }
    }
  }
}

TEST(TestMathPixel_PxClipRectangle, Intersect_BruteForce)
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
    constexpr PxClipRectangle rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        PxClipRectangle rectA(x, y, 4, 3);

        auto res1 = PxClipRectangle::Intersect(rectA, rectB);
        auto res2 = PxClipRectangle::Intersect(rectB, rectA);
        EXPECT_EQ(res1, res2);
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

TEST(TestMathPixel_PxClipRectangle, Union_BruteForce)
{
  constexpr PxClipRectangle rectB(4, 3, 6, 5);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      PxClipRectangle rectA(x, y, 4, 3);

      auto res1 = PxClipRectangle::Union(rectA, rectB);
      auto res2 = PxClipRectangle::Union(rectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), rectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), rectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), rectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), rectB.Bottom()), res1.Bottom());
    }
  }
}
