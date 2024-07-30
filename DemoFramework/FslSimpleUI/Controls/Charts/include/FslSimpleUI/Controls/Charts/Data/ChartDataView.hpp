#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTDATAVIEW_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTDATAVIEW_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslDataBinding/Base/IObservableObject.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartChannelMetaData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartChannelMetaDataInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataEntry.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataStats.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataViewConfig.hpp>
#include <memory>
#include <optional>

namespace Fsl::UI
{
  class AChartData;

  /// <summary>
  /// Represents a view of the data contained in AChartData.
  /// The stats and max view entries are unique to this view.
  /// </summary>
  class ChartDataView final : public DataBinding::IObservableObject
  {
    struct ChannelMetaDataRecord
    {
      bool IsSetLabel{false};
      bool IsSetColor{false};
      ChartChannelMetaData MetaData;
      ChannelMetaDataRecord() = default;
      explicit ChannelMetaDataRecord(ChartChannelMetaData metaData);

      void SetLabel(const StringViewLite label);
      void SetPrimaryColor(const UIColor color);
    };

  public:
    using value_type = uint32_t;

    struct DataCacheRecord
    {
      uint32_t DataChangeId{};
      ChartDataStats Stats;
    };

  private:
    std::shared_ptr<AChartData> m_chartData;
    ChartDataViewConfig m_viewConfig;
    std::optional<MinMax<value_type>> m_customViewMinMax;
    std::array<ChannelMetaDataRecord, ChartDataLimits::MaxChannels> m_customChannelMetaData;
    uint32_t m_channelCount;
    uint32_t m_viewMaxEntries;
    uint32_t m_viewChangeId{0};
    mutable DataCacheRecord m_dataCache;

  public:
    explicit ChartDataView(std::shared_ptr<AChartData> chartData);
    ~ChartDataView() override;

    uint64_t ChangeId() const noexcept;

    void ClearCustomMinMax();
    void SetCustomMinMax(MinMax<value_type> customViewMinMax);

    void SetMaxViewEntries(const uint32_t maxEntries);

    ChartDataStats CalculateDataStats() const;

    uint32_t ChannelCount() const noexcept
    {
      return m_channelCount;
    }

    //! The number of entries in the view
    uint32_t Count() const noexcept;

    ChartDataInfo DataInfo() const;
    ReadOnlySpan<ChartDataEntry> SegmentDataAsReadOnlySpan(const uint32_t segmentIndex) const;

    DataBinding::DataBindingInstanceHandle GetSourceInstanceHandle() const final;

    ChartChannelMetaDataInfo GetChannelMetaDataInfo(const uint32_t channelIndex) const;

    void ClearChannelMetaData(const uint32_t channelIndex);
    void SetChannelMetaData(const uint32_t channelIndex, const ChartChannelMetaData& metaData);
    void SetChannelMetaData(const uint32_t channelIndex, const StringViewLite label);
    void SetChannelMetaData(const uint32_t channelIndex, const UIColor primaryColor);
  };
}


#endif
