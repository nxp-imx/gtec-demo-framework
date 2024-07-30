/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxRectangle2D.hpp>
#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
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
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxValueU width(10);
  PxValueU height(20);
  PxRectangle2D value(offsetX, offsetY, width, height);

  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.Offset);
  EXPECT_EQ(PxExtent2D(width, height), value.Extent);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(static_cast<int32_t>(offsetX.Value + width.Value), value.RawRight());
  EXPECT_EQ(static_cast<int32_t>(offsetY.Value + height.Value), value.RawBottom());
}

TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom)
{
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);
  auto value = PxRectangle2D::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(PxExtent2D::Create(static_cast<uint32_t>(right.Value - left.Value), static_cast<uint32_t>(bottom.Value - top.Value)), value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, CreateFromLeftTopRightBottom)
{
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);
  auto value = PxRectangle2D::CreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(PxExtent2D::Create(static_cast<uint32_t>(right.Value - left.Value), static_cast<uint32_t>(bottom.Value - top.Value)), value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, UncheckedFromLeftTopRightBottom)
{
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);
  auto value = PxRectangle2D::UncheckedFromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(PxExtent2D::Create(static_cast<uint32_t>(right.Value - left.Value), static_cast<uint32_t>(bottom.Value - top.Value)), value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, UncheckedCreateFromLeftTopRightBottom)
{
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);
  auto value = PxRectangle2D::UncheckedCreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

  EXPECT_EQ(PxPoint2(left, top), value.Offset);
  EXPECT_EQ(PxExtent2D::Create(static_cast<uint32_t>(right.Value - left.Value), static_cast<uint32_t>(bottom.Value - top.Value)), value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_XMinXMax)
{
  constexpr PxValue Left(std::numeric_limits<int32_t>::min());
  constexpr PxValue Top(2);
  constexpr PxValue Right(std::numeric_limits<int32_t>::max());
  constexpr PxValue Bottom(20);
  auto value = PxRectangle2D::FromLeftTopRightBottom(Left, Top, Right, Bottom);

  constexpr auto ExpectedExtent = PxExtent2D::Create(static_cast<uint32_t>(static_cast<uint64_t>(Right.Value) - static_cast<uint64_t>(Left.Value)),
                                                     static_cast<uint32_t>(static_cast<uint64_t>(Bottom.Value) - static_cast<uint64_t>(Top.Value)));

  EXPECT_EQ(PxPoint2(Left, Top), value.Offset);
  EXPECT_EQ(ExpectedExtent, value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, UnchekedFromLeftTopRightBottom_XMinXMax)
{
  constexpr PxValue Left(std::numeric_limits<int32_t>::min());
  constexpr PxValue Top(2);
  constexpr PxValue Right(std::numeric_limits<int32_t>::max());
  constexpr PxValue Bottom(20);
  auto value = PxRectangle2D::UncheckedFromLeftTopRightBottom(Left, Top, Right, Bottom);

  constexpr auto ExpectedExtent = PxExtent2D::Create(static_cast<uint32_t>(static_cast<uint64_t>(Right.Value) - static_cast<uint64_t>(Left.Value)),
                                                     static_cast<uint32_t>(static_cast<uint64_t>(Bottom.Value) - static_cast<uint64_t>(Top.Value)));

  EXPECT_EQ(PxPoint2(Left, Top), value.Offset);
  EXPECT_EQ(ExpectedExtent, value.Extent);
}


TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_YMinYMax)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(std::numeric_limits<int32_t>::min());
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle2D::FromLeftTopRightBottom(Left, Top, Right, Bottom);

  constexpr auto ExpectedExtent = PxExtent2D::Create(static_cast<uint32_t>(static_cast<uint64_t>(Right.Value) - static_cast<uint64_t>(Left.Value)),
                                                     static_cast<uint32_t>(static_cast<uint64_t>(Bottom.Value) - static_cast<uint64_t>(Top.Value)));

  EXPECT_EQ(PxPoint2(Left, Top), value.Offset);
  EXPECT_EQ(ExpectedExtent, value.Extent);
}

