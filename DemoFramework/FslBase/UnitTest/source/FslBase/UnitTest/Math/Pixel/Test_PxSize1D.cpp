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

#include <FslBase/Log/Math/Pixel/LogPxSize1D.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxSize1D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxSize1D, Construct_Default)
{
  PxSize1D value;

  EXPECT_EQ(PxSize1D(), value);
  EXPECT_EQ(PxValue::Create(0), value.Value());
}

TEST(TestMathPixel_PxSize1D, Construct)
{
  constexpr const auto value = PxValue::Create(3);
  PxSize1D testValue(value);

  EXPECT_EQ(value, testValue.Value());
}

TEST(TestMathPixel_PxSize1D, Construct_NegativeValue)
{
  constexpr const auto value = PxValue::Create(-1);
  PxSize1D testValue(value);

  EXPECT_EQ(PxValue::Create(0), testValue.Value());
}


// TEST(TestMathPixel_PxSize1D, SetValue)
//{
//   const auto newValue = PxValue::Create(100);
//   auto val1 = PxSize1D::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(newValue, val1.Value());
// }


// TEST(TestMathPixel_PxSize1D, SetValue_Negative)
//{
//   const auto newValue = PxValue::Create(-1);
//   auto val1 = PxSize1D::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(PxValue::Create(0), val1.Value());
// }

TEST(TestMathPixel_PxSize1D, Add_PxSize1D)
{
  const auto newValue = PxSize1D::Create(100);
  auto val1 = PxSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValue::Create(10) + newValue, val1.Value());
}

TEST(TestMathPixel_PxSize1D, Add_PxValue)
{
  const auto newValue = PxValue::Create(100);
  auto val1 = PxSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValue::Create(10) + newValue, val1.Value());
}


TEST(TestMathPixel_PxSize1D, Add_PxValue_Negative)
{
  const auto newValue = PxValue::Create(-5);
  auto val1 = PxSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValue::Create(10) + newValue, val1.Value());
}


TEST(TestMathPixel_PxSize1D, Add_PxValue_Negative_ZeroClamp)
{
  const auto newValue = PxValue::Create(-500);
  auto val1 = PxSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValue::Create(0), val1.Value());
}


TEST(TestMathPixel_PxSize1D, SetMax_Lower_PxSize1D)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxSize1D::Create(9);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1D, SetMax_Lower_PxValue)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxValue::Create(9);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1D, SetMax_Equal_PxSize1D)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxSize1D::Create(10);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1D, SetMax_Equal_PxValue)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxValue::Create(10);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1D, SetMax_Greater_PxSize1D)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxSize1D::Create(11);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathPixel_PxSize1D, SetMax_Greater_PxValue)
{
  const auto initialValue = PxSize1D::Create(10);
  const auto otherValue = PxValue::Create(11);
  PxSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathPixel_PxSize1D, Add_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxSize1D val3 = PxSize1D::Add(val1, val2);

  EXPECT_EQ(PxSize1D::Create(15), val3);
}

TEST(TestMathPixel_PxSize1D, Add_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxSize1D val3 = PxSize1D::Add(val1, val2);

  EXPECT_EQ(PxSize1D::Create(15), val3);
}

TEST(TestMathPixel_PxSize1D, Add_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(5);
  PxSize1D val3 = PxSize1D::Add(val1, val2);

  EXPECT_EQ(PxSize1D::Create(15), val3);
}


TEST(TestMathPixel_PxSize1D, Subtract_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxValue val3 = PxSize1D::Subtract(val1, val2);

  EXPECT_EQ(PxValue::Create(5), val3);
}


TEST(TestMathPixel_PxSize1D, Subtract_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxValue val3 = PxSize1D::Subtract(val1, val2);

  EXPECT_EQ(PxValue::Create(5), val3);
}


TEST(TestMathPixel_PxSize1D, Subtract_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(5);
  PxValue val3 = PxSize1D::Subtract(val1, val2);

  EXPECT_EQ(PxValue::Create(5), val3);
}


