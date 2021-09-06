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

#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangleU32.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxRectangleU32 = TestFixtureFslBase;
}


TEST(TestMathPixel_PxRectangleU32, Empty)
{
  PxRectangleU32 value;

  EXPECT_EQ(PxRectangleU32(), PxRectangleU32::Empty());
  EXPECT_EQ(0u, value.Left());
  EXPECT_EQ(0u, value.Top());
  EXPECT_EQ(0u, value.Right());
  EXPECT_EQ(0u, value.Bottom());
  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
  EXPECT_EQ(PxPoint2U(), value.TopLeft());
  EXPECT_EQ(PxPoint2U(), value.TopRight());
  EXPECT_EQ(PxPoint2U(), value.BottomLeft());
  EXPECT_EQ(PxPoint2U(), value.BottomRight());
}


TEST(TestMathPixel_PxRectangleU32, Construct1)
{
  uint32_t offsetX = 1;
  uint32_t offsetY = 2;
  uint32_t width = 10;
  uint32_t height = 20;
  PxRectangleU32 value(offsetX, offsetY, width, height);

  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
  EXPECT_EQ(PxPoint2U(offsetX, offsetY), value.TopLeft());
  EXPECT_EQ(PxPoint2U(offsetX + width, offsetY), value.TopRight());
  EXPECT_EQ(PxPoint2U(offsetX, offsetY + height), value.BottomLeft());
  EXPECT_EQ(PxPoint2U(offsetX + width, offsetY + height), value.BottomRight());
}

TEST(TestMathPixel_PxRectangleU32, Construct2)
{
  uint32_t left = 1;
  uint32_t top = 2;
  uint32_t right = 10;
  uint32_t bottom = 20;
  auto value = PxRectangleU32::FromLeftTopRightBottom(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(right - left, value.Width);
  EXPECT_EQ(bottom - top, value.Height);
  EXPECT_EQ(PxPoint2U(left, top), value.TopLeft());
  EXPECT_EQ(PxPoint2U(right, top), value.TopRight());
  EXPECT_EQ(PxPoint2U(left, bottom), value.BottomLeft());
  EXPECT_EQ(PxPoint2U(right, bottom), value.BottomRight());
}


TEST(TestMathPixel_PxRectangleU32, SetWidth)
{
  const uint32_t offsetX = 1;
  const uint32_t offsetY = 2;
  const uint32_t height = 4;
  PxRectangleU32 value(offsetX, offsetY, 3, height);

  const uint32_t newWidth = 10;
  value.Width = newWidth;
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + newWidth, value.Right());
  EXPECT_EQ(offsetY + height, value.Bottom());
  EXPECT_EQ(newWidth, value.Width);
  EXPECT_EQ(height, value.Height);
}


TEST(TestMathPixel_PxRectangleU32, SetHeight)
{
  const uint32_t offsetX = 1;
  const uint32_t offsetY = 2;
  const uint32_t width = 3;
  PxRectangleU32 value(offsetX, offsetY, width, 4);

  const uint32_t newHeight = 10;
  value.Height = newHeight;
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetX + width, value.Right());
  EXPECT_EQ(offsetY + newHeight, value.Bottom());
  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(newHeight, value.Height);
}
