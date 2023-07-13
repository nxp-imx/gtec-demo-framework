/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxSize1DF.hpp>
#include <FslBase/Math/Pixel/PxSize1DF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxSize1DF = TestFixtureFslBase;
}


TEST(TestMathPixel_PxSize1DF, Construct_Default)
{
  PxSize1DF value;

  EXPECT_EQ(PxSize1DF(), value);
  EXPECT_EQ(PxValueF::Create(0), value.Value());
}

TEST(TestMathPixel_PxSize1DF, Construct)
{
  constexpr const auto value = PxValueF::Create(3);
  PxSize1DF testValue(value);

  EXPECT_EQ(value, testValue.Value());
}

TEST(TestMathPixel_PxSize1DF, Construct_NegativeValue)
{
  constexpr const auto value = PxValueF::Create(-1);
  PxSize1DF testValue(value);

  EXPECT_EQ(PxValueF::Create(0), testValue.Value());
}


// TEST(TestMathPixel_PxSize1DF, SetValue)
//{
//   const auto newValue = PxValueF::Create(100);
//   auto val1 = PxSize1DF::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(newValue, val1.Value());
// }


// TEST(TestMathPixel_PxSize1DF, SetValue_Negative)
//{
//   const auto newValue = PxValueF::Create(-1);
//   auto val1 = PxSize1DF::Create(10);
//   val1.SetValue(newValue);
//
//   EXPECT_EQ(PxValueF::Create(0), val1.Value());
// }

TEST(TestMathPixel_PxSize1DF, Add_PxSize1DF)
{
  const auto newValue = PxSize1DF::Create(100);
  auto val1 = PxSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValueF::Create(10) + newValue, val1.Value());
}

TEST(TestMathPixel_PxSize1DF, Add_PxValueF)
{
  const auto newValue = PxValueF::Create(100);
  auto val1 = PxSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValueF::Create(10) + newValue, val1.Value());
}


TEST(TestMathPixel_PxSize1DF, Add_PxValueF_Negative)
{
  const auto newValue = PxValueF::Create(-5);
  auto val1 = PxSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValueF::Create(10) + newValue, val1.Value());
}


TEST(TestMathPixel_PxSize1DF, Add_PxValueF_Negative_ZeroClamp)
{
  const auto newValue = PxValueF::Create(-500);
  auto val1 = PxSize1DF::Create(10);
  val1.Add(newValue);

  EXPECT_EQ(PxValueF::Create(0), val1.Value());
}


