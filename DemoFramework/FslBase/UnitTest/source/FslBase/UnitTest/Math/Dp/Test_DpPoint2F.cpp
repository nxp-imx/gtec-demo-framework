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

#include <FslBase/Log/Math/Dp/LogDpPoint2F.hpp>
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpPoint2F = TestFixtureFslBase;
}


TEST(TestMathDp_DpPoint2F, Zero)
{
  EXPECT_EQ(DpPoint2F(), DpPoint2F::Zero());
  EXPECT_EQ(DpPoint2F::value_type(0.0f), DpPoint2F::Zero().X);
  EXPECT_EQ(DpPoint2F::value_type(0.0f), DpPoint2F::Zero().Y);
}


TEST(TestMathDp_DpPoint2F, Construct_Default)
{
  DpPoint2F value;

  EXPECT_EQ(DpPoint2F(), value);
  EXPECT_EQ(DpPoint2F::value_type(0.0f), value.X);
  EXPECT_EQ(DpPoint2F::value_type(0.0f), value.Y);
}

TEST(TestMathDp_DpPoint2F, Construct)
{
  const DpValueF x(3);
  const DpValueF y(2);
  DpPoint2F value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathDp_DpPoint2F, OpPlusEqual)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}


TEST(TestMathDp_DpPoint2F, OpSubEqual)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}


TEST(TestMathDp_DpPoint2F, OpMulEqual)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}


TEST(TestMathDp_DpPoint2F, OpMulEqual_Const)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  DpPoint2F value0(x0, y0);

  const DpValueF mulBy(2);
  value0 *= mulBy;

  EXPECT_EQ(x0 * mulBy, value0.X);
  EXPECT_EQ(y0 * mulBy, value0.Y);
}

TEST(TestMathDp_DpPoint2F, OpDivEqual_Const)
{
  const DpValueF x0(50);
  const DpValueF y0(40);
  DpPoint2F value0(x0, y0);

  const DpValueF divBy(2);

  value0 /= divBy;

  EXPECT_EQ(x0 / divBy, value0.X);
  EXPECT_EQ(y0 / divBy, value0.Y);
}

TEST(TestMathDp_DpPoint2F, OpAdd)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}


TEST(TestMathDp_DpPoint2F, OpSub)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMathDp_DpPoint2F, OpMul)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathDp_DpPoint2F, OpMul_Const)
{
  const DpValueF x0(4);
  const DpValueF y0(3);
  const DpValueF mul(2);
  DpPoint2F value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMathDp_DpPoint2F, OpMul_Const2)
{
  const DpValueF x0(4);
  const DpValueF y0(3);
  const DpValueF mul(2);
  DpPoint2F value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}


TEST(TestMathDp_DpPoint2F, OpDiv_Const)
{
  const DpValueF x0(40);
  const DpValueF y0(30);
  const DpValueF divBy(2);
  DpPoint2F value0(x0, y0);

  const auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 / divBy, value2.X);
  EXPECT_EQ(y0 / divBy, value2.Y);
}
