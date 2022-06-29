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

#include <FslBase/Log/Math/Dp/LogDpSize1D.hpp>
#include <FslBase/Math/Dp/DpSize1D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSize1D = TestFixtureFslBase;
}


TEST(TestMathDp_DpSize1D, Construct_Default)
{
  DpSize1D value;

  EXPECT_EQ(DpSize1D(), value);
  EXPECT_EQ(DpValue::Create(0), value.Value());
}

TEST(TestMathDp_DpSize1D, Construct)
{
  constexpr const auto value = DpValue::Create(3);
  DpSize1D testValue(value);

  EXPECT_EQ(value, testValue.Value());
}

TEST(TestMathDp_DpSize1D, Construct_NegativeValue)
{
  constexpr const auto value = DpValue::Create(-1);
  DpSize1D testValue(value);

  EXPECT_EQ(DpValue::Create(0), testValue.Value());
}


// TEST(TestMathDp_DpSize1D, SetValue)
//{
//   const auto newValue = DpValue::Create(100);
//   auto val1 = DpSize1D::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(newValue, val1.Value());
// }


// TEST(TestMathDp_DpSize1D, SetValue_Negative)
//{
//   const auto newValue = DpValue::Create(-1);
//   auto val1 = DpSize1D::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(DpValue::Create(0), val1.Value());
// }

TEST(TestMathDp_DpSize1D, Add_DpSize1D)
{
  const auto newValue = DpSize1D::Create(100);
  auto val1 = DpSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValue::Create(10) + newValue, val1.Value());
}

TEST(TestMathDp_DpSize1D, Add_DpValue)
{
  const auto newValue = DpValue::Create(100);
  auto val1 = DpSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValue::Create(10) + newValue, val1.Value());
}


TEST(TestMathDp_DpSize1D, Add_DpValue_Negative)
{
  const auto newValue = DpValue::Create(-5);
  auto val1 = DpSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValue::Create(10) + newValue, val1.Value());
}


TEST(TestMathDp_DpSize1D, Add_DpValue_Negative_ZeroClamp)
{
  const auto newValue = DpValue::Create(-500);
  auto val1 = DpSize1D::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValue::Create(0), val1.Value());
}


TEST(TestMathDp_DpSize1D, SetMax_Lower_DpSize1D)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpSize1D::Create(9);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1D, SetMax_Lower_DpValue)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpValue::Create(9);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1D, SetMax_Equal_DpSize1D)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpSize1D::Create(10);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1D, SetMax_Equal_DpValue)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpValue::Create(10);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1D, SetMax_Greater_DpSize1D)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpSize1D::Create(11);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathDp_DpSize1D, SetMax_Greater_DpValue)
{
  const auto initialValue = DpSize1D::Create(10);
  const auto otherValue = DpValue::Create(11);
  DpSize1D testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathDp_DpSize1D, Add_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpSize1D val3 = DpSize1D::Add(val1, val2);

  EXPECT_EQ(DpSize1D::Create(15), val3);
}

TEST(TestMathDp_DpSize1D, Add_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpValue val3 = DpSize1D::Add(val1, val2);

  EXPECT_EQ(DpValue::Create(15), val3);
}

TEST(TestMathDp_DpSize1D, Add_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(5);
  DpValue val3 = DpSize1D::Add(val1, val2);

  EXPECT_EQ(DpValue::Create(15), val3);
}


TEST(TestMathDp_DpSize1D, Subtract_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpValue val3 = DpSize1D::Subtract(val1, val2);

  EXPECT_EQ(DpValue::Create(5), val3);
}


TEST(TestMathDp_DpSize1D, Subtract_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpValue val3 = DpSize1D::Subtract(val1, val2);

  EXPECT_EQ(DpValue::Create(5), val3);
}


TEST(TestMathDp_DpSize1D, Subtract_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(5);
  DpValue val3 = DpSize1D::Subtract(val1, val2);

  EXPECT_EQ(DpValue::Create(5), val3);
}


