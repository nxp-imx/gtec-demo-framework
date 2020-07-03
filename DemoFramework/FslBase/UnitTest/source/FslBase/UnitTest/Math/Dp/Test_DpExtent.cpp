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

#include <FslBase/Math/Dp/DpExtent.hpp>
#include <FslBase/Log/Math/Dp/LogDpExtent.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathDp_DpExtent = TestFixtureFslBase;
}


TEST(TestMathDp_DpExtent, Construct_Default)
{
  const DpExtent value;

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}


TEST(TestMathDp_DpExtent, Construct_Unsigned)
{
  const uint32_t width = 320;
  const uint32_t height = 240;
  const DpExtent value(width, height);

  EXPECT_EQ(width, value.Width);
  EXPECT_EQ(height, value.Height);
}

TEST(TestMathDp_DpExtent, Construct_Point2U)
{
  const DpPointU point(320, 240);
  DpExtent value(point);

  EXPECT_EQ(point.X, value.Width);
  EXPECT_EQ(point.Y, value.Height);
}


TEST(TestMathDp_DpExtent, AddEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  DpExtent value1(width1, height1);

  uint32_t width2 = 200;
  uint32_t height2 = 100;
  DpExtent value2(width2, height2);

  value2 += value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 + width1, value2.Width);
  EXPECT_EQ(height2 + height1, value2.Height);
}


TEST(TestMathDp_DpExtent, SubEquals)
{
  uint32_t width1 = 320;
  uint32_t height1 = 240;
  DpExtent value1(width1, height1);

  uint32_t width2 = 800;
  uint32_t height2 = 600;
  DpExtent value2(width2, height2);

  value2 -= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 - width1, value2.Width);
  EXPECT_EQ(height2 - height1, value2.Height);
}


TEST(TestMathDp_DpExtent, MultEquals)
{
  const uint32_t width1 = 2;
  const uint32_t height1 = 4;
  DpExtent value1(width1, height1);

  const uint32_t width2 = 200;
  const uint32_t height2 = 100;
  DpExtent value2(width2, height2);

  value2 *= value1;

  EXPECT_EQ(width1, value1.Width);
  EXPECT_EQ(height1, value1.Height);
  EXPECT_EQ(width2 * width1, value2.Width);
  EXPECT_EQ(height2 * height1, value2.Height);
}


TEST(TestMathDp_DpExtent, MultEqual)
{
  const uint32_t multBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  DpExtent value(width, height);

  value *= multBy;

  EXPECT_EQ(width * multBy, value.Width);
  EXPECT_EQ(height * multBy, value.Height);
}


TEST(TestMathDp_DpExtent, DivEqual)
{
  const uint32_t divBy = 2;

  uint32_t width = 200;
  uint32_t height = 100;
  DpExtent value(width, height);

  value /= divBy;

  EXPECT_EQ(width / divBy, value.Width);
  EXPECT_EQ(height / divBy, value.Height);
}


TEST(TestMathDp_DpExtent, Equal)
{
  DpExtent value1(320, 240);
  DpExtent value2(320, 240);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathDp_DpExtent, NotEqual)
{
  DpExtent value1(320, 240);
  DpExtent value2(320, 241);
  DpExtent value3(321, 240);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}


TEST(TestMathDp_DpExtent, Zero)
{
  DpExtent value = DpExtent::Zero();

  EXPECT_EQ(0u, value.Width);
  EXPECT_EQ(0u, value.Height);
}


TEST(TestMathDp_DpExtent, OpAdd)
{
  DpExtent value1(2, 3);
  DpExtent value2(40, 80);

  EXPECT_EQ(DpExtent(42, 83), value1 + value2);
  EXPECT_EQ(DpExtent(42, 83), value2 + value1);
}


TEST(TestMathDp_DpExtent, OpSub)
{
  DpExtent value1(2, 3);
  DpExtent value2(40, 80);
  DpExtent value3(140, 181);

  EXPECT_EQ(DpExtent(38, 77), value2 - value1);
  EXPECT_EQ(DpExtent(100, 101), value3 - value2);
}


TEST(TestMathDp_DpExtent, OpMul_DpExtend2D)
{
  DpExtent value1(2, 3);
  DpExtent value2(41, 80);

  EXPECT_EQ(DpExtent(82, 240), value1 * value2);
  EXPECT_EQ(DpExtent(82, 240), value2 * value1);
}


TEST(TestMathDp_DpExtent, OpMul_Const)
{
  DpExtent value1(2, 3);

  EXPECT_EQ(DpExtent(4, 6), value1 * 2u);
  EXPECT_EQ(DpExtent(4, 6), 2u * value1);
}


TEST(TestMathDp_DpExtent, OpDiv_Const)
{
  DpExtent value1(20, 40);

  EXPECT_EQ(DpExtent(10, 20), value1 / 2u);
}