TEST(TestMathPixel_PxSize1D, Multiply_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxSize1D val3 = PxSize1D::Multiply(val1, val2);

  EXPECT_EQ(PxSize1D::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1D, Multiply_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxValue val3 = PxSize1D::Multiply(val1, val2);

  EXPECT_EQ(PxValue::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1D, Multiply_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(5);
  PxValue val3 = PxSize1D::Multiply(val1, val2);

  EXPECT_EQ(PxValue::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1D, Multiply_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-5);
  PxValue val3 = PxSize1D::Multiply(val1, val2);

  EXPECT_EQ((val1.Value() * val2), val3);
}


TEST(TestMathPixel_PxSize1D, Divide_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(5);
  PxSize1D val3 = PxSize1D::Divide(val1, val2);

  EXPECT_EQ(PxSize1D::Create(10 / 5), val3);
}


// TEST(TestMathPixel_PxSize1D, Divide_PxValue_PxSize1D)
//{
//   const auto val1 = PxValue::Create(10);
//   const auto val2 = PxSize1D::Create(5);
//   PxSize1D val3 = PxSize1D::Divide(val1, val2);
//
//   EXPECT_EQ(PxSize1D::Create(10 / 5), val3);
// }


TEST(TestMathPixel_PxSize1D, Divide_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(5);
  PxValue val3 = PxSize1D::Divide(val1, val2);

  EXPECT_EQ(PxValue::Create(10 / 5), val3);
}


TEST(TestMathPixel_PxSize1D, Divide_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-5);
  PxValue val3 = PxSize1D::Divide(val1, val2);

  EXPECT_EQ(val1.Value() / val2, val3);
}


TEST(TestMathPixel_PxSize1D, OpAddEqual_PxSize1D)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(1);
  PxSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAddEqual_PxValue)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(1);
  PxSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAddEqual_PxValue_Negative)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-1);
  PxSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpSubEqual_PxSize1D)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(1);
  PxSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpSubEqual_PxValue)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(1);
  PxSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpSubEqual_PxValue_Negative)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-1);
  PxSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpSubEqual_PxValue_Val2Largest)
{
  auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(100);
  PxSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(PxValue(0), sum.Value());
}


TEST(TestMathPixel_PxSize1D, Min_LhsLess_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsLess_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsLess_PxValue_Negative_PxSize1D)
{
  const auto lhs = PxValue::Create(-9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(PxSize1D(), PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsLess_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(9);
  const auto rhs = PxValue::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
}

TEST(TestMathPixel_PxSize1D, Min_LhsEqual_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsEqual_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(10);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsEqual_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsGreater_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxSize1D::Create(9);

  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsGreater_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(10);
  const auto rhs = PxSize1D::Create(9);

  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsGreater_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(9);

  EXPECT_EQ(rhs, PxSize1D::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Min_LhsGreater_PxSize1D_PxValue_Negative)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(-9);

  EXPECT_EQ(PxSize1D(), PxSize1D::Min(lhs, rhs));
}

TEST(TestMathPixel_PxSize1D, Max_LhsLess_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsLess_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsLess_PxValue_Negative_PxSize1D)
{
  const auto lhs = PxValue::Create(-9);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsLess_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(9);
  const auto rhs = PxValue::Create(10);

  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}

TEST(TestMathPixel_PxSize1D, Max_LhsEqual_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsEqual_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(10);
  const auto rhs = PxSize1D::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsEqual_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(10);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsGreater_PxSize1D_PxSize1D)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxSize1D::Create(9);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsGreater_PxValue_PxSize1D)
{
  const auto lhs = PxValue::Create(10);
  const auto rhs = PxSize1D::Create(9);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsGreater_PxSize1D_PxValue)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(9);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Max_LhsGreater_PxSize1D_PxValue_Negative)
{
  const auto lhs = PxSize1D::Create(10);
  const auto rhs = PxValue::Create(-9);

  EXPECT_EQ(lhs, PxSize1D::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1D, Op_Equal_PxSize1D_PxSize1D)
{
  EXPECT_TRUE(PxSize1D() == PxSize1D());
  EXPECT_TRUE(PxSize1D::Create(1) == PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) == PxSize1D::Create(1));
}


TEST(TestMathPixel_PxSize1D, Op_Equal_PxValue_PxSize1D)
{
  EXPECT_TRUE(PxValue() == PxSize1D());
  EXPECT_TRUE(PxValue::Create(1) == PxSize1D::Create(1));
  EXPECT_FALSE(PxValue::Create(2) == PxSize1D::Create(1));
}


