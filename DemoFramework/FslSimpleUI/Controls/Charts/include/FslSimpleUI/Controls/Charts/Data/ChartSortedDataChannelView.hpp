#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTSORTEDDATACHANNELVIEW_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_DATA_CHARTSORTEDDATACHANNELVIEW_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <memory>
#include <vector>

namespace Fsl::UI
{
  class ChartDataView;

  /// <summary>
  /// </summary>
  class ChartSortedDataChannelView final
  {
    std::shared_ptr<ChartDataView> m_dataView;
    uint32_t m_dataChannelIndex;

    mutable uint64_t m_cachedViewChangeId{0};
    mutable MinMax<uint32_t> m_cachedAxisRange;
    mutable std::vector<uint32_t> m_cachedSortedData;

  public:
    // Request that the compiler deletes the copy constructor and assignment operator
    ChartSortedDataChannelView(const ChartSortedDataChannelView&) = delete;
    ChartSortedDataChannelView& operator=(const ChartSortedDataChannelView&) = delete;

    // move assignment operator
    ChartSortedDataChannelView& operator=(ChartSortedDataChannelView&& other) noexcept;

    // move constructor
    ChartSortedDataChannelView(ChartSortedDataChannelView&& other) noexcept;

    ChartSortedDataChannelView(std::shared_ptr<ChartDataView> dataView, const uint32_t dataChannelIndex);
    ~ChartSortedDataChannelView();

    uint64_t ChangeId() const noexcept
    {
      return m_cachedViewChangeId;
    }

    MinMax<uint32_t> GetAxisRange() const;
    ReadOnlySpan<uint32_t> GetChannelViewSpan() const;

  private:
    void RefreshCacheIfNecessary() const;
  };
}


#endif
