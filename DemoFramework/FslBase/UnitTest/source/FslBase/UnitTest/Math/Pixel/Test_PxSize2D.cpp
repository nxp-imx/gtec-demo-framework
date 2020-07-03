/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
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
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
}

TEST(TestMathPixel_PxSize2D, Construct)
{
  constexpr const int32_t width = 3;
  constexpr const int32_t height = 2;
  PxSize2D value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2D, Construct_NegativeWidth)
{
  constexpr const int32_t width = -1;
  constexpr const int32_t height = 2;
  PxSize2D value(width, height);

  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathPixel_PxSize2D, Construct_NegativeHeight)
{
  constexpr const int32_t width = 3;
  constexpr const int32_t height = -1;
  PxSize2D value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(0, value.Height());
}


TEST(TestMathPixel_PxSize2D, SetWidth)
{
  const int32_t newValue = 100;
  PxSize2D val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2D, SetWidth_Negative)
{
  const int32_t newValue = -1;
  PxSize2D val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2D, SetHeight)
{
  const int32_t newValue = 100;
  PxSize2D val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2D, SetHeight_Negative)
{
  const int32_t newValue = -1;
  PxSize2D val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddWidth)
{
  const int32_t newValue = 100;
  PxSize2D val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddWidth_Negative)
{
  const int32_t newValue = -5;
  PxSize2D val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddWidth_Negative_ZeroClamp)
{
  const int32_t newValue = -500;
  PxSize2D val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddHeight)
{
  const int32_t newValue = 100;
  PxSize2D val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddHeight_Negative)
{
  const int32_t newValue = -5;
  PxSize2D val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathPixel_PxSize2D, AddHeight_Negative_ZeroClamp)
{
  const int32_t newValue = -500;
  PxSize2D val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}

TEST(TestMathPixel_PxSize2D, Op_Equal)
{
  EXPECT_TRUE(PxSize2D() == PxSize2D());
  EXPECT_TRUE(PxSize2D(1, 2) == PxSize2D(1, 2));
  EXPECT_FALSE(PxSize2D(2, 1) == PxSize2D(1, 2));
}

TEST(TestMathPixel_PxSize2D, Op_NotEqual)
{
  EXPECT_FALSE(PxSize2D() != PxSize2D());
  EXPECT_FALSE(PxSize2D(1, 2) != PxSize2D(1, 2));
  EXPECT_TRUE(PxSize2D(2, 1) != PxSize2D(1, 2));
}

TEST(TestMathPixel_PxSize2D, Add)
{
  const PxSize2D val1(10, 200);
  const PxSize2D val2(5, 50);
  PxSize2D val3 = PxSize2D::Add(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(15, val3.Width());
  EXPECT_EQ(250, val3.Height());
}

TEST(TestMathPixel_PxSize2D, Subtract)
{
  const PxSize2D val1(10, 200);
  const PxSize2D val2(5, 50);
  PxSize2D val3 = PxSize2D::Subtract(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(5, val3.Width());
  EXPECT_EQ(150, val3.Height());
}


TEST(TestMathPixel_PxSize2D, OpAdd)
{
  const PxSize2D val1(10, 200);
  const PxSize2D val2(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}
