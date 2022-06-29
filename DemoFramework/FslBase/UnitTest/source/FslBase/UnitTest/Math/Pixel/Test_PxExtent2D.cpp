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

#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxExtent2D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxExtent2D, Construct_Default)
{
  const PxExtent2D value;

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}


TEST(TestMathPixel_PxExtent2D, Construct_Unsigned)
{
  const uint32_t width = 320;
  const uint32_t height = 240;
  const PxExtent2D value(width, height);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
}

TEST(TestMathPixel_PxExtent2D, Construct_Point2U)
{
  const PxPoint2U point(320, 240);
  PxExtent2D value(point);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
}


TEST(TestMathPixel_PxExtent2D, AddEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  PxExtent2D value1(width1, height1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  PxExtent2D value2(width2, height2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, SubEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  PxExtent2D value1(width1, height1);

  uint32_t width2 = 800;
  uint32_t height2 = 600;
  PxExtent2D value2(width2, height2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, MultEquals)
{
  const uint32_t width1 = 2;
  const uint32_t height1 = 4;
  PxExtent2D value1(width1, height1);

  const uint32_t width2 = 200;
  const uint32_t height2 = 100;
  PxExtent2D value2(width2, height2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, MultEqual)
{
  const uint32_t multBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  PxExtent2D value(width, height);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
}


TEST(TestMathPixel_PxExtent2D, DivEqual)
{
  const uint32_t divBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  PxExtent2D value(width, height);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
}


TEST(TestMathPixel_PxExtent2D, Equal)
{
  PxExtent2D value1(320, 240);
  PxExtent2D value2(320, 240);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathPixel_PxExtent2D, NotEqual)
{
  PxExtent2D value1(320, 240);
  PxExtent2D value2(320, 241);
  PxExtent2D value3(321, 240);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathPixel_PxExtent2D, Zero)
{
  PxExtent2D value = PxExtent2D::Zero();

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}


TEST(TestMathPixel_PxExtent2D, OpAdd)
{
  PxExtent2D value1(2, 3);
  PxExtent2D value2(40, 80);

  EXPECT_EQ(PxExtent2D(42, 83), value1 + value2);
  EXPECT_EQ(PxExtent2D(42, 83), value2 + value1);
}


TEST(TestMathPixel_PxExtent2D, OpSub)
{
  PxExtent2D value1(2, 3);
  PxExtent2D value2(40, 80);
  PxExtent2D value3(140, 181);

  EXPECT_EQ(PxExtent2D(38, 77), value2 - value1);
  EXPECT_EQ(PxExtent2D(100, 101), value3 - value2);
}


TEST(TestMathPixel_PxExtent2D, OpMul_PxExtend2D)
{
  PxExtent2D value1(2, 3);
  PxExtent2D value2(41, 80);

  EXPECT_EQ(PxExtent2D(82, 240), value1 * value2);
  EXPECT_EQ(PxExtent2D(82, 240), value2 * value1);
}


TEST(TestMathPixel_PxExtent2D, OpMul_Const)
{
  PxExtent2D value1(2, 3);

  EXPECT_EQ(PxExtent2D(4, 6), value1 * 2u);
  EXPECT_EQ(PxExtent2D(4, 6), 2u * value1);
}


TEST(TestMathPixel_PxExtent2D, OpDiv_Const)
{
  PxExtent2D value1(20, 40);

  EXPECT_EQ(PxExtent2D(10, 20), value1 / 2u);
}
