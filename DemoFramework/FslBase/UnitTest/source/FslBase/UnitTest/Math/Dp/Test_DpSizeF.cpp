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

#include <FslBase/Math/Dp/DpSizeF.hpp>
#include <FslBase/Log/Math/Dp/LogDpSizeF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_DpSizeF = TestFixtureFslBase;
}


TEST(TestMathDp_DpSizeF, Construct_Default)
{
  DpSizeF value;

  EXPECT_EQ(DpSizeF(), value);
  EXPECT_EQ(0.0f, value.Width());
  EXPECT_EQ(0.0f, value.Height());
}

TEST(TestMathDp_DpSizeF, Construct)
{
  constexpr const float width = 3.0f;
  constexpr const float height = 2.0f;
  DpSizeF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSizeF, Construct_NegativeWidth)
{
  constexpr const float width = -1;
  constexpr const float height = 2;
  DpSizeF value(width, height);

  EXPECT_EQ(0.0f, value.Width());
  EXPECT_EQ(height, value.Height());
}

TEST(TestMathDp_DpSizeF, Construct_NegativeHeight)
{
  constexpr const float width = 3;
  constexpr const float height = -1;
  DpSizeF value(width, height);

  EXPECT_EQ(width, value.Width());
  EXPECT_EQ(0, value.Height());
}


TEST(TestMathDp_DpSizeF, SetWidth)
{
  const float newValue = 100;
  DpSizeF val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(newValue, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, SetWidth_Negative)
{
  const float newValue = -1;
  DpSizeF val1(10, 200);
  val1.SetWidth(newValue);

  EXPECT_EQ(0.0f, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, SetHeight)
{
  const float newValue = 100;
  DpSizeF val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(newValue, val1.Height());
}


TEST(TestMathDp_DpSizeF, SetHeight_Negative)
{
  const float newValue = -1;
  DpSizeF val1(10, 200);
  val1.SetHeight(newValue);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(0.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddWidth)
{
  const float newValue = 100;
  DpSizeF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10.0f + newValue, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddWidth_Negative)
{
  const float newValue = -5;
  DpSizeF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(10.0f + newValue, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddWidth_Negative_ZeroClamp)
{
  const float newValue = -500;
  DpSizeF val1(10, 200);
  val1.AddWidth(newValue);

  EXPECT_EQ(0.0f, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddHeight)
{
  const float newValue = 100.0f;
  DpSizeF val1(10.0f, 200.0f);
  val1.AddHeight(newValue);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(200.0f + newValue, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddHeight_Negative)
{
  const float newValue = -5;
  DpSizeF val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(200.0f + newValue, val1.Height());
}


TEST(TestMathDp_DpSizeF, AddHeight_Negative_ZeroClamp)
{
  const float newValue = -500;
  DpSizeF val1(10, 200);
  val1.AddHeight(newValue);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(0.0f, val1.Height());
}

TEST(TestMathDp_DpSizeF, Op_Equal)
{
  EXPECT_TRUE(DpSizeF() == DpSizeF());
  EXPECT_TRUE(DpSizeF(1, 2) == DpSizeF(1, 2));
  EXPECT_FALSE(DpSizeF(2, 1) == DpSizeF(1, 2));
}

TEST(TestMathDp_DpSizeF, Op_NotEqual)
{
  EXPECT_FALSE(DpSizeF() != DpSizeF());
  EXPECT_FALSE(DpSizeF(1, 2) != DpSizeF(1, 2));
  EXPECT_TRUE(DpSizeF(2, 1) != DpSizeF(1, 2));
}

TEST(TestMathDp_DpSizeF, Add)
{
  const DpSizeF val1(10, 200);
  const DpSizeF val2(5, 50);
  DpSizeF val3 = DpSizeF::Add(val1, val2);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
  EXPECT_EQ(5.0f, val2.Width());
  EXPECT_EQ(50.0f, val2.Height());
  EXPECT_EQ(15.0f, val3.Width());
  EXPECT_EQ(250.0f, val3.Height());
}

TEST(TestMathDp_DpSizeF, Subtract)
{
  const DpSizeF val1(10, 200);
  const DpSizeF val2(5, 50);
  DpSizeF val3 = DpSizeF::Subtract(val1, val2);

  EXPECT_EQ(10.0f, val1.Width());
  EXPECT_EQ(200.0f, val1.Height());
  EXPECT_EQ(5.0f, val2.Width());
  EXPECT_EQ(50.0f, val2.Height());
  EXPECT_EQ(5.0f, val3.Width());
  EXPECT_EQ(150.0f, val3.Height());
}


TEST(TestMathDp_DpSizeF, OpAdd)
{
  const DpSizeF val1(10, 200);
  const DpSizeF val2(1, 2);
  const auto sum = val1 + val2;

  EXPECT_EQ(val1.Width() + val2.Width(), sum.Width());
  EXPECT_EQ(val1.Height() + val2.Height(), sum.Height());
}
