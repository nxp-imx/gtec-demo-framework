#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_DATA_ACHARTDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_DATA_ACHARTDATA_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDataBinding/Base/Object/ObservableDataSourceObject.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartChannelMetaDataInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataEntry.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataStats.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataViewConfig.hpp>

namespace Fsl::UI
{
  class AChartData : public DataBinding::ObservableDataSourceObject
  {
  public:
    explicit AChartData(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
      : DataBinding::ObservableDataSourceObject(dataBinding)
    {
    }
    ~AChartData() override = default;

    virtual uint32_t ChangeId() const noexcept = 0;
    virtual uint32_t ChannelCount() const noexcept = 0;
    //! Create a view that is a 1:1 mapping of the data
    virtual ChartDataViewConfig CreateViewConfig() = 0;
    //! Create a view that will hold up to maxEntries.
    virtual ChartDataViewConfig CreateViewConfig(const uint32_t maxEntries, const bool allowCapacityToGrow) = 0;
    //! The number of entries in the chart data view
    virtual uint32_t Count(const ChartDataViewConfig viewConfig) const noexcept = 0;
    virtual ChartDataStats CalculateDataStats(const ChartDataViewConfig viewConfig) const = 0;
    virtual ChartDataInfo DataInfo(const ChartDataViewConfig viewConfig) const = 0;
    virtual ReadOnlySpan<ChartDataEntry> SegmentDataAsReadOnlySpan(const ChartDataViewConfig viewConfig, const uint32_t segmentIndex) const = 0;
    virtual ChartChannelMetaDataInfo GetChannelMetaDataInfo(const uint32_t channelIndex) const = 0;
  };
}


#endif
