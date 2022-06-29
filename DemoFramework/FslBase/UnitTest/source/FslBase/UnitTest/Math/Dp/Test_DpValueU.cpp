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

#include <FslBase/Log/Math/Dp/LogDpValueU.hpp>
#include <FslBase/Math/Dp/DpValueU.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpValueU = TestFixtureFslBase;
}


TEST(TestMathDp_DpValueU, Construct_Default)
{
  DpValueU value;

  EXPECT_EQ(DpValueU(), value);
  EXPECT_EQ(0, value.Value);
}

TEST(TestMathDp_DpValueU, Construct)
{
  constexpr const uint32_t testValue = 3;
  DpValueU value(testValue);

  EXPECT_EQ(testValue, value.Value);
}

TEST(TestMathDp_DpValueU, Op_Equal)
{
  EXPECT_TRUE(DpValueU(0) == DpValueU(0));
  EXPECT_TRUE(DpValueU(1) == DpValueU(1));
  EXPECT_FALSE(DpValueU(2) == DpValueU(1));
}

TEST(TestMathDp_DpValueU, Op_NotEqual)
{
  EXPECT_FALSE(DpValueU(0) != DpValueU(0));
  EXPECT_FALSE(DpValueU(1) != DpValueU(1));
  EXPECT_TRUE(DpValueU(2) != DpValueU(1));
}

TEST(TestMathDp_DpValueU, Op_Less)
{
  EXPECT_FALSE(DpValueU(1) < DpValueU(0));
  EXPECT_FALSE(DpValueU(1) < DpValueU(1));
  EXPECT_TRUE(DpValueU(1) < DpValueU(2));
}

TEST(TestMathDp_DpValueU, Op_LessOrEqual)
{
  EXPECT_FALSE(DpValueU(1) <= DpValueU(0));
  EXPECT_TRUE(DpValueU(1) <= DpValueU(1));
  EXPECT_TRUE(DpValueU(1) <= DpValueU(2));
}

TEST(TestMathDp_DpValueU, Op_Greater)
{
  EXPECT_TRUE(DpValueU(1) > DpValueU(0));
  EXPECT_FALSE(DpValueU(1) > DpValueU(1));
  EXPECT_FALSE(DpValueU(1) > DpValueU(2));
}

TEST(TestMathDp_DpValueU, Op_GreaterOrEqual)
{
  EXPECT_TRUE(DpValueU(1) >= DpValueU(0));
  EXPECT_TRUE(DpValueU(1) >= DpValueU(1));
  EXPECT_FALSE(DpValueU(1) >= DpValueU(2));
}


TEST(TestMathDp_DpValueU, OpAdd)
{
  const DpValueU val1(10);
  const DpValueU val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathDp_DpValueU, OpAddAssignment)
{
  const DpValueU val1(10);
  const DpValueU val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueU, OpSub)
{
  const DpValueU val1(10);
  const DpValueU val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueU, OpSubAssignment)
{
  const DpValueU val1(10);
  const DpValueU val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathDp_DpValueU, OpMul)
{
  const DpValueU val1(10);
  const DpValueU val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueU, OpMul_DpValueU_Constant)
//{
//   const DpValueU val1(10);
//   const DpValueU::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathDp_DpValueU, OpMul_Constant_DpValueU)
//{
//   const DpValueU::value_type val1 = 10;
//   const DpValueU val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathDp_DpValueU, OpMulAssignment)
{
  const DpValueU val1(10);
  const DpValueU val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueU, OpMulAssignment_Constant)
//{
//   const DpValueU val1(10);
//   const DpValueU::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathDp_DpValueU, OpDiv)
{
  const DpValueU val1(10);
  const DpValueU val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathDp_DpValueU, OpDiv_DpValueU_Constant)
//{
//   const DpValueU val1(10);
//   const DpValueU::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathDp_DpValueU, OpDivAssignment)
{
  const DpValueU val1(10);
  const DpValueU val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathDp_DpValueU, OpDivAssignment_Constant)
//{
//   const DpValueU val1(10);
//   const DpValueU::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
