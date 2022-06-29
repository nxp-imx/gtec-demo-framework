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
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <fmt/format.h>
#include <algorithm>

namespace Fsl::UI
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


    const std::array<ChartGridLineRecord, 21> g_grid{
      ChartGridLineRecord(1),      ChartGridLineRecord(10),    ChartGridLineRecord(25),     ChartGridLineRecord(50),     ChartGridLineRecord(100),
      ChartGridLineRecord(250),    ChartGridLineRecord(500),   ChartGridLineRecord(1000),   ChartGridLineRecord(2000),   ChartGridLineRecord(3000),
      ChartGridLineRecord(5000),   ChartGridLineRecord(6667),  ChartGridLineRecord(8333),   ChartGridLineRecord(10000),  ChartGridLineRecord(16667),
      ChartGridLineRecord(33333),  ChartGridLineRecord(50000), ChartGridLineRecord(100000), ChartGridLineRecord(200000), ChartGridLineRecord(500000),
      ChartGridLineRecord(1000000)};
  }

  ChartGridLinesFps::ChartGridLinesFps()
  {
    static_assert(std::tuple_size<decltype(m_gridLines)>() == std::tuple_size<decltype(g_grid)>(), "the two arrays must have the same size");

    for (std::size_t i = 0; i < g_grid.size(); ++i)
    {
      m_gridLines[i] = ChartGridLineInfo(g_grid[i].Microseconds, StringViewLiteUtil::AsStringViewLite(g_grid[i].Label));
    }
  }


  ReadOnlySpan<ChartGridLineInfo> ChartGridLinesFps::GetSuggestedGridLines(const MinMax<uint32_t> minMax) const
  {
    if (minMax.Min() > m_gridLines.back().Position || minMax.Max() < m_gridLines.front().Position)
    {
      // Outside our range
      return {};
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


  StringViewLite ChartGridLinesFps::GetDescriptionString(const uint32_t microseconds)
  {
    return DoGetDescriptionString(m_scratchPad, microseconds);
  }
}
