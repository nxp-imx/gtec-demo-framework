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

#include "ResultScene.hpp"
#include <FslBase/Log/Common/FmtVersionInfo.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxSize1D.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/Activity/ActivityStack.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkData.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/ResultDetailsDialogActivity.hpp>
#include <fmt/chrono.h>
#include <utility>
#include "BenchResultManager.hpp"
#include "Control/CustomControlFactory.hpp"
#include "Control/CustomUIConfig.hpp"
#include "SceneCreateInfo.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr UI::UIColor ChartColor(PackedColor32(0xFF3488A7));    // light blue
    }


    struct AverageRecord
    {
      double UIProcessEvents{0};
      double UIUpdate{0};
      double UIDraw{0};
      double UIDrawPreprocess{0};
      double UIDrawGenMesh{0};
      double UIDrawFillBuffers{0};
      double UIDrawSchedule{0};

      AverageRecord() = default;
      AverageRecord(const double uiProcessEvents, const double uiUpdate, const double uiDraw, const double uiDrawPreprocess,
                    const double uiDrawGenMesh, const double uiDrawFillBuffers, const double uiDrawSchedule)
        : UIProcessEvents(uiProcessEvents)
        , UIUpdate(uiUpdate)
        , UIDraw(uiDraw)
        , UIDrawPreprocess(uiDrawPreprocess)
        , UIDrawGenMesh(uiDrawGenMesh)
        , UIDrawFillBuffers(uiDrawFillBuffers)
        , UIDrawSchedule(uiDrawSchedule)
      {
      }

      double Sum() const
      {
        return UIProcessEvents + UIUpdate + UIDraw + UIDrawPreprocess + UIDrawGenMesh + UIDrawFillBuffers + UIDrawSchedule;
      }
    };

    struct ChartData
    {
      std::shared_ptr<UI::ChartData> Data;
      AverageRecord Average;
    };

    ChartData ExtractData(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService, const AppBenchmarkData& sourceData)
    {
      auto chartData = std::make_shared<UI::ChartData>(dataBindingService, NumericCast<uint32_t>(sourceData.CpuData.Entries.size()),
                                                       CustomControlFactory::MaxCpuProfileDataEntries(), UI::ChartData::Constraints(0, {}));
      AverageRecord average;
      for (std::size_t i = 0; i < sourceData.CpuData.Entries.size(); ++i)
      {
        const auto& srcEntry = sourceData.CpuData.Entries[i];
        UI::ChartDataEntry entry;
        entry.Values[0] = srcEntry.UIDrawSchedule;
        entry.Values[1] = srcEntry.UIDrawFillBuffers;
        entry.Values[2] = srcEntry.UIDrawGenMesh;
        entry.Values[3] = srcEntry.UIDrawPreprocess;
        entry.Values[4] = srcEntry.UIDraw;
        entry.Values[5] = srcEntry.UIUpdate;
        entry.Values[6] = srcEntry.UIProcessEvents;

        chartData->Append(entry);

        average.UIProcessEvents += static_cast<double>(srcEntry.UIProcessEvents);
        average.UIUpdate += static_cast<double>(srcEntry.UIUpdate);
        average.UIDraw += static_cast<double>(srcEntry.UIDraw);
        average.UIDrawPreprocess += static_cast<double>(srcEntry.UIDrawPreprocess);
        average.UIDrawGenMesh += static_cast<double>(srcEntry.UIDrawGenMesh);
        average.UIDrawFillBuffers += static_cast<double>(srcEntry.UIDrawFillBuffers);
        average.UIDrawSchedule += static_cast<double>(srcEntry.UIDrawSchedule);
      }
      if (!sourceData.CpuData.Entries.empty())
      {
        const auto entryCount = static_cast<double>(sourceData.CpuData.Entries.size());
        average.UIProcessEvents /= entryCount;
        average.UIUpdate /= entryCount;
        average.UIDraw /= entryCount;
        average.UIDrawPreprocess /= entryCount;
        average.UIDrawGenMesh /= entryCount;
        average.UIDrawFillBuffers /= entryCount;
        average.UIDrawSchedule /= entryCount;
      }
      return {chartData, average};
    }

    ChartData ExtractGpuData(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService, const AppBenchmarkData& sourceData)
    {
      const AppBenchmarkGpuData& gpuData = sourceData.GpuData.value();
      auto chartData = std::make_shared<UI::ChartData>(dataBindingService, NumericCast<uint32_t>(gpuData.Entries.size()),
                                                       CustomControlFactory::MaxCpuProfileDataEntries(), UI::ChartData::Constraints(0, {}));
      AverageRecord average;
      for (std::size_t i = 0; i < gpuData.Entries.size(); ++i)
      {
        const auto& srcEntry = gpuData.Entries[i];
        UI::ChartDataEntry entry;
        entry.Values[0] = srcEntry.UIRenderTime;
        chartData->Append(entry);

        average.UIProcessEvents += static_cast<double>(srcEntry.UIRenderTime);
      }
      if (!gpuData.Entries.empty())
      {
        const auto entryCount = static_cast<double>(gpuData.Entries.size());
        average.UIProcessEvents /= entryCount;
      }
      return {chartData, average};
    }

    void SetCpuAverage(const CpuDetailedLegendRecord& cpuLegend, const AverageRecord& average)
    {
      CustomControlFactory::SetContent(cpuLegend.AverageTimeLabel.get(), average.Sum());
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[0].AverageTimeLabel.get(), average.UIProcessEvents);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[1].AverageTimeLabel.get(), average.UIUpdate);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[2].AverageTimeLabel.get(), average.UIDraw);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[3].AverageTimeLabel.get(), average.UIDrawPreprocess);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[4].AverageTimeLabel.get(), average.UIDrawGenMesh);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[5].AverageTimeLabel.get(), average.UIDrawFillBuffers);
      CustomControlFactory::SetContent(cpuLegend.ChartCpuUI.Controls[6].AverageTimeLabel.get(), average.UIDrawSchedule);
    }

    void SetGpuAverage(const GpuLegendRecord& gpuLegend, const AverageRecord& average)
    {
      CustomControlFactory::SetContent(gpuLegend.AverageTimeLabel.get(), average.Sum());
    }


    std::shared_ptr<UI::BaseWindow> CreateReportInfoUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceData)
    {
      auto context = uiFactory.GetContext();

      // auto label0 = uiFactory.CreateLabel("BenchApp:");
      // auto label1 = uiFactory.CreateLabel("Version:");
      // auto label2 = uiFactory.CreateLabel("Resolution:");
      // auto label3 = uiFactory.CreateLabel("DensityDpi:");
      // auto label4 = uiFactory.CreateLabel("Build type:");

      // auto labelVal0 = uiFactory.CreateLabel(sourceData.Info.AppName);
      // auto labelVal1 = uiFactory.CreateLabel(fmt::format(FMT_STRING("V{}"),sourceData.Info.AppVersion));
      // auto labelVal2 =
      //  uiFactory.CreateLabel(fmt::format(FMT_STRING("{}x{}"), sourceData.Info.ResolutionPx.Width(), sourceData.Info.ResolutionPx.Height()));
      // auto labelVal3 = uiFactory.CreateLabel(fmt::format(FMT_STRING("{}"), sourceData.Info.DensityDpi));
      // auto labelVal4 = uiFactory.CreateLabel(sourceData.Info.AppDebugMode ? StringViewLite("Debug") : StringViewLite("Release"));
      // labelVal0->SetAlignmentX(UI::ItemAlignment::Far);
      // labelVal1->SetAlignmentX(UI::ItemAlignment::Far);
      // labelVal2->SetAlignmentX(UI::ItemAlignment::Far);
      // labelVal3->SetAlignmentX(UI::ItemAlignment::Far);
      // labelVal4->SetAlignmentX(UI::ItemAlignment::Far);

      // auto layout = std::make_shared<UI::GridLayout>(context);
      // layout->SetAlignmentX(UI::ItemAlignment::Stretch);
      // layout->SetAlignmentY(UI::ItemAlignment::Stretch);
      // layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      // layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      // layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      // layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      // layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      // layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      // layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      // layout->AddChild(label0, 0, 0);
      // layout->AddChild(labelVal0, 1, 0);
      // layout->AddChild(label1, 0, 1);
      // layout->AddChild(labelVal1, 1, 1);
      // layout->AddChild(label2, 0, 2);
      // layout->AddChild(labelVal2, 1, 2);
      // layout->AddChild(label3, 0, 3);
      // layout->AddChild(labelVal3, 1, 3);
      // layout->AddChild(label4, 0, 4);
      // layout->AddChild(labelVal4, 1, 4);
      // return layout;

      auto dateString = fmt::format("{0:%Y-%m-%d} {0:%H:%M:%S}", fmt::localtime(sourceData.Time));
      auto debugString = sourceData.Info.AppDebugMode ? StringViewLite(" (Debug)") : StringViewLite();
      auto descLabel =
        uiFactory.CreateLabel(fmt::format(FMT_STRING("Scene: {}, {}x{}px {}dpi, {}, {} V{}{}"), static_cast<uint32_t>(sourceData.Info.Scene),
                                          sourceData.Info.ResolutionPx.Width(), sourceData.Info.ResolutionPx.Height(), sourceData.Info.DensityDpi,
                                          dateString, sourceData.Info.AppName, sourceData.Info.AppVersion, debugString));
      descLabel->SetAlignmentX(UI::ItemAlignment::Center);
      return descLabel;
    }

    bool IsCompatible(const AppBenchmarkData& data0, const AppBenchmarkData& data1)
    {
      // For now we just use the size
      return data0.CpuData.Entries.size() == data1.CpuData.Entries.size();
    }
  }

  ResultScene::ResultScene(const SceneCreateInfo& createInfo, std::shared_ptr<BenchResultManager> benchResultManager)
    : BasicScene(createInfo)
    , m_benchResultManager(std::move(benchResultManager))
  {
    std::optional<AppBenchmarkData> benchNewResult = m_benchResultManager->TryLoad();
    if (benchNewResult.has_value())
    {
      std::optional<AppBenchmarkData> benchOldResult = m_benchResultManager->TryLoadCompare();
      if (benchOldResult.has_value() && !IsCompatible(benchNewResult.value(), benchOldResult.value()))
      {
        benchOldResult = std::optional<AppBenchmarkData>();
      }
      m_ui = CreateUI(*createInfo.ControlFactory, benchNewResult.value(), benchOldResult);
      createInfo.RootLayout->AddChild(m_ui.Main);
      UpdateCompareAlpha();

      m_benchNewResult = benchNewResult;
      m_benchOldResult = benchOldResult;
    }
    else
    {
      FSLLOG3_WARNING("Could not load a benchmark result");
      ScheduleClose();
    }
  }


  ResultScene::~ResultScene() = default;


  void ResultScene::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (m_state == CurrentState::Ready && !IsClosing() && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_ui.BackButton)
      {
        theEvent->Handled();
        ScheduleClose();
      }
      else if (theEvent->GetSource() == m_ui.DetailsButton)
      {
        theEvent->Handled();

        m_ui.ActivityStack->Push(
          std::make_shared<UI::ResultDetailsDialogActivity>(m_ui.ActivityStack, m_uiControlFactory, m_benchNewResult, m_benchOldResult));
        m_state = CurrentState::DetailsDialog;
      }
    }
    BasicScene::OnSelect(theEvent);
  }


  void ResultScene::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (!theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_ui.Tabs.Report.CompareSlider)
      {
        theEvent->Handled();
        UpdateCompareAlpha();
      }
      else if (theEvent->GetSource() == m_ui.Tabs.Report.CpuChartMaxSlider)
      {
        theEvent->Handled();
        if (m_ui.Tabs.Report.CpuReport.ChartData)
        {
          auto newMaxValue = UncheckedNumericCast<uint32_t>(static_cast<int32_t>(std::round(m_ui.Tabs.Report.CpuChartMaxSlider->GetValue())));
          const auto dataStats = m_ui.Tabs.Report.CpuReport.ChartData->CalculateDataStats();
          const MinMax<uint32_t> minMax(dataStats.ValueMinMax.Min(), std::max(dataStats.ValueMinMax.Min() + 20, newMaxValue));
          m_ui.Tabs.Report.CpuReport.ChartData->SetCustomMinMax(minMax);
          if (m_ui.Tabs.Report.CpuReportOld.ChartData)
          {
            m_ui.Tabs.Report.CpuReportOld.ChartData->SetCustomMinMax(minMax);
          }
        }
      }
      else if (theEvent->GetSource() == m_ui.Tabs.Report.GpuChartMaxSlider)
      {
        theEvent->Handled();
        if (m_ui.Tabs.Report.GpuReport.ChartData)
        {
          auto newMaxValue = UncheckedNumericCast<uint32_t>(static_cast<int32_t>(std::round(m_ui.Tabs.Report.GpuChartMaxSlider->GetValue())));
          const auto dataStats = m_ui.Tabs.Report.GpuReport.ChartData->CalculateDataStats();
          const MinMax<uint32_t> minMax(dataStats.ValueMinMax.Min(), std::max(dataStats.ValueMinMax.Min() + 20, newMaxValue));
          m_ui.Tabs.Report.GpuReport.ChartData->SetCustomMinMax(minMax);
          if (m_ui.Tabs.Report.GpuReportOld.ChartData)
          {
            m_ui.Tabs.Report.GpuReportOld.ChartData->SetCustomMinMax(minMax);
          }
        }
      }
    }
    BasicScene::OnContentChanged(theEvent);
  }


  void ResultScene::OnKeyEvent(const KeyEvent& event)
  {
    if (m_ui.ActivityStack && !m_ui.ActivityStack->IsEmpty())
    {
      m_ui.ActivityStack->OnKeyEvent(event);
    }

    if (!event.IsHandled() && event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::T:
        event.Handled();
        ToggleReportVisibility();
        break;
      default:
        break;
      }
    }
    BasicScene::OnKeyEvent(event);
  }


  void ResultScene::Update(const DemoTime& demoTime)
  {
    m_ui.BaseLayout->SetBaseColor(m_ui.ActivityStack->GetDesiredParentColor());

    switch (m_state)
    {
    case CurrentState::DetailsDialog:
      UpdateDetailsDialogState();
      break;
    default:
      break;
    }

    BasicScene::Update(demoTime);
  }

  void ResultScene::UpdateDetailsDialogState()
  {
    assert(m_state == CurrentState::DetailsDialog);
    // Simple little work around for not having proper activities, so we basically do a form of 'busy' wait.
    if (m_ui.ActivityStack->IsEmpty())
    {
      m_state = CurrentState::Ready;
    }
  }


  void ResultScene::OnBeginClose()
  {
    FSLLOG3_WARNING_IF(m_state != CurrentState::Ready && m_state != CurrentState::Closing, "OnBeginClose called from unexpected state")
    if (m_state == CurrentState::Ready)
    {
      m_state = CurrentState::Closing;
    }
  }


  ResultScene::UIRecord ResultScene::CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                                              const std::optional<AppBenchmarkData>& sourceDataOld)
  {
    auto context = uiFactory.GetContext();
    auto header = uiFactory.CreateLabel("Results", UI::Theme::FontType::Header);
    header->SetAlignmentX(UI::ItemAlignment::Center);
    header->SetAlignmentY(UI::ItemAlignment::Center);

    auto backButton = uiFactory.CreateFloatingButton(UI::Theme::FloatingButtonType::Back);
    backButton->SetAlignmentX(UI::ItemAlignment::Near);
    backButton->SetAlignmentY(UI::ItemAlignment::Far);

    auto reportLabelStack = std::make_shared<UI::StackLayout>(context);
    auto reportLabelNew = CreateReportInfoUI(uiFactory, sourceDataNew);
    reportLabelStack->SetOrientation(UI::LayoutOrientation::Vertical);
    reportLabelStack->AddChild(reportLabelNew);
    if (sourceDataOld.has_value())
    {
      auto reportLabelOld = CreateReportInfoUI(uiFactory, sourceDataOld.value());
      reportLabelStack->AddChild(uiFactory.CreateLabel("vs"));
      reportLabelStack->AddChild(reportLabelOld);
    }

    auto detailsButton = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Details");
    detailsButton->SetAlignmentX(UI::ItemAlignment::Center);
    detailsButton->SetAlignmentY(UI::ItemAlignment::Center);

    auto topLayout = std::make_shared<UI::GridLayout>(context);
    topLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));          // 0
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));    // 1
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));          // 2
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));    // 3
    topLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    topLayout->AddChild(backButton, 0, 0);
    topLayout->AddChild(header, 1, 0);
    topLayout->AddChild(reportLabelStack, 2, 0);
    topLayout->AddChild(detailsButton, 3, 0);

    auto topBar = uiFactory.CreateTopBar(topLayout);

    UIReportTabs uiReport = CreateReportUI(uiFactory, sourceDataNew, sourceDataOld);

    auto bottomBar = uiFactory.CreateBottomBar(uiReport.Layout);
    bottomBar->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddChild(topBar, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(bottomBar, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(layout);

    auto activityStack = std::make_shared<UI::ActivityStack>(context);
    fillLayout->AddChild(activityStack);

    return {fillLayout, layout, backButton, detailsButton, activityStack, uiReport};
  }

  ResultScene::UIReportTabs ResultScene::CreateReportUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                                                        const std::optional<AppBenchmarkData>& sourceDataOld)
  {
    auto context = uiFactory.GetContext();

    const bool haveGpuData = sourceDataNew.GpuData.has_value() && (!sourceDataOld.has_value() || sourceDataOld.value().GpuData.has_value());

    const auto defaultReportType = haveGpuData ? ReportType::Both : ReportType::Cpu;

    // Thew view-group and the radio buttons is basically a poor man's tab control (since we dont have a generic one atm)
    // auto viewGroup = uiFactory.CreateRadioGroup("details");
    // auto viewLayout = std::make_shared<UI::StackLayout>(context);
    // auto radioButtonCpu = uiFactory.CreateRadioButton(viewGroup, "CPU", !haveGpuData);
    // auto radioButtonGpu = uiFactory.CreateRadioButton(viewGroup, "GPU", false);
    // auto radioButtonBoth = uiFactory.CreateRadioButton(viewGroup, "Both", haveGpuData);
    // viewLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    // viewLayout->SetAlignmentX(UI::ItemAlignment::Center);
    // viewLayout->AddChild(radioButtonCpu);
    // viewLayout->AddChild(radioButtonGpu);
    // viewLayout->AddChild(radioButtonBoth);

    auto reportTab = CreateReportTabUI(uiFactory, sourceDataNew, sourceDataOld, defaultReportType);

    auto bottomLayoutStack = std::make_shared<UI::ComplexStackLayout>(context);
    bottomLayoutStack->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomLayoutStack->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomLayoutStack->SetOrientation(UI::LayoutOrientation::Vertical);
    // if (haveGpuData)
    //{
    //  bottomLayoutStack->AddChild(viewLayout, UI::LayoutLength(UI::LayoutUnitType::Auto));
    //}
    bottomLayoutStack->AddChild(reportTab.Layout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    return {bottomLayoutStack, /* radioButtonCpu, radioButtonGpu, radioButtonBoth,*/ reportTab};
  }


  ResultScene::UIReport ResultScene::CreateReportTabUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                                                       const std::optional<AppBenchmarkData>& sourceDataOld, const ReportType reportType)
  {
    auto context = uiFactory.GetContext();
    std::optional<UICpuReport> cpuReportNew =
      reportType != ReportType::Gpu ? CreateCpuReport(uiFactory, sourceDataNew) : std::optional<UICpuReport>();
    std::optional<UIGpuReport> gpuReportNew =
      reportType != ReportType::Cpu ? CreateGpuReport(uiFactory, sourceDataNew) : std::optional<UIGpuReport>();

    auto cpuChartMaxSlider = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0, 1, 1));
    auto gpuChartMaxSlider = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0, 1, 1));

    auto reportLayout = std::make_shared<UI::GridLayout>(context);
    reportLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    reportLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    reportLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    reportLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, CustomUIConfig::FixedSpacingDp.Value().Value));
    reportLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    reportLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, CustomUIConfig::FixedSpacingDp.Value().Value));
    reportLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    reportLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    const auto reportLayoutRowIndex = 1;
    auto gpuReportLayoutRowIndex = reportLayoutRowIndex;
    if (cpuReportNew.has_value())
    {
      reportLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      reportLayout->AddChild(cpuReportNew.value().CpuLegend.Main, 0, reportLayoutRowIndex);
      reportLayout->AddChild(cpuReportNew.value().Chart, 2, reportLayoutRowIndex);
      reportLayout->AddChild(cpuChartMaxSlider, 4, reportLayoutRowIndex);
      ++gpuReportLayoutRowIndex;
    }

    if (gpuReportNew.has_value())
    {
      reportLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      reportLayout->AddChild(gpuReportNew.value().GpuLegend.Main, 0, gpuReportLayoutRowIndex);
      reportLayout->AddChild(gpuReportNew.value().Chart, 2, gpuReportLayoutRowIndex);
      reportLayout->AddChild(gpuChartMaxSlider, 4, gpuReportLayoutRowIndex);
    }

    // If we have old source data then we configure the UI for easy comparison
    std::shared_ptr<UI::SliderAndFmtValueLabel<float>> slider;
    UICpuReport cpuReportOld;
    if (cpuReportNew.has_value())
    {
      if (sourceDataOld.has_value())
      {
        auto sliderLabel = uiFactory.CreateLabel("Report visibility");
        sliderLabel->SetAlignmentY(UI::ItemAlignment::Center);
        slider = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(0, 0, 1.0f));
        // Add the compare alpha slider
        reportLayout->AddChild(sliderLabel, 0, 0);
        reportLayout->AddChild(slider, 2, 0);

        cpuReportOld = CreateCpuReport(uiFactory, sourceDataOld.value());
        reportLayout->AddChild(cpuReportOld.CpuLegend.Main, 0, reportLayoutRowIndex);
        reportLayout->AddChild(cpuReportOld.Chart, 2, reportLayoutRowIndex);

        {    // Force both data sets to use the exact same grid
          auto newGridInfo = cpuReportNew.value().ChartData->CalculateDataStats();
          auto oldGridInfo = cpuReportOld.ChartData->CalculateDataStats();
          MinMax<uint32_t> viewMinMax(std::min(oldGridInfo.ValueMinMax.Min(), newGridInfo.ValueMinMax.Min()),
                                      std::max(oldGridInfo.ValueMinMax.Max(), newGridInfo.ValueMinMax.Max()));

          cpuReportNew.value().ChartData->SetCustomMinMax(viewMinMax);
          cpuReportOld.ChartData->SetCustomMinMax(viewMinMax);
        }
      }

      {    // Set the chartMaxSlider range
        const auto info = cpuReportNew.value().ChartData->CalculateDataStats();
        cpuChartMaxSlider->SetRange(static_cast<float>(info.ValueMinMax.Min()), static_cast<float>(info.ValueMinMax.Max()));
        cpuChartMaxSlider->SetValue(static_cast<float>(info.ValueMinMax.Max()));
        cpuChartMaxSlider->FinishAnimation();
      }
    }
    UIGpuReport gpuReportOld;
    if (gpuReportNew.has_value())
    {
      if (sourceDataOld.has_value())
      {
        gpuReportOld = CreateGpuReport(uiFactory, sourceDataOld.value());
        reportLayout->AddChild(gpuReportOld.GpuLegend.Main, 0, gpuReportLayoutRowIndex);
        reportLayout->AddChild(gpuReportOld.Chart, 2, gpuReportLayoutRowIndex);

        {    // Force both data sets to use the exact same grid
          auto newGridInfo = gpuReportNew.value().ChartData->CalculateDataStats();
          auto oldGridInfo = gpuReportOld.ChartData->CalculateDataStats();
          MinMax<uint32_t> viewMinMax(std::min(oldGridInfo.ValueMinMax.Min(), newGridInfo.ValueMinMax.Min()),
                                      std::max(oldGridInfo.ValueMinMax.Max(), newGridInfo.ValueMinMax.Max()));

          gpuReportNew.value().ChartData->SetCustomMinMax(viewMinMax);
          gpuReportOld.ChartData->SetCustomMinMax(viewMinMax);
        }
      }

      {    // Set the chartMaxSlider range
        const auto info = gpuReportNew.value().ChartData->CalculateDataStats();
        gpuChartMaxSlider->SetRange(static_cast<float>(info.ValueMinMax.Min()), static_cast<float>(info.ValueMinMax.Max()));
        gpuChartMaxSlider->SetValue(static_cast<float>(info.ValueMinMax.Max()));
        gpuChartMaxSlider->FinishAnimation();
      }
    }
    return {reportLayout,
            slider,
            cpuChartMaxSlider,
            gpuChartMaxSlider,
            cpuReportNew.value_or(UICpuReport()),
            cpuReportOld,
            gpuReportNew.value_or(UIGpuReport()),
            gpuReportOld};
  }


  ResultScene::UICpuReport ResultScene::CreateCpuReport(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& source)
  {
    auto context = uiFactory.GetContext();
    auto cpuLegend = CustomControlFactory::CreateDetailedCpuLegend(uiFactory, CustomUIConfig::FixedSpacingDp);

    auto newResult = ExtractData(context->UIDataBindingService, source);
    newResult.Data->SetChannelMetaData(0, LocalConfig::ChartColor);
    SetCpuAverage(cpuLegend, newResult.Average);
    auto cpuChartNew = CustomControlFactory::CreateAreaChart(uiFactory, newResult.Data, CustomControlFactory::OpaqueHack::Enabled);

    return {cpuLegend, cpuChartNew, newResult.Data};
  }

  ResultScene::UIGpuReport ResultScene::CreateGpuReport(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& source)
  {
    auto context = uiFactory.GetContext();

    auto newResult = ExtractGpuData(context->UIDataBindingService, source);

    auto gpuTime = uiFactory.CreateLabel("GPU time");
    auto gpuTimeFmtLabel = uiFactory.CreateFmtValueLabel(0.0f, "{:.2f}ms");
    gpuTimeFmtLabel->SetAlignmentX(UI::ItemAlignment::Far);
    gpuTimeFmtLabel->SetAlignmentY(UI::ItemAlignment::Far);
    auto chart = std::make_shared<UI::AreaChart>(context);
    {
      chart->SetAlignmentX(UI::ItemAlignment::Stretch);
      chart->SetAlignmentY(UI::ItemAlignment::Stretch);
      chart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      chart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      chart->SetGridLines(std::make_shared<UI::ChartGridLinesFps>());
      chart->SetDataView(newResult.Data);
      chart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
      chart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
      chart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    }

    auto gpuDetails = std::make_shared<UI::StackLayout>(context);
    gpuDetails->SetAlignmentX(UI::ItemAlignment::Stretch);
    gpuDetails->SetOrientation(UI::LayoutOrientation::Vertical);
    gpuDetails->AddChild(gpuTime);
    gpuDetails->AddChild(gpuTimeFmtLabel);

    GpuLegendRecord gpuLegend{gpuDetails, gpuTimeFmtLabel};
    SetGpuAverage(gpuLegend, newResult.Average);

    return {gpuLegend, chart, newResult.Data};
  }


  void ResultScene::UpdateCompareAlpha()
  {
    if (m_ui.Tabs.Report.CompareSlider)
    {
      const auto alpha = m_ui.Tabs.Report.CompareSlider->GetValue();
      m_ui.Tabs.Report.SetBaseAlpha(alpha);
    }
  }

  void ResultScene::ToggleReportVisibility()
  {
    if (m_ui.Tabs.Report.CompareSlider)
    {
      const auto alpha = m_ui.Tabs.Report.CompareSlider->GetValue();
      const auto newAlpha = alpha >= 0.5f ? 0.0f : 1.0f;
      m_ui.Tabs.Report.CompareSlider->SetValue(newAlpha);
      UpdateCompareAlpha();
    }
  }
}
