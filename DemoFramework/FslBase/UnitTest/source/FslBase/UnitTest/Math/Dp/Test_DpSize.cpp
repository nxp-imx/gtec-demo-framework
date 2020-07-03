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

#include <FslBase/Math/Dp/DpSize.hpp>
#include <FslBase/Log/Math/Dp/LogDpSize.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSize = TestFixtureFslBase;
}


TEST(TestMathDp_DpSize, Construct_Default)
{
  DpSize value;

  EXPECT_EQ(DpSize(), value);
  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(0, value.Height());
}

TEST(TestMathDp_DpSize, Construct)
{
  constexpr const int32_t width = 3;
  constexpr const int32_t height = 2;
  DpSize value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize, Construct_NegativeWidth)
{
  constexpr const int32_t width = -1;
  constexpr const int32_t height = 2;
  DpSize value(width, height);

  EXPECT_EQ(0, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSize, Construct_NegativeHeight)
{
  constexpr const int32_t width = 3;
  constexpr const int32_t height = -1;
  DpSize value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(0, value.Height());
}


TEST(TestMathDp_DpSize, SetWidth)
{
  const int32_t newValue = 100;
  DpSize val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathDp_DpSize, SetWidth_Negative)
{
  const int32_t newValue = -1;
  DpSize val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathDp_DpSize, SetHeight)
{
  const int32_t newValue = 100;
  DpSize val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathDp_DpSize, SetHeight_Negative)
{
  const int32_t newValue = -1;
  DpSize val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}


TEST(TestMathDp_DpSize, AddWidth)
{
  const int32_t newValue = 100;
  DpSize val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathDp_DpSize, AddWidth_Negative)
{
  const int32_t newValue = -5;
  DpSize val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10 + newValue, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathDp_DpSize, AddWidth_Negative_ZeroClamp)
{
  const int32_t newValue = -500;
  DpSize val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(0, val1.Width());
  EXPECT_EQ(200, val1.Height());
}


TEST(TestMathDp_DpSize, AddHeight)
{
  const int32_t newValue = 100;
  DpSize val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathDp_DpSize, AddHeight_Negative)
{
  const int32_t newValue = -5;
  DpSize val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200 + newValue, val1.Height());
}


TEST(TestMathDp_DpSize, AddHeight_Negative_ZeroClamp)
{
  const int32_t newValue = -500;
  DpSize val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(0, val1.Height());
}

TEST(TestMathDp_DpSize, Op_Equal)
{
  EXPECT_TRUE(DpSize() == DpSize());
  EXPECT_TRUE(DpSize(1, 2) == DpSize(1, 2));
  EXPECT_FALSE(DpSize(2, 1) == DpSize(1, 2));
}

TEST(TestMathDp_DpSize, Op_NotEqual)
{
  EXPECT_FALSE(DpSize() != DpSize());
  EXPECT_FALSE(DpSize(1, 2) != DpSize(1, 2));
  EXPECT_TRUE(DpSize(2, 1) != DpSize(1, 2));
}

TEST(TestMathDp_DpSize, Add)
{
  const DpSize val1(10, 200);
  const DpSize val2(5, 50);
  DpSize val3 = DpSize::Add(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(15, val3.Width());
  EXPECT_EQ(250, val3.Height());
}

TEST(TestMathDp_DpSize, Subtract)
{
  const DpSize val1(10, 200);
  const DpSize val2(5, 50);
  DpSize val3 = DpSize::Subtract(val1, val2);

  EXPECT_EQ(10, val1.Width());
  EXPECT_EQ(200, val1.Height());
  EXPECT_EQ(5, val2.Width());
  EXPECT_EQ(50, val2.Height());
  EXPECT_EQ(5, val3.Width());
  EXPECT_EQ(150, val3.Height());
}


TEST(TestMathDp_DpSize, OpAdd)
{
  const DpSize val1(10, 200);
  const DpSize val2(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}
