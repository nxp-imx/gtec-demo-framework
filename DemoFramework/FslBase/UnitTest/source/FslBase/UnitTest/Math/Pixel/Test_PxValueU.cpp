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

#include <FslBase/Log/Math/Pixel/LogPxValueU.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPx_PxValueU = TestFixtureFslBase;
}


TEST(TestMathPx_PxValueU, Construct_Default)
{
  PxValueU value;

  EXPECT_EQ(PxValueU(), value);
  EXPECT_EQ(0u, value.Value);
}

TEST(TestMathPx_PxValueU, Construct)
{
  constexpr const uint32_t TestValue = 3;
  PxValueU value(TestValue);

  EXPECT_EQ(TestValue, value.Value);
}

TEST(TestMathPx_PxValueU, Op_Equal)
{
  EXPECT_TRUE(PxValueU(0) == PxValueU(0));
  EXPECT_TRUE(PxValueU(1) == PxValueU(1));
  EXPECT_FALSE(PxValueU(2) == PxValueU(1));
}

TEST(TestMathPx_PxValueU, Op_NotEqual)
{
  EXPECT_FALSE(PxValueU(0) != PxValueU(0));
  EXPECT_FALSE(PxValueU(1) != PxValueU(1));
  EXPECT_TRUE(PxValueU(2) != PxValueU(1));
}

TEST(TestMathPx_PxValueU, Op_Less)
{
  EXPECT_FALSE(PxValueU(1) < PxValueU(0));
  EXPECT_FALSE(PxValueU(1) < PxValueU(1));
  EXPECT_TRUE(PxValueU(1) < PxValueU(2));
}

TEST(TestMathPx_PxValueU, Op_LessOrEqual)
{
  EXPECT_FALSE(PxValueU(1) <= PxValueU(0));
  EXPECT_TRUE(PxValueU(1) <= PxValueU(1));
  EXPECT_TRUE(PxValueU(1) <= PxValueU(2));
}

TEST(TestMathPx_PxValueU, Op_Greater)
{
  EXPECT_TRUE(PxValueU(1) > PxValueU(0));
  EXPECT_FALSE(PxValueU(1) > PxValueU(1));
  EXPECT_FALSE(PxValueU(1) > PxValueU(2));
}

TEST(TestMathPx_PxValueU, Op_GreaterOrEqual)
{
  EXPECT_TRUE(PxValueU(1) >= PxValueU(0));
  EXPECT_TRUE(PxValueU(1) >= PxValueU(1));
  EXPECT_FALSE(PxValueU(1) >= PxValueU(2));
}


TEST(TestMathPx_PxValueU, OpAdd)
{
  const PxValueU val1(10);
  const PxValueU val2(2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}

TEST(TestMathPx_PxValueU, OpAddAssignment)
{
  const PxValueU val1(10);
  const PxValueU val2(2);

  auto sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value + val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueU, OpSub)
{
  const PxValueU val1(10);
  const PxValueU val2(2);
  const auto sum = val1 - val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueU, OpSubAssignment)
{
  const PxValueU val1(10);
  const PxValueU val2(2);

  auto sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value - val2.Value, sum.Value);
}


TEST(TestMathPx_PxValueU, OpMul)
{
  const PxValueU val1(10);
  const PxValueU val2(2);
  const auto sum = val1 * val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueU, OpMul_PxValueU_Constant)
//{
//   const PxValueU val1(10);
//   const PxValueU::value_type val2 = 2;
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }
//
//
// TEST(TestMathPx_PxValueU, OpMul_Constant_PxValueU)
//{
//   const PxValueU::value_type val1 = 10;
//   const PxValueU val2(2);
//   const auto sum = val1 * val2;
//
//   EXPECT_EQ(val1 * val2.Value, sum.Value);
// }


TEST(TestMathPx_PxValueU, OpMulAssignment)
{
  const PxValueU val1(10);
  const PxValueU val2(2);

  auto sum = val1;
  sum *= val2;

  EXPECT_EQ(val1.Value * val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueU, OpMulAssignment_Constant)
//{
//   const PxValueU val1(10);
//   const PxValueU::value_type val2 = 2;
//
//   auto sum = val1;
//   sum *= val2;
//
//   EXPECT_EQ(val1.Value * val2, sum.Value);
// }


TEST(TestMathPx_PxValueU, OpDiv)
{
  const PxValueU val1(10);
  const PxValueU val2(2);
  const auto sum = val1 / val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}

// TEST(TestMathPx_PxValueU, OpDiv_PxValueU_Constant)
//{
//   const PxValueU val1(10);
//   const PxValueU::value_type val2 = 2;
//   const auto sum = val1 / val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }


TEST(TestMathPx_PxValueU, OpDivAssignment)
{
  const PxValueU val1(10);
  const PxValueU val2(2);

  auto sum = val1;
  sum /= val2;

  EXPECT_EQ(val1.Value / val2.Value, sum.Value);
}


// TEST(TestMathPx_PxValueU, OpDivAssignment_Constant)
//{
//   const PxValueU val1(10);
//   const PxValueU::value_type val2 = 2;
//
//   auto sum = val1;
//   sum /= val2;
//
//   EXPECT_EQ(val1.Value / val2, sum.Value);
// }
