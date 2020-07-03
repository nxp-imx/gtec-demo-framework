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

#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangle2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxRectangle2D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxRectangle2D, Empty)
{
  PxRectangle2D value;

  EXPECT_EQ(PxPoint2(), value.Offset);
  EXPECT_EQ(PxExtent2D(), value.Extent);
  EXPECT_EQ(PxRectangle2D(), PxRectangle2D::Empty());
}


TEST(TestMathPixel_PxRectangle2D, Construct)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  uint32_t width = 10;
  uint32_t height = 20;
  PxRectangle2D value(offsetX, offsetY, width, height);

  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.Offset);
  EXPECT_EQ(PxExtent2D(width, height), value.Extent);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(static_cast<int32_t>(offsetX + width), value.Right());
  EXPECT_EQ(static_cast<int32_t>(offsetY + height), value.Bottom());
}

TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t right = 10;
  int32_t bottom = 20;
  auto value = PxRectangle2D::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(PxExtent2D(static_cast<uint32_t>(right - left), static_cast<uint32_t>(bottom - top)), value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_XMinXMax)
{
  constexpr int32_t left = std::numeric_limits<int32_t>::min();
  constexpr int32_t top = 2;
  constexpr int32_t right = std::numeric_limits<int32_t>::max();
  constexpr int32_t bottom = 20;
  auto value = PxRectangle2D::FromLeftTopRightBottom(left, top, right, bottom);

  constexpr PxExtent2D expectedExtent(uint32_t(uint64_t(right) - uint64_t(left)), uint32_t(uint64_t(bottom) - uint64_t(top)));

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(expectedExtent, value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_YMinYMax)
{
  constexpr int32_t left = 1;
  constexpr int32_t top = std::numeric_limits<int32_t>::min();
  constexpr int32_t right = 10;
  constexpr int32_t bottom = std::numeric_limits<int32_t>::max();
  auto value = PxRectangle2D::FromLeftTopRightBottom(left, top, right, bottom);

  constexpr PxExtent2D expectedExtent(uint32_t(uint64_t(right) - uint64_t(left)), uint32_t(uint64_t(bottom) - uint64_t(top)));

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(expectedExtent, value.Extent);
}


TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_Invalid)
{
  auto rectangle0 = PxRectangle2D::FromLeftTopRightBottom(1, 2, -10, 20);
  auto rectangle1 = PxRectangle2D::FromLeftTopRightBottom(1, 2, 10, -20);

  EXPECT_EQ(PxPoint2(1, 2), rectangle0.Offset);
  EXPECT_EQ(PxPoint2(1, 2), rectangle1.Offset);
  EXPECT_EQ(PxExtent2D(0, 18), rectangle0.Extent);
  EXPECT_EQ(PxExtent2D(9, 0), rectangle1.Extent);
}

TEST(TestMathPixel_PxRectangle2D, Center)
{
  uint32_t x = 10;
  uint32_t y = 12;
  uint32_t width = 10;
  uint32_t height = 20;
  PxRectangle2D value(x, y, width, height);

  EXPECT_EQ(PxPoint2(x, y), value.Offset);
  EXPECT_EQ(PxExtent2D(width, height), value.Extent);

  EXPECT_EQ(PxPoint2(x + (width / 2), y + (height / 2)), value.Center());
}

TEST(TestMathPixel_PxRectangle2D, Contains_int32_int32)
{
  uint32_t x = 10;
  uint32_t y = 12;
  uint32_t width = 10;
  uint32_t height = 20;
  PxRectangle2D value(x, y, width, height);

  EXPECT_TRUE(value.Contains(x, y));
  EXPECT_TRUE(value.Contains(x + width - 1, y));
  EXPECT_TRUE(value.Contains(x, y + height - 1));
  EXPECT_TRUE(value.Contains(x + width - 1, y + height - 1));

  EXPECT_FALSE(value.Contains(x - 1, y));
  EXPECT_FALSE(value.Contains(x, y - 1));
  EXPECT_FALSE(value.Contains(x, y + height));
  EXPECT_FALSE(value.Contains(x + width, y));
}


TEST(TestMathPixel_PxRectangle2D, Contains_PxPoint2)
{
  uint32_t x = 10;
  uint32_t y = 12;
  uint32_t width = 10;
  uint32_t height = 20;
  PxRectangle2D value(x, y, width, height);

  EXPECT_TRUE(value.Contains(PxPoint2(x, y)));
  EXPECT_TRUE(value.Contains(PxPoint2(x + width - 1, y)));
  EXPECT_TRUE(value.Contains(PxPoint2(x, y + height - 1)));
  EXPECT_TRUE(value.Contains(PxPoint2(x + width - 1, y + height - 1)));

  EXPECT_FALSE(value.Contains(PxPoint2(x - 1, y)));
  EXPECT_FALSE(value.Contains(PxPoint2(x, y - 1)));
  EXPECT_FALSE(value.Contains(PxPoint2(x, y + height)));
  EXPECT_FALSE(value.Contains(PxPoint2(x + width, y)));
}

TEST(TestMathPixel_PxRectangle2D, Intersects)
{
  PxRectangle2D rect1(0, 0, 10, 10);
  PxRectangle2D rect2(1, 1, 9, 9);
  PxRectangle2D rect3(11, 0, 10, 10);

  auto res1 = PxRectangle2D::Intersect(rect1, rect2);
  auto res2 = PxRectangle2D::Intersect(rect2, rect1);
  auto resNoIntersection = PxRectangle2D::Intersect(rect1, rect3);

  EXPECT_EQ(rect2, res1);
  EXPECT_EQ(rect2, res2);
  EXPECT_TRUE(resNoIntersection.IsEmpty());
}


TEST(TestMathPixel_PxRectangle2D, Union)
{
  const PxRectangle2D rect1(0, 0, 5, 5);
  const PxRectangle2D rect2(0, -1, 4, 5);

  auto res = PxRectangle2D::Union(rect1, rect2);

  // 0, 5 |  0, 4
  // 0, 5 | -1, 4
  EXPECT_EQ(PxRectangle2D::FromLeftTopRightBottom(0, -1, 5, 5), res);
}


TEST(TestMathPixel_PxRectangle2D, Intersects_BruteForce)
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
    constexpr PxRectangle2D rectB(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        PxRectangle2D rectA(x, y, 4u, 3u);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(rectB));
        EXPECT_EQ(result[x + yOffset] != 0u, rectB.Intersects(rectA));
      }
    }
  }
}


TEST(TestMathPixel_PxRectangle2D, Intersect_BruteForce)
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
    constexpr PxRectangle2D rectB(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        PxRectangle2D rectA(x, y, 4u, 3u);

        auto res1 = PxRectangle2D::Intersect(rectA, rectB);
        auto res2 = PxRectangle2D::Intersect(rectB, rectA);
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

TEST(TestMathPixel_PxRectangle2D, Union_BruteForce)
{
  constexpr PxRectangle2D rectB(4, 3, 6u, 5u);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      PxRectangle2D rectA(x, y, 4u, 3u);

      auto res1 = PxRectangle2D::Union(rectA, rectB);
      auto res2 = PxRectangle2D::Union(rectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), rectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), rectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), rectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), rectB.Bottom()), res1.Bottom());
    }
  }
}
