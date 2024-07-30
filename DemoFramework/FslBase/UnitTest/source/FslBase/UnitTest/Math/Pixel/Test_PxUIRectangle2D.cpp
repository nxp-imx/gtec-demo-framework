/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxUIRectangle2D.hpp>
#include <FslBase/Math/Pixel/PxUIRectangle2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxUIRectangle2D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxUIRectangle2D, Empty)
{
  PxUIRectangle2D value;

  EXPECT_EQ(PxVector2(), value.Offset);
  EXPECT_EQ(PxSize2D(), value.Size);
  EXPECT_EQ(PxUIRectangle2D(), PxUIRectangle2D::Empty());
}


TEST(TestMathPixel_PxUIRectangle2D, Construct)
{
  PxValueF offsetX(1.0f);
  PxValueF offsetY(2.0f);
  PxSize1D width(PxSize1D::Create(10));
  PxSize1D height(PxSize1D::Create(20));
  PxUIRectangle2D value(offsetX, offsetY, width, height);

  EXPECT_EQ(PxVector2(offsetX, offsetY), value.Offset);
  EXPECT_EQ(PxSize2D(width, height), value.Size);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX.Value + static_cast<float>(width.RawValue()), value.RawRight());
  EXPECT_EQ(offsetY.Value + static_cast<float>(height.RawValue()), value.RawBottom());
}


TEST(TestMathPixel_PxUIRectangle2D, Center)
{
  PxValueF x(10);
  PxValueF y(12);
  PxSize1D width(PxSize1D::Create(10));
  PxSize1D height(PxSize1D::Create(20));
  PxUIRectangle2D value(x, y, width, height);

  EXPECT_EQ(PxVector2(x, y), value.Offset);
  EXPECT_EQ(PxSize2D(width, height), value.Size);

  const int32_t halfWidth = width.RawValue() / 2;
  const int32_t halfHeight = height.RawValue() / 2;

  EXPECT_EQ(PxVector2::Create(x.Value + halfWidth, y.Value + halfHeight), value.Center());
}

TEST(TestMathPixel_PxUIRectangle2D, Contains_int32_int32)
{
  const float x(10);
  const float y(12);
  const int32_t width = 10;
  const int32_t height = 10;
  const auto value = PxUIRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(x, y));
  EXPECT_TRUE(value.Contains(x + width - 1, y));
  EXPECT_TRUE(value.Contains(x, y + height - 1));
  EXPECT_TRUE(value.Contains(x + width - 1, y + height - 1));

  EXPECT_FALSE(value.Contains(x - 1, y));
  EXPECT_FALSE(value.Contains(x, y - 1));
  EXPECT_FALSE(value.Contains(x, y + height));
  EXPECT_FALSE(value.Contains(x + width, y));
}

TEST(TestMathPixel_PxUIRectangle2D, Contains_PxValue_PxValue)
{
  const float x(10);
  const float y(12);
  const int32_t width = 10;
  const int32_t height = 10;
  const auto value = PxUIRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(PxValue(static_cast<int32_t>(x)), PxValue(static_cast<int32_t>(y))));
  EXPECT_TRUE(value.Contains(PxValue(static_cast<int32_t>(x) + width - 1), PxValue(static_cast<int32_t>(y))));
  EXPECT_TRUE(value.Contains(PxValue(static_cast<int32_t>(x)), PxValue(static_cast<int32_t>(y) + height - 1)));
  EXPECT_TRUE(value.Contains(PxValue(static_cast<int32_t>(x) + width - 1), PxValue(static_cast<int32_t>(y) + height - 1)));

  EXPECT_FALSE(value.Contains(PxValue(static_cast<int32_t>(x) - 1), PxValue(static_cast<int32_t>(y))));
  EXPECT_FALSE(value.Contains(PxValue(static_cast<int32_t>(x)), PxValue(static_cast<int32_t>(y) - 1)));
  EXPECT_FALSE(value.Contains(PxValue(static_cast<int32_t>(x)), PxValue(static_cast<int32_t>(y) + height)));
  EXPECT_FALSE(value.Contains(PxValue(static_cast<int32_t>(x) + width), PxValue(static_cast<int32_t>(y))));
}

