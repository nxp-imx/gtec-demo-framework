/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <Shared/System/OnDemandRendering/Chart/AreaChartData.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <algorithm>
#include <fmt/format.h>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      StringViewLite DoGetDescriptionString(fmt::memory_buffer& rScratchPad, const uint32_t microseconds)
      {
        rScratchPad.clear();

        if (microseconds < 1000)
        {
          // fmt::format_to(rScratchPad, "{}us", microseconds);
          fmt::format_to(std::back_inserter(rScratchPad), u8"{}\u03BCs", microseconds);
        }
        else if (microseconds > 0)
        {
          fmt::format_to(std::back_inserter(rScratchPad), "{:.2f}ms ({:.1f}fps)", microseconds / 1000.0, 1000000.0 / microseconds);
        }
        else
        {
          fmt::format_to(std::back_inserter(rScratchPad), "{:.2f}ms", microseconds / 1000.0);
        }
        return StringViewLite(rScratchPad.data(), rScratchPad.size());
      }

      std::string DoGetDescriptionString(const uint32_t microseconds)
      {
        fmt::memory_buffer buffer;
        auto strView = DoGetDescriptionString(buffer, microseconds);
        return StringViewLiteUtil::ToString(strView);
      }

      struct ChartGridLineRecord
      {
        uint32_t Microseconds{0};
        std::string Label;

        explicit ChartGridLineRecord(const uint32_t microseconds)
          : Microseconds(microseconds)
          , Label(DoGetDescriptionString(microseconds))
        {
        }
      };


      const std::array<ChartGridLineRecord, 21> g_grid{ChartGridLineRecord(1),      ChartGridLineRecord(10),     ChartGridLineRecord(25),
                                                       ChartGridLineRecord(50),     ChartGridLineRecord(100),    ChartGridLineRecord(250),
                                                       ChartGridLineRecord(500),    ChartGridLineRecord(1000),   ChartGridLineRecord(2000),
                                                       ChartGridLineRecord(3000),   ChartGridLineRecord(5000),   ChartGridLineRecord(6667),
                                                       ChartGridLineRecord(8333),   ChartGridLineRecord(10000),  ChartGridLineRecord(16667),
                                                       ChartGridLineRecord(33333),  ChartGridLineRecord(50000),  ChartGridLineRecord(100000),
                                                       ChartGridLineRecord(200000), ChartGridLineRecord(500000), ChartGridLineRecord(1000000)};

    }

    AreaChartData::AreaChartData(const uint32_t entries, const uint32_t complexDataEntries)
      : m_buffer(entries)
      , m_complexDataEntries(complexDataEntries)
    {
      static_assert(std::tuple_size<decltype(m_gridLines)>() == std::tuple_size<decltype(g_grid)>(), "the two arrays must have the same size");

      if (complexDataEntries > std::tuple_size<ChartComplexDataEntry::array_type>())
      {
        throw std::invalid_argument("complex data entries exceeds capacity");
      }
      for (std::size_t i = 0; i < g_grid.size(); ++i)
      {
        m_gridLines[i] = ChartGridLineInfo(g_grid[i].Microseconds, StringViewLiteUtil::AsStringViewLite(g_grid[i].Label));
      }
      m_cachedInfo.ComplexDataEntries = m_complexDataEntries;
    }


    void AreaChartData::Clear()
    {
      m_buffer.clear();
      m_cachedGridInfo = {};
      m_cachedInfo = {};
      m_cachedInfo.ComplexDataEntries = m_complexDataEntries;
      m_viewInfo.Clear();
    }


    void AreaChartData::Append(const ChartComplexDataEntry& value)
    {
      const uint32_t valueMax = CalcMax(value);
      if (m_buffer.size() == m_buffer.capacity())
      {
        if (valueMax < m_viewInfo.CurrentMax && !m_buffer.empty() && CalcMax(m_buffer.front()) >= m_viewInfo.CurrentMax)
        {
          m_buffer.pop_front();
          m_viewInfo.SetMax(RebuildMax());
        }
      }

      m_buffer.push_back(value);

      m_cachedInfo.Count = UncheckedNumericCast<uint32_t>(m_buffer.size());
      m_viewInfo.SetMax(std::max(m_viewInfo.CurrentMax, valueMax));
      m_cachedInfo.SegmentCount = m_buffer.segment_count();
      m_cachedGridInfo.ViewMinMax.SetMax(m_viewInfo.CurrentMax);
    }


    uint32_t AreaChartData::GetWindowMaxSize() const
    {
      return UncheckedNumericCast<uint32_t>(m_buffer.size());
    }


    ChartGridInfo AreaChartData::GridInfo() const
    {
      return m_cachedGridInfo;
    }


    ReadOnlySpan<ChartGridLineInfo> AreaChartData::GetSuggestedGridLines(const MinMax<uint32_t> minMax) const
    {
      if (minMax.Min() > m_gridLines.back().Position || minMax.Max() < m_gridLines.front().Position)
      {
        // Outside our range
        return ReadOnlySpan<ChartGridLineInfo>();
      }

      const uint32_t min = minMax.Min();
      const uint32_t max = minMax.Max();
      const auto itrFindMax =
        std::find_if(m_gridLines.begin(), m_gridLines.end(), [max](const ChartGridLineInfo& entry) { return entry.Position > max; });
      const auto itrFindMin = std::find_if(m_gridLines.begin(), itrFindMax, [min](const ChartGridLineInfo& entry) { return entry.Position >= min; });
      const std::size_t indexFirst = std::distance(m_gridLines.begin(), itrFindMin);
      const std::size_t indexLast = std::distance(m_gridLines.begin(), itrFindMax);

      return ReadOnlySpanUtil::AsSpan(m_gridLines, indexFirst, (indexLast - indexFirst));
    }


    StringViewLite AreaChartData::GetDescriptionString(const uint32_t microseconds)
    {
      return DoGetDescriptionString(m_scratchPad, microseconds);
    }


    void AreaChartData::SetWindowMaxSize(const uint32_t desiredNewCapacity)
    {
      const uint32_t newCapacity = std::max(desiredNewCapacity, uint32_t(1));
      if (newCapacity != m_buffer.capacity())
      {
        m_buffer.resize_pop_front(newCapacity);
        m_viewInfo.SetMax(RebuildMax());
        m_cachedInfo.SegmentCount = m_buffer.segment_count();
        m_cachedGridInfo.ViewMinMax.SetMax(m_viewInfo.CurrentMax);
      }
    }


    ChartComplexDataInfo AreaChartData::DataInfo() const
    {
      return m_cachedInfo;
    }


    ReadOnlySpan<ChartComplexDataEntry> AreaChartData::DataAsReadOnlySpan(const uint32_t segmentIndex) const
    {
      assert(segmentIndex < m_cachedInfo.SegmentCount);
      return m_buffer.AsReadOnlySpan(segmentIndex);
    }


    uint32_t AreaChartData::CalcMax(const ChartComplexDataEntry& entry) const
    {
      static_assert(std::tuple_size<ChartComplexDataEntry::array_type>() <= 0xFFFFFFFF, "array size assumption failed");
      uint32_t sum = 0;
      for (uint32_t i = 0; i < m_complexDataEntries; ++i)
      {
        sum += entry.Values[i];
      }
      return sum;
    }


    uint32_t AreaChartData::RebuildMax() const
    {
      uint32_t max = 0;
      for (uint32_t segmentIndex = 0; segmentIndex < m_buffer.segment_count(); ++segmentIndex)
      {
        auto span = m_buffer.AsReadOnlySpan(segmentIndex);
        for (std::size_t i = 0; i < span.size(); ++i)
        {
          const uint32_t valMax = CalcMax(span[i]);
          if (valMax > max)
          {
            max = valMax;
          }
        }
      }
      return max;
    }
  }
}
