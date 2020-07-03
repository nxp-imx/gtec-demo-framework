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

#include <FslBase/Math/Dp/DpPointF.hpp>
#include <FslBase/Log/Math/Dp/LogDpPointF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpPointF = TestFixtureFslBase;
}


TEST(TestMathDp_DpPointF, Zero)
{
  EXPECT_EQ(DpPointF(), DpPointF::Zero());
  EXPECT_EQ(0.0f, DpPointF::Zero().X);
  EXPECT_EQ(0.0f, DpPointF::Zero().Y);
}


TEST(TestMathDp_DpPointF, Construct_Default)
{
  DpPointF value;

  EXPECT_EQ(DpPointF(), value);
  EXPECT_EQ(0.0f, value.X);
  EXPECT_EQ(0.0f, value.Y);
}

TEST(TestMathDp_DpPointF, Construct)
{
  const float x = 3;
  const float y = 2;
  DpPointF value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathDp_DpPointF, OpPlusEqual)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpPointF, OpPlusEqual_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpPointF, OpSubEqual)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpPointF, OpSubEqual_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpPointF, OpMulEqual)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathDp_DpPointF, OpMulEqual_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}


TEST(TestMathDp_DpPointF, OpMulEqual_Const)
{
  const float x0 = 5;
  const float y0 = 4;
  DpPointF value0(x0, y0);

  const float mulBy = 2;
  value0 *= mulBy;

  EXPECT_EQ(x0 * mulBy, value0.X);
  EXPECT_EQ(y0 * mulBy, value0.Y);
}

TEST(TestMathDp_DpPointF, OpDivEqual_Const)
{
  const float x0 = 50;
  const float y0 = 40;
  DpPointF value0(x0, y0);

  const float divBy = 2;

  value0 /= divBy;

  EXPECT_EQ(x0 / divBy, value0.X);
  EXPECT_EQ(y0 / divBy, value0.Y);
}

TEST(TestMathDp_DpPointF, OpAdd)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpAdd_DpPointF_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpAdd_DpSizeF_DpPointF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpSizeF value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpSub)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpSub_DpPointF_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpSub_DpSizeF_DpPointF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpSizeF value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMathDp_DpPointF, OpMul)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpMul_DpPointF_DpSizeF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpSizeF value0(x0, y0);
  DpPointF value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpPointF, OpMul_DpSizeF_DpPointF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  DpPointF value0(x0, y0);
  DpSizeF value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathDp_DpPointF, OpMul_Const)
{
  const float x0 = 4;
  const float y0 = 3;
  const float mul = 2;
  DpPointF value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMathDp_DpPointF, OpMul_Const2)
{
  const float x0 = 4;
  const float y0 = 3;
  const float mul = 2;
  DpPointF value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}


TEST(TestMathDp_DpPointF, OpDiv_Const)
{
  const float x0 = 40;
  const float y0 = 30;
  const float divBy = 2;
  DpPointF value0(x0, y0);

  const auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 / divBy, value2.X);
  EXPECT_EQ(y0 / divBy, value2.Y);
}
