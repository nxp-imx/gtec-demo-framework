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
#include <FslBase/NumericCast.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslUnitTest/TestFixture.hpp>
#include <tuple>

using namespace Fsl;

namespace
{
  using Test_Data_ChartData = TestFixture;

  namespace LocalConfig
  {
    constexpr MinMax<uint32_t> EmptyMinMax;
  }

}

TEST(Test_Data_ChartData, Construct_1_InvalidDataEntryCount)
{
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  EXPECT_THROW(UI::ChartData(dataBinding, 1, NumericCast<uint32_t>(std::tuple_size<UI::ChartDataEntry::array_type>() + 1), {}),
               std::invalid_argument);
}

TEST(Test_Data_ChartData, Construct_0_0)
{
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 0, 0, {});
  EXPECT_EQ(0u, chartData.ChangeId());

  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(0u, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);
}

TEST(Test_Data_ChartData, Construct_0_1)
{
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 0, channelCount, {});
  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);
}

TEST(Test_Data_ChartData, Construct_1_1)
{
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});
  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);
}

TEST(Test_Data_ChartData, Construct_1_1_MaximumMin)
{
  const uint32_t maximumMin = 10;
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), {}));
  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);
}

TEST(Test_Data_ChartData, Construct_1_1_MinimumMax)
{
  const uint32_t minimumMax = 10;
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, UI::ChartData::Constraints({}, std::optional<uint32_t>(minimumMax)));
  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(LocalConfig::EmptyMinMax.Min(), dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());
}

TEST(Test_Data_ChartData, Construct_1_11_MaximumMin_MinimumMax)
{
  const uint32_t maximumMin = 2;
  const uint32_t minimumMax = 10;
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount,
                          UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), std::optional<uint32_t>(minimumMax)));
  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(LocalConfig::EmptyMinMax.Min(), dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());
}


TEST(Test_Data_ChartData, SetCustomMinMax)
{
  const MinMax<uint32_t> customMinMax(10, 20);
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.ChangeId());

  chartData.SetCustomMinMax(customMinMax);

  EXPECT_EQ(1u, chartData.ChangeId());
  auto testCustomMinMax = chartData.GetCustomMixMax();
  EXPECT_TRUE(testCustomMinMax.has_value());
  EXPECT_EQ(customMinMax, testCustomMinMax.value());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);

  // Setting the same value should not modify the change id
  EXPECT_EQ(1u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMax);
  EXPECT_EQ(1u, chartData.ChangeId());
}

TEST(Test_Data_ChartData, SetCustomMinMaxUpdate)
{
  const MinMax<uint32_t> customMinMax(10, 20);
  const MinMax<uint32_t> customMinMaxNew(5, 24);
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});
  EXPECT_FALSE(chartData.GetCustomMixMax());

  EXPECT_EQ(0u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMax);
  EXPECT_EQ(1u, chartData.ChangeId());

  auto testCustomMinMax = chartData.GetCustomMixMax();
  EXPECT_TRUE(testCustomMinMax.has_value());
  EXPECT_EQ(customMinMax, testCustomMinMax.value());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);

  EXPECT_EQ(1u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMaxNew);
  EXPECT_EQ(2u, chartData.ChangeId());

  dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMaxNew, dataStats.ValueMinMax);
}

TEST(Test_Data_ChartData, ClearCustomMinMax)
{
  const MinMax<uint32_t> customMinMax(10, 20);
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});
  EXPECT_FALSE(chartData.GetCustomMixMax());
  EXPECT_EQ(0u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMax);
  EXPECT_EQ(1u, chartData.ChangeId());
  auto testCustomMinMax = chartData.GetCustomMixMax();
  EXPECT_TRUE(testCustomMinMax.has_value());
  EXPECT_EQ(customMinMax, testCustomMinMax.value());

  EXPECT_EQ(1u, chartData.ChangeId());
  chartData.ClearCustomMinMax();
  EXPECT_EQ(2u, chartData.ChangeId());

  EXPECT_FALSE(chartData.GetCustomMixMax());
}


