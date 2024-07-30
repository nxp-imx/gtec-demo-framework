#ifndef SHARED_UI_CHARTS_CHARTSSHARED_HPP
#define SHARED_UI_CHARTS_CHARTSSHARED_HPP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <Shared/UI/Charts/DataGenerator/DataGenerator1.hpp>
#include <Shared/UI/Charts/DataGenerator/DataGenerator2.hpp>
#include <array>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    class AreaChart;
    class BackgroundLabelButton;
    class BaseWindow;
    class BoxPlotChart;
    class ChartData;
    class ChartDataView;
    class RadioButton;
    class Layout;
    class Switch;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class ChartsShared : public UI::EventListener
  {
    struct UIMenuRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::RadioButton> RadioButtonGen1;
      std::shared_ptr<UI::RadioButton> RadioButtonGen2;
      std::shared_ptr<UI::Switch> SwitchComplexGen;
      std::shared_ptr<UI::BackgroundLabelButton> BtnResetGen;
      std::shared_ptr<UI::RadioButton> RadioButtonDataContinuous;
      std::shared_ptr<UI::RadioButton> RadioButtonDataPeriodically;
      std::shared_ptr<UI::RadioButton> RadioButtonDataPaused;
      std::shared_ptr<UI::BackgroundLabelButton> BtnDataGenOne;
      std::shared_ptr<UI::BackgroundLabelButton> BtnDataGenMultiple;
      std::shared_ptr<UI::BackgroundLabelButton> BtnSetDefaults;
    };

    struct UIFullAreaChartsRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::AreaChart> Chart;
      std::shared_ptr<UI::BoxPlotChart> BoxPlot;
    };


    struct UIAreaChartsRecord
    {
      std::shared_ptr<UI::Layout> Main;
      std::shared_ptr<UI::AreaChart> Chart0;
      std::shared_ptr<UI::AreaChart> Chart1;
      std::shared_ptr<UI::AreaChart> Chart2;
      std::shared_ptr<UI::AreaChart> Chart3;
    };

    struct UIBoxPlotsRecord
    {
      std::shared_ptr<UI::Layout> Main;
      std::shared_ptr<UI::BoxPlotChart> BoxPlot0;
      std::shared_ptr<UI::BoxPlotChart> BoxPlot1;
      std::shared_ptr<UI::BoxPlotChart> BoxPlot2;
      std::shared_ptr<UI::BoxPlotChart> BoxPlot3;
    };


    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      UIMenuRecord Menu;
      UIFullAreaChartsRecord FullCharts;
      UIAreaChartsRecord AreaCharts;
      UIAreaChartsRecord AreaChartsActualSize;
      UIBoxPlotsRecord BoxPlots;
    };

    enum class DataMode
    {
      Continuous,
      Periodically,
      Paused,
      GenerateOne,
      GenerateMultiple,
      GenerateLots,
    };

    enum class CachedDataConfig
    {
      NotCached,
      BasicData,
      ComplexData,
    };

    struct DataGenerationRecord
    {
      DataMode Mode{DataMode::GenerateLots};
      TimeSpan TimeLeft;
      // TimeSpan TimePassed;

      void Reset()
      {
        TimeLeft = {};
      }
    };

    struct ChartDataRecord
    {
      std::shared_ptr<UI::ChartData> Data;
      std::array<std::shared_ptr<UI::ChartDataView>, 4> Views;
      std::shared_ptr<UI::ChartDataView> FullView;

      explicit ChartDataRecord(std::shared_ptr<UI::ChartData> data);
    };


    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;

    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    CachedDataConfig m_dataConfig{CachedDataConfig::NotCached};
    ChartDataRecord m_chartDataBasic;
    ChartDataRecord m_chartDataComplex;

    UIRecord m_uiRecord;
    DataGenerationRecord m_dataGenerator;
    DataGenerator1 m_dataGenerator1;
    DataGenerator2 m_dataGenerator2;

  public:
    explicit ChartsShared(const DemoAppConfig& config);
    ~ChartsShared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;

    void OnKeyEvent(const KeyEvent& event);
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Draw();

  private:
    void SetDefaultValues();
    void ReconfigureChartData();
    void ResetGenerators();

    DataMode ExtractUIDataMode();

    static void SetDataViews(UIRecord& rUIRecord, const ChartDataRecord& record);

    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIMenuRecord CreateMenuUI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIFullAreaChartsRecord CreateFullAreaChartUI(UI::Theme::IThemeControlFactory& uiFactory);
    static UIAreaChartsRecord CreateAreaChartsUI(UI::Theme::IThemeControlFactory& uiFactory, const bool useFixedSize);
    static UIBoxPlotsRecord CreateBoxPlotChartsUI(UI::Theme::IThemeControlFactory& uiFactory);
  };
}
#endif