TEST(TestMathPixel_PxSize1DF, SetMax_Lower_PxSize1DF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxSize1DF::Create(9);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, SetMax_Lower_PxValueF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxValueF::Create(9);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, SetMax_Equal_PxSize1DF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxSize1DF::Create(10);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, SetMax_Equal_PxValueF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxValueF::Create(10);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(initialValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, SetMax_Greater_PxSize1DF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxSize1DF::Create(11);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, SetMax_Greater_PxValueF)
{
  const auto initialValue = PxSize1DF::Create(10);
  const auto otherValue = PxValueF::Create(11);
  PxSize1DF testValue = initialValue;
  testValue.SetMax(otherValue);

  EXPECT_EQ(otherValue, testValue);
}


TEST(TestMathPixel_PxSize1DF, Add_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxSize1DF val3 = PxSize1DF::Add(val1, val2);

  EXPECT_EQ(PxSize1DF::Create(15), val3);
}

TEST(TestMathPixel_PxSize1DF, Add_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxValueF val3 = PxSize1DF::Add(val1, val2);

  EXPECT_FLOAT_EQ(PxSize1DF::Create(15).RawValue(), val3.Value);
}

TEST(TestMathPixel_PxSize1DF, Add_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(5);
  PxValueF val3 = PxSize1DF::Add(val1, val2);

  EXPECT_FLOAT_EQ(PxSize1DF::Create(15).RawValue(), val3.Value);
}


TEST(TestMathPixel_PxSize1DF, Subtract_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxValueF val3 = PxSize1DF::Subtract(val1, val2);

  EXPECT_EQ(PxValueF::Create(5), val3);
}


TEST(TestMathPixel_PxSize1DF, Subtract_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxValueF val3 = PxSize1DF::Subtract(val1, val2);

  EXPECT_EQ(PxValueF::Create(5), val3);
}


TEST(TestMathPixel_PxSize1DF, Subtract_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(5);
  PxValueF val3 = PxSize1DF::Subtract(val1, val2);

  EXPECT_EQ(PxValueF::Create(5), val3);
}


TEST(TestMathPixel_PxSize1DF, Multiply_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxSize1DF val3 = PxSize1DF::Multiply(val1, val2);

  EXPECT_EQ(PxSize1DF::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1DF, Multiply_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxValueF val3 = PxSize1DF::Multiply(val1, val2);

  EXPECT_EQ(PxValueF::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1DF, Multiply_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(5);
  PxValueF val3 = PxSize1DF::Multiply(val1, val2);

  EXPECT_EQ(PxValueF::Create(10 * 5), val3);
}


TEST(TestMathPixel_PxSize1DF, Multiply_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-5);
  PxValueF val3 = PxSize1DF::Multiply(val1, val2);

  EXPECT_EQ((val1.Value() * val2), val3);
}


TEST(TestMathPixel_PxSize1DF, Divide_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(5);
  PxSize1DF val3 = PxSize1DF::Divide(val1, val2);

  EXPECT_EQ(PxSize1DF::Create(10.0f / 5.0f), val3);
}


// TEST(TestMathPixel_PxSize1DF, Divide_PxValueF_PxSize1DF)
//{
//   const auto val1 = PxValueF::Create(10);
//   const auto val2 = PxSize1DF::Create(5);
//   PxSize1DF val3 = PxSize1DF::Divide(val1, val2);
//
//   EXPECT_EQ(PxSize1DF::Create(10 / 5), val3);
// }


TEST(TestMathPixel_PxSize1DF, Divide_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(5);
  PxValueF val3 = PxSize1DF::Divide(val1, val2);

  EXPECT_EQ(PxValueF::Create(10.0f / 5.0f), val3);
}


TEST(TestMathPixel_PxSize1DF, Divide_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-5);
  PxValueF val3 = PxSize1DF::Divide(val1, val2);

  EXPECT_EQ((val1.Value() / val2), val3);
}


TEST(TestMathPixel_PxSize1DF, OpAddEqual_PxSize1DF)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  PxSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpAddEqual_PxValueF)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(1);
  PxSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpAddEqual_PxValueF_Negative)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-1);
  PxSize1DF sum = val1;
  sum += val2;

  EXPECT_EQ(val1.Value() + val2, sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpSubEqual_PxSize1DF)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  PxSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpSubEqual_PxValueF)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(1);
  PxSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpSubEqual_PxValueF_Negative)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-1);
  PxSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(val1.Value() - val2, sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpSubEqual_PxValueF_Val2Largest)
{
  auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(100);
  PxSize1DF sum = val1;
  sum -= val2;

  EXPECT_EQ(PxValueF(0), sum.Value());
}


TEST(TestMathPixel_PxSize1DF, Min_LhsLess_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsLess_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsLess_PxValueF_Negative_PxSize1DF)
{
  const auto lhs = PxValueF::Create(-9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(PxSize1DF(), PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsLess_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(9);
  const auto rhs = PxValueF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
}

TEST(TestMathPixel_PxSize1DF, Min_LhsEqual_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsEqual_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(10);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsEqual_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Min(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsGreater_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxSize1DF::Create(9);

  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsGreater_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(10);
  const auto rhs = PxSize1DF::Create(9);

  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsGreater_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(9);

  EXPECT_EQ(rhs, PxSize1DF::Min(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Min_LhsGreater_PxSize1DF_PxValueF_Negative)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(-9);

  EXPECT_EQ(PxSize1DF(), PxSize1DF::Min(lhs, rhs));
}

TEST(TestMathPixel_PxSize1DF, Max_LhsLess_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsLess_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsLess_PxValueF_Negative_PxSize1DF)
{
  const auto lhs = PxValueF::Create(-9);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsLess_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(9);
  const auto rhs = PxValueF::Create(10);

  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}

TEST(TestMathPixel_PxSize1DF, Max_LhsEqual_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsEqual_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(10);
  const auto rhs = PxSize1DF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsEqual_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(10);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
  EXPECT_EQ(rhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsGreater_PxSize1DF_PxSize1DF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxSize1DF::Create(9);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsGreater_PxValueF_PxSize1DF)
{
  const auto lhs = PxValueF::Create(10);
  const auto rhs = PxSize1DF::Create(9);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsGreater_PxSize1DF_PxValueF)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(9);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Max_LhsGreater_PxSize1DF_PxValueF_Negative)
{
  const auto lhs = PxSize1DF::Create(10);
  const auto rhs = PxValueF::Create(-9);

  EXPECT_EQ(lhs, PxSize1DF::Max(lhs, rhs));
}


TEST(TestMathPixel_PxSize1DF, Op_Equal_PxSize1DF_PxSize1DF)
{
  EXPECT_TRUE(PxSize1DF() == PxSize1DF());
  EXPECT_TRUE(PxSize1DF::Create(1) == PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) == PxSize1DF::Create(1));
}


TEST(TestMathPixel_PxSize1DF, Op_Equal_PxValueF_PxSize1DF)
{
  EXPECT_TRUE(PxValueF() == PxSize1DF());
  EXPECT_TRUE(PxValueF::Create(1) == PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF::Create(2) == PxSize1DF::Create(1));
}


TEST(TestMathPixel_PxSize1DF, Op_Equal_PxSize1DF_PxValueF)
{
  EXPECT_TRUE(PxSize1DF() == PxValueF());
  EXPECT_TRUE(PxSize1DF::Create(1) == PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) == PxValueF::Create(1));
}

TEST(TestMathPixel_PxSize1DF, Op_NotEqual_PxSize1DF_PxSize1DF)
{
  EXPECT_FALSE(PxSize1DF() != PxSize1DF());
  EXPECT_FALSE(PxSize1DF::Create(1) != PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) != PxSize1DF::Create(1));
}

TEST(TestMathPixel_PxSize1DF, Op_NotEqual_PxValueF_PxSize1DF)
{
  EXPECT_FALSE(PxValueF() != PxSize1DF());
  EXPECT_FALSE(PxValueF::Create(1) != PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF::Create(2) != PxSize1DF::Create(1));
}

TEST(TestMathPixel_PxSize1DF, Op_NotEqual_PxSize1DF_PxValueF)
{
  EXPECT_FALSE(PxSize1DF() != PxValueF());
  EXPECT_FALSE(PxSize1DF::Create(1) != PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) != PxValueF::Create(1));
}


