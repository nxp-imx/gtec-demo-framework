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

#include <FslBase/Log/Math/Dp/LogDpSize1DF.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSize1DF = TestFixtureFslBase;
}


TEST(TestMathDp_DpSize1DF, Construct_Default)
{
  DpSize1DF value;

  EXPECT_EQ(DpSize1DF(), value);
  EXPECT_EQ(DpValueF::Create(0), value.Value());
}

TEST(TestMathDp_DpSize1DF, Construct)
{
  constexpr const auto Value = DpValueF::Create(3);
  DpSize1DF testValue(Value);

  EXPECT_EQ(Value, testValue.Value());
}

TEST(TestMathDp_DpSize1DF, Construct_NegativeValue)
{
  constexpr const auto Value = DpValueF::Create(-1);
  DpSize1DF testValue(Value);

  EXPECT_EQ(DpValueF::Create(0), testValue.Value());
}


// TEST(TestMathDp_DpSize1DF, SetValue)
//{
//   const auto newValue = DpValueF::Create(100);
//   auto val1 = DpSize1DF::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(newValue, val1.Value());
// }


// TEST(TestMathDp_DpSize1DF, SetValue_Negative)
//{
//   const auto newValue = DpValueF::Create(-1);
//   auto val1 = DpSize1DF::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(DpValueF::Create(0), val1.Value());
// }

TEST(TestMathDp_DpSize1DF, Add_DpSize1DF)
{
  const auto newValue = DpSize1DF::Create(100);
  auto val1 = DpSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValueF::Create(10) + newValue, val1.Value());
}

TEST(TestMathDp_DpSize1DF, Add_DpValueF)
{
  const auto newValue = DpValueF::Create(100);
  auto val1 = DpSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValueF::Create(10) + newValue, val1.Value());
}


TEST(TestMathDp_DpSize1DF, Add_DpValueF_Negative)
{
  const auto newValue = DpValueF::Create(-5);
  auto val1 = DpSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValueF::Create(10) + newValue, val1.Value());
}


TEST(TestMathDp_DpSize1DF, Add_DpValueF_Negative_ZeroClamp)
{
  const auto newValue = DpValueF::Create(-500);
  auto val1 = DpSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(DpValueF::Create(0), val1.Value());
}


TEST(TestMathDp_DpSize1DF, SetMax_Lower_DpSize1DF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpSize1DF::Create(9);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1DF, SetMax_Lower_DpValueF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpValueF::Create(9);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1DF, SetMax_Equal_DpSize1DF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpSize1DF::Create(10);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1DF, SetMax_Equal_DpValueF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpValueF::Create(10);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathDp_DpSize1DF, SetMax_Greater_DpSize1DF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpSize1DF::Create(11);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathDp_DpSize1DF, SetMax_Greater_DpValueF)
{
  const auto initialValue = DpSize1DF::Create(10);
  const auto otherValue = DpValueF::Create(11);
  DpSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathDp_DpSize1DF, Add_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpSize1DF val3 = DpSize1DF::Add(val1, val2);

  EXPECT_EQ(DpSize1DF::Create(15), val3);
}

TEST(TestMathDp_DpSize1DF, Add_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpSize1DF val3 = DpSize1DF::Add(val1, val2);

  EXPECT_EQ(DpSize1DF::Create(15), val3);
}

TEST(TestMathDp_DpSize1DF, Add_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(5);
  DpSize1DF val3 = DpSize1DF::Add(val1, val2);

  EXPECT_EQ(DpSize1DF::Create(15), val3);
}


TEST(TestMathDp_DpSize1DF, Subtract_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpValueF val3 = DpSize1DF::Subtract(val1, val2);

  EXPECT_EQ(DpValueF::Create(5), val3);
}


TEST(TestMathDp_DpSize1DF, Subtract_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpValueF val3 = DpSize1DF::Subtract(val1, val2);

  EXPECT_EQ(DpValueF::Create(5), val3);
}


TEST(TestMathDp_DpSize1DF, Subtract_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(5);
  DpValueF val3 = DpSize1DF::Subtract(val1, val2);

  EXPECT_EQ(DpValueF::Create(5), val3);
}


