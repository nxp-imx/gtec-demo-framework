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

#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Rectangle = TestFixtureFslBase;
}


TEST(TestMath_Rectangle, Empty)
{
  Rectangle value;

  EXPECT_EQ(Rectangle(), Rectangle::Empty());
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(0, value.Right());
  EXPECT_EQ(0, value.Bottom());
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
  EXPECT_EQ(Point2(), value.TopLeft());
  EXPECT_EQ(Point2(), value.TopRight());
  EXPECT_EQ(Point2(), value.BottomLeft());
  EXPECT_EQ(Point2(), value.BottomRight());
}


TEST(TestMath_Rectangle, Construct1)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
  Rectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(Point2(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(Point2(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(Point2(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(Point2(offsetX + width, offsetY + height), value.BottomRight());
}

TEST(TestMath_Rectangle, FromLeftTopRigtBottom)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t right = 10;
  int32_t bottom = 20;
  auto value = Rectangle::FromLeftTopRigtBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(Point2(left, top), value.TopLeft());
  EXPECT_EQ(Point2(right, top), value.TopRight());
  EXPECT_EQ(Point2(left, bottom), value.BottomLeft());
  EXPECT_EQ(Point2(right, bottom), value.BottomRight());
}


TEST(TestMath_Rectangle, SetWidth)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t height = 4;
  Rectangle value(offsetX, offsetY, 3, height);

  const int32_t newWidth = 10;
  value.SetWidth(newWidth);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMath_Rectangle, SetHeight)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t width = 3;
  Rectangle value(offsetX, offsetY, width, 4);

  const int32_t newHeight = 10;
  value.SetHeight(newHeight);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMath_Rectangle, SetWidth_Invalid)
{
  Rectangle value(0, 0, 10, 10);

  value.SetWidth(-10);
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(0, value.Right());
  EXPECT_EQ(10, value.Bottom());
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(10, value.Height());
}


TEST(TestMath_Rectangle, SetHeight_Invalid)
{
  Rectangle value(0, 0, 10, 10);

  value.SetHeight(-10);
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(10, value.Right());
  EXPECT_EQ(0, value.Bottom());
  EXPECT_EQ(10, value.Width());
  EXPECT_EQ(0, value.Height());
}


TEST(TestMath_Rectangle, Intersects_BruteForce)
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
    Rectangle rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        Rectangle rectA(x, y, 4, 3);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(rectB));
        EXPECT_EQ(result[x + yOffset] != 0u, rectB.Intersects(rectA));
      }
    }
  }
}


TEST(TestMath_Rectangle, Intersect_BruteForce)
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
    Rectangle rectB(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        Rectangle rectA(x, y, 4, 3);

        auto res1 = Rectangle::Intersect(rectA, rectB);
        auto res2 = Rectangle::Intersect(rectB, rectA);
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

TEST(TestMath_Rectangle, Union_BruteForce)
{
  Rectangle rectB(4, 3, 6, 5);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      Rectangle rectA(x, y, 4u, 3u);

      auto res1 = Rectangle::Union(rectA, rectB);
      auto res2 = Rectangle::Union(rectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), rectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), rectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), rectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), rectB.Bottom()), res1.Bottom());
    }
  }
}
