#ifndef SHARED_UI_BENCHMARK_SCENE_CONTROL_CUSTOMCONTROLFACTORY_HPP
#define SHARED_UI_BENCHMARK_SCENE_CONTROL_CUSTOMCONTROLFACTORY_HPP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include "CpuDetailedLegendRecord.hpp"
#include "CpuLegendRecord.hpp"
#include "CpuProfileRecord.hpp"

namespace Fsl
{
  namespace UI
  {
    class AreaChart;
    class ChartData;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }


  namespace CustomControlFactory
  {
    enum class OpaqueHack
    {
      Disabled,
      Enabled
    };

    uint32_t MaxCpuProfileDataEntries();
    ReadOnlySpan<CpuProfileRecord> GetCpuProfileRecords();

    CpuLegendRecord CreateCpuLegend(UI::Theme::IThemeControlFactory& uiFactory, const UI::DpLayoutSize1D spacingDp);
    CpuDetailedLegendRecord CreateDetailedCpuLegend(UI::Theme::IThemeControlFactory& uiFactory, const UI::DpLayoutSize1D spacingDp);

    // OpaqueHack hack is only here until we ensure that all sprites comes with two materials so the render backend can switch as needed based on
    // color
    std::shared_ptr<UI::AreaChart> CreateAreaChart(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::ChartData>& data,
                                                   const OpaqueHack hack = OpaqueHack::Disabled);

    void SetContent(UI::FmtValueLabel<float>* pTarget, const double value);
  }
}

#endif
