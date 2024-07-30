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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartSortedDataChannelView.hpp>
#include <FslUnitTest/TestFixture.hpp>
#include <array>
#include <memory>

using namespace Fsl;

namespace
{
  using Test_Data_ChartSortedDataChannelView = TestFixture;

  std::shared_ptr<UI::ChartDataView> CreateFullyEmptyDataView(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
  {
    auto chartData = std::make_shared<UI::ChartData>(dataBinding, 0, 0, UI::ChartData::Constraints());
    return std::make_shared<UI::ChartDataView>(chartData);
  }

  std::shared_ptr<UI::ChartDataView> CreateEmptyDataView(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
  {
    auto chartData = std::make_shared<UI::ChartData>(dataBinding, 10, 1, UI::ChartData::Constraints());
    return std::make_shared<UI::ChartDataView>(chartData);
  }

  std::shared_ptr<UI::ChartDataView> CreateDataViewFromSpan(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
                                                            ReadOnlySpan<uint32_t> span)
  {
    auto chartData = std::make_shared<UI::ChartData>(dataBinding, NumericCast<uint32_t>(span.size()), 1, UI::ChartData::Constraints());
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      chartData->Append(UI::ChartDataEntry(span[i]));
    }
    return std::make_shared<UI::ChartDataView>(chartData);
  }

  // std::shared_ptr<UI::ChartDataView> CreateDataViewFromSpanAndMaxDataCapaity(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
  //                                                                            ReadOnlySpan<uint32_t> span, const uint32_t maxDataCapacity)
  //{
  //   auto chartData = std::make_shared<UI::ChartData>(dataBinding, maxDataCapacity, 1, UI::ChartData::Constraints());
  //   for (std::size_t i = 0; i < span.size(); ++i)
  //   {
  //     chartData->Append(UI::ChartDataEntry(span[i]));
  //   }
  //   return std::make_shared<UI::ChartDataView>(chartData);
  // }

  template <std::size_t TSize>
  std::shared_ptr<UI::ChartDataView> CreateDataViewFromSpan(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
                                                            const std::array<uint32_t, TSize>& source)
  {
    return CreateDataViewFromSpan(dataBinding, SpanUtil::AsReadOnlySpan(source));
  }

}


TEST(Test_Data_ChartSortedDataChannelView, ConstructFromFullyEmpty)
{
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  EXPECT_THROW(UI::ChartSortedDataChannelView(CreateFullyEmptyDataView(dataBinding), 0), std::invalid_argument);
}

TEST(Test_Data_ChartSortedDataChannelView, ConstructFromEmpty)
{
  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartSortedDataChannelView testSortedDataView(CreateEmptyDataView(dataBinding), 0);
}

TEST(Test_Data_ChartSortedDataChannelView, ConstructFromSortedLowToHigh)
{
  std::array<uint32_t, 5> source = {1, 2, 3, 4, 5};

  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartSortedDataChannelView testSortedDataView(CreateDataViewFromSpan(dataBinding, source), 0);

  auto sortedSpan = testSortedDataView.GetChannelViewSpan();
  ASSERT_EQ(5u, sortedSpan.size());
  EXPECT_EQ(1u, sortedSpan[0]);
  EXPECT_EQ(2u, sortedSpan[1]);
  EXPECT_EQ(3u, sortedSpan[2]);
  EXPECT_EQ(4u, sortedSpan[3]);
  EXPECT_EQ(5u, sortedSpan[4]);
}


TEST(Test_Data_ChartSortedDataChannelView, ConstructFromSortedHighToLow)
{
  std::array<uint32_t, 5> source = {5, 4, 3, 2, 1};

  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartSortedDataChannelView testSortedDataView(CreateDataViewFromSpan(dataBinding, source), 0);

  auto sortedSpan = testSortedDataView.GetChannelViewSpan();
  ASSERT_EQ(5u, sortedSpan.size());
  EXPECT_EQ(1u, sortedSpan[0]);
  EXPECT_EQ(2u, sortedSpan[1]);
  EXPECT_EQ(3u, sortedSpan[2]);
  EXPECT_EQ(4u, sortedSpan[3]);
  EXPECT_EQ(5u, sortedSpan[4]);
}


TEST(Test_Data_ChartSortedDataChannelView, ConstructFromUnsorted)
{
  std::array<uint32_t, 5> source = {1, 5, 2, 4, 3};

  auto dataBinding = std::make_shared<DataBinding::DataBindingService>();
  UI::ChartSortedDataChannelView testSortedDataView(CreateDataViewFromSpan(dataBinding, source), 0);

  auto sortedSpan = testSortedDataView.GetChannelViewSpan();
  ASSERT_EQ(5u, sortedSpan.size());
  EXPECT_EQ(1u, sortedSpan[0]);
  EXPECT_EQ(2u, sortedSpan[1]);
  EXPECT_EQ(3u, sortedSpan[2]);
  EXPECT_EQ(4u, sortedSpan[3]);
  EXPECT_EQ(5u, sortedSpan[4]);
}
