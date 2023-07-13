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

  EXPECT_EQ(0u, value.Width.Value);
  EXPECT_EQ(0u, value.Height.Value);
}


TEST(TestMathPixel_PxExtent2D, Construct_Unsigned)
{
  const PxValueU width(320);
  const PxValueU height(240);
  const PxExtent2D value(width, height);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
}

TEST(TestMathPixel_PxExtent2D, Construct_Point2U)
{
  const auto point = PxPoint2U::Create(320, 240);
  PxExtent2D value(point);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
}


TEST(TestMathPixel_PxExtent2D, AddEquals)
{
  PxValueU width1(320);
  PxValueU height1(240);
  PxExtent2D value1(width1, height1);

  PxValueU width2(200);
  PxValueU height2(100);
  PxExtent2D value2(width2, height2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, SubEquals)
{
  PxValueU width1(320);
  PxValueU height1(240);
  PxExtent2D value1(width1, height1);

  PxValueU width2(800);
  PxValueU height2(600);
  PxExtent2D value2(width2, height2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, MultEquals)
{
  const PxValueU width1(2);
  const PxValueU height1(4);
  PxExtent2D value1(width1, height1);

  const PxValueU width2(200);
  const PxValueU height2(100);
  PxExtent2D value2(width2, height2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
}


TEST(TestMathPixel_PxExtent2D, MultEqual)
{
  const PxValueU multBy(2);

  PxValueU width(200);
  PxValueU height(100);
  PxExtent2D value(width, height);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
}


TEST(TestMathPixel_PxExtent2D, DivEqual)
{
  const PxValueU divBy(2);

  PxValueU width(200);
  PxValueU height(100);
  PxExtent2D value(width, height);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
}


TEST(TestMathPixel_PxExtent2D, Equal)
{
  auto value1 = PxExtent2D::Create(320, 240);
  auto value2 = PxExtent2D::Create(320, 240);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathPixel_PxExtent2D, NotEqual)
{
  auto value1 = PxExtent2D::Create(320, 240);
  auto value2 = PxExtent2D::Create(320, 241);
  auto value3 = PxExtent2D::Create(321, 240);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathPixel_PxExtent2D, Zero)
{
  PxExtent2D value = PxExtent2D::Zero();

  EXPECT_EQ(0u, value.Width.Value);
  EXPECT_EQ(0u, value.Height.Value);
}


TEST(TestMathPixel_PxExtent2D, OpAdd)
{
  auto value1 = PxExtent2D::Create(2, 3);
  auto value2 = PxExtent2D::Create(40, 80);

  EXPECT_EQ(PxExtent2D::Create(42, 83), value1 + value2);
  EXPECT_EQ(PxExtent2D::Create(42, 83), value2 + value1);
}


TEST(TestMathPixel_PxExtent2D, OpSub)
{
  auto value1 = PxExtent2D::Create(2, 3);
  auto value2 = PxExtent2D::Create(40, 80);
  auto value3 = PxExtent2D::Create(140, 181);

  EXPECT_EQ(PxExtent2D::Create(38, 77), value2 - value1);
  EXPECT_EQ(PxExtent2D::Create(100, 101), value3 - value2);
}


TEST(TestMathPixel_PxExtent2D, OpMul_PxExtend2D)
{
  auto value1 = PxExtent2D::Create(2, 3);
  auto value2 = PxExtent2D::Create(41, 80);

  EXPECT_EQ(PxExtent2D::Create(82, 240), value1 * value2);
  EXPECT_EQ(PxExtent2D::Create(82, 240), value2 * value1);
}


TEST(TestMathPixel_PxExtent2D, OpMul_Const)
{
  auto value1 = PxExtent2D::Create(2, 3);

  EXPECT_EQ(PxExtent2D::Create(4, 6), value1 * PxValueU(2u));
  EXPECT_EQ(PxExtent2D::Create(4, 6), PxValueU(2u) * value1);
}


TEST(TestMathPixel_PxExtent2D, OpDiv_Const)
{
  auto value1 = PxExtent2D::Create(20, 40);

  EXPECT_EQ(PxExtent2D::Create(10, 20), value1 / PxValueU(2u));
}
