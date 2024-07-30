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

#include <FslBase/Log/Math/Dp/LogDpValueF.hpp>
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpValueF = TestFixtureFslBase;
}


TEST(TestMathDp_DpValueF, Construct_Default)
{
  DpValueF value;

  EXPECT_EQ(DpValueF(), value);
  EXPECT_EQ(0, value.Value);
}

TEST(TestMathDp_DpValueF, Construct)
{
  constexpr const int32_t TestValue = 3;
  DpValueF value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathDp_DpValueF, Construct_NegativeValue)
{
  constexpr const int32_t TestValue = -1;
  DpValueF value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathDp_DpValueF, Op_UnarySub)
{
  EXPECT_TRUE(-DpValueF(2) == DpValueF(-2));
  EXPECT_TRUE(-DpValueF(-2) == DpValueF(2));
}

TEST(TestMathDp_DpValueF, Op_Equal)
{
  EXPECT_TRUE(DpValueF(0) == DpValueF(0));
  EXPECT_TRUE(DpValueF(1) == DpValueF(1));
  EXPECT_FALSE(DpValueF(2) == DpValueF(1));
}

TEST(TestMathDp_DpValueF, Op_NotEqual)
{
  EXPECT_FALSE(DpValueF(0) != DpValueF(0));
  EXPECT_FALSE(DpValueF(1) != DpValueF(1));
  EXPECT_TRUE(DpValueF(2) != DpValueF(1));
}

TEST(TestMathDp_DpValueF, Op_Less)
{
  EXPECT_FALSE(DpValueF(1) < DpValueF(0));
  EXPECT_FALSE(DpValueF(1) < DpValueF(1));
  EXPECT_TRUE(DpValueF(1) < DpValueF(2));
}

TEST(TestMathDp_DpValueF, Op_LessOrEqual)
{
  EXPECT_FALSE(DpValueF(1) <= DpValueF(0));
  EXPECT_TRUE(DpValueF(1) <= DpValueF(1));
  EXPECT_TRUE(DpValueF(1) <= DpValueF(2));
}

TEST(TestMathDp_DpValueF, Op_Greater)
{
  EXPECT_TRUE(DpValueF(1) > DpValueF(0));
  EXPECT_FALSE(DpValueF(1) > DpValueF(1));
  EXPECT_FALSE(DpValueF(1) > DpValueF(2));
}

TEST(TestMathDp_DpValueF, Op_GreaterOrEqual)
{
  EXPECT_TRUE(DpValueF(1) >= DpValueF(0));
  EXPECT_TRUE(DpValueF(1) >= DpValueF(1));
  EXPECT_FALSE(DpValueF(1) >= DpValueF(2));
}


TEST(TestMathDp_DpValueF, OpAdd)
{
  const DpValueF val1(10);
  const DpValueF val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathDp_DpValueF, OpAddAssignment)
{
  const DpValueF val1(10);
  const DpValueF val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueF, OpSub)
{
  const DpValueF val1(10);
  const DpValueF val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueF, OpSubAssignment)
{
  const DpValueF val1(10);
  const DpValueF val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueF, OpMul)
{
  const DpValueF val1(10);
  const DpValueF val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueF, OpMul_DpValueF_Constant)
//{
//   const DpValueF val1(10);
//   const DpValueF::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathDp_DpValueF, OpMul_Constant_DpValueF)
//{
//   const DpValueF::value_type val1 = 10;
//   const DpValueF val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathDp_DpValueF, OpMulAssignment)
{
  const DpValueF val1(10);
  const DpValueF val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueF, OpMulAssignment_Constant)
//{
//   const DpValueF val1(10);
//   const DpValueF::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathDp_DpValueF, OpDiv)
{
  const DpValueF val1(10);
  const DpValueF val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathDp_DpValueF, OpDiv_DpValueF_Constant)
//{
//   const DpValueF val1(10);
//   const DpValueF::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathDp_DpValueF, OpDivAssignment)
{
  const DpValueF val1(10);
  const DpValueF val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueF, OpDivAssignment_Constant)
//{
//   const DpValueF val1(10);
//   const DpValueF::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
