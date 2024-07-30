#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTDATA_HPP
/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslDataBinding/Base/Property/TypedReadOnlyDependencyProperty.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Controls/Charts/Data/AChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartChannelMetaData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataStats.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/ChartGridLineInfo.hpp>
#include <fmt/format.h>
#include <array>
#include <optional>

namespace Fsl::UI
{

  class ChartData final : public AChartData
  {
    using base_type = AChartData;

  public:
    using value_type = uint32_t;

    struct Constraints
    {
      //! min will never be greater than this
      std::optional<value_type> MaximumMin;
      //! max will never be less than this
      std::optional<value_type> MinimumMax;

      Constraints() = default;
      Constraints(const std::optional<value_type> maximumMin, std::optional<value_type> minimumMax)
        : MaximumMin(maximumMin)
        , MinimumMax(minimumMax)
      {
      }
    };

  private:
    struct Record
    {
      value_type CurrentMin{0};
      value_type CurrentMax{0};

      void Clear()
      {
        CurrentMin = 0;
        CurrentMax = 0;
      }

      void SetMinMax(const MinMax<value_type> minMax)
      {
        CurrentMin = minMax.Min();
        CurrentMax = minMax.Max();
      }
    };

    CircularFixedSizeBuffer<ChartDataEntry> m_buffer;
    uint32_t m_dataChannelCount;
    uint32_t m_changeId{0};

    Constraints m_constraints;

    Record m_viewInfo;
    ChartDataStats m_cachedDataStats;
    std::optional<MinMax<value_type>> m_customViewMinMax;
    std::array<ChartChannelMetaData, ChartDataLimits::MaxChannels> m_channelMetaData;

    DataBinding::TypedReadOnlyDependencyProperty<ChartDataEntry> m_propertyLatestEntry;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition PropertyLatestEntry;


    //! @param dataEntries the number of entries in ChartDataEntry that are valid
    explicit ChartData(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding, const uint32_t entries, const uint32_t dataChannelCount,
                       const Constraints constraints);

    void Clear();
    void Append(const ChartDataEntry& value);

    std::optional<MinMax<value_type>> GetCustomMixMax() const
    {
      return m_customViewMinMax;
    }

    void ClearCustomMinMax();
    void SetCustomMinMax(const MinMax<value_type> customViewMinMax);

    uint32_t GetSize() const;
    uint32_t GetCapacity() const;
    void SetCapacity(const uint32_t capacity);

    ChartDataStats CalculateDataStats() const;

    void SetChannelMetaData(const uint32_t channelIndex, const ChartChannelMetaData& metaData);
    void SetChannelMetaData(const uint32_t channelIndex, const StringViewLite label);
    void SetChannelMetaData(const uint32_t channelIndex, const UIColor primaryColor);

    // From AChartData
    uint32_t ChannelCount() const noexcept final
    {
      return m_dataChannelCount;
    }

    //! Get the latest change id (this id changes every time the chart data is modified)
    uint32_t ChangeId() const noexcept final
    {
      return m_changeId;
    }

    ChartDataViewConfig CreateViewConfig() final;
    ChartDataViewConfig CreateViewConfig(const uint32_t maxEntries, const bool allowCapacityToGrow) final;
    ChartDataStats CalculateDataStats(const ChartDataViewConfig viewConfig) const final;
    uint32_t Count(const ChartDataViewConfig viewConfig) const noexcept final;
    ChartDataInfo DataInfo(const ChartDataViewConfig viewConfig) const final;
    ReadOnlySpan<ChartDataEntry> SegmentDataAsReadOnlySpan(const ChartDataViewConfig viewConfig, const uint32_t segmentIndex) const final;
    ChartChannelMetaDataInfo GetChannelMetaDataInfo(const uint32_t channelIndex) const final;

    ChartDataEntry GetLatestEntry() const noexcept;

  protected:
    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final;
    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) final;

  private:
    void UpdateCachedValues(const MinMax<value_type> minMax);
    MinMax<value_type> CalculateMinMax() const noexcept;
    MinMax<value_type> CalculateMinMax(const uint32_t maxEntries) const noexcept;
    MinMax<value_type> ApplyConstraints(const MinMax<value_type> minMax) const;
    static MinMax<uint32_t> CalcSpanMinMax(ReadOnlySpan<ChartDataEntry> span, const uint32_t dataEntries, value_type min, value_type max) noexcept;
    static value_type CalcSum(const ChartDataEntry& entry, const uint32_t dataEntries) noexcept;
    void MarkAsChanged();
  };
}


#endif