TEST(Test_Data_ChartData, Append)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value0, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value0, dataStats.ValueMinMax.Max());
}

TEST(Test_Data_ChartData, Append_MaximumMin)
{
  const uint32_t maximumMin = 10;
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), {}));

  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(maximumMin, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value0, dataStats.ValueMinMax.Max());
}

TEST(Test_Data_ChartData, Append_MinimumMax)
{
  const uint32_t minimumMax = 2000;
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, UI::ChartData::Constraints({}, std::optional<uint32_t>(minimumMax)));

  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value0, dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());
}

TEST(Test_Data_ChartData, Append_MaximumMinMinimumMax)
{
  const uint32_t maximumMin = 10;
  const uint32_t minimumMax = 2000;
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount,
                          UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), std::optional<uint32_t>(minimumMax)));

  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(maximumMin, dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());
}


TEST(Test_Data_ChartData, Append_CustomMinMax_LowMax)
{
  const MinMax<uint32_t> customMinMax(10, 1000);
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  EXPECT_EQ(0u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMax);

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(2u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);
}

TEST(Test_Data_ChartData, Append_CustomMinMax_HighMax)
{
  const MinMax<uint32_t> customMinMax(10, 2000);
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  EXPECT_EQ(0u, chartData.ChangeId());
  chartData.SetCustomMinMax(customMinMax);

  EXPECT_EQ(1u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(2u, chartData.ChangeId());
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig();
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  FSL_PARAM_NOT_USED(segmentData);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);
}


TEST(Test_Data_ChartData, ClearEmpty)
{
  const uint32_t channelCount = 1;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  {
    EXPECT_FALSE(chartData.GetCustomMixMax());
    EXPECT_EQ(0u, chartData.GetSize());

    auto viewConfig = chartData.CreateViewConfig();
    auto dataInfo = chartData.DataInfo(viewConfig);

    ASSERT_EQ(channelCount, dataInfo.ChannelCount);
    ASSERT_EQ(0u, dataInfo.SegmentCount);
    ASSERT_EQ(0u, dataInfo.TotalElementCount);

    auto dataStats = chartData.CalculateDataStats(viewConfig);
    EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);
  }

  EXPECT_EQ(0u, chartData.ChangeId());

  chartData.Clear();

  EXPECT_EQ(1u, chartData.ChangeId());


  {
    EXPECT_FALSE(chartData.GetCustomMixMax());
    EXPECT_EQ(0u, chartData.GetSize());

    auto viewConfig = chartData.CreateViewConfig();
    auto dataInfo = chartData.DataInfo(viewConfig);

    ASSERT_EQ(channelCount, dataInfo.ChannelCount);
    ASSERT_EQ(0u, dataInfo.SegmentCount);
    ASSERT_EQ(0u, dataInfo.TotalElementCount);

    auto dataStats = chartData.CalculateDataStats(viewConfig);
    EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);
  }
}


