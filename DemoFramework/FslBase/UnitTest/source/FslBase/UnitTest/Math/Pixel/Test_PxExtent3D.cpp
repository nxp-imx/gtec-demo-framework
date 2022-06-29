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

#include <FslBase/Log/Math/Pixel/LogPxExtent3D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxExtent3D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxExtent3D, Construct_Default)
{
  const PxExtent3D value;

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
  EXPECT_EQ(0u, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, Construct_Unsigned)
{
  const uint32_t width = 320;
  const uint32_t height = 240;
  const uint32_t depth = 120;
  const PxExtent3D value(width, height, depth);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
  EXPECT_EQ(depth, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, Construct_PxPoint2U)
{
  const PxPoint2U point(320, 240);
  const uint32_t depth = 120;
  PxExtent3D value(point, depth);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
  EXPECT_EQ(depth, depth);
}


TEST(TestMathPixel_PxExtent3D, Construct_PxExtent2D)
{
  const PxExtent2D point(320, 240);
  const uint32_t depth = 120;
  PxExtent3D value(point, depth);

  EXPECT_EQ(point.Width, value.Width);
  EXPECT_EQ(point.Height, value.Height);
  EXPECT_EQ(depth, depth);
}


TEST(TestMathPixel_PxExtent3D, AddEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  uint32_t depth1 = 120;
  PxExtent3D value1(width1, height1, depth1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  uint32_t depth2 = 50;
  PxExtent3D value2(width2, height2, depth2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(depth1, value1.Depth);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
  EXPECT_EQ(depth2 + depth1, value2.Depth);
}


TEST(TestMathPixel_PxExtent3D, SubEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  uint32_t depth1 = 120;
  PxExtent3D value1(width1, height1, depth1);

  uint32_t width2 = 2000;
  uint32_t height2 = 1000;
  uint32_t depth2 = 500;
  PxExtent3D value2(width2, height2, depth2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
  EXPECT_EQ(depth2 - depth1, value2.Depth);
}


TEST(TestMathPixel_PxExtent3D, MultEquals)
{
  uint32_t width1 = 2;
  uint32_t height1 = 4;
  uint32_t depth1 = 5;
  PxExtent3D value1(width1, height1, depth1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  uint32_t depth2 = 50;
  PxExtent3D value2(width2, height2, depth2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
  EXPECT_EQ(depth2 * depth1, value2.Depth);
}


TEST(TestMathPixel_PxExtent3D, MultEqual)
{
  int multBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  uint32_t depth = 50;
  PxExtent3D value(width, height, depth);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
  EXPECT_EQ(depth * multBy, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, DivEqual)
{
  const uint32_t divBy = 2;

  const uint32_t width = 200;
  const uint32_t height = 100;
  const uint32_t depth = 50;
  PxExtent3D value(width, height, depth);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
  EXPECT_EQ(depth / divBy, value.Depth);
}

TEST(TestMathPixel_PxExtent3D, Equal)
{
  PxExtent3D value1(320, 240, 120);
  PxExtent3D value2(320, 240, 120);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathPixel_PxExtent3D, NotEqual)
{
  PxExtent3D value1(320, 240, 100);
  PxExtent3D value2(320, 241, 101);
  PxExtent3D value3(321, 240, 102);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathPixel_PxExtent3D, Zero)
{
  PxExtent3D value = PxExtent3D::Zero();

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
  EXPECT_EQ(0u, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, OpAdd)
{
  PxExtent3D value1(2, 3, 4);
  PxExtent3D value2(40, 80, 120);

  EXPECT_EQ(PxExtent3D(42, 83, 124), value1 + value2);
  EXPECT_EQ(PxExtent3D(42, 83, 124), value2 + value1);
}


TEST(TestMathPixel_PxExtent3D, OpSub)
{
  PxExtent3D value1(2, 3, 4);
  PxExtent3D value2(40, 80, 120);
  PxExtent3D value3(140, 181, 202);

  EXPECT_EQ(PxExtent3D(38, 77, 116), value2 - value1);
  EXPECT_EQ(PxExtent3D(100, 101, 82), value3 - value2);
}


TEST(TestMathPixel_PxExtent3D, OpMul_PxExtend2D)
{
  PxExtent3D value1(2, 3, 4);
  PxExtent3D value2(41, 80, 120);

  EXPECT_EQ(PxExtent3D(82, 240, 480), value1 * value2);
  EXPECT_EQ(PxExtent3D(82, 240, 480), value2 * value1);
}


TEST(TestMathPixel_PxExtent3D, OpMul_Const)
{
  PxExtent3D value1(2, 3, 4);

  EXPECT_EQ(PxExtent3D(4, 6, 8), value1 * 2u);
  EXPECT_EQ(PxExtent3D(4, 6, 8), 2u * value1);
}


TEST(TestMathPixel_PxExtent3D, OpDiv_Const)
{
  PxExtent3D value1(20, 40, 60);

  EXPECT_EQ(PxExtent3D(10, 20, 30), value1 / 2u);
}
