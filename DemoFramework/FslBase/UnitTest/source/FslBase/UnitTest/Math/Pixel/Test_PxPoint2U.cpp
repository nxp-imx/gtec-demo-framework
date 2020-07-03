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

#include <FslBase/Math/Pixel/PxPoint2U.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2U.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxPoint2U = TestFixtureFslBase;
}


TEST(TestMathPixel_PxPoint2U, Zero)
{
  EXPECT_EQ(PxPoint2U(), PxPoint2U::Zero());
  EXPECT_EQ(0u, PxPoint2U::Zero().X);
  EXPECT_EQ(0u, PxPoint2U::Zero().Y);
}


TEST(TestMathPixel_PxPoint2U, Construct_Default)
{
  PxPoint2U value;

  EXPECT_EQ(PxPoint2U(), value);
  EXPECT_EQ(0u, value.X);
  EXPECT_EQ(0u, value.Y);
}

TEST(TestMathPixel_PxPoint2U, Construct)
{
  const uint32_t x = 3;
  const uint32_t y = 2;
  PxPoint2U value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}

TEST(TestMathPixel_PxPoint2U, OpPlusEqual)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 3;
  const uint32_t y1 = 2;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}


TEST(TestMathPixel_PxPoint2U, OpSubEqual)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 30;
  const uint32_t y1 = 20;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathPixel_PxPoint2U, OpMulEqual)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 3;
  const uint32_t y1 = 2;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathPixel_PxPoint2U, OpMulEqual_Const)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  PxPoint2U value0(x0, y0);

  const uint32_t mulBy = 2;
  value0 *= mulBy;

  EXPECT_EQ(x0 * mulBy, value0.X);
  EXPECT_EQ(y0 * mulBy, value0.Y);
}

TEST(TestMathPixel_PxPoint2U, OpDivEqual_Const)
{
  const uint32_t x0 = 50;
  const uint32_t y0 = 40;
  PxPoint2U value0(x0, y0);

  const uint32_t divBy = 2;
  value0 /= divBy;

  EXPECT_EQ(x0 / divBy, value0.X);
  EXPECT_EQ(y0 / divBy, value0.Y);
}

TEST(TestMathPixel_PxPoint2U, OpAdd)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 3;
  const uint32_t y1 = 2;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}


TEST(TestMathPixel_PxPoint2U, OpSub)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 30;
  const uint32_t y1 = 20;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMathPixel_PxPoint2U, OpMul)
{
  const uint32_t x0 = 5;
  const uint32_t y0 = 4;
  const uint32_t x1 = 3;
  const uint32_t y1 = 2;
  PxPoint2U value0(x0, y0);
  PxPoint2U value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathPixel_PxPoint2U, OpMul_Const)
{
  const uint32_t x0 = 4;
  const uint32_t y0 = 3;
  const uint32_t mul = 2;
  PxPoint2U value0(x0, y0);

  auto value2 = value0 * mul;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 * mul, value2.X);
  EXPECT_EQ(y0 * mul, value2.Y);
}


TEST(TestMathPixel_PxPoint2U, OpMul_Const2)
{
  const uint32_t x0 = 4;
  const uint32_t y0 = 3;
  const uint32_t mul = 2;
  PxPoint2U value0(x0, y0);

  auto value2 = mul * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(mul * x0, value2.X);
  EXPECT_EQ(mul * y0, value2.Y);
}


TEST(TestMathPixel_PxPoint2U, OpDiv_Const)
{
  const uint32_t x0 = 40;
  const uint32_t y0 = 30;
  const uint32_t divBy = 2;
  PxPoint2U value0(x0, y0);

  auto value2 = value0 / divBy;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x0 / divBy, value2.X);
  EXPECT_EQ(y0 / divBy, value2.Y);
}
