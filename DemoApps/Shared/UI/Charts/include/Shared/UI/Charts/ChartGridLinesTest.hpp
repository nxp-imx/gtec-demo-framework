#ifndef SHARED_UI_CHARTS_CHARTGRIDLINESTEST_HPP
#define SHARED_UI_CHARTS_CHARTGRIDLINESTEST_HPP
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

#include <FslBase/String/StringViewLite.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/ChartGridLineInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/IChartGridLines.hpp>
#include <fmt/format.h>
#include <array>

namespace Fsl
{
  class ChartGridLinesTest final : public UI::IChartGridLines
  {
    // This is used early so it ought to be one of the very first things constructed
    fmt::memory_buffer m_scratchPad;

    std::array<UI::ChartGridLineInfo, 31> m_gridLines;


  public:
    //! @param complexDataEntries the number of entries in ChartComplexDataEntry that are valid
    explicit ChartGridLinesTest();

    // From IChartGridLines
    ReadOnlySpan<UI::ChartGridLineInfo> GetSuggestedGridLines(const MinMax<uint32_t> minMax) const final;
    StringViewLite GetDescriptionString(const uint32_t microseconds) final;
  };
}


#endif
