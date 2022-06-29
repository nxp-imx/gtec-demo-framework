/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Log/Math/Dp/LogDpExtent2D.hpp>
#include <FslBase/Math/Dp/DpExtent2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathDp_DpExtent2D = TestFixtureFslBase;
}


TEST(TestMathDp_DpExtent2D, Construct_Default)
{
  const DpExtent2D value;

  EXPECT_EQ(DpExtent2D::value_type(0u), value.Width);
  EXPECT_EQ(DpExtent2D::value_type(0u), value.Height);
}


TEST(TestMathDp_DpExtent2D, Construct_Unsigned)
{
  const DpValueU width(320);
  const DpValueU height(240);
  const DpExtent2D value(width, height);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
}

TEST(TestMathDp_DpExtent2D, Construct_Point2U)
{
  const auto point = DpPoint2U::Create(320, 240);
  DpExtent2D value(point);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
}


TEST(TestMathDp_DpExtent2D, AddEquals)
{
  DpValueU width1(320);
  DpValueU height1(240);
  DpExtent2D value1(width1, height1);

  DpValueU width2(200);
  DpValueU height2(100);
  DpExtent2D value2(width2, height2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
}


TEST(TestMathDp_DpExtent2D, SubEquals)
{
  DpValueU width1(320);
  DpValueU height1(240);
  DpExtent2D value1(width1, height1);

  DpValueU width2(800);
  DpValueU height2(600);
  DpExtent2D value2(width2, height2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
}


TEST(TestMathDp_DpExtent2D, MultEquals)
{
  const DpValueU width1(2);
  const DpValueU height1(4);
  DpExtent2D value1(width1, height1);

  const DpValueU width2(200);
  const DpValueU height2(100);
  DpExtent2D value2(width2, height2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
}


TEST(TestMathDp_DpExtent2D, MultEqual)
{
  const DpValueU multBy(2);

  DpValueU width(200);
  DpValueU height(100);
  DpExtent2D value(width, height);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
}


TEST(TestMathDp_DpExtent2D, DivEqual)
{
  const DpValueU divBy(2);

  DpValueU width(200);
  DpValueU height(100);
  DpExtent2D value(width, height);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
}


TEST(TestMathDp_DpExtent2D, Equal)
{
  auto value1 = DpExtent2D::Create(320, 240);
  auto value2 = DpExtent2D::Create(320, 240);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathDp_DpExtent2D, NotEqual)
{
  auto value1 = DpExtent2D::Create(320, 240);
  auto value2 = DpExtent2D::Create(320, 241);
  auto value3 = DpExtent2D::Create(321, 240);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathDp_DpExtent2D, Zero)
{
  DpExtent2D value = DpExtent2D::Zero();

  EXPECT_EQ(DpExtent2D::value_type(0u), value.Width);
  EXPECT_EQ(DpExtent2D::value_type(0u), value.Height);
}


TEST(TestMathDp_DpExtent2D, OpAdd)
{
  auto value1 = DpExtent2D::Create(2, 3);
  auto value2 = DpExtent2D::Create(40, 80);

  EXPECT_EQ(DpExtent2D::Create(42, 83), value1 + value2);
  EXPECT_EQ(DpExtent2D::Create(42, 83), value2 + value1);
}


TEST(TestMathDp_DpExtent2D, OpSub)
{
  auto value1 = DpExtent2D::Create(2, 3);
  auto value2 = DpExtent2D::Create(40, 80);
  auto value3 = DpExtent2D::Create(140, 181);

  EXPECT_EQ(DpExtent2D::Create(38, 77), value2 - value1);
  EXPECT_EQ(DpExtent2D::Create(100, 101), value3 - value2);
}


TEST(TestMathDp_DpExtent2D, OpMul_DpExtend2D)
{
  auto value1 = DpExtent2D::Create(2, 3);
  auto value2 = DpExtent2D::Create(41, 80);

  EXPECT_EQ(DpExtent2D::Create(82, 240), value1 * value2);
  EXPECT_EQ(DpExtent2D::Create(82, 240), value2 * value1);
}


TEST(TestMathDp_DpExtent2D, OpMul_Const)
{
  auto value1 = DpExtent2D::Create(2, 3);

  EXPECT_EQ(DpExtent2D::Create(4, 6), value1 * DpExtent2D::value_type(2u));
  EXPECT_EQ(DpExtent2D::Create(4, 6), DpExtent2D::value_type(2u) * value1);
}


TEST(TestMathDp_DpExtent2D, OpDiv_Const)
{
  auto value1 = DpExtent2D::Create(20, 40);

  EXPECT_EQ(DpExtent2D::Create(10, 20), value1 / DpExtent2D::value_type(2u));
}