TEST(TestMathPixel_PxRectangle2D, UncheckedFromLeftTopRightBottom_YMinYMax)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(std::numeric_limits<int32_t>::min());
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle2D::UncheckedFromLeftTopRightBottom(Left, Top, Right, Bottom);

  constexpr auto ExpectedExtent = PxExtent2D::Create(static_cast<uint32_t>(static_cast<uint64_t>(Right.Value) - static_cast<uint64_t>(Left.Value)),
                                                     static_cast<uint32_t>(static_cast<uint64_t>(Bottom.Value) - static_cast<uint64_t>(Top.Value)));

  EXPECT_EQ(PxPoint2(Left, Top), value.Offset);
  EXPECT_EQ(ExpectedExtent, value.Extent);
}


TEST(TestMathPixel_PxRectangle2D, FromLeftTopRightBottom_Invalid)
{
  const auto left = PxValue::Create(1);
  const auto top = PxValue::Create(2);

  const auto right0 = PxValue::Create(-10);
  const auto right1 = PxValue::Create(10);

  const auto bottom0 = PxValue::Create(20);
  const auto bottom1 = PxValue::Create(-20);

  auto rectangle0 = PxRectangle2D::FromLeftTopRightBottom(left, top, right0, bottom0);
  auto rectangle1 = PxRectangle2D::FromLeftTopRightBottom(left, top, right1, bottom1);

  EXPECT_EQ(PxPoint2(left, top), rectangle0.Offset);
  EXPECT_EQ(PxPoint2(left, top), rectangle1.Offset);
  EXPECT_EQ(PxExtent2D::Create(0, 18), rectangle0.Extent);
  EXPECT_EQ(PxExtent2D::Create(9, 0), rectangle1.Extent);
}

TEST(TestMathPixel_PxRectangle2D, Center)
{
  PxValue x(10);
  PxValue y(12);
  PxValueU width(10);
  PxValueU height(20);
  PxRectangle2D value(x, y, width, height);

  EXPECT_EQ(PxPoint2(x, y), value.Offset);
  EXPECT_EQ(PxExtent2D(width, height), value.Extent);

  EXPECT_EQ(PxPoint2::Create(x.Value + (width.Value / 2), y.Value + (height.Value / 2)), value.Center());
}

TEST(TestMathPixel_PxRectangle2D, Contains_int32_int32)
{
  const int32_t x(10);
  const int32_t y(12);
  const uint16_t width = 10;
  const uint16_t height = 10;
  const auto value = PxRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(x, y));
  EXPECT_TRUE(value.Contains(x + width - 1, y));
  EXPECT_TRUE(value.Contains(x, y + height - 1));
  EXPECT_TRUE(value.Contains(x + width - 1, y + height - 1));

  EXPECT_FALSE(value.Contains(x - 1, y));
  EXPECT_FALSE(value.Contains(x, y - 1));
  EXPECT_FALSE(value.Contains(x, y + height));
  EXPECT_FALSE(value.Contains(x + width, y));
}

TEST(TestMathPixel_PxRectangle2D, Contains_PxValue_PxValue)
{
  const int32_t x(10);
  const int32_t y(12);
  const uint16_t width = 10;
  const uint16_t height = 10;
  const auto value = PxRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(PxValue(x), PxValue(y)));
  EXPECT_TRUE(value.Contains(PxValue(x + width - 1), PxValue(y)));
  EXPECT_TRUE(value.Contains(PxValue(x), PxValue(y + height - 1)));
  EXPECT_TRUE(value.Contains(PxValue(x + width - 1), PxValue(y + height - 1)));

  EXPECT_FALSE(value.Contains(PxValue(x - 1), PxValue(y)));
  EXPECT_FALSE(value.Contains(PxValue(x), PxValue(y - 1)));
  EXPECT_FALSE(value.Contains(PxValue(x), PxValue(y + height)));
  EXPECT_FALSE(value.Contains(PxValue(x + width), PxValue(y)));
}

