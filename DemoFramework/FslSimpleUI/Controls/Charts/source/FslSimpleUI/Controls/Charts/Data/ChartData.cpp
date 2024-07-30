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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Object/ObservableDataSourceObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <limits>

namespace Fsl::UI
{
  using TClass = ChartData;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyLatestEntry = TFactory::CreateReadOnly<ChartDataEntry, TClass, &TClass::GetLatestEntry>("LatestEntry");
}

namespace Fsl::UI
{
  ChartData::ChartData(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding, const uint32_t entries, const uint32_t dataChannelCount,
                       const Constraints constraints)
    : AChartData(dataBinding)
    , m_buffer(entries > 0 ? entries : 1u)
    , m_dataChannelCount(dataChannelCount)
    , m_constraints(constraints)
  {
    if (dataChannelCount > std::tuple_size<ChartDataEntry::array_type>())
    {
      throw std::invalid_argument("data entries exceeds capacity");
    }
    for (auto& rEntry : m_channelMetaData)
    {
      rEntry.PrimaryColor = UIColors::White();
    }
    UpdateCachedValues({});
  }


  void ChartData::Clear()
  {
    m_buffer.clear();
    MarkAsChanged();
    m_cachedDataStats = {};
    m_viewInfo.Clear();
    UpdateCachedValues({});
  }


  void ChartData::Append(const ChartDataEntry& value)
  {
    const auto currentValue = CalcSum(value, m_dataChannelCount);
    if (!m_buffer.empty() && m_buffer.size() == m_buffer.capacity())
    {
      const bool canAffectMinCache = currentValue > m_viewInfo.CurrentMin;
      const bool canAffectMaxCache = currentValue < m_viewInfo.CurrentMax;
      if (canAffectMinCache || canAffectMaxCache)
      {
        const auto frontValue = CalcSum(m_buffer.front(), m_dataChannelCount);
        if (frontValue <= m_viewInfo.CurrentMin || frontValue >= m_viewInfo.CurrentMax)
        {
          m_buffer.pop_front();
          m_viewInfo.SetMinMax(CalculateMinMax());
        }
      }
    }

    if (m_buffer.empty())
    {
      // As the buffer was empty, the current value becomes the min max
      m_viewInfo.SetMinMax(MinMax<value_type>(currentValue, currentValue));
    }

    m_buffer.push_back(value);
    MarkAsChanged();

    UpdateCachedValues(MinMax<value_type>(std::min(m_viewInfo.CurrentMin, currentValue), std::max(m_viewInfo.CurrentMax, currentValue)));
  }


  void ChartData::ClearCustomMinMax()
  {
    if (m_customViewMinMax.has_value())
    {
      m_customViewMinMax = std::optional<MinMax<value_type>>();
      MarkAsChanged();
    }
  }

  void ChartData::SetCustomMinMax(MinMax<value_type> customViewMinMax)
  {
    if (customViewMinMax != m_customViewMinMax)
    {
      m_customViewMinMax = customViewMinMax;
      MarkAsChanged();
    }
  }


  uint32_t ChartData::GetSize() const
  {
    return UncheckedNumericCast<uint32_t>(m_buffer.size());
  }


  uint32_t ChartData::GetCapacity() const
  {
    return UncheckedNumericCast<uint32_t>(std::min(m_buffer.capacity(), UncheckedNumericCast<std::size_t>(std::numeric_limits<uint32_t>::max())));
  }


  void ChartData::SetCapacity(const uint32_t capacity)
  {
    const uint32_t newCapacity = std::max(capacity, static_cast<uint32_t>(1));
    if (newCapacity < m_buffer.capacity())
    {
      m_buffer.resize_pop_front(newCapacity);
      auto newMinMax = CalculateMinMax();
      UpdateCachedValues(newMinMax);
      MarkAsChanged();
    }
    else if (newCapacity > m_buffer.capacity())
    {
      m_buffer.grow(newCapacity);
    }
  }

  ChartDataStats ChartData::CalculateDataStats() const
  {
    return !m_customViewMinMax.has_value() ? m_cachedDataStats : ChartDataStats(m_customViewMinMax.value());
  }


  void ChartData::SetChannelMetaData(const uint32_t channelIndex, const ChartChannelMetaData& metaData)
  {
    auto& rEntry = m_channelMetaData.at(channelIndex);
    bool changed = metaData != rEntry;
    if (changed)
    {
      rEntry = metaData;
      MarkAsChanged();
    }
  }

  void ChartData::SetChannelMetaData(const uint32_t channelIndex, const StringViewLite label)
  {
    auto& rEntry = m_channelMetaData.at(channelIndex);
    bool changed = StringViewLite(rEntry.Label) != label;
    if (changed)
    {
      rEntry.Label = label;
      MarkAsChanged();
    }
  }

  void ChartData::SetChannelMetaData(const uint32_t channelIndex, const UIColor primaryColor)
  {
    auto& rEntry = m_channelMetaData.at(channelIndex);
    bool changed = rEntry.PrimaryColor != primaryColor;
    if (changed)
    {
      rEntry.PrimaryColor = primaryColor;
      MarkAsChanged();
    }
  }