TEST(TestMathDp_DpSize1D, Multiply_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpSize1D val3 = DpSize1D::Multiply(val1, val2);

  EXPECT_EQ(DpSize1D::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1D, Multiply_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpValue val3 = DpSize1D::Multiply(val1, val2);

  EXPECT_EQ(DpValue::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1D, Multiply_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(5);
  DpValue val3 = DpSize1D::Multiply(val1, val2);

  EXPECT_EQ(DpValue::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1D, Multiply_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-5);
  DpValue val3 = DpSize1D::Multiply(val1, val2);

  EXPECT_EQ((val1.Value() * val2), val3);
}


TEST(TestMathDp_DpSize1D, Divide_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(5);
  DpSize1D val3 = DpSize1D::Divide(val1, val2);

  EXPECT_EQ(DpSize1D::Create(10 / 5), val3);
}


// TEST(TestMathDp_DpSize1D, Divide_DpValue_DpSize1D)
//{
//   const auto val1 = DpValue::Create(10);
//   const auto val2 = DpSize1D::Create(5);
//   DpSize1D val3 = DpSize1D::Divide(val1, val2);
//
//   EXPECT_EQ(DpSize1D::Create(10 / 5), val3);
// }


TEST(TestMathDp_DpSize1D, Divide_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(5);
  DpValue val3 = DpSize1D::Divide(val1, val2);

  EXPECT_EQ(DpValue::Create(10 / 5), val3);
}


TEST(TestMathDp_DpSize1D, Divide_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-5);
  DpValue val3 = DpSize1D::Divide(val1, val2);

  EXPECT_EQ((val1.Value() / val2), val3);
}


TEST(TestMathDp_DpSize1D, OpAddEqual_DpSize1D)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(1);
  DpSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1D, OpAddEqual_DpValue)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(1);
  DpSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1D, OpAddEqual_DpValue_Negative)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-1);
  DpSize1D sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1D, OpSubEqual_DpSize1D)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(1);
  DpSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1D, OpSubEqual_DpValue)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(1);
  DpSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathDp_DpSize1D, OpSubEqual_DpValue_Negative)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-1);
  DpSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathDp_DpSize1D, OpSubEqual_DpValue_Val2Largest)
{
  auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(100);
  DpSize1D sum = val1;
  sum -= val2;

  EXPECT_EQ(DpValue(0), sum.Value());
}


TEST(TestMathDp_DpSize1D, Min_LhsLess_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsLess_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsLess_DpValue_Negative_DpSize1D)
{
  const auto lhs = DpValue::Create(-9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(DpSize1D(), DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsLess_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(9);
  const auto rhs = DpValue::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
}

TEST(TestMathDp_DpSize1D, Min_LhsEqual_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsEqual_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(10);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsEqual_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsGreater_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpSize1D::Create(9);

  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsGreater_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(10);
  const auto rhs = DpSize1D::Create(9);

  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsGreater_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(9);

  EXPECT_EQ(rhs, DpSize1D::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Min_LhsGreater_DpSize1D_DpValue_Negative)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(-9);

  EXPECT_EQ(DpSize1D(), DpSize1D::Min(lhs, rhs));
}

TEST(TestMathDp_DpSize1D, Max_LhsLess_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsLess_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsLess_DpValue_Negative_DpSize1D)
{
  const auto lhs = DpValue::Create(-9);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsLess_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(9);
  const auto rhs = DpValue::Create(10);

  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}

TEST(TestMathDp_DpSize1D, Max_LhsEqual_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsEqual_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(10);
  const auto rhs = DpSize1D::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsEqual_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(10);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsGreater_DpSize1D_DpSize1D)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpSize1D::Create(9);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsGreater_DpValue_DpSize1D)
{
  const auto lhs = DpValue::Create(10);
  const auto rhs = DpSize1D::Create(9);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsGreater_DpSize1D_DpValue)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(9);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Max_LhsGreater_DpSize1D_DpValue_Negative)
{
  const auto lhs = DpSize1D::Create(10);
  const auto rhs = DpValue::Create(-9);

  EXPECT_EQ(lhs, DpSize1D::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1D, Op_Equal_DpSize1D_DpSize1D)
{
  EXPECT_TRUE(DpSize1D() == DpSize1D());
  EXPECT_TRUE(DpSize1D::Create(1) == DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) == DpSize1D::Create(1));
}


TEST(TestMathDp_DpSize1D, Op_Equal_DpValue_DpSize1D)
{
  EXPECT_TRUE(DpValue() == DpSize1D());
  EXPECT_TRUE(DpValue::Create(1) == DpSize1D::Create(1));
  EXPECT_FALSE(DpValue::Create(2) == DpSize1D::Create(1));
}


