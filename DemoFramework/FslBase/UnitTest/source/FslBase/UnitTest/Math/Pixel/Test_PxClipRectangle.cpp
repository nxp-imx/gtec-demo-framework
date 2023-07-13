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

#include <FslBase/Log/Math/Pixel/LogPxClipRectangle.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
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
  EXPECT_EQ(0, value.RawLeft());
  EXPECT_EQ(0, value.RawTop());
  EXPECT_EQ(0, value.RawRight());
  EXPECT_EQ(0, value.RawBottom());
  EXPECT_EQ(0, value.Width().Value);
  EXPECT_EQ(0, value.Height().Value);
  EXPECT_EQ(0, value.TopLeft().X.Value);
  EXPECT_EQ(0, value.TopLeft().Y.Value);
  EXPECT_EQ(0, value.TopRight().X.Value);
  EXPECT_EQ(0, value.TopRight().Y.Value);
  EXPECT_EQ(0, value.BottomLeft().X.Value);
  EXPECT_EQ(0, value.BottomLeft().Y.Value);
  EXPECT_EQ(0, value.BottomRight().X.Value);
  EXPECT_EQ(0, value.BottomRight().Y.Value);
}


TEST(TestMathPixel_PxClipRectangle, Construct1)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
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

TEST(TestMathPixel_PxClipRectangle, Create)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  PxClipRectangle value = PxClipRectangle::Create(offsetX.Value, offsetY.Value, width.RawValue(), height.RawValue());

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
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);
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

TEST(TestMathPixel_PxClipRectangle, CreateFromLeftTopRightBottom)
{
  PxValue left(1);
  PxValue top(2);
  PxValue right(10);
  PxValue bottom(20);

  auto value = PxClipRectangle::CreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

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
  constexpr auto left = PxValue::Create(std::numeric_limits<int32_t>::min());
  constexpr auto top = PxValue::Create(2);
  constexpr auto right = PxValue::Create(std::numeric_limits<int32_t>::max());
  constexpr auto bottom = PxValue::Create(20);
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}

TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowX2)
{
  constexpr auto left = PxValue::Create(-1);
  constexpr auto top = PxValue::Create(2);
  constexpr auto right = PxValue::Create(std::numeric_limits<int32_t>::max());
  constexpr auto bottom = PxValue::Create(20);
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowY)
{
  constexpr auto left = PxValue::Create(1);
  constexpr auto top = PxValue::Create(std::numeric_limits<int32_t>::min());
  constexpr auto right = PxValue::Create(10);
  constexpr auto bottom = PxValue::Create(std::numeric_limits<int32_t>::max());
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, FromLeftTopRightBottom_OverflowY2)
{
  constexpr auto left = PxValue::Create(1);
  constexpr auto top = PxValue::Create(-1);
  constexpr auto right = PxValue::Create(10);
  constexpr auto bottom = PxValue::Create(std::numeric_limits<int32_t>::max());
  EXPECT_DEBUG_DEATH(PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom), "");
}

TEST(TestMathPixel_PxClipRectangle, CreateFromLeftTopRightBottom_OverflowX)
{
  constexpr int32_t left = std::numeric_limits<int32_t>::min();
  constexpr int32_t top = 2;
  constexpr int32_t right = std::numeric_limits<int32_t>::max();
  constexpr int32_t bottom = 20;
  EXPECT_DEBUG_DEATH(PxClipRectangle::CreateFromLeftTopRightBottom(left, top, right, bottom), "");
}

