/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxSize2D = TestFixtureFslBase;
}


TEST(TestMathPixel_PxSize2D, Construct_Default)
{
  PxSize2D value;

  EXPECT_EQ(PxSize2D(), value);
  EXPECT_EQ(PxSize1D(), value.Width());
  EXPECT_EQ(PxSize1D(), value.Height());
  EXPECT_EQ(0, value.RawWidth());
  EXPECT_EQ(0, value.RawHeight());
}

TEST(TestMathPixel_PxSize2D, Construct)
{
  constexpr auto Width = PxSize1D::Create(3);
  constexpr auto Height = PxSize1D::Create(2);
  PxSize2D value(Width, Height);

  EXPECT_EQ(Width, value.Width());
  EXPECT_EQ(Height, value.Height());
}

TEST(TestMathPixel_PxSize2D, Create)
{
  constexpr auto Width = PxSize1D::Create(3);
  constexpr auto Height = PxSize1D::Create(2);
  PxSize2D value = PxSize2D::Create(Width.RawValue(), Height.RawValue());

  EXPECT_EQ(Width, value.Width());
  EXPECT_EQ(Height, value.Height());
}


TEST(TestMathPixel_PxSize2D, Construct_NegativeWidth)
{
  constexpr auto Width = PxSize1D::Create(-1);
  constexpr auto Height = PxSize1D::Create(2);
  PxSize2D value(Width, Height);

  EXPECT_EQ(PxSize1D::Create(0), value.Width());
  EXPECT_EQ(Height, value.Height());
}

TEST(TestMathPixel_PxSize2D, Construct_NegativeHeight)
{
  constexpr auto Width = PxSize1D::Create(3);
  constexpr auto Height = PxSize1D::Create(-1);
  PxSize2D value(Width, Height);

  EXPECT_EQ(Width, value.Width());
  EXPECT_EQ(PxSize1D::Create(0), value.Height());
}


TEST(TestMathPixel_PxSize2D, SetWidth_PxSize1D)
{
  const auto height = PxSize1D::Create(200);
  const auto newValue = PxSize1D::Create(100);
  PxSize2D val1(PxSize1D::Create(10), height);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(height, val1.Height());
}


TEST(TestMathPixel_PxSize2D, SetWidth_PxValue)
{
  const auto height = PxSize1D::Create(200);
  const auto newValue = PxValue::Create(100);
  PxSize2D val1(PxSize1D::Create(10), height);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(height, val1.Height());
}

TEST(TestMathPixel_PxSize2D, SetWidth_PxSize1D_Negative)
{
  const auto height = PxSize1D::Create(200);
  const auto newValue = PxSize1D::Create(-1);
  PxSize2D val1(PxSize1D::Create(10), height);
  val1.SetWidth(newValue);

  EXPECT_EQ(PxSize1D::Create(0), val1.Width());
  EXPECT_EQ(height, val1.Height());
}

TEST(TestMathPixel_PxSize2D, SetWidth_PxValue_Negative)
{
  const auto height = PxValue::Create(200);
  const auto newValue = PxSize1D::Create(-1);
  PxSize2D val1(PxSize1D::Create(10), height);
  val1.SetWidth(newValue);

  EXPECT_EQ(PxSize1D::Create(0), val1.Width());
  EXPECT_EQ(height, val1.Height());
}

