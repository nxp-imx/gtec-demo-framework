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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <Shared/System/OnDemandRendering/Chart/CustomChartGridLines.hpp>
#include <fmt/format.h>
#include <algorithm>

namespace Fsl::UI
{
  namespace
  {
    StringViewLite DoGetDescriptionString(fmt::memory_buffer& rScratchPad, const uint32_t callCount)
    {
      rScratchPad.clear();
      if (callCount != 1)
      {
        fmt::format_to(std::back_inserter(rScratchPad), "{} calls", callCount);
      }
      else
      {
        fmt::format_to(std::back_inserter(rScratchPad), "{} call", callCount);
      }
      return StringViewLite(rScratchPad.data(), rScratchPad.size());
    }

    std::string DoGetDescriptionString(const uint32_t callCount)
    {
      fmt::memory_buffer buffer;
      auto strView = DoGetDescriptionString(buffer, callCount);
      return std::string(strView);
    }

    struct ChartGridLineRecord
    {
      uint32_t CallCount{0};
      std::string Label;

      explicit ChartGridLineRecord(const uint32_t callCount)
        : CallCount(callCount)
        , Label(DoGetDescriptionString(callCount))
      {
      }
    };


    const std::array<ChartGridLineRecord, 21> g_grid{
      ChartGridLineRecord(1),  ChartGridLineRecord(2),  ChartGridLineRecord(3),  ChartGridLineRecord(4),  ChartGridLineRecord(5),
      ChartGridLineRecord(6),  ChartGridLineRecord(7),  ChartGridLineRecord(8),  ChartGridLineRecord(9),  ChartGridLineRecord(10),
      ChartGridLineRecord(11), ChartGridLineRecord(12), ChartGridLineRecord(13), ChartGridLineRecord(14), ChartGridLineRecord(15),
      ChartGridLineRecord(16), ChartGridLineRecord(17), ChartGridLineRecord(18), ChartGridLineRecord(19), ChartGridLineRecord(20),
      ChartGridLineRecord(21)};
  }

  CustomChartGridLines::CustomChartGridLines()
  {
    static_assert(std::tuple_size<decltype(m_gridLines)>() == std::tuple_size<decltype(g_grid)>(), "the two arrays must have the same size");

    for (std::size_t i = 0; i < g_grid.size(); ++i)
    {
      m_gridLines[i] = ChartGridLineInfo(g_grid[i].CallCount, StringViewLite(g_grid[i].Label));
    }
  }


  ReadOnlySpan<ChartGridLineInfo> CustomChartGridLines::GetSuggestedGridLines(const MinMax<uint32_t> minMax) const
  {
    if (minMax.Min() > m_gridLines.back().Position || minMax.Max() < m_gridLines.front().Position)
    {
      // Outside our range
      return {};
    }

    const uint32_t min = minMax.Min();
    const uint32_t max = minMax.Max();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrFindMax =
      std::find_if(m_gridLines.begin(), m_gridLines.end(), [max](const ChartGridLineInfo& entry) { return entry.Position > max; });
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrFindMin = std::find_if(m_gridLines.begin(), itrFindMax, [min](const ChartGridLineInfo& entry) { return entry.Position >= min; });
    const std::size_t indexFirst = std::distance(m_gridLines.begin(), itrFindMin);
    const std::size_t indexLast = std::distance(m_gridLines.begin(), itrFindMax);

    return SpanUtil::AsReadOnlySpan(m_gridLines, indexFirst, (indexLast - indexFirst));
  }


  StringViewLite CustomChartGridLines::GetDescriptionString(const uint32_t microseconds)
  {
    return DoGetDescriptionString(m_scratchPad, microseconds);
  }
}