TEST(TestMathPixel_PxClipRectangle, CreateFromLeftTopRightBottom_OverflowX2)
{
  constexpr int32_t left = -1;
  constexpr int32_t top = 2;
  constexpr int32_t right = std::numeric_limits<int32_t>::max();
  constexpr int32_t bottom = 20;
  EXPECT_DEBUG_DEATH(PxClipRectangle::CreateFromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, CreateFromLeftTopRightBottom_OverflowY)
{
  constexpr int32_t left = 1;
  constexpr int32_t top = std::numeric_limits<int32_t>::min();
  constexpr int32_t right = 10;
  constexpr int32_t bottom = std::numeric_limits<int32_t>::max();
  EXPECT_DEBUG_DEATH(PxClipRectangle::CreateFromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, CreateFromLeftTopRightBottom_OverflowY2)
{
  constexpr int32_t left = 1;
  constexpr int32_t top = -1;
  constexpr int32_t right = 10;
  constexpr int32_t bottom = std::numeric_limits<int32_t>::max();
  EXPECT_DEBUG_DEATH(PxClipRectangle::CreateFromLeftTopRightBottom(left, top, right, bottom), "");
}


TEST(TestMathPixel_PxClipRectangle, UncheckedFromLeftTopRightBottom)
{
  const auto left = PxValue::Create(1);
  const auto top = PxValue::Create(2);
  const auto right = PxValue::Create(10);
  const auto bottom = PxValue::Create(20);
  auto value = PxClipRectangle::UncheckedFromLeftTopRightBottom(left, top, right, bottom);

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

TEST(TestMathPixel_PxClipRectangle, UncheckedCreateFromLeftTopRightBottom)
{
  const auto left = PxValue::Create(1);
  const auto top = PxValue::Create(2);
  const auto right = PxValue::Create(10);
  const auto bottom = PxValue::Create(20);
  auto value = PxClipRectangle::UncheckedCreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

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
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(width, value.GetSize().X);
  EXPECT_EQ(height, value.GetSize().Y);
}

TEST(TestMathPixel_PxClipRectangle, TopLeft)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.TopLeft().X);
  EXPECT_EQ(offsetY, value.TopLeft().Y);
}

TEST(TestMathPixel_PxClipRectangle, TopRight)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + width, value.TopRight().X);
  EXPECT_EQ(offsetY, value.TopRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, BottomLeft)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.BottomLeft().X);
  EXPECT_EQ(offsetY + height, value.BottomLeft().Y);
}

TEST(TestMathPixel_PxClipRectangle, BottomRight)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX + width, value.BottomRight().X);
  EXPECT_EQ(offsetY + height, value.BottomRight().Y);
}

TEST(TestMathPixel_PxClipRectangle, Location)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.Location().X);
  EXPECT_EQ(offsetY, value.Location().Y);
}

TEST(TestMathPixel_PxClipRectangle, Center)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX.Value + (width.RawValue() / 2), value.Center().X.Value);
  EXPECT_EQ(offsetY.Value + (height.RawValue() / 2), value.Center().Y.Value);
}

TEST(TestMathPixel_PxClipRectangle, IsEmpty)
{
  EXPECT_TRUE(PxClipRectangle().IsEmpty());
  EXPECT_TRUE(PxClipRectangle::Create(0, 0, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle::Create(1, 0, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle::Create(0, 1, 0, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle::Create(0, 0, 1, 0).IsEmpty());
  EXPECT_FALSE(PxClipRectangle::Create(0, 0, 0, 1).IsEmpty());
  EXPECT_FALSE(PxClipRectangle::Create(1, 2, 3, 4).IsEmpty());
}

TEST(TestMathPixel_PxClipRectangle, MoveLocation)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  const auto newLoc = PxPoint2::Create(20, 30);
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
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  const PxValue newX(20);
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
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  const PxValue newY(20);
  value.MoveTop(newY);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(newY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(newY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetWidth_PxSize1D)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, PxSize1D::Create(3), height);

  const auto newWidth = PxSize1D::Create(10);
  value.SetWidth(newWidth);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetWidth_PxValue)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, PxSize1D::Create(3), height);

  const auto newWidth = PxValue::Create(10);
  value.SetWidth(newWidth);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetHeight_PxSize1D)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  PxClipRectangle value(offsetX, offsetY, width, PxSize1D::Create(4));

  const auto newHeight = PxSize1D::Create(10);
  value.SetHeight(newHeight);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetHeight_PxValue)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  PxClipRectangle value(offsetX, offsetY, width, PxSize1D::Create(4));

  const auto newHeight = PxValue::Create(10);
  value.SetHeight(newHeight);

  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMathPixel_PxClipRectangle, SetWidth_PxValue_Invalid)
{
  PxClipRectangle value = PxClipRectangle::Create(0, 0, 10, 10);

  value.SetWidth(PxValue(-10));
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0, value.RawLeft());
  EXPECT_EQ(0, value.RawTop());
  EXPECT_EQ(0, value.RawRight());
  EXPECT_EQ(10, value.RawBottom());
  EXPECT_EQ(0, value.Width().Value);
  EXPECT_EQ(10, value.Height().Value);
}


