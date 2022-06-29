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

#include <FslBase/Log/Math/Pixel/LogPxValueF.hpp>
#include <FslBase/Math/Pixel/PxValueF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPx_PxValueF = TestFixtureFslBase;
}


TEST(TestMathPx_PxValueF, Construct_Default)
{
  PxValueF value;

  EXPECT_EQ(PxValueF(), value);
  EXPECT_EQ(0, value.Value);
}

TEST(TestMathPx_PxValueF, Construct)
{
  constexpr const int32_t testValue = 3;
  PxValueF value(testValue);

  EXPECT_EQ(testValue, value.Value);
}

TEST(TestMathPx_PxValueF, Construct_NegativeValue)
{
  constexpr const int32_t testValue = -1;
  PxValueF value(testValue);

  EXPECT_EQ(testValue, value.Value);
}

TEST(TestMathPx_PxValueF, Op_UnarySub)
{
  EXPECT_TRUE(-PxValueF(2) == PxValueF(-2));
  EXPECT_TRUE(-PxValueF(-2) == PxValueF(2));
}

TEST(TestMathPx_PxValueF, Op_Equal)
{
  EXPECT_TRUE(PxValueF(0) == PxValueF(0));
  EXPECT_TRUE(PxValueF(1) == PxValueF(1));
  EXPECT_FALSE(PxValueF(2) == PxValueF(1));
}

TEST(TestMathPx_PxValueF, Op_NotEqual)
{
  EXPECT_FALSE(PxValueF(0) != PxValueF(0));
  EXPECT_FALSE(PxValueF(1) != PxValueF(1));
  EXPECT_TRUE(PxValueF(2) != PxValueF(1));
}

TEST(TestMathPx_PxValueF, Op_Less)
{
  EXPECT_FALSE(PxValueF(1) < PxValueF(0));
  EXPECT_FALSE(PxValueF(1) < PxValueF(1));
  EXPECT_TRUE(PxValueF(1) < PxValueF(2));
}

TEST(TestMathPx_PxValueF, Op_LessOrEqual)
{
  EXPECT_FALSE(PxValueF(1) <= PxValueF(0));
  EXPECT_TRUE(PxValueF(1) <= PxValueF(1));
  EXPECT_TRUE(PxValueF(1) <= PxValueF(2));
}

TEST(TestMathPx_PxValueF, Op_Greater)
{
  EXPECT_TRUE(PxValueF(1) > PxValueF(0));
  EXPECT_FALSE(PxValueF(1) > PxValueF(1));
  EXPECT_FALSE(PxValueF(1) > PxValueF(2));
}

TEST(TestMathPx_PxValueF, Op_GreaterOrEqual)
{
  EXPECT_TRUE(PxValueF(1) >= PxValueF(0));
  EXPECT_TRUE(PxValueF(1) >= PxValueF(1));
  EXPECT_FALSE(PxValueF(1) >= PxValueF(2));
}


TEST(TestMathPx_PxValueF, OpAdd)
{
  const PxValueF val1(10);
  const PxValueF val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathPx_PxValueF, OpAddAssignment)
{
  const PxValueF val1(10);
  const PxValueF val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueF, OpSub)
{
  const PxValueF val1(10);
  const PxValueF val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueF, OpSubAssignment)
{
  const PxValueF val1(10);
  const PxValueF val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueF, OpMul)
{
  const PxValueF val1(10);
  const PxValueF val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueF, OpMul_PxValueF_Constant)
//{
//   const PxValueF val1(10);
//   const PxValueF::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathPx_PxValueF, OpMul_Constant_PxValueF)
//{
//   const PxValueF::value_type val1 = 10;
//   const PxValueF val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathPx_PxValueF, OpMulAssignment)
{
  const PxValueF val1(10);
  const PxValueF val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueF, OpMulAssignment_Constant)
//{
//   const PxValueF val1(10);
//   const PxValueF::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathPx_PxValueF, OpDiv)
{
  const PxValueF val1(10);
  const PxValueF val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathPx_PxValueF, OpDiv_PxValueF_Constant)
//{
//   const PxValueF val1(10);
//   const PxValueF::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathPx_PxValueF, OpDivAssignment)
{
  const PxValueF val1(10);
  const PxValueF val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueF, OpDivAssignment_Constant)
//{
//   const PxValueF val1(10);
//   const PxValueF::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