TEST(TestMathDp_DpSize1D, Op_Equal_DpSize1D_DpValue)
{
  EXPECT_TRUE(DpSize1D() == DpValue());
  EXPECT_TRUE(DpSize1D::Create(1) == DpValue::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) == DpValue::Create(1));
}

TEST(TestMathDp_DpSize1D, Op_NotEqual_DpSize1D_DpSize1D)
{
  EXPECT_FALSE(DpSize1D() != DpSize1D());
  EXPECT_FALSE(DpSize1D::Create(1) != DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) != DpSize1D::Create(1));
}

TEST(TestMathDp_DpSize1D, Op_NotEqual_DpValue_DpSize1D)
{
  EXPECT_FALSE(DpValue() != DpSize1D());
  EXPECT_FALSE(DpValue::Create(1) != DpSize1D::Create(1));
  EXPECT_TRUE(DpValue::Create(2) != DpSize1D::Create(1));
}

TEST(TestMathDp_DpSize1D, Op_NotEqual_DpSize1D_DpValue)
{
  EXPECT_FALSE(DpSize1D() != DpValue());
  EXPECT_FALSE(DpSize1D::Create(1) != DpValue::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) != DpValue::Create(1));
}


TEST(TestMathDp_DpSize1D, Op_Less_DpSize1D_DpSize1D)
{
  EXPECT_FALSE(DpSize1D() < DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D() < DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D() < DpSize1D::Create(2));

  EXPECT_FALSE(DpSize1D::Create(1) < DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D::Create(1) < DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D::Create(1) < DpSize1D::Create(2));

  EXPECT_FALSE(DpSize1D::Create(2) < DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D::Create(2) < DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) < DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_Less_DpValue_DpSize1D)
{
  EXPECT_FALSE(DpValue() < DpSize1D::Create(0));
  EXPECT_TRUE(DpValue() < DpSize1D::Create(1));
  EXPECT_TRUE(DpValue() < DpSize1D::Create(2));

  EXPECT_FALSE(DpValue::Create(1) < DpSize1D::Create(0));
  EXPECT_FALSE(DpValue::Create(1) < DpSize1D::Create(1));
  EXPECT_TRUE(DpValue::Create(1) < DpSize1D::Create(2));

  EXPECT_FALSE(DpValue::Create(2) < DpSize1D::Create(0));
  EXPECT_FALSE(DpValue::Create(2) < DpSize1D::Create(1));
  EXPECT_FALSE(DpValue::Create(2) < DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_Less_DpSize1D_DpValue)
{
  EXPECT_FALSE(DpSize1D() < DpValue::Create(0));
  EXPECT_TRUE(DpSize1D() < DpValue::Create(1));
  EXPECT_TRUE(DpSize1D() < DpValue::Create(2));

  EXPECT_FALSE(DpSize1D::Create(1) < DpValue::Create(0));
  EXPECT_FALSE(DpSize1D::Create(1) < DpValue::Create(1));
  EXPECT_TRUE(DpSize1D::Create(1) < DpValue::Create(2));

  EXPECT_FALSE(DpSize1D::Create(2) < DpValue::Create(0));
  EXPECT_FALSE(DpSize1D::Create(2) < DpValue::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) < DpValue::Create(2));
}