  ChartDataViewConfig ChartData::CreateViewConfig()
  {
    // This is basically equal to no limit
    return ChartDataViewConfig(std::numeric_limits<uint32_t>::max());
  }


  ChartDataViewConfig ChartData::CreateViewConfig(const uint32_t maxEntries, const bool allowCapacityToGrow)
  {
    if (allowCapacityToGrow && maxEntries > m_buffer.capacity())
    {
      SetCapacity(maxEntries);
    }
    return ChartDataViewConfig(maxEntries);
  }


  ChartDataStats ChartData::CalculateDataStats(const ChartDataViewConfig viewConfig) const
  {
    if (m_customViewMinMax.has_value())
    {
      return ChartDataStats(m_customViewMinMax.value());
    }
    if (viewConfig.MaxEntries >= m_buffer.size())
    {
      return m_cachedDataStats;
    }
    // This view shows less entries than we have cached stats for so we need to calculate some custom stats for the view.
    auto minMax = ApplyConstraints(CalculateMinMax(viewConfig.MaxEntries));
    return ChartDataStats(minMax);
  }

  uint32_t ChartData::Count(const ChartDataViewConfig viewConfig) const noexcept
  {
    return viewConfig.MaxEntries >= m_buffer.size() ? UncheckedNumericCast<uint32_t>(m_buffer.size()) : viewConfig.MaxEntries;
  }


  ChartDataInfo ChartData::DataInfo(const ChartDataViewConfig viewConfig) const
  {
    if (viewConfig.MaxEntries <= 0u)
    {
      return {0u, 0u, m_dataChannelCount};
    }
    if (viewConfig.MaxEntries >= m_buffer.size())
    {
      // There are more or equal amounts of entries in this view so just give access to all data
      return {UncheckedNumericCast<uint32_t>(m_buffer.size()), m_buffer.segment_count(), m_dataChannelCount};
    }
    // if there are a non zero amount of entries in the buffer there should be at least one segment
    assert(m_buffer.segment_count() != 0);
    // We only expect there to ever be 0,1 or two segments, and we already check for zero.
    assert(m_buffer.segment_count() == 1u || m_buffer.segment_count() == 2u);

    if (m_buffer.segment_count() == 1u)
    {
      return {viewConfig.MaxEntries, 1u, m_dataChannelCount};
    }
    // There are two segments, so we need to determine if the last segment contains everything
    assert(m_buffer.segment_count() == 2u);
    auto lastSpan = m_buffer.AsReadOnlySpan(1u);
    return {viewConfig.MaxEntries, viewConfig.MaxEntries <= lastSpan.size() ? 1u : 2u, m_dataChannelCount};
  }


  ReadOnlySpan<ChartDataEntry> ChartData::SegmentDataAsReadOnlySpan(const ChartDataViewConfig viewConfig, const uint32_t segmentIndex) const
  {
    if (viewConfig.MaxEntries <= 0u)
    {
      return {};
    }
    if (viewConfig.MaxEntries >= m_buffer.size())
    {
      // There are more or equal amounts of entries in this view so just give access to all data
      return m_buffer.AsReadOnlySpan(segmentIndex);
    }

    // if there are a non zero amount of entries in the buffer there should be at least one segment
    assert(m_buffer.segment_count() != 0);
    // We only expect there to ever be 0,1 or two segments, and we already check for zero.
    assert(m_buffer.segment_count() == 1u || m_buffer.segment_count() == 2u);

    if (m_buffer.segment_count() <= 1u)
    {
      assert(segmentIndex == 0u);
      auto lastSpan = m_buffer.AsReadOnlySpan(0u);
      if (viewConfig.MaxEntries <= lastSpan.size())
      {
        return lastSpan.subspan(lastSpan.size() - viewConfig.MaxEntries, viewConfig.MaxEntries);
      }
    }

    // So there are two segments, so we need to determine if the last segment contains everything
    assert(m_buffer.segment_count() == 2u);
    auto lastSpan = m_buffer.AsReadOnlySpan(1u);
    if (viewConfig.MaxEntries <= lastSpan.size())
    {
      assert(segmentIndex == 0u);
      return lastSpan.subspan(lastSpan.size() - viewConfig.MaxEntries, viewConfig.MaxEntries);
    }
    if (segmentIndex == 1u)
    {
      assert(lastSpan.size() < viewConfig.MaxEntries);
      return lastSpan;
    }
    assert(viewConfig.MaxEntries >= lastSpan.size());
    auto entriesLeft = viewConfig.MaxEntries - UncheckedNumericCast<uint32_t>(lastSpan.size());
    auto span = m_buffer.AsReadOnlySpan(0u);
    return span.subspan(span.size() - entriesLeft, entriesLeft);
  }