TEST(TestMathPixel_PxSize1DF, Op_Less_PxSize1DF_PxSize1DF)
{
  EXPECT_FALSE(PxSize1DF() < PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF() < PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF() < PxSize1DF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(1) < PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(1) < PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(1) < PxSize1DF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(2) < PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(2) < PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) < PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_Less_PxValueF_PxSize1DF)
{
  EXPECT_FALSE(PxValueF() < PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF() < PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF() < PxSize1DF::Create(2));

  EXPECT_FALSE(PxValueF::Create(1) < PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF::Create(1) < PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF::Create(1) < PxSize1DF::Create(2));

  EXPECT_FALSE(PxValueF::Create(2) < PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF::Create(2) < PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF::Create(2) < PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_Less_PxSize1DF_PxValueF)
{
  EXPECT_FALSE(PxSize1DF() < PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF() < PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF() < PxValueF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(1) < PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(1) < PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(1) < PxValueF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(2) < PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(2) < PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) < PxValueF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, OpLessOrEqual_PxSize1DF_PxSize1DF)
{
  EXPECT_TRUE(PxSize1DF() <= PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF() <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF() <= PxSize1DF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(1) <= PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(1) <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(1) <= PxSize1DF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(2) <= PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(2) <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) <= PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_LessOrEqual_PxValueF_PxSize1DF)
{
  EXPECT_TRUE(PxValueF() <= PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF() <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF() <= PxSize1DF::Create(2));

  EXPECT_FALSE(PxValueF::Create(1) <= PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF::Create(1) <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF::Create(1) <= PxSize1DF::Create(2));

  EXPECT_FALSE(PxValueF::Create(2) <= PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF::Create(2) <= PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF::Create(2) <= PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_LessOrEqual_PxSize1DF_PxValueF)
{
  EXPECT_TRUE(PxSize1DF() <= PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF() <= PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF() <= PxValueF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(1) <= PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(1) <= PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(1) <= PxValueF::Create(2));

  EXPECT_FALSE(PxSize1DF::Create(2) <= PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(2) <= PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) <= PxValueF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_Greater_PxSize1DF_PxSize1DF)
{
  EXPECT_FALSE(PxSize1DF() > PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF() > PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF() > PxSize1DF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(1) > PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(1) > PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(1) > PxSize1DF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(2) > PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(2) > PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) > PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_Greater_PxValueF_PxSize1DF)
{
  EXPECT_FALSE(PxValueF() > PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF() > PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF() > PxSize1DF::Create(2));

  EXPECT_TRUE(PxValueF::Create(1) > PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF::Create(1) > PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF::Create(1) > PxSize1DF::Create(2));

  EXPECT_TRUE(PxValueF::Create(2) > PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF::Create(2) > PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF::Create(2) > PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_Greater_PxSize1DF_PxValueF)
{
  EXPECT_FALSE(PxSize1DF() > PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF() > PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF() > PxValueF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(1) > PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF::Create(1) > PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(1) > PxValueF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(2) > PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(2) > PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(2) > PxValueF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, OpGreaterOrEqual_PxSize1DF_PxSize1DF)
{
  EXPECT_TRUE(PxSize1DF() >= PxSize1DF::Create(0));
  EXPECT_FALSE(PxSize1DF() >= PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF() >= PxSize1DF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(1) >= PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(1) >= PxSize1DF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(1) >= PxSize1DF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(2) >= PxSize1DF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(2) >= PxSize1DF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) >= PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_GreaterOrEqual_PxValueF_PxSize1DF)
{
  EXPECT_TRUE(PxValueF() >= PxSize1DF::Create(0));
  EXPECT_FALSE(PxValueF() >= PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF() >= PxSize1DF::Create(2));

  EXPECT_TRUE(PxValueF::Create(1) >= PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF::Create(1) >= PxSize1DF::Create(1));
  EXPECT_FALSE(PxValueF::Create(1) >= PxSize1DF::Create(2));

  EXPECT_TRUE(PxValueF::Create(2) >= PxSize1DF::Create(0));
  EXPECT_TRUE(PxValueF::Create(2) >= PxSize1DF::Create(1));
  EXPECT_TRUE(PxValueF::Create(2) >= PxSize1DF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, Op_GreaterOrEqual_PxSize1DF_PxValueF)
{
  EXPECT_TRUE(PxSize1DF() >= PxValueF::Create(0));
  EXPECT_FALSE(PxSize1DF() >= PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF() >= PxValueF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(1) >= PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(1) >= PxValueF::Create(1));
  EXPECT_FALSE(PxSize1DF::Create(1) >= PxValueF::Create(2));

  EXPECT_TRUE(PxSize1DF::Create(2) >= PxValueF::Create(0));
  EXPECT_TRUE(PxSize1DF::Create(2) >= PxValueF::Create(1));
  EXPECT_TRUE(PxSize1DF::Create(2) >= PxValueF::Create(2));
}


TEST(TestMathPixel_PxSize1DF, OpAdd_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  const PxSize1DF sum = val1 + val2;

  EXPECT_EQ(val1.Value() + val2.Value(), sum.Value());
}


TEST(TestMathPixel_PxSize1DF, OpAdd_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  const PxValueF sum = val1 + val2;

  EXPECT_FLOAT_EQ(val1.Value + val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpAdd_PxValueF_Negative_PxSize1DF)
{
  const auto val1 = PxValueF::Create(-10);
  const auto val2 = PxSize1DF::Create(100);
  const PxValueF sum = val1 + val2;

  EXPECT_FLOAT_EQ(val1.Value + val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpAdd_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(1);
  const PxValueF sum = val1 + val2;

  EXPECT_FLOAT_EQ(val1.RawValue() + val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpAdd_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-1);
  const PxValueF sum = val1 + val2;

  EXPECT_FLOAT_EQ(val1.RawValue() + val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.RawValue() - val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(1);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.Value - val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(1);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.RawValue() - val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_PxValueF_Negative_PxSize1DF)
{
  const auto val1 = PxValueF::Create(-10);
  const auto val2 = PxSize1DF::Create(100);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.Value - val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-1);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.RawValue() - val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_NegativeResult_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(11);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.RawValue() - val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_NegativeResult_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(11);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.Value - val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpSub_NegativeResult_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(11);
  const PxValueF sum = val1 - val2;

  EXPECT_FLOAT_EQ(val1.RawValue() - val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpMul_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(2);
  const PxSize1DF sum = val1 * val2;

  EXPECT_FLOAT_EQ(val1.RawValue() * val2.RawValue(), sum.RawValue());
}


TEST(TestMathPixel_PxSize1DF, OpMul_PxValueF_PxSize1DF)
{
  const auto val1 = PxValueF::Create(10);
  const auto val2 = PxSize1DF::Create(2);
  const PxValueF sum = val1 * val2;

  EXPECT_FLOAT_EQ(val1.Value * val2.RawValue(), sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpMul_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(2);
  const PxValueF sum = val1 * val2;

  EXPECT_FLOAT_EQ(val1.RawValue() * val2.Value, sum.Value);
}


TEST(TestMathPixel_PxSize1DF, OpMul_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-2);
  const PxValueF sum = val1 * val2;

  EXPECT_FLOAT_EQ(val1.RawValue() * val2.Value, sum.Value);
}

TEST(TestMathPixel_PxSize1DF, OpDiv_PxSize1DF_PxSize1DF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxSize1DF::Create(2);
  const PxSize1DF sum = val1 / val2;

  EXPECT_FLOAT_EQ(val1.RawValue() / val2.RawValue(), sum.RawValue());
}

TEST(TestMathPixel_PxSize1DF, OpDiv_PxSize1DF_PxValueF)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(2);
  const PxValueF sum = val1 / val2;

  EXPECT_FLOAT_EQ(val1.RawValue() / val2.Value, sum.Value);
}

TEST(TestMathPixel_PxSize1DF, OpDiv_PxSize1DF_PxValueF_Negative)
{
  const auto val1 = PxSize1DF::Create(10);
  const auto val2 = PxValueF::Create(-2);
  const PxValueF sum = val1 / val2;

  EXPECT_FLOAT_EQ((val1.RawValue() / val2.Value), sum.Value);
}
