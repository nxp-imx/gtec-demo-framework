/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <Shared/Stats/ChartGridLinesCpu.hpp>
#include <fmt/format.h>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    StringViewLite DoGetDescriptionString(fmt::memory_buffer& rScratchPad, const uint32_t value)
    {
      rScratchPad.clear();
      fmt::format_to(std::back_inserter(rScratchPad), "{}", value);
      return StringViewLite(rScratchPad.data(), rScratchPad.size());
    }

    std::string DoGetDescriptionString(const uint32_t microseconds)
    {
      fmt::memory_buffer buffer;
      auto strView = DoGetDescriptionString(buffer, microseconds);
      return std::string(strView);
    }

    struct ChartGridLineRecord
    {
      uint32_t Percentage{0};
      std::string Label;

      explicit ChartGridLineRecord(const uint32_t percentage)
        : Percentage(percentage)
        , Label(DoGetDescriptionString(percentage))
      {
      }
    };


    const std::array<ChartGridLineRecord, 1> g_grid{ChartGridLineRecord(50)};

  }

  ChartGridLinesCpu::ChartGridLinesCpu()
  {
    static_assert(std::tuple_size<decltype(m_gridLines)>() == std::tuple_size<decltype(g_grid)>(), "the two arrays must have the same size");

    for (std::size_t i = 0; i < g_grid.size(); ++i)
    {
      const auto& rec = g_grid[i];
      m_gridLines[i] = UI::ChartGridLineInfo(rec.Percentage, StringViewLite(rec.Label));
    }
  }


  ReadOnlySpan<UI::ChartGridLineInfo> ChartGridLinesCpu::GetSuggestedGridLines(const MinMax<uint32_t> minMax) const
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
      std::find_if(m_gridLines.begin(), m_gridLines.end(), [max](const UI::ChartGridLineInfo& entry) { return entry.Position > max; });
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrFindMin =
      std::find_if(m_gridLines.begin(), itrFindMax, [min](const UI::ChartGridLineInfo& entry) { return entry.Position >= min; });
    const std::size_t indexFirst = std::distance(m_gridLines.begin(), itrFindMin);
    const std::size_t indexLast = std::distance(m_gridLines.begin(), itrFindMax);

    return SpanUtil::AsReadOnlySpan(m_gridLines, indexFirst, (indexLast - indexFirst));
  }


  StringViewLite ChartGridLinesCpu::GetDescriptionString(const uint32_t value)
  {
    return DoGetDescriptionString(m_scratchPad, value);
  }
}