  ChartChannelMetaDataInfo ChartData::GetChannelMetaDataInfo(const uint32_t channelIndex) const
  {
    const auto& entry = m_channelMetaData.at(channelIndex);
    return {StringViewLite(entry.Label), entry.PrimaryColor};
  }


  ChartDataEntry ChartData::GetLatestEntry() const noexcept
  {
    return m_propertyLatestEntry.Get();
  }

  DataBinding::DataBindingInstanceHandle ChartData::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::ObservableDataSourceObjectHelper::TryGetPropertyHandle(
      this, ThisDataSourceObject(), sourceDef, DataBinding::PropLinkRefs(PropertyLatestEntry, m_propertyLatestEntry));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  void ChartData::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyLatestEntry);
  }


  void ChartData::UpdateCachedValues(const MinMax<value_type> minMax)
  {
    auto constrainedMinMax = ApplyConstraints(minMax);
    m_viewInfo.SetMinMax(constrainedMinMax);
    m_cachedDataStats.ValueMinMax = constrainedMinMax;
  }


  MinMax<ChartData::value_type> ChartData::CalculateMinMax() const noexcept
  {
    if (m_buffer.empty())
    {
      return MinMax<value_type>(0, 0);
    }

    value_type min = std::numeric_limits<value_type>::max();
    value_type max = std::numeric_limits<value_type>::min();
    for (uint32_t segmentIndex = 0; segmentIndex < m_buffer.segment_count(); ++segmentIndex)
    {
      auto span = m_buffer.AsReadOnlySpan(segmentIndex);
      assert(span.size() > 0);
      auto minMax = CalcSpanMinMax(span, m_dataChannelCount, min, max);
      min = minMax.Min();
      max = minMax.Max();
    }
    return MinMax<value_type>(min, max);
  }


  MinMax<ChartData::value_type> ChartData::CalculateMinMax(const uint32_t maxEntries) const noexcept
  {
    if (m_buffer.empty() || maxEntries <= 0)
    {
      return MinMax<value_type>(0, 0);
    }
    if (maxEntries >= m_buffer.size())
    {
      return CalculateMinMax();
    }

    // We only expect there to ever be 0,1 or two segments, and we already check for zero.
    assert(m_buffer.segment_count() == 1u || m_buffer.segment_count() == 2u);

    value_type min = std::numeric_limits<value_type>::max();
    value_type max = std::numeric_limits<value_type>::min();

    uint32_t entriesLeft = maxEntries;
    uint32_t segmentIndex = m_buffer.segment_count();
    while (segmentIndex > 0 && entriesLeft > 0)
    {
      --segmentIndex;
      auto span = m_buffer.AsReadOnlySpan(segmentIndex);
      if (entriesLeft < span.size())
      {
        span = span.unchecked_subspan(span.size() - entriesLeft, entriesLeft);
      }
      assert(span.size() > 0);
      auto minMax = CalcSpanMinMax(span, m_dataChannelCount, min, max);
      assert(entriesLeft >= span.size());
      entriesLeft -= UncheckedNumericCast<uint32_t>(span.size());
      min = minMax.Min();
      max = minMax.Max();
    }
    return MinMax<value_type>(min, max);
  }


  MinMax<ChartData::value_type> ChartData::ApplyConstraints(const MinMax<value_type> minMax) const
  {
    auto min = minMax.Min();
    auto max = minMax.Max();
    if (m_constraints.MaximumMin.has_value())
    {
      min = std::min(min, m_constraints.MaximumMin.value());
    }
    if (m_constraints.MinimumMax.has_value())
    {
      max = std::max(max, m_constraints.MinimumMax.value());
    }

    return MinMax<ChartData::value_type>(min, max);
  }


  MinMax<ChartData::value_type> ChartData::CalcSpanMinMax(ReadOnlySpan<ChartDataEntry> span, const uint32_t dataEntries, value_type min,
                                                          value_type max) noexcept
  {
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      const value_type val = CalcSum(span[i], dataEntries);
      if (val < min)
      {
        min = val;
      }
      if (val > max)
      {
        max = val;
      }
    }
    return MinMax<value_type>(min, max);
  }


  ChartData::value_type ChartData::CalcSum(const ChartDataEntry& entry, const uint32_t dataEntries) noexcept
  {
    static_assert(std::tuple_size<ChartDataEntry::array_type>() <= 0xFFFFFFFF, "array size assumption failed");
    value_type sum = 0;
    for (uint32_t i = 0; i < dataEntries; ++i)
    {
      sum += entry.Values[i];
    }
    return sum;
  }


  void ChartData::MarkAsChanged()
  {
    ++m_changeId;
    if (!m_buffer.empty())
    {
      m_propertyLatestEntry.Set(ThisDataSourceObject(), m_buffer.back(), DataBinding::PropertyChangeReason::Modified);
    }
    else
    {
      m_propertyLatestEntry.Set(ThisDataSourceObject(), ChartDataEntry(), DataBinding::PropertyChangeReason::Modified);
    }

    // Let the data binding system know that the content changed
    MarkAsChangedNow();
  }
}
