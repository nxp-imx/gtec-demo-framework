#ifndef SHARED_GPUTIMESTAMP_CHART_AREACHARTDATA_HPP
#define SHARED_GPUTIMESTAMP_CHART_AREACHARTDATA_HPP
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

#include <FslBase/String/StringViewLite.hpp>
//#include <FslBase/Transition/TransitionCache.hpp>
//#include <FslBase/Transition/TransitionValue.hpp>
#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <FslSimpleUI/Controls/Charts/Data/IChartComplexDataWindow.hpp>
#include <array>
#include <fmt/format.h>

namespace Fsl
{
  namespace UI
  {
    class AreaChartData final : public IChartComplexDataWindow
    {
      struct Record
      {
        uint32_t CurrentMax{0};

        void Clear()
        {
          CurrentMax = 0;
        }
        void SetMax(const uint32_t max)
        {
          if (max != CurrentMax)
          {
            CurrentMax = max;
          }
        }
      };

      // This is used early so it ought to be one of the very first things constructed
      fmt::memory_buffer m_scratchPad;

      std::array<ChartGridLineInfo, 21> m_gridLines;

      CircularFixedSizeBuffer<ChartComplexDataEntry> m_buffer;

      uint32_t m_complexDataEntries;

      Record m_viewInfo;
      ChartGridInfo m_cachedGridInfo;
      ChartComplexDataInfo m_cachedInfo;

    public:
      //! @param complexDataEntries the number of entries in ChartComplexDataEntry that are valid
      explicit AreaChartData(const uint32_t entries, const uint32_t complexDataEntries);

      void Clear();
      void Append(const ChartComplexDataEntry& value);

      uint32_t GetWindowMaxSize() const;

      // From IChartDataGridLines
      ChartGridInfo GridInfo() const final;
      ReadOnlySpan<ChartGridLineInfo> GetSuggestedGridLines(const MinMax<uint32_t> minMax) const final;
      StringViewLite GetDescriptionString(const uint32_t microseconds) final;

      // From IChartComplexDataWindow
      void SetWindowMaxSize(const uint32_t desiredNewCapacity) final;
      ChartComplexDataInfo DataInfo() const final;
      ReadOnlySpan<ChartComplexDataEntry> DataAsReadOnlySpan(const uint32_t segmentIndex) const final;

    private:
      uint32_t CalcMax(const ChartComplexDataEntry& entry) const;
      uint32_t RebuildMax() const;
      // void RebuildGridLines();
      // void UpdateCachedMax();
    };
  }
}


#endif
