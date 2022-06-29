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
  EXPECT_EQ(0, value.Left());
  EXPECT_EQ(0, value.Top());
  EXPECT_EQ(0, value.Right());
  EXPECT_EQ(0, value.Bottom());
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
  EXPECT_EQ(PxPoint2(), value.TopLeft());
  EXPECT_EQ(PxPoint2(), value.TopRight());
  EXPECT_EQ(PxPoint2(), value.BottomLeft());
  EXPECT_EQ(PxPoint2(), value.BottomRight());
  EXPECT_EQ(PxSize2D(), value.GetSize());
}


TEST(TestMathPixel_PxRectangle, Construct1)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
  PxRectangle value(offsetX, offsetY, width, height);

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
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
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
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
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


TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom)
{
  constexpr const int32_t left = 1;
  constexpr const int32_t top = 2;
  constexpr const int32_t right = 10;
  constexpr const int32_t bottom = 20;
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
  constexpr const int32_t left = 0;
  constexpr const int32_t top = 2;
  constexpr const int32_t right = std::numeric_limits<int32_t>::max();
  constexpr const int32_t bottom = 20;
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
  constexpr const int32_t left = 1;
  constexpr const int32_t top = 0;
  constexpr const int32_t right = 10;
  constexpr const int32_t bottom = std::numeric_limits<int32_t>::max();
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

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_NoCheck)
{
  constexpr const int32_t left = 1;
  constexpr const int32_t top = 2;
  constexpr const int32_t right = 10;
  constexpr const int32_t bottom = 20;
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom, OptimizationCheckFlag::NoCheck);

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

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_NoCheck_MaxWidth)
{
  constexpr const int32_t left = 0;
  constexpr const int32_t top = 2;
  constexpr const int32_t right = std::numeric_limits<int32_t>::max();
  constexpr const int32_t bottom = 20;
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom, OptimizationCheckFlag::NoCheck);

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

TEST(TestMathPixel_PxRectangle, FromLeftTopRightBottom_NoCheck_MaxHeight)
{
  constexpr const int32_t left = 1;
  constexpr const int32_t top = 0;
  constexpr const int32_t right = 10;
  constexpr const int32_t bottom = std::numeric_limits<int32_t>::max();
  auto value = PxRectangle::FromLeftTopRightBottom(left, top, right, bottom, OptimizationCheckFlag::NoCheck);

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


TEST(TestMathPixel_PxRectangle, SetWidth)
{
  constexpr const int32_t offsetX = 1;
  constexpr const int32_t offsetY = 2;
  constexpr const int32_t height = 4;
  PxRectangle value(offsetX, offsetY, 3, height);

  constexpr const int32_t newWidth = 10;
  value.SetWidth(newWidth);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width());
  EXPECT_EQ(height, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetHeight)
{
  constexpr const int32_t offsetX = 1;
  constexpr const int32_t offsetY = 2;
  constexpr const int32_t width = 3;
  PxRectangle value(offsetX, offsetY, width, 4);

  constexpr const int32_t newHeight = 10;
  value.SetHeight(newHeight);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(newHeight, value.Height());
}


TEST(TestMathPixel_PxRectangle, SetLocation)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
  PxRectangle value(offsetX, offsetY, width, height);

  constexpr const PxPoint2 newLocation(500, 1000);
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
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t width = 10;
  int32_t height = 20;
  PxRectangle value(offsetX, offsetY, width, height);

  EXPECT_EQ(PxPoint2(offsetX + (width / 2), offsetY + (height / 2)), value.GetCenter());
}
