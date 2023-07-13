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

  EXPECT_EQ(0u, value.Width.Value);
  EXPECT_EQ(0u, value.Height.Value);
  EXPECT_EQ(0u, value.Depth.Value);
}


TEST(TestMathPixel_PxExtent3D, Construct_Unsigned)
{
  const PxValueU width(320);
  const PxValueU height(240);
  const PxValueU depth(120);
  const PxExtent3D value(width, height, depth);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
  EXPECT_EQ(depth, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, Construct_PxPoint2U)
{
  const auto point = PxPoint2U::Create(320, 240);
  const PxValueU depth(120);
  PxExtent3D value(point, depth);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
  EXPECT_EQ(depth, depth);
}


TEST(TestMathPixel_PxExtent3D, Construct_PxExtent2D)
{
  const PxExtent2D point = PxExtent2D::Create(320, 240);
  const PxValueU depth(120);
  PxExtent3D value(point, depth);

  EXPECT_EQ(point.Width, value.Width);
  EXPECT_EQ(point.Height, value.Height);
  EXPECT_EQ(depth, depth);
}


TEST(TestMathPixel_PxExtent3D, AddEquals)
{
  PxValueU width1(320);
  PxValueU height1(240);
  PxValueU depth1(120);
  PxExtent3D value1(width1, height1, depth1);

  PxValueU width2(200);
  PxValueU height2(100);
  PxValueU depth2(50);
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
  PxValueU width1(320);
  PxValueU height1(240);
  PxValueU depth1(120);
  PxExtent3D value1(width1, height1, depth1);

  PxValueU width2(2000);
  PxValueU height2(1000);
  PxValueU depth2(500);
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
  PxValueU width1(2);
  PxValueU height1(4);
  PxValueU depth1(5);
  PxExtent3D value1(width1, height1, depth1);

  PxValueU width2(200);
  PxValueU height2(100);
  PxValueU depth2(50);
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
  PxValueU multBy(2);

  PxValueU width(200);
  PxValueU height(100);
  PxValueU depth(50);
  PxExtent3D value(width, height, depth);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
  EXPECT_EQ(depth * multBy, value.Depth);
}


TEST(TestMathPixel_PxExtent3D, DivEqual)
{
  const PxValueU divBy(2);

  const PxValueU width(200);
  const PxValueU height(100);
  const PxValueU depth(50);
  PxExtent3D value(width, height, depth);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
  EXPECT_EQ(depth / divBy, value.Depth);
}

TEST(TestMathPixel_PxExtent3D, Equal)
{
  const auto value1 = PxExtent3D::Create(320, 240, 120);
  const auto value2 = PxExtent3D::Create(320, 240, 120);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathPixel_PxExtent3D, NotEqual)
{
  const auto value1 = PxExtent3D::Create(320, 240, 100);
  const auto value2 = PxExtent3D::Create(320, 241, 101);
  const auto value3 = PxExtent3D::Create(321, 240, 102);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathPixel_PxExtent3D, Zero)
{
  PxExtent3D value = PxExtent3D::Zero();

  EXPECT_EQ(0u, value.Width.Value);
  EXPECT_EQ(0u, value.Height.Value);
  EXPECT_EQ(0u, value.Depth.Value);
}


TEST(TestMathPixel_PxExtent3D, OpAdd)
{
  const auto value1 = PxExtent3D::Create(2, 3, 4);
  const auto value2 = PxExtent3D::Create(40, 80, 120);

  EXPECT_EQ(PxExtent3D::Create(42, 83, 124), value1 + value2);
  EXPECT_EQ(PxExtent3D::Create(42, 83, 124), value2 + value1);
}


TEST(TestMathPixel_PxExtent3D, OpSub)
{
  const auto value1 = PxExtent3D::Create(2, 3, 4);
  const auto value2 = PxExtent3D::Create(40, 80, 120);
  const auto value3 = PxExtent3D::Create(140, 181, 202);

  EXPECT_EQ(PxExtent3D::Create(38, 77, 116), value2 - value1);
  EXPECT_EQ(PxExtent3D::Create(100, 101, 82), value3 - value2);
}


TEST(TestMathPixel_PxExtent3D, OpMul_PxExtend2D)
{
  const auto value1 = PxExtent3D::Create(2, 3, 4);
  const auto value2 = PxExtent3D::Create(41, 80, 120);

  EXPECT_EQ(PxExtent3D::Create(82, 240, 480), value1 * value2);
  EXPECT_EQ(PxExtent3D::Create(82, 240, 480), value2 * value1);
}


TEST(TestMathPixel_PxExtent3D, OpMul_Const)
{
  const auto value1 = PxExtent3D::Create(2, 3, 4);

  EXPECT_EQ(PxExtent3D::Create(4, 6, 8), value1 * PxValueU(2u));
  EXPECT_EQ(PxExtent3D::Create(4, 6, 8), PxValueU(2u) * value1);
}


TEST(TestMathPixel_PxExtent3D, OpDiv_Const)
{
  const auto value1 = PxExtent3D::Create(20, 40, 60);

  EXPECT_EQ(PxExtent3D::Create(10, 20, 30), value1 / PxValueU(2u));
}
