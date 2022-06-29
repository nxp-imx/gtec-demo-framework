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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslSimpleUI/Controls/Charts/Data/AChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <limits>
#include <stdexcept>
#include <utility>

namespace Fsl::UI
{
  ChartDataView::ChannelMetaDataRecord::ChannelMetaDataRecord(ChartChannelMetaData metaData)
    : IsSetLabel(true)
    , IsSetColor(true)
    , MetaData(std::move(metaData))
  {
  }


  void ChartDataView::ChannelMetaDataRecord::SetLabel(const StringViewLite label)
  {
    IsSetLabel = true;
    StringViewLiteUtil::Set(MetaData.Label, label);
  }


  void ChartDataView::ChannelMetaDataRecord::SetPrimaryColor(const Color color)
  {
    IsSetColor = true;
    MetaData.PrimaryColor = color;
  }


  ChartDataView::ChartDataView(std::shared_ptr<AChartData> chartData)
    : m_chartData(std::move(chartData))
    , m_viewMaxEntries(std::numeric_limits<uint32_t>::max())
  {
    if (!m_chartData)
    {
      throw std::invalid_argument("chartData can not be null");
    }

    m_channelCount = m_chartData->ChannelCount();
    m_viewConfig = m_chartData->CreateViewConfig();
    // Ensure that our cached data change id is different from the current one in the data view
    m_dataCache.DataChangeId = m_chartData->ChangeId() - 1;
  }

  ChartDataView::~ChartDataView() = default;


  uint64_t ChartDataView::ChangeId() const noexcept
  {
    // We query the actual data change id here so we report changes to it right away
    const uint64_t dataChangeId = m_chartData->ChangeId();
    const uint64_t viewChangeId = m_viewChangeId;
    return (dataChangeId | (viewChangeId << 32));
  }

  void ChartDataView::ClearCustomMinMax()
  {
    if (m_customViewMinMax.has_value())
    {
      m_customViewMinMax = std::optional<MinMax<value_type>>();
      ++m_viewChangeId;
    }
  }

  void ChartDataView::SetCustomMinMax(MinMax<value_type> customViewMinMax)
  {
    if (customViewMinMax != m_customViewMinMax)
    {
      m_customViewMinMax = customViewMinMax;
      ++m_viewChangeId;
    }
  }

  void ChartDataView::SetMaxViewEntries(const uint32_t maxEntries)
  {
    if (m_viewMaxEntries != maxEntries)
    {
      m_viewMaxEntries = maxEntries;
      ++m_viewChangeId;
      m_viewConfig = m_chartData->CreateViewConfig(maxEntries, true);
    }
  }

  ChartDataStats ChartDataView::CalculateDataStats() const
  {
    if (m_customViewMinMax.has_value())
    {
      return ChartDataStats(m_customViewMinMax.value());
    }
    const AChartData* pChartData = m_chartData.get();
    assert(pChartData != nullptr);
    auto currentDataChangeId = pChartData->ChangeId();
    if (currentDataChangeId != m_dataCache.DataChangeId)
    {
      // the change id was modified so recalculate the stats
      m_dataCache.Stats = pChartData->CalculateDataStats(m_viewConfig);
      m_dataCache.DataChangeId = currentDataChangeId;
    }
    // return our cached value
    return m_dataCache.Stats;
  }


  uint32_t ChartDataView::Count() const noexcept
  {
    return m_chartData->Count(m_viewConfig);
  }


  ChartDataInfo ChartDataView::DataInfo() const
  {
    return m_chartData->DataInfo(m_viewConfig);
  }


  ReadOnlySpan<ChartDataEntry> ChartDataView::SegmentDataAsReadOnlySpan(const uint32_t segmentIndex) const
  {
    return m_chartData->SegmentDataAsReadOnlySpan(m_viewConfig, segmentIndex);
  }


  DataBinding::DataBindingInstanceHandle ChartDataView::GetSourceInstanceHandle() const
  {
    return m_chartData->GetSourceInstanceHandle();
  }


  ChartChannelMetaDataInfo ChartDataView::GetChannelMetaDataInfo(const uint32_t channelIndex) const
  {
    if (channelIndex >= m_channelCount)
    {
      throw std::out_of_range("GetChannelMetaData index out of range");
    }
    const auto& localEntry = m_customChannelMetaData[channelIndex];

    if (localEntry.IsSetLabel && localEntry.IsSetColor)
    {
      return {StringViewLiteUtil::AsStringViewLite(localEntry.MetaData.Label), localEntry.MetaData.PrimaryColor};
    }
    auto remoteEntry = m_chartData->GetChannelMetaDataInfo(channelIndex);

    return {localEntry.IsSetLabel ? StringViewLiteUtil::AsStringViewLite(localEntry.MetaData.Label) : remoteEntry.Label,
            localEntry.IsSetColor ? localEntry.MetaData.PrimaryColor : remoteEntry.PrimaryColor};
  }


  void ChartDataView::SetChannelMetaData(const uint32_t channelIndex, const ChartChannelMetaData& metaData)
  {
    if (channelIndex >= m_channelCount)
    {
      throw std::out_of_range("GetChannelMetaData index out of range");
    }
    m_customChannelMetaData[channelIndex] = ChannelMetaDataRecord(metaData);
  }

  void ChartDataView::SetChannelMetaData(const uint32_t channelIndex, const StringViewLite label)
  {
    if (channelIndex >= m_channelCount)
    {
      throw std::out_of_range("GetChannelMetaData index out of range");
    }
    m_customChannelMetaData[channelIndex].SetLabel(label);
  }

  void ChartDataView::SetChannelMetaData(const uint32_t channelIndex, const Color primaryColor)
  {
    if (channelIndex >= m_channelCount)
    {
      throw std::out_of_range("GetChannelMetaData index out of range");
    }
    m_customChannelMetaData[channelIndex].SetPrimaryColor(primaryColor);
  }


  void ChartDataView::ClearChannelMetaData(const uint32_t channelIndex)
  {
    if (channelIndex >= m_channelCount)
    {
      throw std::out_of_range("GetChannelMetaData index out of range");
    }
    m_customChannelMetaData[channelIndex] = {};
  }

}