TEST(TestMathPixel_PxSize2D, SetHeight)
{
  const auto width = PxSize1D::Create(10);
  const auto newValue = PxSize1D::Create(100);
  PxSize2D val1(width, PxSize1D::Create(200));
  val1.SetHeight(newValue);

  EXPECT_EQ(width, val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2D, SetHeight_Negative)
{
  const auto width = PxSize1D::Create(10);
  const auto newValue = PxValue::Create(-1);
  PxSize2D val1(width, PxSize1D::Create(200));
  val1.SetHeight(newValue);

  EXPECT_EQ(width, val1.Width());
  EXPECT_EQ(PxSize1D::Create(0), val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddWidth_PxValue)
{
  const auto newValue = PxValue::Create(100);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue.Value, val1.RawWidth());
  EXPECT_EQ(200, val1.RawHeight());
}

TEST(TestMathPixel_PxSize2D, AddWidth_PxSize1D)
{
  const auto newValue = PxSize1D::Create(100);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue.RawValue(), val1.RawWidth());
  EXPECT_EQ(200, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2D, AddWidth_PxValue_Negative)
{
  const auto newValue = PxValue::Create(-5);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue.Value, val1.RawWidth());
  EXPECT_EQ(200, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2D, AddWidth_PxValue_Negative_ZeroClamp)
{
  const auto newValue = PxValue::Create(-500);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(PxSize1D::Create(0), val1.Width());
  EXPECT_EQ(200, val1.RawHeight());
}

TEST(TestMathPixel_PxSize2D, AddHeight_PxValue)
{
  const PxValue newValue(100);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(200 + newValue.Value, val1.RawHeight());
}

TEST(TestMathPixel_PxSize2D, AddHeight_PxSize1D)
{
  const auto newValue = PxSize1D::Create(100);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(200 + newValue.RawValue(), val1.RawHeight());
}


TEST(TestMathPixel_PxSize2D, AddHeight_PxValue_Negative)
{
  const auto newValue = PxValue::Create(-5);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(200 + newValue.Value, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2D, AddHeight_PxValue_Negative_ZeroClamp)
{
  const auto newValue = PxValue::Create(-500);
  auto val1 = PxSize2D::Create(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(PxSize1D::Create(0), val1.Height());
}

TEST(TestMathPixel_PxSize2D, Op_Equal)
{
  EXPECT_TRUE(PxSize2D() == PxSize2D());
  EXPECT_TRUE(PxSize2D::Create(1, 2) == PxSize2D::Create(1, 2));
  EXPECT_FALSE(PxSize2D::Create(2, 1) == PxSize2D::Create(1, 2));
}

TEST(TestMathPixel_PxSize2D, Op_NotEqual)
{
  EXPECT_FALSE(PxSize2D() != PxSize2D());
  EXPECT_FALSE(PxSize2D::Create(1, 2) != PxSize2D::Create(1, 2));
  EXPECT_TRUE(PxSize2D::Create(2, 1) != PxSize2D::Create(1, 2));
}

TEST(TestMathPixel_PxSize2D, Add)
{
  const auto val1 = PxSize2D::Create(10, 200);
  const auto val2 = PxSize2D::Create(5, 50);
  PxSize2D val3 = PxSize2D::Add(val1, val2);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(200, val1.RawHeight());
  EXPECT_EQ(5, val2.RawWidth());
  EXPECT_EQ(50, val2.RawHeight());
  EXPECT_EQ(15, val3.RawWidth());
  EXPECT_EQ(250, val3.RawHeight());
}

TEST(TestMathPixel_PxSize2D, Subtract)
{
  const auto val1 = PxSize2D::Create(10, 200);
  const auto val2 = PxSize2D::Create(5, 50);

  PxPoint2 val3 = PxSize2D::Subtract(val1, val2);

  EXPECT_EQ(10, val1.RawWidth());
  EXPECT_EQ(200, val1.RawHeight());
  EXPECT_EQ(5, val2.RawWidth());
  EXPECT_EQ(50, val2.RawHeight());
  EXPECT_EQ(5, val3.X.Value);
  EXPECT_EQ(150, val3.Y.Value);
}


TEST(TestMathPixel_PxSize2D, OpAdd)
{
  const auto val1 = PxSize2D::Create(10, 200);
  const auto val2 = PxSize2D::Create(1, 2);
  const PxSize2D sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}

TEST(TestMathPixel_PxSize2D, OpPlusEqual_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 + x0, value1.X);
  EXPECT_EQ(y1 + y0, value1.Y);
}

TEST(TestMathPixel_PxSize2D, OpSubEqual_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 - x0, value1.X);
  EXPECT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathPixel_PxSize2D, OpMulEqual_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1 * x0, value1.X);
  EXPECT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathPixel_PxSize2D, OpAdd_PxPoint2_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathPixel_PxSize2D, OpAdd_PxSize2D_PxPoint2)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxPoint2 value0(x0, y0);
  PxSize2D value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 + x0, value3.X);
  EXPECT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathPixel_PxSize2D, OpSub_PxPoint2_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}


TEST(TestMathPixel_PxSize2D, OpSub_PxSize2D_PxPoint2)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxPoint2 value0(x0, y0);
  PxSize2D value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 - x0, value3.X);
  EXPECT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathPixel_PxSize2D, OpMul_PxPoint2_PxSize2D)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxSize2D value0(x0, y0);
  PxPoint2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}

TEST(TestMathPixel_PxSize2D, OpMul_PxSize2D_PxPoint2)
{
  const PxValue x0(5);
  const PxValue y0(4);
  const PxValue x1(3);
  const PxValue y1(2);
  PxPoint2 value0(x0, y0);
  PxSize2D value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_EQ(x1 * x0, value3.X);
  EXPECT_EQ(y1 * y0, value3.Y);
}
