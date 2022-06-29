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

#include <FslBase/Log/Math/Pixel/LogPxSize2DF.hpp>
#include <FslBase/Math/Pixel/PxSize2DF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxSize2DF = TestFixtureFslBase;
}


TEST(TestMathPixel_PxSize2DF, Construct_Default)
{
  PxSize2DF value;

  EXPECT_EQ(PxSize2DF(), value);
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
}

TEST(TestMathPixel_PxSize2DF, Construct)
{
  constexpr const float width = 3;
  constexpr const float height = 2;
  PxSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2DF, Construct_NegativeWidth)
{
  constexpr const float width = -1;
  constexpr const float height = 2;
  PxSize2DF value(width, height);

  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2DF, Construct_NegativeHeight)
{
  constexpr const float width = 3;
  constexpr const float height = -1;
  PxSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(0, value.Height());
}


TEST(TestMathPixel_PxSize2DF, SetWidth)
{
  const float newValue = 100;
  PxSize2DF val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, SetWidth_Negative)
{
  const float newValue = -1;
  PxSize2DF val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, SetHeight)
{
  const float newValue = 100;
  PxSize2DF val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, SetHeight_Negative)
{
  const float newValue = -1;
  PxSize2DF val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddWidth)
{
  const float newValue = 100;
  PxSize2DF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_FLOAT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddWidth_Negative)
{
  const float newValue = -5;
  PxSize2DF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_FLOAT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddWidth_Negative_ZeroClamp)
{
  const float newValue = -500;
  PxSize2DF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddHeight)
{
  const float newValue = 100;
  PxSize2DF val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddHeight_Negative)
{
  const float newValue = -5;
  PxSize2DF val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, AddHeight_Negative_ZeroClamp)
{
  const float newValue = -500;
  PxSize2DF val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}

TEST(TestMathPixel_PxSize2DF, Op_Equal)
{
  EXPECT_TRUE(PxSize2DF() == PxSize2DF());
  EXPECT_TRUE(PxSize2DF(1, 2) == PxSize2DF(1, 2));
  EXPECT_FALSE(PxSize2DF(2, 1) == PxSize2DF(1, 2));
}

TEST(TestMathPixel_PxSize2DF, Op_NotEqual)
{
  EXPECT_FALSE(PxSize2DF() != PxSize2DF());
  EXPECT_FALSE(PxSize2DF(1, 2) != PxSize2DF(1, 2));
  EXPECT_TRUE(PxSize2DF(2, 1) != PxSize2DF(1, 2));
}

TEST(TestMathPixel_PxSize2DF, Add)
{
  const PxSize2DF val1(10, 200);
  const PxSize2DF val2(5, 50);
  PxSize2DF val3 = PxSize2DF::Add(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(15, val3.Width());
  EXPECT_EQ(250, val3.Height());
}

TEST(TestMathPixel_PxSize2DF, Subtract)
{
  const PxSize2DF val1(10, 200);
  const PxSize2DF val2(5, 50);
  PxVector2 val3 = PxSize2DF::Subtract(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(5, val3.X);
  EXPECT_EQ(150, val3.Y);
}


TEST(TestMathPixel_PxSize2DF, OpAdd)
{
  const PxSize2DF val1(10, 200);
  const PxSize2DF val2(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}

TEST(TestMathPixel_PxSize2DF, OpPlusEqual_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_FLOAT_EQ(x1 + x0, value1.X);
  EXPECT_FLOAT_EQ(y1 + y0, value1.Y);
}


TEST(TestMathPixel_PxSize2DF, OpSubEqual_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_FLOAT_EQ(x1 - x0, value1.X);
  EXPECT_FLOAT_EQ(y1 - y0, value1.Y);
}

TEST(TestMathPixel_PxSize2DF, OpMulEqual_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_FLOAT_EQ(x1 * x0, value1.X);
  EXPECT_FLOAT_EQ(y1 * y0, value1.Y);
}

TEST(TestMathPixel_PxSize2DF, OpAdd_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1 + x0, value3.X);
  EXPECT_FLOAT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathPixel_PxSize2DF, OpAdd_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxVector2 value0(x0, y0);
  PxSize2DF value1(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_FLOAT_EQ(x1 + x0, value3.X);
  EXPECT_FLOAT_EQ(y1 + y0, value3.Y);
}

TEST(TestMathPixel_PxSize2DF, OpSub_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1 - x0, value3.X);
  EXPECT_FLOAT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathPixel_PxSize2DF, OpSub_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxVector2 value0(x0, y0);
  PxSize2DF value1(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_FLOAT_EQ(x1 - x0, value3.X);
  EXPECT_FLOAT_EQ(y1 - y0, value3.Y);
}

TEST(TestMathPixel_PxSize2DF, OpMul_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxSize2DF value0(x0, y0);
  PxVector2 value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.Width());
  EXPECT_EQ(y0, value0.Height());
  EXPECT_EQ(x1, value1.X);
  EXPECT_EQ(y1, value1.Y);
  EXPECT_FLOAT_EQ(x1 * x0, value3.X);
  EXPECT_FLOAT_EQ(y1 * y0, value3.Y);
}


TEST(TestMathPixel_PxSize2DF, OpMul_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  PxVector2 value0(x0, y0);
  PxSize2DF value1(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X);
  EXPECT_EQ(y0, value0.Y);
  EXPECT_EQ(x1, value1.Width());
  EXPECT_EQ(y1, value1.Height());
  EXPECT_FLOAT_EQ(x1 * x0, value3.X);
  EXPECT_FLOAT_EQ(y1 * y0, value3.Y);
}
