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
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslBase/Log/Math/LogExtent3D.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Extent3D = TestFixtureFslBase;
}


TEST(TestMath_Extent3D, Construct_Default)
{
  const Extent3D value;

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
  EXPECT_EQ(0u, value.Depth);
}


TEST(TestMath_Extent3D, Construct_Unsigned)
{
  const uint32_t width = 320;
  const uint32_t height = 240;
  const uint32_t depth = 100;
  const Extent3D value(width, height, depth);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
  EXPECT_EQ(depth, value.Depth);
}

TEST(TestMath_Extent3D, Construct_Signed)
{
  const int32_t width = 320;
  const int32_t height = 240;
  const int32_t depth = 100;
  const Extent3D value(width, height, depth);

  EXPECT_EQ(static_cast<uint32_t>(width), value.Width);
  EXPECT_EQ(static_cast<uint32_t>(height), value.Height);
  EXPECT_EQ(static_cast<uint32_t>(depth), value.Depth);
}


TEST(TestMath_Extent3D, Construct_Signed_Negative)
{
  EXPECT_THROW(Extent3D(-320, 200, 100), std::invalid_argument);
  EXPECT_THROW(Extent3D(320, -200, 100), std::invalid_argument);
  EXPECT_THROW(Extent3D(320, 200, -100), std::invalid_argument);
}


TEST(TestMath_Extent3D, Construct_Point2)
{
  const Point2 point(320, 240);
  const uint32_t depth = 100;
  Extent3D value(point, depth);

  EXPECT_EQ(static_cast<uint32_t>(point.X), value.Width);
  EXPECT_EQ(static_cast<uint32_t>(point.Y), value.Height);
  EXPECT_EQ(depth, value.Depth);
}


TEST(TestMath_Extent3D, Construct_Point2_Negative)
{
  EXPECT_THROW(Extent3D(Point2(-320, 200), 100u), std::invalid_argument);
  EXPECT_THROW(Extent3D(Point2(320, -200), 100u), std::invalid_argument);
}


TEST(TestMath_Extent3D, Construct_Extend2D)
{
  const Extent2D extent(320, 240);
  const uint32_t depth = 100;
  Extent3D value(extent, depth);

  EXPECT_EQ(extent.Width, value.Width);
  EXPECT_EQ(extent.Height, value.Height);
  EXPECT_EQ(depth, value.Depth);
}


TEST(TestMath_Extent3D, AddEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  uint32_t depth1 = 100;
  Extent3D value1(width1, height1, depth1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  uint32_t depth2 = 50;
  Extent3D value2(width2, height2, depth2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(depth1, value1.Depth);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
  EXPECT_EQ(depth2 + depth1, value2.Depth);
}


TEST(TestMath_Extent3D, SubEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  uint32_t depth1 = 100;
  Extent3D value1(width1, height1, depth1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  uint32_t depth2 = 50;
  Extent3D value2(width2, height2, depth2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(depth1, value1.Depth);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
  EXPECT_EQ(depth2 - depth1, value2.Depth);
}


TEST(TestMath_Extent3D, MultEquals)
{
  uint32_t width1 = 2;
  uint32_t height1 = 4;
  uint32_t depth1 = 8;
  Extent3D value1(width1, height1, depth1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  uint32_t depth2 = 50;
  Extent3D value2(width2, height2, depth2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(depth1, value1.Depth);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
  EXPECT_EQ(depth2 * depth1, value2.Depth);
}


TEST(TestMath_Extent3D, MultEqual)
{
  int multBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  uint32_t depth = 50;
  Extent3D value(width, height, depth);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
  EXPECT_EQ(depth * multBy, value.Depth);
}


TEST(TestMath_Extent3D, Equal)
{
  Extent3D value1(320, 240, 100);
  Extent3D value2(320, 240, 100);

  EXPECT_EQ(value1, value2);
}


TEST(TestMath_Extent3D, NotEqual)
{
  Extent3D value1(320, 240, 100);
  Extent3D value2(321, 240, 100);
  Extent3D value3(320, 241, 100);
  Extent3D value4(320, 240, 101);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
  EXPECT_NE(value1, value4);
}


TEST(TestMath_Extent3D, Zero)
{
  auto value = Extent3D::Zero();

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
  EXPECT_EQ(0u, value.Depth);
}
