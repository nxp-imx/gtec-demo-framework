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

#include <FslBase/Log/Math/Dp/LogDpPoint2.hpp>
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpPoint2 = TestFixtureFslBase;
}


TEST(TestMathDp_DpPoint2, Zero)
{
  EXPECT_EQ(DpPoint2(), DpPoint2::Zero());
  EXPECT_EQ(DpValue(0), DpPoint2::Zero().X);
  EXPECT_EQ(DpValue(0), DpPoint2::Zero().Y);
}


TEST(TestMathDp_DpPoint2, Construct_Default)
{
  DpPoint2 value;

  EXPECT_EQ(DpPoint2(), value);
  EXPECT_EQ(DpValue(0), value.X);
  EXPECT_EQ(DpValue(0), value.Y);
}

TEST(TestMathDp_DpPoint2, Construct)
{
  const DpValue x(3);
  const DpValue y(2);
  DpPoint2 value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathDp_DpPoint2, OpPlusEqual)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpPoint2, OpSubEqual)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}


TEST(TestMathDp_DpPoint2, OpMulEqual)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}


TEST(TestMathDp_DpPoint2, OpMulEqual_Const)
{
  const DpValue x0(5);
  const DpValue y0(4);
  DpPoint2 value0(x0, y0);

  const DpValue mulBy(2);
  value0 *= mulBy;

  EXPECT_EQ(x0 * mulBy, value0.X);
  EXPECT_EQ(y0 * mulBy, value0.Y);
}

TEST(TestMathDp_DpPoint2, OpDivEqual_Const)
{
  const DpValue x0(50);
  const DpValue y0(40);
  DpPoint2 value0(x0, y0);

  const DpValue divBy(2);

  value0 /= divBy;

  EXPECT_EQ(x0 / divBy, value0.X);
  EXPECT_EQ(y0 / divBy, value0.Y);
}

TEST(TestMathDp_DpPoint2, OpAdd)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPoint2, OpSub)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpPoint2, OpMul)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathDp_DpPoint2, OpMul_Const)
{
  const DpValue x0(4);
  const DpValue y0(3);
  const DpValue mul(2);
  DpPoint2 value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMathDp_DpPoint2, OpMul_Cons)
{
  const DpValue x0(4);
  const DpValue y0(3);
  const DpValue mul(2);
  DpPoint2 value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}


TEST(TestMathDp_DpPoint2, OpDiv_Const)
{
  const DpValue x0(40);
  const DpValue y0(30);
  const DpValue divBy(2);
  DpPoint2 value0(x0, y0);

  const auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 / divBy, value2.X);
  EXPECT_EQ(y0 / divBy, value2.Y);
}
