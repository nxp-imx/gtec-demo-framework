/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
  EXPECT_EQ(0, value.RawWidth());
  EXPECT_EQ(0, value.RawHeight());
}

TEST(TestMathPixel_PxSize2DF, Construct)
{
  constexpr const auto width = PxSize1DF::Create(3);
  constexpr const auto height = PxSize1DF::Create(2);
  PxSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2DF, Construct_NegativeWidth)
{
  constexpr const auto width = PxValueF::Create(-1);
  constexpr const auto height = PxValueF::Create(2);
  PxSize2DF value(width, height);

  EXPECT_EQ(0, value.RawWidth());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2DF, Construct_NegativeHeight)
{
  constexpr const auto width = PxValueF::Create(3);
  constexpr const auto height = PxValueF::Create(-1);
  PxSize2DF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(0, value.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, SetWidth_PxSize1DF)
{
  const auto newValue = PxSize1DF::Create(100);
  const float height = 200;
  auto val1 = PxSize2DF::Create(10, height);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(height, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, SetWidth_Negative)
{
  const auto newValue = PxValueF::Create(-1);
  const float height = 200;
  auto val1 = PxSize2DF::Create(10, height);
  val1.SetWidth(newValue);

  EXPECT_EQ(0, val1.RawWidth());
  EXPECT_EQ(height, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, SetHeight)
{
  const auto newValue = PxValueF::Create(100);
  const float width = 10;
  auto val1 = PxSize2DF::Create(width, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(width, val1.RawWidth());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2DF, SetHeight_Negative)
{
  const auto newValue = PxValueF::Create(-1);
  const float width = 10;
  auto val1 = PxSize2DF::Create(width, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(width, val1.RawWidth());
  EXPECT_EQ(0, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddWidth)
{
  const auto newValue = PxValueF::Create(100);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddWidth(newValue);

  EXPECT_FLOAT_EQ(width + newValue.Value, val1.RawWidth());
  EXPECT_EQ(height, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddWidth_Negative)
{
  const auto newValue = PxValueF::Create(-5);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddWidth(newValue);

  EXPECT_FLOAT_EQ(width + newValue.Value, val1.RawWidth());
  EXPECT_EQ(height, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddWidth_Negative_ZeroClamp)
{
  const auto newValue = PxValueF::Create(-500);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddWidth(newValue);

  EXPECT_EQ(0, val1.RawWidth());
  EXPECT_EQ(height, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddHeight)
{
  const auto newValue = PxValueF::Create(100);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddHeight(newValue);

  EXPECT_EQ(width, val1.RawWidth());
  EXPECT_EQ(height + newValue.Value, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddHeight_Negative)
{
  const auto newValue = PxValueF::Create(-5);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddHeight(newValue);

  EXPECT_EQ(width, val1.RawWidth());
  EXPECT_EQ(height + newValue.Value, val1.RawHeight());
}


TEST(TestMathPixel_PxSize2DF, AddHeight_Negative_ZeroClamp)
{
  const auto newValue = PxValueF::Create(-500);
  const float width = 10;
  const float height = 200;
  auto val1 = PxSize2DF::Create(width, height);
  val1.AddHeight(newValue);

  EXPECT_EQ(width, val1.RawWidth());
  EXPECT_EQ(0, val1.RawHeight());
}

TEST(TestMathPixel_PxSize2DF, Op_Equal)
{
  EXPECT_TRUE(PxSize2DF() == PxSize2DF());
  EXPECT_TRUE(PxSize2DF::Create(1, 2) == PxSize2DF::Create(1, 2));
  EXPECT_FALSE(PxSize2DF::Create(2, 1) == PxSize2DF::Create(1, 2));
}

TEST(TestMathPixel_PxSize2DF, Op_NotEqual)
{
  EXPECT_FALSE(PxSize2DF() != PxSize2DF());
  EXPECT_FALSE(PxSize2DF::Create(1, 2) != PxSize2DF::Create(1, 2));
  EXPECT_TRUE(PxSize2DF::Create(2, 1) != PxSize2DF::Create(1, 2));
}

TEST(TestMathPixel_PxSize2DF, Add)
{
  const float width1 = 10;
  const float height1 = 200;
  const float width2 = 5;
  const float height2 = 50;
  const auto val1 = PxSize2DF::Create(width1, height1);
  const auto val2 = PxSize2DF::Create(width2, height2);
  PxSize2DF val3 = PxSize2DF::Add(val1, val2);

  EXPECT_EQ(width1, val1.RawWidth());
  EXPECT_EQ(height1, val1.RawHeight());
  EXPECT_EQ(width2, val2.RawWidth());
  EXPECT_EQ(height2, val2.RawHeight());
  EXPECT_FLOAT_EQ(width1 + width2, val3.RawWidth());
  EXPECT_FLOAT_EQ(height1 + height2, val3.RawHeight());
}

TEST(TestMathPixel_PxSize2DF, Subtract)
{
  const float width1 = 10;
  const float height1 = 200;
  const float width2 = 5;
  const float height2 = 50;
  const auto val1 = PxSize2DF::Create(width1, height1);
  const auto val2 = PxSize2DF::Create(width2, height2);
  PxVector2 val3 = PxSize2DF::Subtract(val1, val2);

  EXPECT_EQ(width1, val1.RawWidth());
  EXPECT_EQ(height1, val1.RawHeight());
  EXPECT_EQ(width2, val2.RawWidth());
  EXPECT_EQ(height2, val2.RawHeight());
  EXPECT_FLOAT_EQ(width1 - width2, val3.X.Value);
  EXPECT_FLOAT_EQ(height1 - height2, val3.Y.Value);
}


TEST(TestMathPixel_PxSize2DF, OpAdd)
{
  const auto val1 = PxSize2DF::Create(10, 200);
  const auto val2 = PxSize2DF::Create(1, 2);
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
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  value1 += value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_FLOAT_EQ(x1 + x0, value1.X.Value);
  EXPECT_FLOAT_EQ(y1 + y0, value1.Y.Value);
}


TEST(TestMathPixel_PxSize2DF, OpSubEqual_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  value1 -= value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_FLOAT_EQ(x1 - x0, value1.X.Value);
  EXPECT_FLOAT_EQ(y1 - y0, value1.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpMulEqual_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  value1 *= value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_FLOAT_EQ(x1 * x0, value1.X.Value);
  EXPECT_FLOAT_EQ(y1 * y0, value1.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpAdd_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_EQ(x1, value1.X.Value);
  EXPECT_EQ(y1, value1.Y.Value);
  EXPECT_FLOAT_EQ(x1 + x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 + y0, value3.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpAdd_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxVector2::Create(x0, y0);
  auto value1 = PxSize2DF::Create(x1, y1);

  auto value3 = value1 + value0;

  EXPECT_EQ(x0, value0.X.Value);
  EXPECT_EQ(y0, value0.Y.Value);
  EXPECT_EQ(x1, value1.RawWidth());
  EXPECT_EQ(y1, value1.RawHeight());
  EXPECT_FLOAT_EQ(x1 + x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 + y0, value3.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpSub_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_EQ(x1, value1.X.Value);
  EXPECT_EQ(y1, value1.Y.Value);
  EXPECT_FLOAT_EQ(x1 - x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 - y0, value3.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpSub_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxVector2::Create(x0, y0);
  auto value1 = PxSize2DF::Create(x1, y1);

  auto value3 = value1 - value0;

  EXPECT_EQ(x0, value0.X.Value);
  EXPECT_EQ(y0, value0.Y.Value);
  EXPECT_EQ(x1, value1.RawWidth());
  EXPECT_EQ(y1, value1.RawHeight());
  EXPECT_FLOAT_EQ(x1 - x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 - y0, value3.Y.Value);
}

TEST(TestMathPixel_PxSize2DF, OpMul_PxVector2_PxSize2DF)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxSize2DF::Create(x0, y0);
  auto value1 = PxVector2::Create(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.RawWidth());
  EXPECT_EQ(y0, value0.RawHeight());
  EXPECT_EQ(x1, value1.X.Value);
  EXPECT_EQ(y1, value1.Y.Value);
  EXPECT_FLOAT_EQ(x1 * x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 * y0, value3.Y.Value);
}


TEST(TestMathPixel_PxSize2DF, OpMul_PxSize2DF_PxVector2)
{
  const float x0 = 5;
  const float y0 = 4;
  const float x1 = 3;
  const float y1 = 2;
  auto value0 = PxVector2::Create(x0, y0);
  auto value1 = PxSize2DF::Create(x1, y1);

  auto value3 = value1 * value0;

  EXPECT_EQ(x0, value0.X.Value);
  EXPECT_EQ(y0, value0.Y.Value);
  EXPECT_EQ(x1, value1.RawWidth());
  EXPECT_EQ(y1, value1.RawHeight());
  EXPECT_FLOAT_EQ(x1 * x0, value3.X.Value);
  EXPECT_FLOAT_EQ(y1 * y0, value3.Y.Value);
}