TEST(TestMathPixel_PxSize1D, Op_Equal_PxSize1D_PxValue)
{
  EXPECT_TRUE(PxSize1D() == PxValue());
  EXPECT_TRUE(PxSize1D::Create(1) == PxValue::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) == PxValue::Create(1));
}

TEST(TestMathPixel_PxSize1D, Op_NotEqual_PxSize1D_PxSize1D)
{
  EXPECT_FALSE(PxSize1D() != PxSize1D());
  EXPECT_FALSE(PxSize1D::Create(1) != PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) != PxSize1D::Create(1));
}

TEST(TestMathPixel_PxSize1D, Op_NotEqual_PxValue_PxSize1D)
{
  EXPECT_FALSE(PxValue() != PxSize1D());
  EXPECT_FALSE(PxValue::Create(1) != PxSize1D::Create(1));
  EXPECT_TRUE(PxValue::Create(2) != PxSize1D::Create(1));
}

TEST(TestMathPixel_PxSize1D, Op_NotEqual_PxSize1D_PxValue)
{
  EXPECT_FALSE(PxSize1D() != PxValue());
  EXPECT_FALSE(PxSize1D::Create(1) != PxValue::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) != PxValue::Create(1));
}


TEST(TestMathPixel_PxSize1D, Op_Less_PxSize1D_PxSize1D)
{
  EXPECT_FALSE(PxSize1D() < PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D() < PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D() < PxSize1D::Create(2));

  EXPECT_FALSE(PxSize1D::Create(1) < PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D::Create(1) < PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D::Create(1) < PxSize1D::Create(2));

  EXPECT_FALSE(PxSize1D::Create(2) < PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D::Create(2) < PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) < PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_Less_PxValue_PxSize1D)
{
  EXPECT_FALSE(PxValue() < PxSize1D::Create(0));
  EXPECT_TRUE(PxValue() < PxSize1D::Create(1));
  EXPECT_TRUE(PxValue() < PxSize1D::Create(2));

  EXPECT_FALSE(PxValue::Create(1) < PxSize1D::Create(0));
  EXPECT_FALSE(PxValue::Create(1) < PxSize1D::Create(1));
  EXPECT_TRUE(PxValue::Create(1) < PxSize1D::Create(2));

  EXPECT_FALSE(PxValue::Create(2) < PxSize1D::Create(0));
  EXPECT_FALSE(PxValue::Create(2) < PxSize1D::Create(1));
  EXPECT_FALSE(PxValue::Create(2) < PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_Less_PxSize1D_PxValue)
{
  EXPECT_FALSE(PxSize1D() < PxValue::Create(0));
  EXPECT_TRUE(PxSize1D() < PxValue::Create(1));
  EXPECT_TRUE(PxSize1D() < PxValue::Create(2));

  EXPECT_FALSE(PxSize1D::Create(1) < PxValue::Create(0));
  EXPECT_FALSE(PxSize1D::Create(1) < PxValue::Create(1));
  EXPECT_TRUE(PxSize1D::Create(1) < PxValue::Create(2));

  EXPECT_FALSE(PxSize1D::Create(2) < PxValue::Create(0));
  EXPECT_FALSE(PxSize1D::Create(2) < PxValue::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) < PxValue::Create(2));
}


