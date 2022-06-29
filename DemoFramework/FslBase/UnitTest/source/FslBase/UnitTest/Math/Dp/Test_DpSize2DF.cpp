/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Log/Math/Dp/LogDpSize2DF.hpp>
#include <FslBase/Math/Dp/DpSize2DF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSize2DF = TestFixtureFslBase;
}


TEST(TestMathDp_DpSize2DF, Construct_Default)
{
  DpSize2DF value;

  EXPECT_EQ(DpSize2DF(), value);
  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), value.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), value.Height());
}

TEST(TestMathDp_DpSize2DF, Construct)
{
  constexpr const DpValueF width(3.0f);
  constexpr const DpValueF height(2.0f);
  DpSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize2DF, Construct_NegativeWidth)
{
  constexpr const DpValueF width(-1);
  constexpr const DpValueF height(2);
  DpSize2DF value(width, height);

  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize2DF, Construct_NegativeHeight)
{
  constexpr const DpValueF width(3);
  constexpr const DpValueF height(-1);
  DpSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(0), value.Height());
}


TEST(TestMathDp_DpSize2DF, SetWidth)
{
  const DpValueF newValue(100);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, SetWidth_Negative)
{
  const DpValueF newValue(-1);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, SetHeight)
{
  const DpValueF newValue(100);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathDp_DpSize2DF, SetHeight_Negative)
{
  const DpValueF newValue(-1);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddWidth)
{
  const DpValueF newValue(100);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f) + newValue, val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddWidth_Negative)
{
  const DpValueF newValue(-5);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f) + newValue, val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddWidth_Negative_ZeroClamp)
{
  const DpValueF newValue(-500);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddHeight)
{
  const DpValueF newValue(100.0f);
  auto val1 = DpSize2DF::Create(10.0f, 200.0f);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f) + newValue, val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddHeight_Negative)
{
  const DpValueF newValue(-5);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f) + newValue, val1.Height());
}


TEST(TestMathDp_DpSize2DF, AddHeight_Negative_ZeroClamp)
{
  const DpValueF newValue(-500);
  auto val1 = DpSize2DF::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(0.0f), val1.Height());
}

TEST(TestMathDp_DpSize2DF, Op_Equal)
{
  EXPECT_TRUE(DpSize2DF::Create(0, 0) == DpSize2DF::Create(0, 0));
  EXPECT_TRUE(DpSize2DF::Create(1, 2) == DpSize2DF::Create(1, 2));
  EXPECT_FALSE(DpSize2DF::Create(2, 1) == DpSize2DF::Create(1, 2));
}

TEST(TestMathDp_DpSize2DF, Op_NotEqual)
{
  EXPECT_FALSE(DpSize2DF::Create(0, 0) != DpSize2DF::Create(0, 0));
  EXPECT_FALSE(DpSize2DF::Create(1, 2) != DpSize2DF::Create(1, 2));
  EXPECT_TRUE(DpSize2DF::Create(2, 1) != DpSize2DF::Create(1, 2));
}

TEST(TestMathDp_DpSize2DF, Add)
{
  const auto val1 = DpSize2DF::Create(10, 200);
  const auto val2 = DpSize2DF::Create(5, 50);
  DpSize2DF val3 = DpSize2DF::Add(val1, val2);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
  EXPECT_EQ(DpSize2DF::value_type::Create(5.0f), val2.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(50.0f), val2.Height());
  EXPECT_EQ(DpSize2DF::value_type::Create(15.0f), val3.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(250.0f), val3.Height());
}

TEST(TestMathDp_DpSize2DF, Subtract)
{
  const auto val1 = DpSize2DF::Create(10, 200);
  const auto val2 = DpSize2DF::Create(5, 50);
  DpPoint2F val3 = DpSize2DF::Subtract(val1, val2);

  EXPECT_EQ(DpSize2DF::value_type::Create(10.0f), val1.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(200.0f), val1.Height());
  EXPECT_EQ(DpSize2DF::value_type::Create(5.0f), val2.Width());
  EXPECT_EQ(DpSize2DF::value_type::Create(50.0f), val2.Height());
  EXPECT_EQ(DpSize2DF::value_type::Create(5.0f), val3.X);
  EXPECT_EQ(DpSize2DF::value_type::Create(150.0f), val3.Y);
}


TEST(TestMathDp_DpSize2DF, OpAdd)
{
  const auto val1 = DpSize2DF::Create(10, 200);
  const auto val2 = DpSize2DF::Create(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}


TEST(TestMathDp_DpSize2DF, OpPlusEqual_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathDp_DpSize2DF, OpSubEqual_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpSize2DF, OpMulEqual_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathDp_DpSize2DF, OpAdd_DpPoint2F_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpSize2DF, OpAdd_DpSize2DF_DpPoint2F)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpSize2DF value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpSize2DF, OpSub_DpPoint2F_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpSize2DF, OpSub_DpSize2DF_DpPoint2F)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpSize2DF value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpSize2DF, OpMul_DpPoint2F_DpSize2DF)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpSize2DF value0(x0, y0);
  DpPoint2F value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpSize2DF, OpMul_DpSize2DF_DpPoint2F)
{
  const DpValueF x0(5);
  const DpValueF y0(4);
  const DpValueF x1(3);
  const DpValueF y1(2);
  DpPoint2F value0(x0, y0);
  DpSize2DF value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}
