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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMath_Point2 = TestFixtureFslBase;
}


TEST(TestMath_Point2, Zero)
{
  EXPECT_EQ(Point2(), Point2::Zero());
  EXPECT_EQ(0, Point2::Zero().X);
  EXPECT_EQ(0, Point2::Zero().Y);
}


TEST(TestMath_Point2, Construct_Default)
{
  Point2 value;

  EXPECT_EQ(Point2(), value);
  EXPECT_EQ(0, value.X);
  EXPECT_EQ(0, value.Y);
}

TEST(TestMath_Point2, Construct)
{
  const int32_t x = 3;
  const int32_t y = 2;
  Point2 value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMath_Point2, OpPlusEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}


TEST(TestMath_Point2, OpSubEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMath_Point2, OpMulEqual)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}


TEST(TestMath_Point2, OpAdd)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}


TEST(TestMath_Point2, OpSub)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMath_Point2, OpMul)
{
  const int32_t x0 = 5;
  const int32_t y0 = 4;
  const int32_t x1 = 3;
  const int32_t y1 = 2;
  Point2 value0(x0, y0);
  Point2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMath_Point2, OpMul_Const)
{
  const int32_t x0 = 4;
  const int32_t y0 = 3;
  const int32_t mul = 2;
  Point2 value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMath_Point2, OpMul_Const2)
{
  const int32_t x0 = 4;
  const int32_t y0 = 3;
  const int32_t mul = 2;
  Point2 value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}
