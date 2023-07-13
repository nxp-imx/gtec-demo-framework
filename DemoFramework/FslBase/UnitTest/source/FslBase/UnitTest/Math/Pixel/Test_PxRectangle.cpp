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

#include <FslBase/Log/Math/Pixel/LogPxRectangle.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxRectangle = TestFixtureFslBase;
}


TEST(TestMathPixel_PxRectangle, Empty)
{
  PxRectangle value;

  EXPECT_EQ(PxRectangle(), PxRectangle::Empty());
  EXPECT_EQ(0, value.RawLeft());
  EXPECT_EQ(0, value.RawTop());
  EXPECT_EQ(0, value.RawRight());
  EXPECT_EQ(0, value.RawBottom());
  EXPECT_EQ(0, value.RawWidth());
  EXPECT_EQ(0, value.RawHeight());
  EXPECT_EQ(PxPoint2(), value.TopLeft());
  EXPECT_EQ(PxPoint2(), value.TopRight());
  EXPECT_EQ(PxPoint2(), value.BottomLeft());
  EXPECT_EQ(PxPoint2(), value.BottomRight());
  EXPECT_EQ(PxSize2D(), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, Construct1)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  const PxRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(PxPoint2(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY + height), value.BottomRight());
  EXPECT_EQ(PxSize2D(width, height), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, Create)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  const PxRectangle value = PxRectangle::Create(offsetX.Value, offsetY.Value, width.RawValue(), height.RawValue());

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(PxPoint2(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY + height), value.BottomRight());
  EXPECT_EQ(PxSize2D(width, height), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, Construct_PxPoint_PxPoint)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxValue width(10);
  PxValue height(20);
  PxRectangle value(PxPoint2(offsetX, offsetY), PxPoint2(width, height));

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(PxPoint2(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY + height), value.BottomRight());
  EXPECT_EQ(PxSize2D(width, height), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, Construct_PxPoint_PxSize2D)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  PxRectangle value(PxPoint2(offsetX, offsetY), PxSize2D(width, height));

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(PxPoint2(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(offsetX + width, offsetY + height), value.BottomRight());
  EXPECT_EQ(PxSize2D(width, height), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, CreateFromLeftTopRightBottom)
{
  constexpr PxValue left(1);
  constexpr PxValue top(2);
  constexpr PxValue right(10);
  constexpr PxValue bottom(20);
  auto value = PxRectangle::CreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom)
{
  constexpr PxValue left(1);
  constexpr PxValue top(2);
  constexpr PxValue right(10);
  constexpr PxValue bottom(20);
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_MaxWidth)
{
  constexpr PxValue left(0);
  constexpr PxValue top(2);
  constexpr PxValue right(std::numeric_limits<int32_t>::max());
  constexpr PxValue bottom(20);
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_MaxHeight)
{
  constexpr PxValue left(1);
  constexpr PxValue top(0);
  constexpr PxValue right(10);
  constexpr PxValue bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedCreateFromLeftTopRightBottom)
{
  constexpr PxValue left(1);
  constexpr PxValue top(2);
  constexpr PxValue right(10);
  constexpr PxValue bottom(20);
  auto value = PxRectangle::UncheckedCreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom)
{
  constexpr PxValue left(1);
  constexpr PxValue top(2);
  constexpr PxValue right(10);
  constexpr PxValue bottom(20);
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom_MaxWidth)
{
  constexpr PxValue left(0);
  constexpr PxValue top(2);
  constexpr PxValue right(std::numeric_limits<int32_t>::max());
  constexpr PxValue bottom(20);
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom_NoCheck_MaxHeight)
{
  constexpr PxValue left(1);
  constexpr PxValue top(0);
  constexpr PxValue right(10);
  constexpr PxValue bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width());
  EXPECT_EQ(bottom - top, value.Height());
  EXPECT_EQ(PxPoint2(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(right, bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(right - left, bottom - top), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, SetWidth_PxValue)
{
  constexpr PxValue offsetX(1);
  constexpr PxValue offsetY(2);
  constexpr auto height = PxSize1D::Create(4);
  PxRectangle value(offsetX, offsetY, PxSize1D::Create(3), height);

  constexpr PxValue newWidth(10);
  value.SetWidth(newWidth);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetWidth_PxSize1D)
{
  constexpr PxValue offsetX(1);
  constexpr PxValue offsetY(2);
  constexpr auto height = PxSize1D::Create(4);
  PxRectangle value(offsetX, offsetY, PxSize1D::Create(3), height);

  constexpr PxSize1D newWidth = PxSize1D::Create(10);
  value.SetWidth(newWidth);

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetHeight_PxValue)
{
  constexpr PxValue offsetX(1);
  constexpr PxValue offsetY(2);
  constexpr PxSize1D width = PxSize1D::Create(3);
  PxRectangle value(offsetX, offsetY, width, PxSize1D::Create(4));

  constexpr PxValue newHeight = PxValue::Create(10);
  value.SetHeight(newHeight);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}

TEST(TestMathPixel_PxRectangle, SetHeight_PxSize1D)
{
  constexpr PxValue offsetX(1);
  constexpr PxValue offsetY(2);
  constexpr PxSize1D width = PxSize1D::Create(3);
  PxRectangle value(offsetX, offsetY, width, PxSize1D::Create(4));

  constexpr PxSize1D newHeight = PxSize1D::Create(10);
  value.SetHeight(newHeight);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetLocation_PxPoint2)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  PxRectangle value(offsetX, offsetY, width, height);

  constexpr PxPoint2 newLocation = PxPoint2::Create(500, 1000);
  value.SetLocation(newLocation);

  EXPECT_EQ(newLocation.X, value.Left());
  EXPECT_EQ(newLocation.Y, value.Top());
  EXPECT_EQ(newLocation.X + width, value.Right());
  EXPECT_EQ(newLocation.Y + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(newLocation.X, newLocation.Y), value.Location());
  EXPECT_EQ(PxPoint2(newLocation.X, newLocation.Y), value.TopLeft());
  EXPECT_EQ(PxPoint2(newLocation.X + width, newLocation.Y), value.TopRight());
  EXPECT_EQ(PxPoint2(newLocation.X, newLocation.Y + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(newLocation.X + width, newLocation.Y + height), value.BottomRight());
  EXPECT_EQ(PxSize2D(width, height), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, GetCenter)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  PxRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(PxPoint2::Create(offsetX.Value + (width.RawValue() / 2), offsetY.Value + (height.RawValue() / 2)), value.GetCenter());
}