TEST(TestMathDp_DpSize1D, OpLessOrEqual_DpSize1D_DpSize1D)
{
  EXPECT_TRUE(DpSize1D() <= DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D() <= DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D() <= DpSize1D::Create(2));

  EXPECT_FALSE(DpSize1D::Create(1) <= DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D::Create(1) <= DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D::Create(1) <= DpSize1D::Create(2));

  EXPECT_FALSE(DpSize1D::Create(2) <= DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D::Create(2) <= DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) <= DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_LessOrEqual_DpValue_DpSize1D)
{
  EXPECT_TRUE(DpValue() <= DpSize1D::Create(0));
  EXPECT_TRUE(DpValue() <= DpSize1D::Create(1));
  EXPECT_TRUE(DpValue() <= DpSize1D::Create(2));

  EXPECT_FALSE(DpValue::Create(1) <= DpSize1D::Create(0));
  EXPECT_TRUE(DpValue::Create(1) <= DpSize1D::Create(1));
  EXPECT_TRUE(DpValue::Create(1) <= DpSize1D::Create(2));

  EXPECT_FALSE(DpValue::Create(2) <= DpSize1D::Create(0));
  EXPECT_FALSE(DpValue::Create(2) <= DpSize1D::Create(1));
  EXPECT_TRUE(DpValue::Create(2) <= DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_LessOrEqual_DpSize1D_DpValue)
{
  EXPECT_TRUE(DpSize1D() <= DpValue::Create(0));
  EXPECT_TRUE(DpSize1D() <= DpValue::Create(1));
  EXPECT_TRUE(DpSize1D() <= DpValue::Create(2));

  EXPECT_FALSE(DpSize1D::Create(1) <= DpValue::Create(0));
  EXPECT_TRUE(DpSize1D::Create(1) <= DpValue::Create(1));
  EXPECT_TRUE(DpSize1D::Create(1) <= DpValue::Create(2));

  EXPECT_FALSE(DpSize1D::Create(2) <= DpValue::Create(0));
  EXPECT_FALSE(DpSize1D::Create(2) <= DpValue::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) <= DpValue::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_Greater_DpSize1D_DpSize1D)
{
  EXPECT_FALSE(DpSize1D() > DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D() > DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D() > DpSize1D::Create(2));

  EXPECT_TRUE(DpSize1D::Create(1) > DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D::Create(1) > DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D::Create(1) > DpSize1D::Create(2));

  EXPECT_TRUE(DpSize1D::Create(2) > DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D::Create(2) > DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) > DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_Greater_DpValue_DpSize1D)
{
  EXPECT_FALSE(DpValue() > DpSize1D::Create(0));
  EXPECT_FALSE(DpValue() > DpSize1D::Create(1));
  EXPECT_FALSE(DpValue() > DpSize1D::Create(2));

  EXPECT_TRUE(DpValue::Create(1) > DpSize1D::Create(0));
  EXPECT_FALSE(DpValue::Create(1) > DpSize1D::Create(1));
  EXPECT_FALSE(DpValue::Create(1) > DpSize1D::Create(2));

  EXPECT_TRUE(DpValue::Create(2) > DpSize1D::Create(0));
  EXPECT_TRUE(DpValue::Create(2) > DpSize1D::Create(1));
  EXPECT_FALSE(DpValue::Create(2) > DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_Greater_DpSize1D_DpValue)
{
  EXPECT_FALSE(DpSize1D() > DpValue::Create(0));
  EXPECT_FALSE(DpSize1D() > DpValue::Create(1));
  EXPECT_FALSE(DpSize1D() > DpValue::Create(2));

  EXPECT_TRUE(DpSize1D::Create(1) > DpValue::Create(0));
  EXPECT_FALSE(DpSize1D::Create(1) > DpValue::Create(1));
  EXPECT_FALSE(DpSize1D::Create(1) > DpValue::Create(2));

  EXPECT_TRUE(DpSize1D::Create(2) > DpValue::Create(0));
  EXPECT_TRUE(DpSize1D::Create(2) > DpValue::Create(1));
  EXPECT_FALSE(DpSize1D::Create(2) > DpValue::Create(2));
}


TEST(TestMathDp_DpSize1D, OpGreaterOrEqual_DpSize1D_DpSize1D)
{
  EXPECT_TRUE(DpSize1D() >= DpSize1D::Create(0));
  EXPECT_FALSE(DpSize1D() >= DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D() >= DpSize1D::Create(2));

  EXPECT_TRUE(DpSize1D::Create(1) >= DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D::Create(1) >= DpSize1D::Create(1));
  EXPECT_FALSE(DpSize1D::Create(1) >= DpSize1D::Create(2));

  EXPECT_TRUE(DpSize1D::Create(2) >= DpSize1D::Create(0));
  EXPECT_TRUE(DpSize1D::Create(2) >= DpSize1D::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) >= DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_GreaterOrEqual_DpValue_DpSize1D)
{
  EXPECT_TRUE(DpValue() >= DpSize1D::Create(0));
  EXPECT_FALSE(DpValue() >= DpSize1D::Create(1));
  EXPECT_FALSE(DpValue() >= DpSize1D::Create(2));

  EXPECT_TRUE(DpValue::Create(1) >= DpSize1D::Create(0));
  EXPECT_TRUE(DpValue::Create(1) >= DpSize1D::Create(1));
  EXPECT_FALSE(DpValue::Create(1) >= DpSize1D::Create(2));

  EXPECT_TRUE(DpValue::Create(2) >= DpSize1D::Create(0));
  EXPECT_TRUE(DpValue::Create(2) >= DpSize1D::Create(1));
  EXPECT_TRUE(DpValue::Create(2) >= DpSize1D::Create(2));
}


TEST(TestMathDp_DpSize1D, Op_GreaterOrEqual_DpSize1D_DpValue)
{
  EXPECT_TRUE(DpSize1D() >= DpValue::Create(0));
  EXPECT_FALSE(DpSize1D() >= DpValue::Create(1));
  EXPECT_FALSE(DpSize1D() >= DpValue::Create(2));

  EXPECT_TRUE(DpSize1D::Create(1) >= DpValue::Create(0));
  EXPECT_TRUE(DpSize1D::Create(1) >= DpValue::Create(1));
  EXPECT_FALSE(DpSize1D::Create(1) >= DpValue::Create(2));

  EXPECT_TRUE(DpSize1D::Create(2) >= DpValue::Create(0));
  EXPECT_TRUE(DpSize1D::Create(2) >= DpValue::Create(1));
  EXPECT_TRUE(DpSize1D::Create(2) >= DpValue::Create(2));
}


TEST(TestMathDp_DpSize1D, OpAdd_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(1);
  const DpSize1D sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1D, OpAdd_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(1);
  const DpValue sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum);
}


TEST(TestMathDp_DpSize1D, OpAdd_DpValue_Negative_DpSize1D)
{
  const auto val1 = DpValue::Create(-10);
  const auto val2 = DpSize1D::Create(100);
  const DpValue sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum);
}


