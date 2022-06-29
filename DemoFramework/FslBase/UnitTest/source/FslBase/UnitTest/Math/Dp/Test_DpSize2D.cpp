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

#include <FslBase/Log/Math/Dp/LogDpSize2D.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSize2D = TestFixtureFslBase;
}


TEST(TestMathDp_DpSize2D, Construct_Default)
{
  DpSize2D value;

  EXPECT_EQ(DpSize2D(), value);
  EXPECT_EQ(DpValue(0), value.Width());
  EXPECT_EQ(DpValue(0), value.Height());
}

TEST(TestMathDp_DpSize2D, Construct)
{
  constexpr const DpValue width(3);
  constexpr const DpValue height(2);
  DpSize2D value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize2D, Construct_NegativeWidth)
{
  constexpr const DpValue width(-1);
  constexpr const DpValue height(2);
  DpSize2D value(width, height);

  EXPECT_EQ(DpValue(0), value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize2D, Construct_NegativeHeight)
{
  constexpr const DpValue width(3);
  constexpr const DpValue height(-1);
  DpSize2D value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(DpValue(0), value.Height());
}


TEST(TestMathDp_DpSize2D, SetWidth)
{
  const DpValue newValue(100);
  auto val1 = DpSize2D::Create(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
}


TEST(TestMathDp_DpSize2D, SetWidth_Negative)
{
  const DpValue newValue(-1);
  auto val1 = DpSize2D::Create(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(DpValue(0), val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
}


TEST(TestMathDp_DpSize2D, SetHeight)
{
  const DpValue newValue(100);
  auto val1 = DpSize2D::Create(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathDp_DpSize2D, SetHeight_Negative)
{
  const DpValue newValue(-1);
  auto val1 = DpSize2D::Create(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(0), val1.Height());
}


TEST(TestMathDp_DpSize2D, AddWidth)
{
  const DpValue newValue(100);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpValue(10) + newValue, val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
}


TEST(TestMathDp_DpSize2D, AddWidth_Negative)
{
  const DpValue newValue(-5);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpValue(10) + newValue, val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
}


TEST(TestMathDp_DpSize2D, AddWidth_Negative_ZeroClamp)
{
  const DpValue newValue(-500);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(DpValue(0), val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
}


TEST(TestMathDp_DpSize2D, AddHeight)
{
  const DpValue newValue(100);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(200) + newValue, val1.Height());
}


TEST(TestMathDp_DpSize2D, AddHeight_Negative)
{
  const DpValue newValue(-5);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(200) + newValue, val1.Height());
}


TEST(TestMathDp_DpSize2D, AddHeight_Negative_ZeroClamp)
{
  const DpValue newValue(-500);
  auto val1 = DpSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(0), val1.Height());
}

TEST(TestMathDp_DpSize2D, Op_Equal)
{
  EXPECT_TRUE(DpSize2D() == DpSize2D());
  EXPECT_TRUE(DpSize2D::Create(1, 2) == DpSize2D::Create(1, 2));
  EXPECT_FALSE(DpSize2D::Create(2, 1) == DpSize2D::Create(1, 2));
}

TEST(TestMathDp_DpSize2D, Op_NotEqual)
{
  EXPECT_FALSE(DpSize2D() != DpSize2D());
  EXPECT_FALSE(DpSize2D::Create(1, 2) != DpSize2D::Create(1, 2));
  EXPECT_TRUE(DpSize2D::Create(2, 1) != DpSize2D::Create(1, 2));
}

TEST(TestMathDp_DpSize2D, Add)
{
  const auto val1 = DpSize2D::Create(10, 200);
  const auto val2 = DpSize2D::Create(5, 50);
  DpSize2D val3 = DpSize2D::Add(val1, val2);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
  EXPECT_EQ(DpValue(5), val2.Width());
  EXPECT_EQ(DpValue(50), val2.Height());
  EXPECT_EQ(DpValue(15), val3.Width());
  EXPECT_EQ(DpValue(250), val3.Height());
}

TEST(TestMathDp_DpSize2D, Subtract)
{
  const auto val1 = DpSize2D::Create(10, 200);
  const auto val2 = DpSize2D::Create(5, 50);
  DpPoint2 val3 = DpSize2D::Subtract(val1, val2);

  EXPECT_EQ(DpValue(10), val1.Width());
  EXPECT_EQ(DpValue(200), val1.Height());
  EXPECT_EQ(DpValue(5), val2.Width());
  EXPECT_EQ(DpValue(50), val2.Height());
  EXPECT_EQ(DpValue(5), val3.X);
  EXPECT_EQ(DpValue(150), val3.Y);
}


TEST(TestMathDp_DpSize2D, OpAdd)
{
  const auto val1 = DpSize2D::Create(10, 200);
  const auto val2 = DpSize2D::Create(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}

TEST(TestMathDp_DpSize2D, OpPlusEqual_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}


TEST(TestMathDp_DpSize2D, OpSubEqual_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathDp_DpSize2D, OpMulEqual_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathDp_DpSize2D, OpAdd_DpPoint2_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpSize2D, OpAdd_DpSize2D_DpPoint2)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpSize2D value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathDp_DpSize2D, OpSub_DpPoint2_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpSize2D, OpSub_DpSize2D_DpPoint2)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpSize2D value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathDp_DpSize2D, OpMul_DpPoint2_DpSize2D)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpSize2D value0(x0, y0);
  DpPoint2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathDp_DpSize2D, OpMul_DpSize2D_DpPoint2)
{
  const DpValue x0(5);
  const DpValue y0(4);
  const DpValue x1(3);
  const DpValue y1(2);
  DpPoint2 value0(x0, y0);
  DpSize2D value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}