TEST(TestMathDp_DpSize1DF, Multiply_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpSize1DF val3 = DpSize1DF::Multiply(val1, val2);

  EXPECT_EQ(DpSize1DF::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1DF, Multiply_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpValueF val3 = DpSize1DF::Multiply(val1, val2);

  EXPECT_EQ(DpValueF::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1DF, Multiply_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(5);
  DpValueF val3 = DpSize1DF::Multiply(val1, val2);

  EXPECT_EQ(DpValueF::Create(10 * 5), val3);
}


TEST(TestMathDp_DpSize1DF, Multiply_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-5);
  DpValueF val3 = DpSize1DF::Multiply(val1, val2);

  EXPECT_EQ((val1.Value() * val2), val3);
}


TEST(TestMathDp_DpSize1DF, Divide_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(5);
  DpSize1DF val3 = DpSize1DF::Divide(val1, val2);

  EXPECT_EQ(DpSize1DF::Create(10.0f / 5.0f), val3);
}


// TEST(TestMathDp_DpSize1DF, Divide_DpValueF_DpSize1DF)
//{
//   const auto val1 = DpValueF::Create(10);
//   const auto val2 = DpSize1DF::Create(5);
//   DpSize1DF val3 = DpSize1DF::Divide(val1, val2);
//
//   EXPECT_EQ(DpSize1DF::Create(10 / 5), val3);
// }


TEST(TestMathDp_DpSize1DF, Divide_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(5);
  DpValueF val3 = DpSize1DF::Divide(val1, val2);

  EXPECT_EQ(DpValueF::Create(10.0f / 5.0f), val3);
}


TEST(TestMathDp_DpSize1DF, Divide_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-5);
  DpValueF val3 = DpSize1DF::Divide(val1, val2);

  EXPECT_EQ((val1.Value() / val2), val3);
}


TEST(TestMathDp_DpSize1DF, OpAddEqual_DpSize1DF)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  DpSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAddEqual_DpValueF)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(1);
  DpSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAddEqual_DpValueF_Negative)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-1);
  DpSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpSubEqual_DpSize1DF)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  DpSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpSubEqual_DpValueF)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(1);
  DpSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpSubEqual_DpValueF_Negative)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-1);
  DpSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpSubEqual_DpValueF_Val2Largest)
{
  auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(100);
  DpSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(DpValueF(0), sum.Value());
}


TEST(TestMathDp_DpSize1DF, Min_LhsLess_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsLess_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsLess_DpValueF_Negative_DpSize1DF)
{
  const auto lhs = DpValueF::Create(-9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(DpSize1DF(), DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsLess_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(9);
  const auto rhs = DpValueF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
}

TEST(TestMathDp_DpSize1DF, Min_LhsEqual_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsEqual_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(10);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsEqual_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsGreater_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpSize1DF::Create(9);

  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsGreater_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(10);
  const auto rhs = DpSize1DF::Create(9);

  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsGreater_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(9);

  EXPECT_EQ(rhs, DpSize1DF::Min(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Min_LhsGreater_DpSize1DF_DpValueF_Negative)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(-9);

  EXPECT_EQ(DpSize1DF(), DpSize1DF::Min(lhs, rhs));
}

TEST(TestMathDp_DpSize1DF, Max_LhsLess_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsLess_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsLess_DpValueF_Negative_DpSize1DF)
{
  const auto lhs = DpValueF::Create(-9);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsLess_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(9);
  const auto rhs = DpValueF::Create(10);

  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}

TEST(TestMathDp_DpSize1DF, Max_LhsEqual_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsEqual_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(10);
  const auto rhs = DpSize1DF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsEqual_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(10);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsGreater_DpSize1DF_DpSize1DF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpSize1DF::Create(9);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsGreater_DpValueF_DpSize1DF)
{
  const auto lhs = DpValueF::Create(10);
  const auto rhs = DpSize1DF::Create(9);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsGreater_DpSize1DF_DpValueF)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(9);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Max_LhsGreater_DpSize1DF_DpValueF_Negative)
{
  const auto lhs = DpSize1DF::Create(10);
  const auto rhs = DpValueF::Create(-9);

  EXPECT_EQ(lhs, DpSize1DF::Max(lhs, rhs));
}


TEST(TestMathDp_DpSize1DF, Op_Equal_DpSize1DF_DpSize1DF)
{
  EXPECT_TRUE(DpSize1DF() == DpSize1DF());
  EXPECT_TRUE(DpSize1DF::Create(1) == DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) == DpSize1DF::Create(1));
}


TEST(TestMathDp_DpSize1DF, Op_Equal_DpValueF_DpSize1DF)
{
  EXPECT_TRUE(DpValueF() == DpSize1DF());
  EXPECT_TRUE(DpValueF::Create(1) == DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF::Create(2) == DpSize1DF::Create(1));
}


TEST(TestMathDp_DpSize1DF, Op_Equal_DpSize1DF_DpValueF)
{
  EXPECT_TRUE(DpSize1DF() == DpValueF());
  EXPECT_TRUE(DpSize1DF::Create(1) == DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) == DpValueF::Create(1));
}

