#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_BOXPLOTDRAWDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_BOXPLOTDRAWDATA_HPP
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
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Controls/Charts/Canvas/ChartCanvas1D.hpp>
#include <FslSimpleUI/Controls/Charts/Data/BoxPlotData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataLimits.hpp>
#include <FslSimpleUI/Controls/Charts/Render/BoxPlotDrawInfo.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>
#include <array>
#include <cassert>

namespace Fsl::UI::Render
{
  class BoxPlotDrawData : public ICustomDrawData
  {
  public:
    struct ChannelRecord
    {
      BoxPlotData BoxPlot;
      Color PrimaryColor;
    };

  private:
    std::array<ChannelRecord, ChartDataLimits::MaxChannels> m_channels;

    uint32_t m_channelEntries{0};

  public:
    uint64_t ChangeId{0};
    Render::BoxPlotDrawInfo DrawInfo;
    ChartCanvas1D Canvas;
    Color MedianColor;


    explicit BoxPlotDrawData(const SpriteUnitConverter& unitConverter)
      : Canvas(unitConverter)
    {
    }

    ReadOnlySpan<ChannelRecord> AsReadOnlySpan() const noexcept
    {
      return ReadOnlySpanUtil::AsSpan(m_channels, 0, m_channelEntries, OptimizationCheckFlag::NoCheck);
    }

    bool IsEmpty() const noexcept
    {
      return m_channelEntries <= 0u;
    }

    void Clear() noexcept
    {
      m_channelEntries = 0;
    }

    void Add(const BoxPlotData& boxPlotData, const Color primaryColor) noexcept
    {
      assert(m_channelEntries < m_channels.size());
      m_channels[m_channelEntries].BoxPlot = boxPlotData;
      m_channels[m_channelEntries].PrimaryColor = primaryColor;
      ++m_channelEntries;
    }
  };
}


#endif
