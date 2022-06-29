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

#include <FslSimpleUI/Controls/Charts/Data/BoxPlotData.hpp>
#include <FslUnitTest/TestFixture.hpp>

using namespace Fsl;

namespace
{
  using Test_Data_BoxPlotData = TestFixture;
}

TEST(Test_Data_BoxPlotData, ConstructEmpty)
{
  UI::BoxPlotData data;
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.OutlierMin);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.Min);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.Q1);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.Q2);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.Q3);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.Max);
  EXPECT_EQ(UI::BoxPlotData::value_type(0.0), data.OutlierMax);
}


TEST(Test_Data_BoxPlotData, Construct)
{
  UI::BoxPlotData data(1, 2, 3, 4, 5, 6, 7);
  EXPECT_EQ(UI::BoxPlotData::value_type(1.0), data.OutlierMin);
  EXPECT_EQ(UI::BoxPlotData::value_type(2.0), data.Min);
  EXPECT_EQ(UI::BoxPlotData::value_type(3.0), data.Q1);
  EXPECT_EQ(UI::BoxPlotData::value_type(4.0), data.Q2);
  EXPECT_EQ(UI::BoxPlotData::value_type(5.0), data.Q3);
  EXPECT_EQ(UI::BoxPlotData::value_type(6.0), data.Max);
  EXPECT_EQ(UI::BoxPlotData::value_type(7.0), data.OutlierMax);
}


TEST(Test_Data_BoxPlotData, OpEqual)
{
  UI::BoxPlotData data(1, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataEQ(1, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE0(9, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE1(1, 9, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE2(1, 2, 9, 4, 5, 6, 7);
  UI::BoxPlotData dataNE3(1, 2, 3, 9, 5, 6, 7);
  UI::BoxPlotData dataNE4(1, 2, 3, 4, 9, 6, 7);
  UI::BoxPlotData dataNE5(1, 2, 3, 4, 5, 9, 7);
  UI::BoxPlotData dataNE6(1, 2, 3, 4, 5, 6, 9);

  EXPECT_TRUE(data == dataEQ);
  EXPECT_FALSE(data == dataNE0);
  EXPECT_FALSE(data == dataNE1);
  EXPECT_FALSE(data == dataNE2);
  EXPECT_FALSE(data == dataNE3);
  EXPECT_FALSE(data == dataNE4);
  EXPECT_FALSE(data == dataNE5);
  EXPECT_FALSE(data == dataNE6);
}

TEST(Test_Data_BoxPlotData, OpNotEqual)
{
  UI::BoxPlotData data(1, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataEQ(1, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE0(9, 2, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE1(1, 9, 3, 4, 5, 6, 7);
  UI::BoxPlotData dataNE2(1, 2, 9, 4, 5, 6, 7);
  UI::BoxPlotData dataNE3(1, 2, 3, 9, 5, 6, 7);
  UI::BoxPlotData dataNE4(1, 2, 3, 4, 9, 6, 7);
  UI::BoxPlotData dataNE5(1, 2, 3, 4, 5, 9, 7);
  UI::BoxPlotData dataNE6(1, 2, 3, 4, 5, 6, 9);

  EXPECT_FALSE(data != dataEQ);
  EXPECT_TRUE(data != dataNE0);
  EXPECT_TRUE(data != dataNE1);
  EXPECT_TRUE(data != dataNE2);
  EXPECT_TRUE(data != dataNE3);
  EXPECT_TRUE(data != dataNE4);
  EXPECT_TRUE(data != dataNE5);
  EXPECT_TRUE(data != dataNE6);
}
