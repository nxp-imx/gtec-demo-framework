/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxValue.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPx_PxValue = TestFixtureFslBase;
}


TEST(TestMathPx_PxValue, Construct_Default)
{
  PxValue value;

  EXPECT_EQ(PxValue(), value);
  EXPECT_EQ(0, value.Value);
}

TEST(TestMathPx_PxValue, Construct)
{
  constexpr const int32_t TestValue = 3;
  PxValue value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathPx_PxValue, Construct_NegativeValue)
{
  constexpr const int32_t TestValue = -1;
  PxValue value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathPx_PxValue, Op_UnarySub)
{
  EXPECT_TRUE(-PxValue(2) == PxValue(-2));
  EXPECT_TRUE(-PxValue(-2) == PxValue(2));
}

TEST(TestMathPx_PxValue, Op_Equal)
{
  EXPECT_TRUE(PxValue(0) == PxValue(0));
  EXPECT_TRUE(PxValue(1) == PxValue(1));
  EXPECT_FALSE(PxValue(2) == PxValue(1));
}

TEST(TestMathPx_PxValue, Op_NotEqual)
{
  EXPECT_FALSE(PxValue(0) != PxValue(0));
  EXPECT_FALSE(PxValue(1) != PxValue(1));
  EXPECT_TRUE(PxValue(2) != PxValue(1));
}

TEST(TestMathPx_PxValue, Op_Less)
{
  EXPECT_FALSE(PxValue(1) < PxValue(0));
  EXPECT_FALSE(PxValue(1) < PxValue(1));
  EXPECT_TRUE(PxValue(1) < PxValue(2));
}

TEST(TestMathPx_PxValue, Op_LessOrEqual)
{
  EXPECT_FALSE(PxValue(1) <= PxValue(0));
  EXPECT_TRUE(PxValue(1) <= PxValue(1));
  EXPECT_TRUE(PxValue(1) <= PxValue(2));
}

TEST(TestMathPx_PxValue, Op_Greater)
{
  EXPECT_TRUE(PxValue(1) > PxValue(0));
  EXPECT_FALSE(PxValue(1) > PxValue(1));
  EXPECT_FALSE(PxValue(1) > PxValue(2));
}

TEST(TestMathPx_PxValue, Op_GreaterOrEqual)
{
  EXPECT_TRUE(PxValue(1) >= PxValue(0));
  EXPECT_TRUE(PxValue(1) >= PxValue(1));
  EXPECT_FALSE(PxValue(1) >= PxValue(2));
}


TEST(TestMathPx_PxValue, OpAdd)
{
  const PxValue val1(10);
  const PxValue val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathPx_PxValue, OpAddAssignment)
{
  const PxValue val1(10);
  const PxValue val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathPx_PxValue, OpSub)
{
  const PxValue val1(10);
  const PxValue val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValue, OpSubAssignment)
{
  const PxValue val1(10);
  const PxValue val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValue, OpMul)
{
  const PxValue val1(10);
  const PxValue val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValue, OpMul_PxValue_Constant)
//{
//   const PxValue val1(10);
//   const PxValue::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathPx_PxValue, OpMul_Constant_PxValue)
//{
//   const PxValue::value_type val1 = 10;
//   const PxValue val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathPx_PxValue, OpMulAssignment)
{
  const PxValue val1(10);
  const PxValue val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValue, OpMulAssignment_Constant)
//{
//   const PxValue val1(10);
//   const PxValue::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathPx_PxValue, OpDiv)
{
  const PxValue val1(10);
  const PxValue val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathPx_PxValue, OpDiv_PxValue_Constant)
//{
//   const PxValue val1(10);
//   const PxValue::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathPx_PxValue, OpDivAssignment)
{
  const PxValue val1(10);
  const PxValue val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValue, OpDivAssignment_Constant)
//{
//   const PxValue val1(10);
//   const PxValue::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