TEST(TestMathDp_DpSize1DF, Op_NotEqual_DpSize1DF_DpSize1DF)
{
  EXPECT_FALSE(DpSize1DF() != DpSize1DF());
  EXPECT_FALSE(DpSize1DF::Create(1) != DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) != DpSize1DF::Create(1));
}

TEST(TestMathDp_DpSize1DF, Op_NotEqual_DpValueF_DpSize1DF)
{
  EXPECT_FALSE(DpValueF() != DpSize1DF());
  EXPECT_FALSE(DpValueF::Create(1) != DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF::Create(2) != DpSize1DF::Create(1));
}

TEST(TestMathDp_DpSize1DF, Op_NotEqual_DpSize1DF_DpValueF)
{
  EXPECT_FALSE(DpSize1DF() != DpValueF());
  EXPECT_FALSE(DpSize1DF::Create(1) != DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) != DpValueF::Create(1));
}


TEST(TestMathDp_DpSize1DF, Op_Less_DpSize1DF_DpSize1DF)
{
  EXPECT_FALSE(DpSize1DF() < DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF() < DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF() < DpSize1DF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(1) < DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(1) < DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(1) < DpSize1DF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(2) < DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(2) < DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) < DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_Less_DpValueF_DpSize1DF)
{
  EXPECT_FALSE(DpValueF() < DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF() < DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF() < DpSize1DF::Create(2));

  EXPECT_FALSE(DpValueF::Create(1) < DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF::Create(1) < DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF::Create(1) < DpSize1DF::Create(2));

  EXPECT_FALSE(DpValueF::Create(2) < DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF::Create(2) < DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF::Create(2) < DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_Less_DpSize1DF_DpValueF)
{
  EXPECT_FALSE(DpSize1DF() < DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF() < DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF() < DpValueF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(1) < DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(1) < DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(1) < DpValueF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(2) < DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(2) < DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) < DpValueF::Create(2));
}


