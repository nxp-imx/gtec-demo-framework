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
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Extent2D = TestFixtureFslBase;
}


TEST(TestMath_Extent2D, Construct_Default)
{
  const Extent2D value;

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}


TEST(TestMath_Extent2D, Construct_Unsigned)
{
  const uint32_t width = 320;
  const uint32_t height = 240;
  const Extent2D value(width, height);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
}


TEST(TestMath_Extent2D, Construct_Signed)
{
  const int32_t width = 320;
  const int32_t height = 240;
  const Extent2D value(width, height);

  EXPECT_EQ(static_cast<uint32_t>(width), value.Width);
  EXPECT_EQ(static_cast<uint32_t>(height), value.Height);
}


TEST(TestMath_Extent2D, Construct_Signed_Negative)
{
  EXPECT_THROW(Extent2D(-320, 200), std::invalid_argument);
  EXPECT_THROW(Extent2D(320, -200), std::invalid_argument);
}


TEST(TestMath_Extent2D, Construct_Point2)
{
  const Point2 point(320, 240);
  Extent2D value(point);

  EXPECT_EQ(static_cast<uint32_t>(point.X), value.Width);
  EXPECT_EQ(static_cast<uint32_t>(point.Y), value.Height);
}


TEST(TestMath_Extent2D, Construct_Point2_Negative)
{
  EXPECT_THROW(Extent2D(Point2(-320, 200)), std::invalid_argument);
  EXPECT_THROW(Extent2D(Point2(320, -200)), std::invalid_argument);
}


TEST(TestMath_Extent2D, AddEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  Extent2D value1(width1, height1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  Extent2D value2(width2, height2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
}


TEST(TestMath_Extent2D, SubEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  Extent2D value1(width1, height1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  Extent2D value2(width2, height2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
}


TEST(TestMath_Extent2D, MultEquals)
{
  uint32_t width1 = 2;
  uint32_t height1 = 4;
  Extent2D value1(width1, height1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  Extent2D value2(width2, height2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
}


TEST(TestMath_Extent2D, MultEqual)
{
  int multBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  Extent2D value(width, height);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
}


TEST(TestMath_Extent2D, Equal)
{
  Extent2D value1(320, 240);
  Extent2D value2(320, 240);

  EXPECT_EQ(value1, value2);
}


TEST(TestMath_Extent2D, NotEqual)
{
  Extent2D value1(320, 240);
  Extent2D value2(320, 241);
  Extent2D value3(321, 240);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMath_Extent2D, Zero)
{
  Extent2D value = Extent2D::Zero();

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}
