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
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslBase/Log/Math/LogRectangle2D.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Rectangle2D = TestFixtureFslBase;
}


TEST(TestMath_Rectangle2D, Empty)
{
  Rectangle2D value;

  EXPECT_EQ(Offset2D(), value.Offset);
  EXPECT_EQ(Extent2D(), value.Extent);
  EXPECT_EQ(Rectangle2D(), Rectangle2D::Empty());
}


TEST(TestMath_Rectangle2D, Construct1)
{
  int32_t offsetX = 1;
  int32_t offsetY = 2;
  uint32_t width = 10;
  uint32_t height = 20;
  Rectangle2D value(offsetX, offsetY, width, height);

  EXPECT_EQ(Offset2D(offsetX, offsetY), value.Offset);
  EXPECT_EQ(Extent2D(width, height), value.Extent);
  EXPECT_EQ(offsetX, value.Left());
  EXPECT_EQ(offsetY, value.Top());
  EXPECT_EQ(static_cast<int32_t>(offsetX + width), value.Right());
  EXPECT_EQ(static_cast<int32_t>(offsetY + height), value.Bottom());
}

TEST(TestMath_Rectangle2D, Construct2)
{
  int32_t left = 1;
  int32_t top = 2;
  int32_t right = 10;
  int32_t bottom = 20;
  Rectangle2D value(left, top, right, bottom, true);

  EXPECT_EQ(Offset2D(left, top), value.Offset);
  EXPECT_EQ(Extent2D(static_cast<uint32_t>(right - left), static_cast<uint32_t>(bottom - top)), value.Extent);
}

TEST(TestMath_Rectangle2D, Construct2_invalid)
{
  EXPECT_THROW(Rectangle2D(1, 2, -10, 20, true), std::invalid_argument);
  EXPECT_THROW(Rectangle2D(1, 2, 10, -20, true), std::invalid_argument);
}


TEST(TestMath_Rectangle2D, Inflate)
{
  const int32_t offsetX = 1;
  const int32_t offsetY = 2;
  const uint32_t width = 3;
  const uint32_t height = 4;
  Rectangle2D value(offsetX, offsetY, width, height);

  const int32_t inflateX = 1;
  const int32_t inflateY = 2;
  value.Inflate(inflateX, inflateY);

  EXPECT_EQ(Offset2D(offsetX - inflateX, offsetY - inflateY), value.Offset);
  EXPECT_EQ(Extent2D(width + (2 * inflateX), height + (2 * inflateY)), value.Extent);
  EXPECT_EQ(offsetX - inflateX, value.Left());
  EXPECT_EQ(offsetY - inflateY, value.Top());
  EXPECT_EQ(static_cast<int32_t>(offsetX - inflateX + width + (2 * inflateX)), value.Right());
  EXPECT_EQ(static_cast<int32_t>(offsetY - inflateY + height + (2 * inflateY)), value.Bottom());
}


TEST(TestMath_Rectangle2D, Intersects)
{
  Rectangle2D rect1(0, 0, 10, 10);
  Rectangle2D rect2(1, 1, 9, 9);
  Rectangle2D rect3(11, 0, 10, 10);

  auto res1 = Rectangle2D::Intersect(rect1, rect2);
  auto res2 = Rectangle2D::Intersect(rect2, rect1);
  auto resNoIntersection = Rectangle2D::Intersect(rect1, rect3);

  EXPECT_EQ(rect2, res1);
  EXPECT_EQ(rect2, res2);
  EXPECT_TRUE(resNoIntersection.IsEmpty());
}


TEST(TestMath_Rectangle2D, Union)
{
  const Rectangle2D rect1(0, 0, 5, 5);
  const Rectangle2D rect2(0, -1, 4, 5);

  auto res = Rectangle2D::Union(rect1, rect2);

  // 0, 5 |  0, 4
  // 0, 5 | -1, 4
  EXPECT_EQ(Rectangle2D(0, -1, 5, 5, true), res);
}
