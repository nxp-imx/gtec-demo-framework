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

#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Log/Math/Dp/LogDpThicknessF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathDp_DpThicknessF = TestFixtureFslBase;
}


TEST(TestMathDp_DpThicknessF, Default)
{
  DpThicknessF value;

  EXPECT_EQ(0.0f, value.Left());
  EXPECT_EQ(0.0f, value.Top());
  EXPECT_EQ(0.0f, value.Right());
  EXPECT_EQ(0.0f, value.Bottom());
  EXPECT_EQ(0.0f, value.SumX());
  EXPECT_EQ(0.0f, value.SumY());
}


TEST(TestMathDp_DpThicknessF, Values)
{
  const float left = 1.0f;
  const float top = 2.0f;
  const float right = 3.0f;
  const float bottom = 4.0f;
  DpThicknessF value(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_EQ(left + right, value.SumX());
  EXPECT_EQ(top + bottom, value.SumY());
}

TEST(TestMathDp_DpThicknessF, OperatorEqual)
{
  const float left = 1;
  const float top = 2;
  const float right = 3;
  const float bottom = 4;
  DpThicknessF value1(left, top, right, bottom);
  DpThicknessF value2(left, top, right, bottom);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathDp_DpThicknessF, OperatorNotEqual)
{
  const float left = 1;
  const float top = 2;
  const float right = 3;
  const float bottom = 4;
  DpThicknessF value1(left, top, right, bottom);
  DpThicknessF value2(left, top, right, 5);

  EXPECT_NE(value1, value2);
}
