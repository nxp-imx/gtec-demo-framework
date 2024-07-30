/****************************************************************************************************************************************************
 * Copyright 2020, 2023-2024 NXP
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
  constexpr PxValue Left(1);
  constexpr PxValue Top(2);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::CreateFromLeftTopRightBottom(Left.Value, Top.Value, Right.Value, Bottom.Value);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(2);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::FromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_MaxWidth)
{
  constexpr PxValue Left(0);
  constexpr PxValue Top(2);
  constexpr PxValue Right(std::numeric_limits<int32_t>::max());
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::FromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_MaxHeight)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(0);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle::FromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedCreateFromLeftTopRightBottom)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(2);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::UncheckedCreateFromLeftTopRightBottom(Left.Value, Top.Value, Right.Value, Bottom.Value);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(2);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom_MaxWidth)
{
  constexpr PxValue Left(0);
  constexpr PxValue Top(2);
  constexpr PxValue Right(std::numeric_limits<int32_t>::max());
  constexpr PxValue Bottom(20);
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}

TEST(TestMathPixel_PxRectangle, UncheckedFromLeftTopRightBottom_NoCheck_MaxHeight)
{
  constexpr PxValue Left(1);
  constexpr PxValue Top(0);
  constexpr PxValue Right(10);
  constexpr PxValue Bottom(std::numeric_limits<int32_t>::max());
  auto value = PxRectangle::UncheckedFromLeftTopRightBottom(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Right - Left, value.Width());
  EXPECT_EQ(Bottom - Top, value.Height());
  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
  EXPECT_EQ(PxSize2D(Right - Left, Bottom - Top), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, SetWidth_PxValue)
{
  constexpr PxValue OffsetX(1);
  constexpr PxValue OffsetY(2);
  constexpr auto Height = PxSize1D::Create(4);
  PxRectangle value(OffsetX, OffsetY, PxSize1D::Create(3), Height);

  constexpr PxValue NewWidth(10);
  value.SetWidth(NewWidth);
  EXPECT_EQ(OffsetX, value.Left());
  EXPECT_EQ(OffsetY, value.Top());
  EXPECT_EQ(OffsetX + NewWidth, value.Right());
  EXPECT_EQ(OffsetY + Height, value.Bottom());
  EXPECT_EQ(NewWidth, value.Width());
  EXPECT_EQ(Height, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetWidth_PxSize1D)
{
  constexpr PxValue OffsetX(1);
  constexpr PxValue OffsetY(2);
  constexpr auto Height = PxSize1D::Create(4);
  PxRectangle value(OffsetX, OffsetY, PxSize1D::Create(3), Height);

  constexpr PxSize1D NewWidth = PxSize1D::Create(10);
  value.SetWidth(NewWidth);

  EXPECT_EQ(OffsetX, value.Left());
  EXPECT_EQ(OffsetY, value.Top());
  EXPECT_EQ(OffsetX + NewWidth, value.Right());
  EXPECT_EQ(OffsetY + Height, value.Bottom());
  EXPECT_EQ(NewWidth, value.Width());
  EXPECT_EQ(Height, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetHeight_PxValue)
{
  constexpr PxValue OffsetX(1);
  constexpr PxValue OffsetY(2);
  constexpr PxSize1D Width = PxSize1D::Create(3);
  PxRectangle value(OffsetX, OffsetY, Width, PxSize1D::Create(4));

  constexpr PxValue NewHeight = PxValue::Create(10);
  value.SetHeight(NewHeight);
  EXPECT_EQ(OffsetX, value.Left());
  EXPECT_EQ(OffsetY, value.Top());
  EXPECT_EQ(OffsetX + Width, value.Right());
  EXPECT_EQ(OffsetY + NewHeight, value.Bottom());
  EXPECT_EQ(Width, value.Width());
  EXPECT_EQ(NewHeight, value.Height());
}

TEST(TestMathPixel_PxRectangle, SetHeight_PxSize1D)
{
  constexpr PxValue OffsetX(1);
  constexpr PxValue OffsetY(2);
  constexpr PxSize1D Width = PxSize1D::Create(3);
  PxRectangle value(OffsetX, OffsetY, Width, PxSize1D::Create(4));

  constexpr PxSize1D NewHeight = PxSize1D::Create(10);
  value.SetHeight(NewHeight);
  EXPECT_EQ(OffsetX, value.Left());
  EXPECT_EQ(OffsetY, value.Top());
  EXPECT_EQ(OffsetX + Width, value.Right());
  EXPECT_EQ(OffsetY + NewHeight, value.Bottom());
  EXPECT_EQ(Width, value.Width());
  EXPECT_EQ(NewHeight, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetLocation_PxPoint2)
{
  PxValue offsetX(1);
  PxValue offsetY(2);
  PxSize1D width = PxSize1D::Create(10);
  PxSize1D height = PxSize1D::Create(20);
  PxRectangle value(offsetX, offsetY, width, height);

  constexpr PxPoint2 NewLocation = PxPoint2::Create(500, 1000);
  value.SetLocation(NewLocation);

  EXPECT_EQ(NewLocation.X, value.Left());
  EXPECT_EQ(NewLocation.Y, value.Top());
  EXPECT_EQ(NewLocation.X + width, value.Right());
  EXPECT_EQ(NewLocation.Y + height, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
  EXPECT_EQ(PxPoint2(NewLocation.X, NewLocation.Y), value.Location());
  EXPECT_EQ(PxPoint2(NewLocation.X, NewLocation.Y), value.TopLeft());
  EXPECT_EQ(PxPoint2(NewLocation.X + width, NewLocation.Y), value.TopRight());
  EXPECT_EQ(PxPoint2(NewLocation.X, NewLocation.Y + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2(NewLocation.X + width, NewLocation.Y + height), value.BottomRight());
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