TEST(TestMathPixel_PxSize1D, OpLessOrEqual_PxSize1D_PxSize1D)
{
  EXPECT_TRUE(PxSize1D() <= PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D() <= PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D() <= PxSize1D::Create(2));

  EXPECT_FALSE(PxSize1D::Create(1) <= PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D::Create(1) <= PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D::Create(1) <= PxSize1D::Create(2));

  EXPECT_FALSE(PxSize1D::Create(2) <= PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D::Create(2) <= PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) <= PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_LessOrEqual_PxValue_PxSize1D)
{
  EXPECT_TRUE(PxValue() <= PxSize1D::Create(0));
  EXPECT_TRUE(PxValue() <= PxSize1D::Create(1));
  EXPECT_TRUE(PxValue() <= PxSize1D::Create(2));

  EXPECT_FALSE(PxValue::Create(1) <= PxSize1D::Create(0));
  EXPECT_TRUE(PxValue::Create(1) <= PxSize1D::Create(1));
  EXPECT_TRUE(PxValue::Create(1) <= PxSize1D::Create(2));

  EXPECT_FALSE(PxValue::Create(2) <= PxSize1D::Create(0));
  EXPECT_FALSE(PxValue::Create(2) <= PxSize1D::Create(1));
  EXPECT_TRUE(PxValue::Create(2) <= PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_LessOrEqual_PxSize1D_PxValue)
{
  EXPECT_TRUE(PxSize1D() <= PxValue::Create(0));
  EXPECT_TRUE(PxSize1D() <= PxValue::Create(1));
  EXPECT_TRUE(PxSize1D() <= PxValue::Create(2));

  EXPECT_FALSE(PxSize1D::Create(1) <= PxValue::Create(0));
  EXPECT_TRUE(PxSize1D::Create(1) <= PxValue::Create(1));
  EXPECT_TRUE(PxSize1D::Create(1) <= PxValue::Create(2));

  EXPECT_FALSE(PxSize1D::Create(2) <= PxValue::Create(0));
  EXPECT_FALSE(PxSize1D::Create(2) <= PxValue::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) <= PxValue::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_Greater_PxSize1D_PxSize1D)
{
  EXPECT_FALSE(PxSize1D() > PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D() > PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D() > PxSize1D::Create(2));

  EXPECT_TRUE(PxSize1D::Create(1) > PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D::Create(1) > PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D::Create(1) > PxSize1D::Create(2));

  EXPECT_TRUE(PxSize1D::Create(2) > PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D::Create(2) > PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) > PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_Greater_PxValue_PxSize1D)
{
  EXPECT_FALSE(PxValue() > PxSize1D::Create(0));
  EXPECT_FALSE(PxValue() > PxSize1D::Create(1));
  EXPECT_FALSE(PxValue() > PxSize1D::Create(2));

  EXPECT_TRUE(PxValue::Create(1) > PxSize1D::Create(0));
  EXPECT_FALSE(PxValue::Create(1) > PxSize1D::Create(1));
  EXPECT_FALSE(PxValue::Create(1) > PxSize1D::Create(2));

  EXPECT_TRUE(PxValue::Create(2) > PxSize1D::Create(0));
  EXPECT_TRUE(PxValue::Create(2) > PxSize1D::Create(1));
  EXPECT_FALSE(PxValue::Create(2) > PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_Greater_PxSize1D_PxValue)
{
  EXPECT_FALSE(PxSize1D() > PxValue::Create(0));
  EXPECT_FALSE(PxSize1D() > PxValue::Create(1));
  EXPECT_FALSE(PxSize1D() > PxValue::Create(2));

  EXPECT_TRUE(PxSize1D::Create(1) > PxValue::Create(0));
  EXPECT_FALSE(PxSize1D::Create(1) > PxValue::Create(1));
  EXPECT_FALSE(PxSize1D::Create(1) > PxValue::Create(2));

  EXPECT_TRUE(PxSize1D::Create(2) > PxValue::Create(0));
  EXPECT_TRUE(PxSize1D::Create(2) > PxValue::Create(1));
  EXPECT_FALSE(PxSize1D::Create(2) > PxValue::Create(2));
}


TEST(TestMathPixel_PxSize1D, OpGreaterOrEqual_PxSize1D_PxSize1D)
{
  EXPECT_TRUE(PxSize1D() >= PxSize1D::Create(0));
  EXPECT_FALSE(PxSize1D() >= PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D() >= PxSize1D::Create(2));

  EXPECT_TRUE(PxSize1D::Create(1) >= PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D::Create(1) >= PxSize1D::Create(1));
  EXPECT_FALSE(PxSize1D::Create(1) >= PxSize1D::Create(2));

  EXPECT_TRUE(PxSize1D::Create(2) >= PxSize1D::Create(0));
  EXPECT_TRUE(PxSize1D::Create(2) >= PxSize1D::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) >= PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_GreaterOrEqual_PxValue_PxSize1D)
{
  EXPECT_TRUE(PxValue() >= PxSize1D::Create(0));
  EXPECT_FALSE(PxValue() >= PxSize1D::Create(1));
  EXPECT_FALSE(PxValue() >= PxSize1D::Create(2));

  EXPECT_TRUE(PxValue::Create(1) >= PxSize1D::Create(0));
  EXPECT_TRUE(PxValue::Create(1) >= PxSize1D::Create(1));
  EXPECT_FALSE(PxValue::Create(1) >= PxSize1D::Create(2));

  EXPECT_TRUE(PxValue::Create(2) >= PxSize1D::Create(0));
  EXPECT_TRUE(PxValue::Create(2) >= PxSize1D::Create(1));
  EXPECT_TRUE(PxValue::Create(2) >= PxSize1D::Create(2));
}


TEST(TestMathPixel_PxSize1D, Op_GreaterOrEqual_PxSize1D_PxValue)
{
  EXPECT_TRUE(PxSize1D() >= PxValue::Create(0));
  EXPECT_FALSE(PxSize1D() >= PxValue::Create(1));
  EXPECT_FALSE(PxSize1D() >= PxValue::Create(2));

  EXPECT_TRUE(PxSize1D::Create(1) >= PxValue::Create(0));
  EXPECT_TRUE(PxSize1D::Create(1) >= PxValue::Create(1));
  EXPECT_FALSE(PxSize1D::Create(1) >= PxValue::Create(2));

  EXPECT_TRUE(PxSize1D::Create(2) >= PxValue::Create(0));
  EXPECT_TRUE(PxSize1D::Create(2) >= PxValue::Create(1));
  EXPECT_TRUE(PxSize1D::Create(2) >= PxValue::Create(2));
}


TEST(TestMathPixel_PxSize1D, OpAdd_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(1);
  const PxSize1D sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAdd_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(1);
  const PxSize1D sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAdd_PxValue_Negative_PxSize1D)
{
  const auto val1 = PxValue::Create(-10);
  const auto val2 = PxSize1D::Create(100);
  const PxSize1D sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAdd_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(1);
  const PxSize1D sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpAdd_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-1);
  const PxSize1D sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpSub_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(1);
  const PxValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(1);
  const PxValue sum = val1 - val2;

  EXPECT_EQ(val1 - val2.Value(), sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(1);
  const PxValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_PxValue_Negative_PxSize1D)
{
  const auto val1 = PxValue::Create(-10);
  const auto val2 = PxSize1D::Create(100);
  const PxValue sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-1);
  const PxValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_NegativeResult_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(11);
  const PxValue sum = val1 - val2;

  EXPECT_EQ((val1.Value() - val2.Value()), sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_NegativeResult_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(11);
  const PxValue sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathPixel_PxSize1D, OpSub_NegativeResult_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(11);
  const PxValue sum = val1 - val2;

  EXPECT_EQ((val1.Value() - val2), sum);
}


TEST(TestMathPixel_PxSize1D, OpMul_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(2);
  const PxSize1D sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1D, OpMul_PxValue_PxSize1D)
{
  const auto val1 = PxValue::Create(10);
  const auto val2 = PxSize1D::Create(2);
  const PxValue sum = val1 * val2;

  EXPECT_EQ(val1 * val2.Value(), sum);
}


TEST(TestMathPixel_PxSize1D, OpMul_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(2);
  const PxValue sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2, sum);
}


TEST(TestMathPixel_PxSize1D, OpMul_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-2);
  const PxValue sum = val1 * val2;

  EXPECT_EQ((val1.Value() * val2), sum);
}

TEST(TestMathPixel_PxSize1D, OpDiv_PxSize1D_PxSize1D)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxSize1D::Create(2);
  const PxSize1D sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2.Value(), sum.Value());
}

TEST(TestMathPixel_PxSize1D, OpDiv_PxSize1D_PxValue)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(2);
  const PxValue sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2, sum);
}

TEST(TestMathPixel_PxSize1D, OpDiv_PxSize1D_PxValue_Negative)
{
  const auto val1 = PxSize1D::Create(10);
  const auto val2 = PxValue::Create(-2);
  const PxValue sum = val1 / val2;

  EXPECT_EQ((val1.Value() / val2), sum);
}
