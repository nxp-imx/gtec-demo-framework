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

#include <FslBase/Log/Math/Dp/LogDpValue.hpp>
#include <FslBase/Math/Dp/DpValue.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpValue = TestFixtureFslBase;
}


TEST(TestMathDp_DpValue, Construct_Default)
{
  DpValue value;

  EXPECT_EQ(DpValue(), value);
  EXPECT_EQ(0, value.Value);
}

TEST(TestMathDp_DpValue, Construct)
{
  constexpr const int32_t TestValue = 3;
  DpValue value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathDp_DpValue, Construct_NegativeValue)
{
  constexpr const int32_t TestValue = -1;
  DpValue value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathDp_DpValue, Op_UnarySub)
{
  EXPECT_TRUE(-DpValue(2) == DpValue(-2));
  EXPECT_TRUE(-DpValue(-2) == DpValue(2));
}

TEST(TestMathDp_DpValue, Op_Equal)
{
  EXPECT_TRUE(DpValue(0) == DpValue(0));
  EXPECT_TRUE(DpValue(1) == DpValue(1));
  EXPECT_FALSE(DpValue(2) == DpValue(1));
}

TEST(TestMathDp_DpValue, Op_NotEqual)
{
  EXPECT_FALSE(DpValue(0) != DpValue(0));
  EXPECT_FALSE(DpValue(1) != DpValue(1));
  EXPECT_TRUE(DpValue(2) != DpValue(1));
}

TEST(TestMathDp_DpValue, Op_Less)
{
  EXPECT_FALSE(DpValue(1) < DpValue(0));
  EXPECT_FALSE(DpValue(1) < DpValue(1));
  EXPECT_TRUE(DpValue(1) < DpValue(2));
}

TEST(TestMathDp_DpValue, Op_LessOrEqual)
{
  EXPECT_FALSE(DpValue(1) <= DpValue(0));
  EXPECT_TRUE(DpValue(1) <= DpValue(1));
  EXPECT_TRUE(DpValue(1) <= DpValue(2));
}

TEST(TestMathDp_DpValue, Op_Greater)
{
  EXPECT_TRUE(DpValue(1) > DpValue(0));
  EXPECT_FALSE(DpValue(1) > DpValue(1));
  EXPECT_FALSE(DpValue(1) > DpValue(2));
}

TEST(TestMathDp_DpValue, Op_GreaterOrEqual)
{
  EXPECT_TRUE(DpValue(1) >= DpValue(0));
  EXPECT_TRUE(DpValue(1) >= DpValue(1));
  EXPECT_FALSE(DpValue(1) >= DpValue(2));
}


TEST(TestMathDp_DpValue, OpAdd)
{
  const DpValue val1(10);
  const DpValue val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathDp_DpValue, OpAddAssignment)
{
  const DpValue val1(10);
  const DpValue val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathDp_DpValue, OpSub)
{
  const DpValue val1(10);
  const DpValue val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValue, OpSubAssignment)
{
  const DpValue val1(10);
  const DpValue val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValue, OpMul)
{
  const DpValue val1(10);
  const DpValue val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValue, OpMul_DpValue_Constant)
//{
//   const DpValue val1(10);
//   const DpValue::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathDp_DpValue, OpMul_Constant_DpValue)
//{
//   const DpValue::value_type val1 = 10;
//   const DpValue val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathDp_DpValue, OpMulAssignment)
{
  const DpValue val1(10);
  const DpValue val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValue, OpMulAssignment_Constant)
//{
//   const DpValue val1(10);
//   const DpValue::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathDp_DpValue, OpDiv)
{
  const DpValue val1(10);
  const DpValue val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathDp_DpValue, OpDiv_DpValue_Constant)
//{
//   const DpValue val1(10);
//   const DpValue::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathDp_DpValue, OpDivAssignment)
{
  const DpValue val1(10);
  const DpValue val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValue, OpDivAssignment_Constant)
//{
//   const DpValue val1(10);
//   const DpValue::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