TEST(Test_Data_ChartData, Clear)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  EXPECT_EQ(0u, chartData.ChangeId());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());

  chartData.Clear();

  EXPECT_EQ(2u, chartData.ChangeId());

  {
    EXPECT_FALSE(chartData.GetCustomMixMax());
    EXPECT_EQ(0u, chartData.GetSize());

    auto viewConfig = chartData.CreateViewConfig();
    auto dataInfo = chartData.DataInfo(viewConfig);

    ASSERT_EQ(channelCount, dataInfo.ChannelCount);
    ASSERT_EQ(0u, dataInfo.SegmentCount);
    ASSERT_EQ(0u, dataInfo.TotalElementCount);

    auto dataStats = chartData.CalculateDataStats(viewConfig);
    EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append_ViewEntries0)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1337;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 1, channelCount, {});

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  EXPECT_EQ(1u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(0, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(0u, dataInfo.SegmentCount);
  ASSERT_EQ(0u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(LocalConfig::EmptyMinMax, dataStats.ValueMinMax);

  // Reading the zero segment is really not valid (but it works for safety)
  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(0u, segmentData.size());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.ChangeId());
  EXPECT_EQ(0u, chartData.GetSize());

  chartData.Append(UI::ChartDataEntry(value0));

  EXPECT_EQ(1u, chartData.ChangeId());

  chartData.Append(UI::ChartDataEntry(value1));

  EXPECT_EQ(2u, chartData.ChangeId());
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value1, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value1, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1_MaximumMin)
{
  const uint32_t maximumMin = 10;
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), {}));
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(maximumMin, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value1, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1_MinimumMax)
{
  const uint32_t minimumMax = 3000;
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, UI::ChartData::Constraints({}, std::optional<uint32_t>(minimumMax)));
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value1, dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1_MaximumMinMinimumMax)
{
  const uint32_t maximumMin = 10;
  const uint32_t minimumMax = 3000;

  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount,
                          UI::ChartData::Constraints(std::optional<uint32_t>(maximumMin), std::optional<uint32_t>(minimumMax)));
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(maximumMin, dataStats.ValueMinMax.Min());
  EXPECT_EQ(minimumMax, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1_CustomMinMax_LowMax)
{
  const MinMax<uint32_t> customMinMax(10, 1500);
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  chartData.SetCustomMinMax(customMinMax);
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries1_CustomMinMax_HighMax)
{
  const MinMax<uint32_t> customMinMax(10, 3000);
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  chartData.SetCustomMinMax(customMinMax);
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(customMinMax, dataStats.ValueMinMax);

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value1, segmentData[0].Values[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_Data_ChartData, Append2_Capacity2_ViewEntries2)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(2, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(2u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value0, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value1, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(2u, segmentData.size());
  EXPECT_EQ(value0, segmentData[0].Values[0]);
  EXPECT_EQ(value1, segmentData[1].Values[0]);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_Data_ChartData, Append3_Capacity2_ViewEntries1)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  const uint32_t value2 = 3000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  chartData.Append(UI::ChartDataEntry(value2));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(1, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(1u, dataInfo.SegmentCount);
  ASSERT_EQ(1u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value2, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value2, dataStats.ValueMinMax.Max());

  const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
  ASSERT_EQ(1u, segmentData.size());
  EXPECT_EQ(value2, segmentData[0].Values[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_Data_ChartData, Append3_Capacity2_ViewEntries2)
{
  const uint32_t channelCount = 1;
  const uint32_t value0 = 1000;
  const uint32_t value1 = 2000;
  const uint32_t value2 = 3000;
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartData chartData(dataBinding, 2, channelCount, {});
  EXPECT_EQ(2u, chartData.GetCapacity());

  EXPECT_EQ(0u, chartData.GetSize());
  chartData.Append(UI::ChartDataEntry(value0));
  chartData.Append(UI::ChartDataEntry(value1));
  chartData.Append(UI::ChartDataEntry(value2));
  EXPECT_EQ(2u, chartData.GetSize());

  auto viewConfig = chartData.CreateViewConfig(2, false);
  auto dataInfo = chartData.DataInfo(viewConfig);

  ASSERT_EQ(channelCount, dataInfo.ChannelCount);
  ASSERT_EQ(2u, dataInfo.SegmentCount);
  ASSERT_EQ(2u, dataInfo.TotalElementCount);

  auto dataStats = chartData.CalculateDataStats(viewConfig);
  EXPECT_EQ(value1, dataStats.ValueMinMax.Min());
  EXPECT_EQ(value2, dataStats.ValueMinMax.Max());

  {
    const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 0);
    ASSERT_EQ(1u, segmentData.size());
    EXPECT_EQ(value1, segmentData[0].Values[0]);
  }
  {
    const auto segmentData = chartData.SegmentDataAsReadOnlySpan(viewConfig, 1);
    ASSERT_EQ(1u, segmentData.size());
    EXPECT_EQ(value2, segmentData[0].Values[0]);
  }
}
