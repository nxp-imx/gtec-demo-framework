/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include "CustomControlFactory.hpp"
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <array>
#include <utility>

namespace Fsl::CustomControlFactory
{
  namespace
  {
    namespace LocalProfileColor
    {
      constexpr Color Profile0(0xFF7B9E05);    // green
      constexpr Color Profile1(0xFF882807);    // brown
      constexpr Color Profile2(0xFF614985);    // purple
      constexpr Color Profile3(0xFFC5A206);    // light yellow
      constexpr Color Profile4(0xFF3488A7);    // light blue
      constexpr Color Profile5(0xFFCC7000);    // orange
      constexpr Color Profile6(0xFFA00000);    // red

      // constexpr Color Profile0(0xFF7B9E05); // green
      // constexpr Color Profile1(0xFF3488A7); // light blue
      // constexpr Color Profile2(0xFFCC7000); // orange
      // constexpr Color Profile3(0xFF5AB2BC); // cyan
      // constexpr Color Profile4(0xFF72680C); // dark yellow
      // constexpr Color Profile5(0xFFC5A206); // light yellow
      // constexpr Color Profile6(0xFF882807); // brown
      // constexpr Color Profile7(0xFF614985); // purple
      // constexpr Color Profile8(0xFF7A7B1E); // yellow
    }

    std::array<CpuProfileRecord, 7> g_profileRecords = {
      CpuProfileRecord("Events", LocalProfileColor::Profile0, 6),      CpuProfileRecord("Update", LocalProfileColor::Profile1, 5),
      CpuProfileRecord("Draw", LocalProfileColor::Profile2, 4),        CpuProfileRecord("- Preprocess", LocalProfileColor::Profile3, 3),
      CpuProfileRecord("- GenMeshes", LocalProfileColor::Profile4, 2), CpuProfileRecord("- FillBuffers", LocalProfileColor::Profile5, 1),
      CpuProfileRecord("- Schedule", LocalProfileColor::Profile6, 0)};
  }


  uint32_t MaxCpuProfileDataEntries()
  {
    return UncheckedNumericCast<uint32_t>(g_profileRecords.size());
  }


  ReadOnlySpan<CpuProfileRecord> GetCpuProfileRecords()
  {
    return SpanUtil::AsReadOnlySpan(g_profileRecords);
  }


  CpuLegendRecord CreateCpuLegend(UI::Theme::IThemeControlFactory& uiFactory, const UI::DpLayoutSize1D spacingDp)
  {
    auto context = uiFactory.GetContext();
    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, spacingDp.Value().Value));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, spacingDp.Value().Value));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 70.0f));

    auto profileRecords = CustomControlFactory::GetCpuProfileRecords();
    std::vector<CpuLegendRecord::ControlRecord> controls(profileRecords.size());

    for (uint32_t i = 0; i < profileRecords.size(); ++i)
    {
      layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

      auto uiImage = uiFactory.CreateImage(uiFactory.GetResources().GetColorMarkerNineSliceSprite());
      uiImage->SetAlignmentY(UI::ItemAlignment::Center);
      uiImage->SetContentColor(profileRecords[i].ItemColor);
      auto uiLabel = uiFactory.CreateLabel(profileRecords[i].Label);
      auto uiAverage = uiFactory.CreateFmtValueLabel(0.0f, "{:.2f}ms");
      uiAverage->SetAlignmentX(UI::ItemAlignment::Far);

      layout->AddChild(uiImage, 0, i);
      layout->AddChild(uiLabel, 2, i);
      layout->AddChild(uiAverage, 4, i);

      controls[i].AverageTimeLabel = uiAverage;
    }
    return {layout, std::move(controls)};
  }


  CpuDetailedLegendRecord CreateDetailedCpuLegend(UI::Theme::IThemeControlFactory& uiFactory, const UI::DpLayoutSize1D spacingDp)
  {
    auto context = uiFactory.GetContext();
    auto cpuProfileLabel = uiFactory.CreateLabel("CPU usage");
    auto cpuUiAverage = uiFactory.CreateFmtValueLabel(0.0f, "{:.2f}ms");
    cpuUiAverage->SetAlignmentX(UI::ItemAlignment::Far);


    auto cpuLabelStack = std::make_shared<UI::ComplexStackLayout>(context);
    cpuLabelStack->SetOrientation(UI::LayoutOrientation::Horizontal);
    cpuLabelStack->AddChild(cpuProfileLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));
    cpuLabelStack->AddChild(cpuUiAverage, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    cpuLabelStack->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto cpuUI = CustomControlFactory::CreateCpuLegend(uiFactory, spacingDp);

    auto cpuDetails = std::make_shared<UI::StackLayout>(context);
    cpuDetails->SetOrientation(UI::LayoutOrientation::Vertical);
    cpuDetails->AddChild(cpuLabelStack);
    cpuDetails->AddChild(cpuUI.MainLayout);

    return {cpuDetails, cpuUiAverage, cpuUI};
  }


  std::shared_ptr<UI::AreaChart> CreateAreaChart(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::ChartData>& data,
                                                 const OpaqueHack hack)
  {
    auto context = uiFactory.GetContext();
    auto cpuTimeChart = std::make_shared<UI::AreaChart>(context);
    cpuTimeChart->SetAlignmentX(UI::ItemAlignment::Stretch);
    cpuTimeChart->SetAlignmentY(UI::ItemAlignment::Stretch);
    cpuTimeChart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(hack == OpaqueHack::Disabled));
    cpuTimeChart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
    cpuTimeChart->SetGridLines(std::make_shared<UI::ChartGridLinesFps>());
    cpuTimeChart->SetDataView(data);
    cpuTimeChart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
    cpuTimeChart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
    cpuTimeChart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    auto cpuProfileRecords = CustomControlFactory::GetCpuProfileRecords();
    for (uint32_t i = 0; i < cpuProfileRecords.size(); ++i)
    {
      data->SetChannelMetaData(cpuProfileRecords[i].Index, cpuProfileRecords[i].ItemColor);
    }
    return cpuTimeChart;
  }

  void SetContent(UI::FmtValueLabel<float>* pTarget, const double value)
  {
    if (pTarget != nullptr)
    {
      if (value <= 10000.0)
      {
        pTarget->SetFormatString(reinterpret_cast<const char*>(u8"{:.0f}\u03BCs"));
        pTarget->SetContent(static_cast<float>(value));
      }
      else
      {
        pTarget->SetFormatString("{:.2f}ms");
        pTarget->SetContent(static_cast<float>(value / 1000.0));
      }
    }
  }
}
