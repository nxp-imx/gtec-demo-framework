#ifndef SHARED_UI_BENCHMARK_SCENE_RESULTSCENE_HPP
#define SHARED_UI_BENCHMARK_SCENE_RESULTSCENE_HPP
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

#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkData.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "BasicScene.hpp"
#include "Control/CpuDetailedLegendRecord.hpp"

namespace Fsl
{
  namespace UI
  {
    class ActivityStack;
    class ButtonBase;
    class BaseWindow;
    class ChartData;
    class RadioButton;
  }

  class BenchResultManager;

  struct GpuLegendRecord
  {
    std::shared_ptr<UI::BaseWindow> Main;
    std::shared_ptr<UI::FmtValueLabel<float>> AverageTimeLabel;
  };

  class ResultScene final : public BasicScene
  {
    enum class CurrentState
    {
      Ready,
      DetailsDialog,
      Closing
    };

    enum class ReportType
    {
      Cpu = 0x01,
      Gpu = 0x02,
      Both = Cpu | Gpu
    };

    struct UICpuReport
    {
      CpuDetailedLegendRecord CpuLegend;
      std::shared_ptr<UI::BaseWindow> Chart;
      std::shared_ptr<UI::ChartData> ChartData;

      void SetBaseAlpha(const float alpha)
      {
        if (CpuLegend.Main)
        {
          CpuLegend.Main->SetBaseColor(UI::UIColor(1.0f, 1.0f, 1.0f, alpha));
        }
        if (Chart)
        {
          Chart->SetBaseColor(UI::UIColor(1.0f, 1.0f, 1.0f, alpha));
        }
      }
    };


    struct UIGpuReport
    {
      GpuLegendRecord GpuLegend;
      std::shared_ptr<UI::BaseWindow> Chart;
      std::shared_ptr<UI::ChartData> ChartData;

      void SetBaseAlpha(const float alpha)
      {
        if (GpuLegend.Main)
        {
          GpuLegend.Main->SetBaseColor(UI::UIColor(1.0f, 1.0f, 1.0f, alpha));
        }
        if (Chart)
        {
          Chart->SetBaseColor(UI::UIColor(1.0f, 1.0f, 1.0f, alpha));
        }
      }
    };

    struct UIReport
    {
      std::shared_ptr<UI::BaseWindow> Layout;
      std::shared_ptr<UI::SliderAndFmtValueLabel<float>> CompareSlider;    // This can be null
      std::shared_ptr<UI::Slider<float>> CpuChartMaxSlider;
      std::shared_ptr<UI::Slider<float>> GpuChartMaxSlider;
      UICpuReport CpuReport;
      UICpuReport CpuReportOld;
      UIGpuReport GpuReport;
      UIGpuReport GpuReportOld;

      void SetBaseAlpha(const float alpha)
      {
        CpuReport.SetBaseAlpha(1.0f - alpha);
        CpuReportOld.SetBaseAlpha(alpha);
        GpuReport.SetBaseAlpha(1.0f - alpha);
        GpuReportOld.SetBaseAlpha(alpha);
      }
    };

    struct UIReportTabs
    {
      std::shared_ptr<UI::BaseWindow> Layout;
      // std::shared_ptr<UI::RadioButton> RadioButtonCpu;
      // std::shared_ptr<UI::RadioButton> RadioButtonGpu;
      // std::shared_ptr<UI::RadioButton> RadioButtonBoth;
      UIReport Report;
    };


    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::BaseWindow> BaseLayout;
      std::shared_ptr<UI::ButtonBase> BackButton;
      std::shared_ptr<UI::ButtonBase> DetailsButton;
      std::shared_ptr<UI::ActivityStack> ActivityStack;
      UIReportTabs Tabs;
    };


    CurrentState m_state{CurrentState::Ready};
    std::shared_ptr<BenchResultManager> m_benchResultManager;
    UIRecord m_ui;
    std::optional<AppBenchmarkData> m_benchNewResult;
    std::optional<AppBenchmarkData> m_benchOldResult;

  public:
    explicit ResultScene(const SceneCreateInfo& createInfo, std::shared_ptr<BenchResultManager> benchResultManager);
    ~ResultScene() override;

    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnKeyEvent(const KeyEvent& event) final;
    void Update(const DemoTime& demoTime) final;

  protected:
    void UpdateDetailsDialogState();
    void OnBeginClose() final;

  private:
    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                             const std::optional<AppBenchmarkData>& sourceDataOld);
    static UIReportTabs CreateReportUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                                       const std::optional<AppBenchmarkData>& sourceDataOld);
    static UIReport CreateReportTabUI(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& sourceDataNew,
                                      const std::optional<AppBenchmarkData>& sourceDataOld, const ReportType reportType);
    static UICpuReport CreateCpuReport(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& source);
    static UIGpuReport CreateGpuReport(UI::Theme::IThemeControlFactory& uiFactory, const AppBenchmarkData& source);

    void UpdateCompareAlpha();
    void ToggleReportVisibility();
  };
}

#endif
