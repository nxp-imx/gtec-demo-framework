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
#include <FslSimpleUI/Controls/Charts/Data/ChartDataEntry.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartSortedDataChannelView.hpp>
#include <algorithm>
#include <limits>
#include <utility>

namespace Fsl::UI
{
  // move assignment operator
  ChartSortedDataChannelView& ChartSortedDataChannelView::operator=(ChartSortedDataChannelView&& other) noexcept
  {
    if (this != &other)
    {
      // Claim ownership here
      m_dataView = std::move(other.m_dataView);
      m_dataChannelIndex = other.m_dataChannelIndex;
      m_cachedViewChangeId = other.m_cachedViewChangeId;
      m_cachedAxisRange = other.m_cachedAxisRange;
      m_cachedSortedData = std::move(other.m_cachedSortedData);

      // Remove the data from other
      other.m_dataChannelIndex = 0;
      other.m_cachedViewChangeId = 0;
      other.m_cachedAxisRange = {};
    }
    return *this;
  }

  // move constructor
  ChartSortedDataChannelView::ChartSortedDataChannelView(ChartSortedDataChannelView&& other) noexcept
    : m_dataView(std::move(other.m_dataView))
    , m_dataChannelIndex(other.m_dataChannelIndex)
    , m_cachedViewChangeId(other.m_cachedViewChangeId)
    , m_cachedAxisRange(other.m_cachedAxisRange)
    , m_cachedSortedData(std::move(other.m_cachedSortedData))
  {
    // Remove the data from other
    other.m_dataChannelIndex = 0;
    other.m_cachedViewChangeId = 0;
    other.m_cachedAxisRange = {};
  }


  ChartSortedDataChannelView::ChartSortedDataChannelView(std::shared_ptr<ChartDataView> dataView, const uint32_t dataChannelIndex)
    : m_dataView(std::move(dataView))
    , m_dataChannelIndex(dataChannelIndex)
  {
    if (!m_dataView)
    {
      throw std::invalid_argument("can not be null");
    }
    if (dataChannelIndex >= m_dataView->DataInfo().ChannelCount)
    {
      throw std::invalid_argument("invalid data channel index");
    }

    // Ensure that we dont have a valid change id
    m_cachedViewChangeId = m_dataView->ChangeId() - 1;
  }

  ChartSortedDataChannelView::~ChartSortedDataChannelView() = default;


  ReadOnlySpan<uint32_t> ChartSortedDataChannelView::GetChannelViewSpan() const
  {
    RefreshCacheIfNecessary();
    return ReadOnlySpanUtil::AsSpan(m_cachedSortedData);
  }


  MinMax<uint32_t> ChartSortedDataChannelView::GetAxisRange() const
  {
    RefreshCacheIfNecessary();
    return m_cachedAxisRange;
  }


  void ChartSortedDataChannelView::RefreshCacheIfNecessary() const
  {
    const ChartDataView* pDataView = m_dataView.get();
    assert(pDataView != nullptr);
    auto currentViewChangeId = m_dataView->ChangeId();
    if (currentViewChangeId != m_cachedViewChangeId)
    {
      // Resort the view data
      m_cachedViewChangeId = currentViewChangeId;

      const auto dataInfo = pDataView->DataInfo();

      m_cachedSortedData.clear();
      m_cachedSortedData.reserve(dataInfo.TotalElementCount);
      if (dataInfo.SegmentCount > 0)
      {
        auto min = std::numeric_limits<uint32_t>::max();
        auto max = std::numeric_limits<uint32_t>::min();
        // Insert all the elements from the view into our cached sorted list (low to high)
        for (uint32_t segmentIndex = 0; segmentIndex < dataInfo.SegmentCount; ++segmentIndex)
        {
          const auto span = pDataView->SegmentDataAsReadOnlySpan(segmentIndex);
          for (uint32_t spanIndex = 0; spanIndex < span.size(); ++spanIndex)
          {
            const auto newValue = span[spanIndex].Values[m_dataChannelIndex];
            m_cachedSortedData.insert(std::lower_bound(m_cachedSortedData.begin(), m_cachedSortedData.end(), newValue), newValue);
            min = min <= newValue ? min : newValue;
            max = max >= newValue ? max : newValue;
          }
        }
        assert(min <= max);
        m_cachedAxisRange = MinMax<uint32_t>(min, max);
      }
      else
      {
        m_cachedAxisRange = {};
      }
    }
  }

}
