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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <Shared/UI/Charts/ChartsShared.hpp>
// #include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/BoxPlotChart.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Charts/ChartGridLinesTest.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t ChartDataCapacity = 4096;

      constexpr MinMax<uint32_t> ChartDataFixedMinMax(0, 150);

      constexpr TimeSpan DataGeneratorPeriod = TimeSpan::FromMilliseconds(100);

      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
    }

    namespace LocalColor
    {
      constexpr Color Median(0xFFC0C0C0);

      constexpr Color Green(0xFF7B9E05);        // green
      constexpr Color LightBlue(0xFF3488A7);    // light blue
      constexpr Color Orange(0xFFCC7000);       // orange
      constexpr Color Red(0xFFA00000);          // red

      // constexpr Color Profile0(0xFF7B9E05);    // green
      // constexpr Color Profile1(0xFF882807);    // brown
      // constexpr Color Profile2(0xFF614985);    // purple
      // constexpr Color Profile3(0xFFC5A206);    // light yellow
      // constexpr Color Profile4(0xFF3488A7);    // light blue
      // constexpr Color Profile5(0xFFCC7000);    // orange
      // constexpr Color Profile6(0xFFA00000);    // red
    }

    std::shared_ptr<UI::AreaChart> CreateAreaChart(const std::shared_ptr<UI::BaseWindowContext>& context, const UI::Theme::IThemeResources& resources)
    {
      auto chart = std::make_shared<UI::AreaChart>(context);
      chart->SetAlignmentX(UI::ItemAlignment::Far);
      chart->SetAlignmentY(UI::ItemAlignment::Stretch);
      chart->SetOpaqueFillSprite(resources.GetBasicFillSprite(true));
      chart->SetTransparentFillSprite(resources.GetBasicFillSprite(false));
      chart->SetFont(resources.GetDefaultSpriteFont());
      chart->SetLabelBackground(resources.GetToolTipNineSliceSprite());
      return chart;
    }

    std::shared_ptr<UI::BoxPlotChart> CreateBoxPlotChart(const std::shared_ptr<UI::BaseWindowContext>& context,
                                                         const UI::Theme::IThemeResources& resources)
    {
      auto chart = std::make_shared<UI::BoxPlotChart>(context);
      chart->SetAlignmentX(UI::ItemAlignment::Near);
      chart->SetAlignmentY(UI::ItemAlignment::Stretch);
      chart->SetOpaqueFillSprite(resources.GetBasicFillSprite(true));
      chart->SetOutlierSize(UI::DpLayoutSize1D::Create(3));
      chart->SetWhiskerSize(UI::DpLayoutSize1D::Create(21));
      chart->SetBoxSize(UI::DpLayoutSize1D::Create(29));
      chart->SetMedianColor(LocalColor::Median);
      return chart;
    }


    std::array<std::shared_ptr<UI::ChartDataView>, 4> CreateViews(const std::shared_ptr<UI::ChartData>& data)
    {
      std::array<std::shared_ptr<UI::ChartDataView>, 4> dataViews;
      {    // Configure data views
        for (std::size_t i = 0; i < dataViews.size(); ++i)
        {
          dataViews[i] = std::make_shared<UI::ChartDataView>(data);
        }
        dataViews[2]->SetCustomMinMax(LocalConfig::ChartDataFixedMinMax);
        dataViews[3]->SetCustomMinMax(LocalConfig::ChartDataFixedMinMax);
      }
      return dataViews;
    }
  }

  ChartsShared::ChartDataRecord::ChartDataRecord(std::shared_ptr<UI::ChartData> data)
    : Data(std::move(data))
    , Views(CreateViews(Data))
    , FullView(std::make_shared<UI::ChartDataView>(Data))
  {
  }


  ChartsShared::ChartsShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_chartDataBasic(
        std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), LocalConfig::ChartDataCapacity, 1, UI::ChartData::Constraints(0, {})))
    , m_chartDataComplex(
        std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), LocalConfig::ChartDataCapacity, 2, UI::ChartData::Constraints(0, {})))

  {
    m_chartDataBasic.Data->SetChannelMetaData(0, LocalColor::LightBlue);
    m_chartDataBasic.Views[0]->SetChannelMetaData(0, LocalColor::Orange);    // top right
    m_chartDataBasic.Views[1]->SetChannelMetaData(0, LocalColor::Green);
    m_chartDataBasic.Views[2]->SetChannelMetaData(0, LocalColor::Red);
    m_chartDataBasic.Views[3]->SetChannelMetaData(0, LocalColor::LightBlue);    // lower left


    m_chartDataComplex.Data->SetChannelMetaData(0, LocalColor::LightBlue);
    m_chartDataComplex.Data->SetChannelMetaData(1, LocalColor::Orange);


    FSLLOG3_INFO("Preparing UI");
    {    // Build a simple UI
      auto uiFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
      m_uiRecord = CreateUI(*uiFactory);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_uiRecord.Main);
    }
    SetDefaultValues();
    // Override the initial generator mode
    m_dataGenerator.Mode = DataMode::GenerateLots;
    OnConfigurationChanged(config.WindowMetrics);
  }


  ChartsShared::~ChartsShared() = default;


  void ChartsShared::OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    if (theEvent->GetSource() == m_uiRecord.Menu.BtnDataGenOne)
    {
      m_dataGenerator.Mode = DataMode::GenerateOne;
    }
    else if (theEvent->GetSource() == m_uiRecord.Menu.BtnDataGenMultiple)
    {
      m_dataGenerator.Mode = DataMode::GenerateMultiple;
    }
    else if (theEvent->GetSource() == m_uiRecord.Menu.BtnSetDefaults)
    {
      SetDefaultValues();
    }
    else if (theEvent->GetSource() == m_uiRecord.Menu.BtnResetGen)
    {
      ResetGenerators();
    }
  }

  void ChartsShared::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    // if (theEvent->GetSource() == m_uiRecord.DrawOutlineCheckBox || theEvent->GetSource() == m_uiRecord.DrawShadowCheckBox)
    //{
    //  UpdateLinkedUIState();
    //}
    if (theEvent->GetSource() == m_uiRecord.Menu.RadioButtonDataContinuous || theEvent->GetSource() == m_uiRecord.Menu.RadioButtonDataPeriodically ||
        theEvent->GetSource() == m_uiRecord.Menu.RadioButtonDataPaused)
    {
      m_dataGenerator.Mode = ExtractUIDataMode();
    }
    if (theEvent->GetSource() == m_uiRecord.Menu.SwitchComplexGen)
    {
      ReconfigureChartData();
    }
  }


  void ChartsShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    // switch (event.GetKey())
    //{
    ////case VirtualKey::Delete:
    ////  break;
    // default:
    //   break;
    // }
  }


  void ChartsShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }

  void ChartsShared::Update(const DemoTime& demoTime)
  {
    uint32_t generateCount = 1;
    if (demoTime.ElapsedTime.Ticks() > 0)
    {
      switch (m_dataGenerator.Mode)
      {
      case DataMode::GenerateOne:
      case DataMode::GenerateMultiple:
        generateCount = m_dataGenerator.Mode == DataMode::GenerateOne ? 1 : 20;
        m_dataGenerator.TimeLeft = TimeSpan(-1);
        m_dataGenerator.Mode = DataMode::Paused;
        // m_dataGenerator.TimePassed += LocalConfig::DataGeneratorPeriod;
        m_uiRecord.Menu.RadioButtonDataPaused->SetIsChecked(true);
        break;
      case DataMode::GenerateLots:
        generateCount = 1000;
        m_dataGenerator.TimeLeft = TimeSpan(-1);
        m_dataGenerator.Mode = DataMode::Continuous;
        // m_dataGenerator.TimePassed += LocalConfig::DataGeneratorPeriod;
        m_uiRecord.Menu.RadioButtonDataContinuous->SetIsChecked(true);
        break;
      case DataMode::Paused:
        m_dataGenerator.TimeLeft = TimeSpan(1);
        // m_dataGenerator.TimePassed = {};
        break;
      case DataMode::Periodically:
        m_dataGenerator.TimeLeft -= demoTime.ElapsedTime;
        // m_dataGenerator.TimePassed += demoTime.ElapsedTime;
        break;
      case DataMode::Continuous:
      default:
        m_dataGenerator.TimeLeft = TimeSpan(-1);
        // m_dataGenerator.TimePassed = demoTime.ElapsedTime;
        break;
      }

      if (m_dataGenerator.TimeLeft.Ticks() <= 0)
      {
        // float generatorDeltaTime = static_cast<float>(static_cast<double>(m_dataGenerator.TimePassed.Ticks()) / double(TimeInfo::TicksPerSecond));
        const float generatorDeltaTime = demoTime.DeltaTime;
        m_dataGenerator.TimeLeft = LocalConfig::DataGeneratorPeriod;
        // m_dataGenerator.TimePassed = {};
        for (uint32_t i = 0; i < generateCount; ++i)
        {
          if (m_dataConfig == CachedDataConfig::BasicData)
          {
            UI::ChartDataEntry entry;

            if (m_uiRecord.Menu.RadioButtonGen1->IsChecked())
            {
              entry.Values[0] = m_dataGenerator1.Next(generatorDeltaTime).Sum();
            }
            else
            {
              entry.Values[0] = m_dataGenerator2.Next().Sum();
            }
            m_chartDataBasic.Data->Append(entry);
          }
          else
          {
            UI::ChartDataEntry entry;

            if (m_uiRecord.Menu.RadioButtonGen1->IsChecked())
            {
              auto result = m_dataGenerator1.Next(generatorDeltaTime);
              entry.Values[0] = result.Val0;
              entry.Values[1] = result.Val1;
              // entry.Values[2] = 0;
              // entry.Values[3] = 0;
              // entry.Values[4] = 0;
              // entry.Values[5] = 0;
              // entry.Values[6] = 0;
            }
            else
            {
              auto result = m_dataGenerator2.Next();
              // entry.Values[0] = result.UIProcessEvents;
              // entry.Values[1] = result.UIUpdate;
              // entry.Values[2] = result.UIDraw;
              // entry.Values[3] = result.UIDrawPreprocess;
              // entry.Values[4] = result.UIDrawGenMesh;
              // entry.Values[5] = result.UIDrawFillBuffers;
              // entry.Values[6] = result.UIDrawSchedule;

              entry.Values[0] = result.UIProcessEvents + result.UIUpdate + result.UIDraw;
              entry.Values[1] = result.UIDrawPreprocess + result.UIDrawGenMesh + result.UIDrawFillBuffers + result.UIDrawSchedule;
            }
            m_chartDataComplex.Data->Append(entry);
          }
        }
      }
    }
  }

  void ChartsShared::Draw()
  {
    m_uiExtension->Draw();
  }


  void ChartsShared::SetDefaultValues()
  {
    m_uiRecord.Menu.SwitchComplexGen->SetIsChecked(false);
    m_uiRecord.Menu.RadioButtonGen1->SetIsChecked(true);
    m_uiRecord.Menu.RadioButtonDataContinuous->SetIsChecked(true);

    ReconfigureChartData();
    m_dataGenerator.Mode = ExtractUIDataMode();
  }

  void ChartsShared::ReconfigureChartData()
  {
    const CachedDataConfig requested = (!m_uiRecord.Menu.SwitchComplexGen->IsChecked()) ? CachedDataConfig::BasicData : CachedDataConfig::ComplexData;

    if (requested != m_dataConfig)
    {
      m_dataConfig = requested;
      // Change detected, so reconfigure the data
      m_chartDataBasic.Data->Clear();
      m_chartDataComplex.Data->Clear();
      SetDataViews(m_uiRecord, requested == CachedDataConfig::BasicData ? m_chartDataBasic : m_chartDataComplex);
    }
  }


  void ChartsShared::ResetGenerators()
  {
    m_chartDataBasic.Data->Clear();
    m_chartDataComplex.Data->Clear();
    m_dataGenerator.Reset();
    m_dataGenerator1.Reset();
    m_dataGenerator2.Reset();
  }


  ChartsShared::DataMode ChartsShared::ExtractUIDataMode()
  {
    if (m_uiRecord.Menu.RadioButtonDataPeriodically->IsChecked())
    {
      return DataMode::Periodically;
    }
    if (m_uiRecord.Menu.RadioButtonDataPaused->IsChecked())
    {
      return DataMode::Paused;
    }
    return DataMode::Continuous;
  }


  void ChartsShared::SetDataViews(UIRecord& rUIRecord, const ChartDataRecord& record)
  {
    rUIRecord.FullCharts.Chart->SetDataView(record.FullView);

    rUIRecord.AreaCharts.Chart0->SetDataView(record.Views[0]);
    rUIRecord.AreaCharts.Chart1->SetDataView(record.Views[1]);
    rUIRecord.AreaCharts.Chart2->SetDataView(record.Views[2]);
    rUIRecord.AreaCharts.Chart3->SetDataView(record.Views[3]);
  }


  ChartsShared::UIRecord ChartsShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto menuUI = CreateMenuUI(uiFactory);
    auto fullAreaChart = CreateFullAreaChartUI(uiFactory);
    auto areaCharts = CreateAreaChartsUI(uiFactory, true);
    auto areaChartsActualSize = CreateAreaChartsUI(uiFactory, false);
    auto boxPlots = CreateBoxPlotChartsUI(uiFactory);

    const auto& context = uiFactory.GetContext();

    auto exampleLayout = std::make_shared<UI::StackLayout>(context);
    exampleLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    exampleLayout->SetSpacing(DpSize1DF::Create(4));


    {    // Area charts
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("AreaCharts", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(areaCharts.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }
    {    // box plots
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("BoxPlots", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(boxPlots.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }

    {    // Area charts
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("AreaCharts", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(areaChartsActualSize.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }


    auto bottomLayout = std::make_shared<UI::ComplexStackLayout>(context);
    bottomLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    bottomLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomLayout->SetAlignmentY(UI::ItemAlignment::Stretch);

    {    // Full area chart
      auto stackLayout = std::make_shared<UI::ComplexStackLayout>(context);
      auto caption = uiFactory.CreateLabel("Full area chart", UI::Theme::FontType::Header);
      stackLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
      stackLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
      stackLayout->AddChild(caption, {});
      stackLayout->AddChild(fullAreaChart.Main, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      background->SetAlignmentX(UI::ItemAlignment::Stretch);
      background->SetAlignmentY(UI::ItemAlignment::Stretch);
      bottomLayout->AddChild(background, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    }

    auto contentLayout = std::make_shared<UI::ComplexStackLayout>(context);
    contentLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    contentLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    contentLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    contentLayout->SetSpacing(DpSize1DF::Create(4));
    contentLayout->AddChild(exampleLayout, {});
    contentLayout->AddChild(bottomLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));


    auto rootLayout = std::make_shared<UI::ComplexStackLayout>(context);
    rootLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    rootLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    rootLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    rootLayout->AddChild(contentLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    rootLayout->AddChild(menuUI.Main, UI::LayoutLength(UI::LayoutUnitType::Auto));


    // Databind the boxplots to the area charts (so we don't have to manually update them on source change)
    boxPlots.BoxPlot0->SetBinding(UI::BoxPlotChart::PropertyDataView, areaCharts.Chart0->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    boxPlots.BoxPlot1->SetBinding(UI::BoxPlotChart::PropertyDataView, areaCharts.Chart1->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    boxPlots.BoxPlot2->SetBinding(UI::BoxPlotChart::PropertyDataView, areaCharts.Chart2->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    boxPlots.BoxPlot3->SetBinding(UI::BoxPlotChart::PropertyDataView, areaCharts.Chart3->GetPropertyHandle(UI::AreaChart::PropertyDataView));

    // Databind the area charts 'actual size' to the area charts (so we don't have to manually update them on source change)
    areaChartsActualSize.Chart0->SetBinding(UI::AreaChart::PropertyDataView, areaCharts.Chart0->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    areaChartsActualSize.Chart1->SetBinding(UI::AreaChart::PropertyDataView, areaCharts.Chart1->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    areaChartsActualSize.Chart2->SetBinding(UI::AreaChart::PropertyDataView, areaCharts.Chart2->GetPropertyHandle(UI::AreaChart::PropertyDataView));
    areaChartsActualSize.Chart3->SetBinding(UI::AreaChart::PropertyDataView, areaCharts.Chart3->GetPropertyHandle(UI::AreaChart::PropertyDataView));

    return {rootLayout, menuUI, fullAreaChart, areaCharts, areaChartsActualSize, boxPlots};
  }


  ChartsShared::UIMenuRecord ChartsShared::CreateMenuUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();

    auto labelGen = uiFactory.CreateLabel("Generator");
    auto radioGroupDataGen = uiFactory.CreateRadioGroup("Data generator");
    auto rbGen1 = uiFactory.CreateRadioButton(radioGroupDataGen, "SinCurve", true);
    auto rbGen2 = uiFactory.CreateRadioButton(radioGroupDataGen, "Playback", false);
    auto switchComplexGen = uiFactory.CreateSwitch("Complex", false);
    switchComplexGen->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto btnResetGen = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Restart");
    btnResetGen->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto labelMode = uiFactory.CreateLabel("Data");
    auto radioGroupDataMode = uiFactory.CreateRadioGroup("Data mode");
    auto rbContinuous = uiFactory.CreateRadioButton(radioGroupDataMode, "Continuous", true);
    auto rbPeriodically = uiFactory.CreateRadioButton(radioGroupDataMode, "Periodically", false);
    auto rbPaused = uiFactory.CreateRadioButton(radioGroupDataMode, "Paused", false);

    auto btnDataGenOne = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Step1");
    btnDataGenOne->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto btnDataGenMultiple = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Step20");
    btnDataGenMultiple->SetAlignmentX(UI::ItemAlignment::Stretch);


    auto btnSetDefaults = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Set defaults");
    btnSetDefaults->SetAlignmentX(UI::ItemAlignment::Stretch);
    btnSetDefaults->SetAlignmentY(UI::ItemAlignment::Far);

    auto stack = std::make_shared<UI::ComplexStackLayout>(context);
    stack->SetAlignmentY(UI::ItemAlignment::Stretch);
    stack->SetOrientation(UI::LayoutOrientation::Vertical);
    stack->AddChild(labelGen, {});
    stack->AddChild(rbGen1, {});
    stack->AddChild(rbGen2, {});
    stack->AddChild(switchComplexGen, {});
    stack->AddChild(btnResetGen, {});
    stack->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), {});
    stack->AddChild(labelMode, {});
    stack->AddChild(rbContinuous, {});
    stack->AddChild(rbPeriodically, {});
    stack->AddChild(rbPaused, {});
    stack->AddChild(btnDataGenOne, {});
    stack->AddChild(btnDataGenMultiple, {});
    stack->AddChild(btnSetDefaults, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));


    auto bar = uiFactory.CreateRightBar(stack);
    return {bar,           rbGen1, rbGen2, switchComplexGen, btnResetGen, rbContinuous, rbPeriodically, rbPaused, btnDataGenOne, btnDataGenMultiple,
            btnSetDefaults};
  }


  ChartsShared::UIFullAreaChartsRecord ChartsShared::CreateFullAreaChartUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();

    auto chart = CreateAreaChart(context, resources);
    // chart->SetAlignmentX(UI::ItemAlignment::Near);
    // chart->SetAlignmentY(UI::ItemAlignment::Near);
    chart->SetAlignmentX(UI::ItemAlignment::Stretch);
    chart->SetAlignmentY(UI::ItemAlignment::Stretch);
    // chart->SetGridLines(std::make_shared<ChartGridLinesTest>());
    chart->SetGridLines(std::make_shared<UI::ChartGridLinesFps>());
    chart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);

    auto boxPlot = CreateBoxPlotChart(context, resources);
    boxPlot->SetAlignmentX(UI::ItemAlignment::Stretch);
    boxPlot->SetAlignmentY(UI::ItemAlignment::Stretch);
    boxPlot->SetOrientation(UI::LayoutOrientation::Vertical);
    boxPlot->SetOutlierSize(UI::DpLayoutSize1D::Create(3));
    boxPlot->SetWhiskerSize(UI::DpLayoutSize1D::Create(9));
    boxPlot->SetBoxSize(UI::DpLayoutSize1D::Create(14));


    auto stack = std::make_shared<UI::ComplexStackLayout>(context);
    stack->SetSpacing(DpSize1DF::Create(4.0f));
    stack->SetAlignmentX(UI::ItemAlignment::Stretch);
    stack->SetAlignmentY(UI::ItemAlignment::Stretch);
    stack->SetOrientation(UI::LayoutOrientation::Horizontal);
    stack->AddChild(chart, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stack->AddChild(boxPlot, {});


    // Databind the boxplot to the area chart (so we don't have to manually update them on source change)
    boxPlot->SetBinding(UI::BoxPlotChart::PropertyDataView, chart->GetPropertyHandle(UI::AreaChart::PropertyDataView));

    return {stack, chart, boxPlot};
  }


  ChartsShared::UIAreaChartsRecord ChartsShared::CreateAreaChartsUI(UI::Theme::IThemeControlFactory& uiFactory, const bool useFixedSize)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();

    auto lblGridDesc0 = uiFactory.CreateLabel("Fit");
    auto lblGridDesc1 = uiFactory.CreateLabel("Fixed");

    auto chart0 = CreateAreaChart(context, resources);
    auto chart1 = CreateAreaChart(context, resources);
    auto chart2 = CreateAreaChart(context, resources);
    auto chart3 = CreateAreaChart(context, resources);

    UI::GridRowDefinition gridRowDef(UI::GridUnitType::Auto);
    if (useFixedSize)
    {
      chart0->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
      chart1->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
      chart2->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
      chart3->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
      chart0->SetWidth(UI::DpLayoutSize1D(DpValue(125)));
      chart1->SetWidth(UI::DpLayoutSize1D(DpValue(250)));
      chart2->SetWidth(UI::DpLayoutSize1D(DpValue(125)));
      chart3->SetWidth(UI::DpLayoutSize1D(DpValue(250)));
      gridRowDef = {UI::GridUnitType::Fixed, 100.0f};
    }
    else
    {
      chart0->SetMatchDataViewEntries(false);
      chart1->SetMatchDataViewEntries(false);
      chart2->SetMatchDataViewEntries(false);
      chart3->SetMatchDataViewEntries(false);
    }

    auto gridLines = std::make_shared<ChartGridLinesTest>();
    chart0->SetGridLines(gridLines);
    chart1->SetGridLines(gridLines);
    chart2->SetGridLines(gridLines);
    chart3->SetGridLines(gridLines);


    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 2.0f));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 1.0f));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(gridRowDef);
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, 2.0f));
    layout->AddRowDefinition(gridRowDef);

    layout->AddChild(lblGridDesc0, 0, 0);
    layout->AddChild(lblGridDesc1, 0, 2);


    layout->AddChild(chart0, 4, 0);    // small
    layout->AddChild(chart1, 2, 0);    // large
    layout->AddChild(chart2, 4, 2);    // small
    layout->AddChild(chart3, 2, 2);    // large

    return {layout, chart0, chart1, chart2, chart3};
  }


  ChartsShared::UIBoxPlotsRecord ChartsShared::CreateBoxPlotChartsUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();

    auto lblGridDesc0 = uiFactory.CreateLabel("Fit");
    auto lblGridDesc1 = uiFactory.CreateLabel("Fixed");

    auto chart0 = CreateBoxPlotChart(context, resources);
    auto chart1 = CreateBoxPlotChart(context, resources);
    auto chart2 = CreateBoxPlotChart(context, resources);
    auto chart3 = CreateBoxPlotChart(context, resources);

    // chart0->SetWidth(UI::DpLayoutSize1D(DpValue(250)));
    // chart1->SetWidth(UI::DpLayoutSize1D(DpValue(250)));
    // chart2->SetWidth(UI::DpLayoutSize1D(DpValue(250)));
    // chart3->SetWidth(UI::DpLayoutSize1D(DpValue(250)));


    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 2.0f));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 4.0f));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, 2.0f));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    layout->AddChild(lblGridDesc0, 0, 0);
    layout->AddChild(lblGridDesc1, 0, 2);
    // layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Vertical), 3, 0);
    // layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Vertical), 3, 2);

    layout->AddChild(chart0, 4, 0);    // small
    layout->AddChild(chart1, 2, 0);    // large
    layout->AddChild(chart2, 4, 2);    // small
    layout->AddChild(chart3, 2, 2);    // large

    return {layout, chart0, chart1, chart2, chart3};
  }

}
