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

#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpPoint = TestFixtureFslBase;
}


TEST(TestMathDp_DpPoint, Zero)
{
  EXPECT_EQ(DpPoint(), DpPoint::Zero());
  EXPECT_EQ(0, DpPoint::Zero().X);
  EXPECT_EQ(0, DpPoint::Zero().Y);
}


TEST(TestMathDp_DpPoint, Construct_Default)
{
  DpPoint value;

  EXPECT_EQ(DpPoint(), value);
  EXPECT_EQ(0, value.X);
  EXPECT_EQ(0, value.Y);
}

TEST(TestMathDp_DpPoint, Construct)
{
  const int32_t x = 3;
  const int32_t y = 2;
  DpPoint value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathDp_DpPoint, OpPlusEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpPoint, OpPlusEqual_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpPoint, OpSubEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpPoint, OpSubEqual_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpPoint, OpMulEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathDp_DpPoint, OpMulEqual_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}


TEST(TestMathDp_DpPoint, OpMulEqual_Const)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  DpPoint value0(x0, y0);

  const int32_t mulBy = 2;
  value0 *= mulBy;

  EXPECT_EQ(x0 * mulBy, value0.X);
  EXPECT_EQ(y0 * mulBy, value0.Y);
}

TEST(TestMathDp_DpPoint, OpDivEqual_Const)
{
  const int32_t x0 = 50;
  const int32_t y0 = 40;
  DpPoint value0(x0, y0);

  const int32_t divBy = 2;

  value0 /= divBy;

  EXPECT_EQ(x0 / divBy, value0.X);
  EXPECT_EQ(y0 / divBy, value0.Y);
}

TEST(TestMathDp_DpPoint, OpAdd)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpAdd_DpPoint_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpAdd_DpSize_DpPoint)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpSize value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpSub)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpSub_DpPoint_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpSub_DpSize_DpPoint)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpSize value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMathDp_DpPoint, OpMul)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpMul_DpPoint_DpSize)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpSize value0(x0, y0);
  DpPoint value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpPoint, OpMul_DpSize_DpPoint)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  DpPoint value0(x0, y0);
  DpSize value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathDp_DpPoint, OpMul_Const)
{
  const int32_t x0 = 4;
  const int32_t y0 = 3;
  const int32_t mul = 2;
  DpPoint value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMathDp_DpPoint, OpMul_Const2)
{
  const int32_t x0 = 4;
  const int32_t y0 = 3;
  const int32_t mul = 2;
  DpPoint value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}


TEST(TestMathDp_DpPoint, OpDiv_Const)
{
  const int32_t x0 = 40;
  const int32_t y0 = 30;
  const int32_t divBy = 2;
  DpPoint value0(x0, y0);

  const auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 / divBy, value2.X);
  EXPECT_EQ(y0 / divBy, value2.Y);
}