TEST(TestMathPixel_PxUIRectangle2D, Contains_PxVector2)
{
  const float x(10);
  const float y(12);
  const int32_t width = 10;
  const int32_t height = 10;
  const auto value = PxUIRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(PxVector2::Create(x, y)));
  EXPECT_TRUE(value.Contains(PxVector2::Create(x + width - 1, y)));
  EXPECT_TRUE(value.Contains(PxVector2::Create(x, y + height - 1)));
  EXPECT_TRUE(value.Contains(PxVector2::Create(x + width - 1, y + height - 1)));

  EXPECT_FALSE(value.Contains(PxVector2::Create(x - 1, y)));
  EXPECT_FALSE(value.Contains(PxVector2::Create(x, y - 1)));
  EXPECT_FALSE(value.Contains(PxVector2::Create(x, y + height)));
  EXPECT_FALSE(value.Contains(PxVector2::Create(x + width, y)));
}

TEST(TestMathPixel_PxUIRectangle2D, Intersects)
{
  PxUIRectangle2D rect1 = PxUIRectangle2D::Create(0, 0, 10, 10);
  PxUIRectangle2D rect2 = PxUIRectangle2D::Create(1, 1, 9, 9);
  PxUIRectangle2D rect3 = PxUIRectangle2D::Create(11, 0, 10, 10);

  auto res1 = PxUIRectangle2D::Intersect(rect1, rect2);
  auto res2 = PxUIRectangle2D::Intersect(rect2, rect1);
  auto resNoIntersection = PxUIRectangle2D::Intersect(rect1, rect3);

  EXPECT_EQ(rect2, res1);
  EXPECT_EQ(rect2, res2);
  EXPECT_TRUE(resNoIntersection.IsEmpty());
}


TEST(TestMathPixel_PxUIRectangle2D, Union)
{
  const auto rect1 = PxUIRectangle2D::Create(0.0f, 0.0f, 5, 5);
  const auto rect2 = PxUIRectangle2D::Create(0.0f, -1.0f, 4, 5);

  auto res = PxUIRectangle2D::Union(rect1, rect2);

  // 0, 5 |  0, 4
  // 0, 5 | -1, 4
  EXPECT_EQ(PxUIRectangle2D::Create(0.0f, -1.0f, 5, 6), res);
}


TEST(TestMathPixel_PxUIRectangle2D, Intersects_BruteForce)
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
    constexpr auto RectB = PxUIRectangle2D::Create(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        const auto rectA = PxUIRectangle2D::Create(static_cast<float>(x), static_cast<float>(y), 4u, 3u);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(RectB));
        EXPECT_EQ(result[x + yOffset] != 0u, RectB.Intersects(rectA));
      }
    }
  }
}


TEST(TestMathPixel_PxUIRectangle2D, Intersect_BruteForce)
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
    constexpr auto RectB = PxUIRectangle2D::Create(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        auto rectA = PxUIRectangle2D::Create(static_cast<float>(x), static_cast<float>(y), 4u, 3u);

        auto res1 = PxUIRectangle2D::Intersect(rectA, RectB);
        auto res2 = PxUIRectangle2D::Intersect(RectB, rectA);
        EXPECT_EQ(res1, res2);
        // check for intersection
        EXPECT_EQ(result[x + yOffset] != 0u, !res1.IsEmpty());
        // check result
        if (result[x + yOffset] != 0u)
        {
          EXPECT_EQ(std::max(rectA.Left(), RectB.Left()), res1.Left());
          EXPECT_EQ(std::max(rectA.Top(), RectB.Top()), res1.Top());
          EXPECT_EQ(std::min(rectA.Right(), RectB.Right()), res1.Right());
          EXPECT_EQ(std::min(rectA.Bottom(), RectB.Bottom()), res1.Bottom());
        }
      }
    }
  }
}

TEST(TestMathPixel_PxUIRectangle2D, Union_BruteForce)
{
  constexpr auto RectB = PxUIRectangle2D::Create(4, 3, 6u, 5u);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      auto rectA = PxUIRectangle2D::Create(static_cast<float>(x), static_cast<float>(y), 4u, 3u);

      auto res1 = PxUIRectangle2D::Union(rectA, RectB);
      auto res2 = PxUIRectangle2D::Union(RectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), RectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), RectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), RectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), RectB.Bottom()), res1.Bottom());
    }
  }
}
