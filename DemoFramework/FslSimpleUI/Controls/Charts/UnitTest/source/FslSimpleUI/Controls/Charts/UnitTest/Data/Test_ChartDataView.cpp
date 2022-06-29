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

#include <FslBase/Log/Math/LogMinMax.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslUnitTest/TestFixture.hpp>
#include <memory>

using namespace Fsl;

namespace
{
  using Test_Data_ChartDataView = TestFixture;
}


TEST(Test_Data_ChartDataView, ConstructFromEmpty)
{
  const uint32_t channelCount = 0;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 0, channelCount, UI::ChartData::Constraints());
  UI::ChartDataView testView(chartData);

  ASSERT_EQ(0u, testView.ChannelCount());
  ASSERT_EQ(0u, testView.Count());

  auto dataInfo = testView.DataInfo();
  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);
}


TEST(Test_Data_ChartDataView, Construct)
{
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 10, channelCount, UI::ChartData::Constraints());
  UI::ChartDataView testView(chartData);

  ASSERT_EQ(channelCount, testView.ChannelCount());
  ASSERT_EQ(0u, testView.Count());

  auto dataInfo = testView.DataInfo();
  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);
}


TEST(Test_Data_ChartDataView, CalculateDataStats_Empty0)
{
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 0, 0, UI::ChartData::Constraints());
  UI::ChartDataView testView(chartData);

  auto dataStats = testView.CalculateDataStats();
  EXPECT_EQ(MinMax<uint32_t>(), dataStats.ValueMinMax);
}


TEST(Test_Data_ChartDataView, CalculateDataStats_Empty1)
{
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 10, channelCount, UI::ChartData::Constraints());
  UI::ChartDataView testView(chartData);

  auto dataStats = testView.CalculateDataStats();
  EXPECT_EQ(MinMax<uint32_t>(), dataStats.ValueMinMax);
}

TEST(Test_Data_ChartDataView, CalculateDataStats_Empty_SetCustomMinMax)
{
  const uint32_t channelCount = 1;
  const MinMax<uint32_t> customMinMax(10, 100);

  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 10, channelCount, UI::ChartData::Constraints());

  UI::ChartDataView testView(chartData);

  const auto initalChangeId = testView.ChangeId();
  testView.SetCustomMinMax(customMinMax);
  EXPECT_NE(initalChangeId, testView.ChangeId());

  auto dataStats = testView.CalculateDataStats();
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);
}

TEST(Test_Data_ChartDataView, CalculateDataStats_SetCustomMinMax_Append)
{
  const uint32_t channelCount = 1;
  const MinMax<uint32_t> customMinMax(10, 100);

  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  auto chartData = std::make_shared<UI::ChartData>(dataBinding, 10, channelCount, UI::ChartData::Constraints());

  UI::ChartDataView testView(chartData);

  {
    const auto initalChangeId = testView.ChangeId();
    testView.SetCustomMinMax(customMinMax);
    EXPECT_NE(initalChangeId, testView.ChangeId());
  }
  {
    const auto initalChangeId = testView.ChangeId();
    chartData->Append(UI::ChartDataEntry(42));
    EXPECT_NE(initalChangeId, testView.ChangeId());
  }
}
