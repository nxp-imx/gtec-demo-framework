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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Rectangle3D.hpp>
#include <FslBase/Log/Math/LogRectangle3D.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Rectangle3D = TestFixtureFslBase;
}


TEST(TestMath_Rectangle3D, Empty)
{
  Rectangle3D value;

  EXPECT_EQ(Offset3D(), value.Offset);
  EXPECT_EQ(Extent3D(), value.Extent);
  EXPECT_EQ(Rectangle3D(), Rectangle3D::Empty());
}


TEST(TestMath_Rectangle3D, Construct1)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  int32_t offsetZ = 3;
  uint16_t width = 10;
  uint16_t height = 20;
  uint16_t depth = 30;
  Rectangle3D value(offsetX, offsetY, offsetZ, width, height, depth);

  EXPECT_EQ(Offset3D(offsetX, offsetY, offsetZ), value.Offset);
  EXPECT_EQ(Extent3D(width, height, depth), value.Extent);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(offsetZ, value.Front());
  EXPECT_EQ(static_cast<int32_t>(offsetX + width), value.Right());
  EXPECT_EQ(static_cast<int32_t>(offsetY + height), value.Bottom());
  EXPECT_EQ(static_cast<int32_t>(offsetZ + depth), value.Back());
}

TEST(TestMath_Rectangle3D, FromLeftTopFrontRightBottomBack)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t front = 3;
  int32_t right = 10;
  int32_t bottom = 20;
  int32_t back = 30;
  auto value = Rectangle3D::FromLeftTopFrontRightBottomBack(left, top, front, right, bottom, back);

  EXPECT_EQ(Offset3D(left, top, front), value.Offset);
  EXPECT_EQ(Extent3D(static_cast<uint32_t>(right - left), static_cast<uint32_t>(bottom - top), static_cast<uint32_t>(back - front)), value.Extent);
}

TEST(TestMath_Rectangle3D, FromLeftTopFrontRightBottomBack_Invalid)
{
  EXPECT_THROW(Rectangle3D::FromLeftTopFrontRightBottomBack(1, 2, 3, -10, 20, 30), std::invalid_argument);
  EXPECT_THROW(Rectangle3D::FromLeftTopFrontRightBottomBack(1, 2, 3, 10, -20, 30), std::invalid_argument);
  EXPECT_THROW(Rectangle3D::FromLeftTopFrontRightBottomBack(1, 2, 3, 10, 20, -30), std::invalid_argument);
}


TEST(TestMath_Rectangle3D, Inflate)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t offsetZ = 3;
  const uint32_t width = 4;
  const uint32_t height = 5;
  const uint32_t depth = 6;
  Rectangle3D value(offsetX, offsetY, offsetZ, width, height, depth);

  const int32_t inflateX = 1;
  const int32_t inflateY = 2;
  const int32_t inflateZ = 3;
  value.Inflate(inflateX, inflateY, inflateZ);

  EXPECT_EQ(Offset3D(offsetX - inflateX, offsetY - inflateY, offsetZ - inflateZ), value.Offset);
  EXPECT_EQ(Extent3D(width + (2 * inflateX), height + (2 * inflateY), depth + (2 * inflateZ)), value.Extent);
  EXPECT_EQ(offsetX - inflateX, value.Left());
  EXPECT_EQ(offsetY - inflateY, value.Top());
  EXPECT_EQ(offsetZ - inflateZ, value.Front());
  EXPECT_EQ(static_cast<int32_t>(offsetX - inflateX + width + (2 * inflateX)), value.Right());
  EXPECT_EQ(static_cast<int32_t>(offsetY - inflateY + height + (2 * inflateY)), value.Bottom());
  EXPECT_EQ(static_cast<int32_t>(offsetZ - inflateZ + depth + (2 * inflateZ)), value.Back());
}


TEST(TestMath_Rectangle3D, Inflate_InvalidX)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t offsetZ = 3;
  const uint32_t width = 4;
  const uint32_t height = 5;
  const uint32_t depth = 6;
  Rectangle3D value(offsetX, offsetY, offsetZ, width, height, depth);

  const int32_t inflateX = -1;
  const int32_t inflateY = 2;
  const int32_t inflateZ = 3;
  EXPECT_THROW(value.Inflate(inflateX, inflateY, inflateZ), std::invalid_argument);
}


TEST(TestMath_Rectangle3D, Inflate_InvalidY)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t offsetZ = 3;
  const uint32_t width = 4;
  const uint32_t height = 5;
  const uint32_t depth = 6;
  Rectangle3D value(offsetX, offsetY, offsetZ, width, height, depth);

  const int32_t inflateX = 1;
  const int32_t inflateY = -2;
  const int32_t inflateZ = 3;
  EXPECT_THROW(value.Inflate(inflateX, inflateY, inflateZ), std::invalid_argument);
}


TEST(TestMath_Rectangle3D, Inflate_InvalidZ)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const int32_t offsetZ = 3;
  const uint32_t width = 4;
  const uint32_t height = 5;
  const uint32_t depth = 6;
  Rectangle3D value(offsetX, offsetY, offsetZ, width, height, depth);

  const int32_t inflateX = 1;
  const int32_t inflateY = 2;
  const int32_t inflateZ = -3;
  EXPECT_THROW(value.Inflate(inflateX, inflateY, inflateZ), std::invalid_argument);
}


TEST(TestMath_Rectangle3D, Intersects)
{
  const Rectangle3D rect1(0, 0, 0, 10, 10, 10);
  const Rectangle3D rect2(1, 1, 1, 9, 9, 9);
  const Rectangle3D rect3(11, 0, 0, 10, 10, 10);
  auto res1 = Rectangle3D::Intersect(rect1, rect2);
  auto res2 = Rectangle3D::Intersect(rect2, rect1);
  auto resNoIntersection = Rectangle3D::Intersect(rect1, rect3);

  EXPECT_EQ(rect2, res1);
  EXPECT_EQ(rect2, res2);
  EXPECT_TRUE(resNoIntersection.IsEmpty());
}


TEST(TestMath_Rectangle3D, Union)
{
  const Rectangle3D rect1(0, 0, 0, 5, 5, 5);
  const Rectangle3D rect2(0, -1, -2, 4, 5, 8);

  auto res = Rectangle3D::Union(rect1, rect2);

  // 0, 5 |  0, 4
  // 0, 5 | -1, 4
  // 0, 5 | -2, 6
  EXPECT_EQ(Rectangle3D::FromLeftTopFrontRightBottomBack(0, -1, -2, 5, 5, 6), res);
}