TEST(TestMathPixel_PxClipRectangle, SetHeight_Invalid)
{
  PxClipRectangle value = PxClipRectangle::Create(0, 0, 10, 10);

  value.SetHeight(PxValue(-10));
  // The rect stores left, top, right, bottom so they ought to be exact values
  EXPECT_EQ(0, value.RawLeft());
  EXPECT_EQ(0, value.RawTop());
  EXPECT_EQ(10, value.RawRight());
  EXPECT_EQ(0, value.RawBottom());
  EXPECT_EQ(10, value.Width().Value);
  EXPECT_EQ(0, value.Height().Value);
}

TEST(TestMathPixel_PxClipRectangle, Contains_PxValue_PxValue)
{
  const PxValue offsetX(1);
  const PxValue offsetY(2);
  const PxSize1D width = PxSize1D::Create(3);
  const PxSize1D height = PxSize1D::Create(4);
  PxClipRectangle value(offsetX, offsetY, width, height);

  EXPECT_TRUE(value.Contains(offsetX, offsetY));
  EXPECT_TRUE(value.Contains(offsetX + width - PxValue(1), offsetY));
  EXPECT_TRUE(value.Contains(offsetX, offsetY + height - PxValue(1)));
  EXPECT_TRUE(value.Contains(offsetX + width - PxValue(1), offsetY + height - PxValue(1)));

  EXPECT_TRUE(value.Contains(offsetX + PxValue(1), offsetY + PxValue(2)));

  EXPECT_FALSE(value.Contains(offsetX + width, offsetY));
  EXPECT_FALSE(value.Contains(offsetX, offsetY + height));
  EXPECT_FALSE(value.Contains(offsetX + height, offsetY + height));

  EXPECT_FALSE(value.Contains(PxValue(std::numeric_limits<int32_t>::min()), offsetY));
  EXPECT_FALSE(value.Contains(PxValue(std::numeric_limits<int32_t>::max()), offsetY));
  EXPECT_FALSE(value.Contains(offsetX, PxValue(std::numeric_limits<int32_t>::min())));
  EXPECT_FALSE(value.Contains(offsetY, PxValue(std::numeric_limits<int32_t>::max())));
}

TEST(TestMathPixel_PxClipRectangle, IntersectsSelf)
{
  {
    constexpr auto rectA = PxClipRectangle::Create(1, 2, 5, 4);
    constexpr auto rectB = PxClipRectangle::Create(7, 1, 7, 6);
    constexpr auto rectC = PxClipRectangle::Create(6, 1, 7, 6);
    constexpr auto rectD = PxClipRectangle::Create(5, 1, 7, 6);
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
    constexpr auto rectB = PxClipRectangle::Create(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        auto rectA = PxClipRectangle::Create(x, y, 4, 3);
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
    constexpr auto rectB = PxClipRectangle::Create(4, 3, 6, 5);

    for (int32_t y = 0; y < 10; ++y)
    {
      const int32_t yOffset = y * 11;
      for (int32_t x = 0; x < 11; ++x)
      {
        auto rectA = PxClipRectangle::Create(x, y, 4, 3);

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
  constexpr auto rectB = PxClipRectangle::Create(4, 3, 6, 5);

  for (int32_t y = 0; y < 10; ++y)
  {
    for (int32_t x = 0; x < 11; ++x)
    {
      auto rectA = PxClipRectangle::Create(x, y, 4, 3);

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
