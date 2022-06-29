/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslSimpleUI/Controls/Charts/Util/BoxPlotHelper.hpp>
#include <FslUnitTest/TestFixture.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using Test_Util_BoxPlotHelper = TestFixture;
}

TEST(Test_Util_BoxPlotHelper, Calculate_ToSmallSpan)
{
  std::array<uint32_t, 4> sortedData = {};
  EXPECT_THROW(UI::BoxPlotHelper::Calculate(ReadOnlySpanUtil::AsSpan(sortedData)), NotSupportedException);
}


// The test is inspired by the calculation from https://www.geeksforgeeks.org/box-plot/
TEST(Test_Util_BoxPlotHelper, Calculate_Even)
{
  //                                       0    1    2    3    4    5    6    7    8    9   10   11
  std::array<uint32_t, 12> sortedData = {100, 110, 110, 110, 120, 120, 130, 140, 140, 150, 170, 220};

  auto result = UI::BoxPlotHelper::Calculate(ReadOnlySpanUtil::AsSpan(sortedData));


  EXPECT_EQ(100.0, result.Min);
  EXPECT_EQ(110.0, result.Q1);
  EXPECT_EQ(125.0, result.Q2);
  EXPECT_EQ(145.0, result.Q3);
  EXPECT_EQ(170.0, result.Max);
}

TEST(Test_Util_BoxPlotHelper, Calculate_Odd)
{
  //                                       0    1    2    3    4    5    6    7    8    9   10
  std::array<uint32_t, 11> sortedData = {100, 110, 110, 110, 120, 120, 130, 140, 140, 150, 220};

  auto result = UI::BoxPlotHelper::Calculate(ReadOnlySpanUtil::AsSpan(sortedData));

  EXPECT_EQ(100.0, result.Min);
  EXPECT_EQ(110.0, result.Q1);
  EXPECT_EQ(120.0, result.Q2);
  EXPECT_EQ(140.0, result.Q3);
  EXPECT_EQ(150.0, result.Max);
}
