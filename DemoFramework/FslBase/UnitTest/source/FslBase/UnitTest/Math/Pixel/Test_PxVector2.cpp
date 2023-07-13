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

#include <FslBase/Log/Math/Pixel/LogPxVector2.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxVector2 = TestFixtureFslBase;
}


TEST(TestMathPixel_PxVector2, Zero)
{
  EXPECT_EQ(PxVector2(), PxVector2::Zero());
  EXPECT_EQ(0, PxVector2::Zero().X.Value);
  EXPECT_EQ(0, PxVector2::Zero().Y.Value);
}


TEST(TestMathPixel_PxVector2, Construct_Default)
{
  PxVector2 value;

  EXPECT_EQ(PxVector2(), value);
  EXPECT_EQ(0, value.X.Value);
  EXPECT_EQ(0, value.Y.Value);
}

TEST(TestMathPixel_PxVector2, Construct)
{
  const PxValueF x(3);
  const PxValueF y(2);
  PxVector2 value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathPixel_PxVector2, OpPlusEqual)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(x1.Value + x0.Value, value1.X.Value);
  EXPECT_FLOAT_EQ(y1.Value + y0.Value, value1.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpSubEqual)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(x1.Value - x0.Value, value1.X.Value);
  EXPECT_FLOAT_EQ(y1.Value - y0.Value, value1.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpMulEqual)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(x1.Value * x0.Value, value1.X.Value);
  EXPECT_FLOAT_EQ(y1.Value * y0.Value, value1.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpMulEqual_Const)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  PxVector2 value0(x0, y0);

  const PxValueF mulBy(2);
  value0 *= mulBy;

  EXPECT_FLOAT_EQ(x0.Value * mulBy.Value, value0.X.Value);
  EXPECT_FLOAT_EQ(y0.Value * mulBy.Value, value0.Y.Value);
}

TEST(TestMathPixel_PxVector2, OpDivEqual_Const)
{
  const PxValueF x0(50);
  const PxValueF y0(40);
  PxVector2 value0(x0, y0);

  const PxValueF divBy(2);

  value0 /= divBy;

  EXPECT_FLOAT_EQ(x0.Value / divBy.Value, value0.X.Value);
  EXPECT_FLOAT_EQ(y0.Value / divBy.Value, value0.Y.Value);
}

TEST(TestMathPixel_PxVector2, OpAdd)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1.Value + x0.Value, value3.X.Value);
  EXPECT_FLOAT_EQ(y1.Value + y0.Value, value3.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpSub)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1.Value - x0.Value, value3.X.Value);
  EXPECT_FLOAT_EQ(y1.Value - y0.Value, value3.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpMul)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF x1(3);
  const PxValueF y1(2);
  PxVector2 value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1.Value * x0.Value, value3.X.Value);
  EXPECT_FLOAT_EQ(y1.Value * y0.Value, value3.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpMul_Const)
{
  const PxValueF x0(5);
  const PxValueF y0(4);
  const PxValueF mul(2);
  PxVector2 value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(x0.Value * mul.Value, value2.X.Value);
  EXPECT_FLOAT_EQ(y0.Value * mul.Value, value2.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpMul_Const2)
{
  const PxValueF x0(4);
  const PxValueF y0(3);
  const PxValueF mul(2);
  PxVector2 value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(mul.Value * x0.Value, value2.X.Value);
  EXPECT_FLOAT_EQ(mul.Value * y0.Value, value2.Y.Value);
}


TEST(TestMathPixel_PxVector2, OpDiv_Const)
{
  const PxValueF x0(40);
  const PxValueF y0(30);
  const PxValueF divBy(2);
  PxVector2 value0(x0, y0);

  const auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_FLOAT_EQ(x0.Value / divBy.Value, value2.X.Value);
  EXPECT_FLOAT_EQ(y0.Value / divBy.Value, value2.Y.Value);
}