TEST(TestMathDp_DpSize1DF, OpLessOrEqual_DpSize1DF_DpSize1DF)
{
  EXPECT_TRUE(DpSize1DF() <= DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF() <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF() <= DpSize1DF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(1) <= DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(1) <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(1) <= DpSize1DF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(2) <= DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(2) <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) <= DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_LessOrEqual_DpValueF_DpSize1DF)
{
  EXPECT_TRUE(DpValueF() <= DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF() <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF() <= DpSize1DF::Create(2));

  EXPECT_FALSE(DpValueF::Create(1) <= DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF::Create(1) <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF::Create(1) <= DpSize1DF::Create(2));

  EXPECT_FALSE(DpValueF::Create(2) <= DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF::Create(2) <= DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF::Create(2) <= DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_LessOrEqual_DpSize1DF_DpValueF)
{
  EXPECT_TRUE(DpSize1DF() <= DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF() <= DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF() <= DpValueF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(1) <= DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(1) <= DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(1) <= DpValueF::Create(2));

  EXPECT_FALSE(DpSize1DF::Create(2) <= DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(2) <= DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) <= DpValueF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_Greater_DpSize1DF_DpSize1DF)
{
  EXPECT_FALSE(DpSize1DF() > DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF() > DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF() > DpSize1DF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(1) > DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(1) > DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(1) > DpSize1DF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(2) > DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(2) > DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) > DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_Greater_DpValueF_DpSize1DF)
{
  EXPECT_FALSE(DpValueF() > DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF() > DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF() > DpSize1DF::Create(2));

  EXPECT_TRUE(DpValueF::Create(1) > DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF::Create(1) > DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF::Create(1) > DpSize1DF::Create(2));

  EXPECT_TRUE(DpValueF::Create(2) > DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF::Create(2) > DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF::Create(2) > DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_Greater_DpSize1DF_DpValueF)
{
  EXPECT_FALSE(DpSize1DF() > DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF() > DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF() > DpValueF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(1) > DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF::Create(1) > DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(1) > DpValueF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(2) > DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(2) > DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(2) > DpValueF::Create(2));
}


TEST(TestMathDp_DpSize1DF, OpGreaterOrEqual_DpSize1DF_DpSize1DF)
{
  EXPECT_TRUE(DpSize1DF() >= DpSize1DF::Create(0));
  EXPECT_FALSE(DpSize1DF() >= DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF() >= DpSize1DF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(1) >= DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(1) >= DpSize1DF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(1) >= DpSize1DF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(2) >= DpSize1DF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(2) >= DpSize1DF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) >= DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_GreaterOrEqual_DpValueF_DpSize1DF)
{
  EXPECT_TRUE(DpValueF() >= DpSize1DF::Create(0));
  EXPECT_FALSE(DpValueF() >= DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF() >= DpSize1DF::Create(2));

  EXPECT_TRUE(DpValueF::Create(1) >= DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF::Create(1) >= DpSize1DF::Create(1));
  EXPECT_FALSE(DpValueF::Create(1) >= DpSize1DF::Create(2));

  EXPECT_TRUE(DpValueF::Create(2) >= DpSize1DF::Create(0));
  EXPECT_TRUE(DpValueF::Create(2) >= DpSize1DF::Create(1));
  EXPECT_TRUE(DpValueF::Create(2) >= DpSize1DF::Create(2));
}


TEST(TestMathDp_DpSize1DF, Op_GreaterOrEqual_DpSize1DF_DpValueF)
{
  EXPECT_TRUE(DpSize1DF() >= DpValueF::Create(0));
  EXPECT_FALSE(DpSize1DF() >= DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF() >= DpValueF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(1) >= DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(1) >= DpValueF::Create(1));
  EXPECT_FALSE(DpSize1DF::Create(1) >= DpValueF::Create(2));

  EXPECT_TRUE(DpSize1DF::Create(2) >= DpValueF::Create(0));
  EXPECT_TRUE(DpSize1DF::Create(2) >= DpValueF::Create(1));
  EXPECT_TRUE(DpSize1DF::Create(2) >= DpValueF::Create(2));
}


TEST(TestMathDp_DpSize1DF, OpAdd_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  const DpSize1DF sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAdd_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  const DpSize1DF sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAdd_DpValueF_Negative_DpSize1DF)
{
  const auto val1 = DpValueF::Create(-10);
  const auto val2 = DpSize1DF::Create(100);
  const DpSize1DF sum = val1 + val2;

  EXPECT_EQ(val1 + val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAdd_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(1);
  const DpSize1DF sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpAdd_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-1);
  const DpSize1DF sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpSub_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(1);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ(val1 - val2.Value(), sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(1);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_DpValueF_Negative_DpSize1DF)
{
  const auto val1 = DpValueF::Create(-10);
  const auto val2 = DpSize1DF::Create(100);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-1);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ(val1.Value() - val2, sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_NegativeResult_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(11);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ((val1.Value() - val2.Value()), sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_NegativeResult_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(11);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ((val1 - val2.Value()), sum);
}


TEST(TestMathDp_DpSize1DF, OpSub_NegativeResult_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(11);
  const DpValueF sum = val1 - val2;

  EXPECT_EQ((val1.Value() - val2), sum);
}


TEST(TestMathDp_DpSize1DF, OpMul_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(2);
  const DpSize1DF sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2.Value(), sum.Value());
}


TEST(TestMathDp_DpSize1DF, OpMul_DpValueF_DpSize1DF)
{
  const auto val1 = DpValueF::Create(10);
  const auto val2 = DpSize1DF::Create(2);
  const DpValueF sum = val1 * val2;

  EXPECT_EQ(val1 * val2.Value(), sum);
}


TEST(TestMathDp_DpSize1DF, OpMul_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(2);
  const DpValueF sum = val1 * val2;

  EXPECT_EQ(val1.Value() * val2, sum);
}


TEST(TestMathDp_DpSize1DF, OpMul_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-2);
  const DpValueF sum = val1 * val2;

  EXPECT_EQ((val1.Value() * val2), sum);
}

TEST(TestMathDp_DpSize1DF, OpDiv_DpSize1DF_DpSize1DF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpSize1DF::Create(2);
  const DpSize1DF sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2.Value(), sum.Value());
}

TEST(TestMathDp_DpSize1DF, OpDiv_DpSize1DF_DpValueF)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(2);
  const DpValueF sum = val1 / val2;

  EXPECT_EQ(val1.Value() / val2, sum);
}

TEST(TestMathDp_DpSize1DF, OpDiv_DpSize1DF_DpValueF_Negative)
{
  const auto val1 = DpSize1DF::Create(10);
  const auto val2 = DpValueF::Create(-2);
  const DpValueF sum = val1 / val2;

  EXPECT_EQ((val1.Value() / val2), sum);
}