TEST(TestMathDp_DpSize1D, OpAdd_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(1);
  const DpValue sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum);
}


TEST(TestMathDp_DpSize1D, OpAdd_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-1);
  const DpValue sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum);
}


TEST(TestMathDp_DpSize1D, OpSub_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(1);
  const DpValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum);
}


TEST(TestMathDp_DpSize1D, OpSub_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(1);
  const DpValue sum = val1 - val2;

  EXPECT_EQ(val1 - val2.Value(), sum);
}


TEST(TestMathDp_DpSize1D, OpSub_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(1);
  const DpValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathDp_DpSize1D, OpSub_DpValue_Negative_DpSize1D)
{
  const auto val1 = DpValue::Create(-10);
  const auto val2 = DpSize1D::Create(100);
  const DpValue sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathDp_DpSize1D, OpSub_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-1);
  const DpValue sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathDp_DpSize1D, OpSub_NegativeResult_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(11);
  const DpValue sum = val1 - val2;

  EXPECT_EQ((val1 - val2), sum);
}


TEST(TestMathDp_DpSize1D, OpSub_NegativeResult_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(11);
  const DpValue sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathDp_DpSize1D, OpSub_NegativeResult_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(11);
  const DpValue sum = val1 - val2;

  EXPECT_EQ((val1.Value() - val2), sum);
}


TEST(TestMathDp_DpSize1D, OpMul_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(2);
  const DpSize1D sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1D, OpMul_DpValue_DpSize1D)
{
  const auto val1 = DpValue::Create(10);
  const auto val2 = DpSize1D::Create(2);
  const DpValue sum = val1 * val2;

  EXPECT_EQ(val1 * val2.Value(), sum);
}


TEST(TestMathDp_DpSize1D, OpMul_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(2);
  const DpValue sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2, sum);
}


TEST(TestMathDp_DpSize1D, OpMul_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-2);
  const DpValue sum = val1 * val2;

  EXPECT_EQ((val1.Value() * val2), sum);
}

TEST(TestMathDp_DpSize1D, OpDiv_DpSize1D_DpSize1D)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpSize1D::Create(2);
  const DpSize1D sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2.Value(), sum.Value());
}

TEST(TestMathDp_DpSize1D, OpDiv_DpSize1D_DpValue)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(2);
  const DpValue sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2, sum);
}

TEST(TestMathDp_DpSize1D, OpDiv_DpSize1D_DpValue_Negative)
{
  const auto val1 = DpSize1D::Create(10);
  const auto val2 = DpValue::Create(-2);
  const DpValue sum = val1 / val2;

  EXPECT_EQ((val1.Value() / val2), sum);
}