TEST(TestMathPixel_PxRectangle2D, Contains_PxPoint2)
{
  const int32_t x(10);
  const int32_t y(12);
  const uint16_t width = 10;
  const uint16_t height = 10;
  const auto value = PxRectangle2D::Create(x, y, width, height);

  EXPECT_TRUE(value.Contains(PxPoint2::Create(x, y)));
  EXPECT_TRUE(value.Contains(PxPoint2::Create(x + width - 1, y)));
  EXPECT_TRUE(value.Contains(PxPoint2::Create(x, y + height - 1)));
  EXPECT_TRUE(value.Contains(PxPoint2::Create(x + width - 1, y + height - 1)));

  EXPECT_FALSE(value.Contains(PxPoint2::Create(x - 1, y)));
  EXPECT_FALSE(value.Contains(PxPoint2::Create(x, y - 1)));
  EXPECT_FALSE(value.Contains(PxPoint2::Create(x, y + height)));
  EXPECT_FALSE(value.Contains(PxPoint2::Create(x + width, y)));
}

TEST(TestMathPixel_PxRectangle2D, Intersects)
{
  PxRectangle2D rect1 = PxRectangle2D::Create(0, 0, 10, 10);
  PxRectangle2D rect2 = PxRectangle2D::Create(1, 1, 9, 9);
  PxRectangle2D rect3 = PxRectangle2D::Create(11, 0, 10, 10);

  auto res1 = PxRectangle2D::Intersect(rect1, rect2);
  auto res2 = PxRectangle2D::Intersect(rect2, rect1);
  auto resNoIntersection = PxRectangle2D::Intersect(rect1, rect3);

  EXPECT_EQ(rect2, res1);
  EXPECT_EQ(rect2, res2);
  EXPECT_TRUE(resNoIntersection.IsEmpty());
}


TEST(TestMathPixel_PxRectangle2D, Union)
{
  const auto rect1 = PxRectangle2D::Create(0, 0, 5, 5);
  const auto rect2 = PxRectangle2D::Create(0, -1, 4, 5);

  auto res = PxRectangle2D::Union(rect1, rect2);

  // 0, 5 |  0, 4
  // 0, 5 | -1, 4
  EXPECT_EQ(PxRectangle2D::CreateFromLeftTopRightBottom(0, -1, 5, 5), res);
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
    constexpr auto RectB = PxRectangle2D::Create(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        const auto rectA = PxRectangle2D::Create(x, y, 4u, 3u);
        EXPECT_EQ(result[x + yOffset] != 0u, rectA.Intersects(RectB));
        EXPECT_EQ(result[x + yOffset] != 0u, RectB.Intersects(rectA));
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
    constexpr auto RectB = PxRectangle2D::Create(4, 3, 6u, 5u);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        auto rectA = PxRectangle2D::Create(x, y, 4u, 3u);

        auto res1 = PxRectangle2D::Intersect(rectA, RectB);
        auto res2 = PxRectangle2D::Intersect(RectB, rectA);
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

TEST(TestMathPixel_PxRectangle2D, Union_BruteForce)
{
  constexpr auto RectB = PxRectangle2D::Create(4, 3, 6u, 5u);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      auto rectA = PxRectangle2D::Create(x, y, 4u, 3u);

      auto res1 = PxRectangle2D::Union(rectA, RectB);
      auto res2 = PxRectangle2D::Union(RectB, rectA);
      EXPECT_EQ(res1, res2);

      EXPECT_EQ(std::min(rectA.Left(), RectB.Left()), res1.Left());
      EXPECT_EQ(std::min(rectA.Top(), RectB.Top()), res1.Top());
      EXPECT_EQ(std::max(rectA.Right(), RectB.Right()), res1.Right());
      EXPECT_EQ(std::max(rectA.Bottom(), RectB.Bottom()), res1.Bottom());
    }
  }
}
