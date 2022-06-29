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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/Common/ChartGridLinesFps.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/GpuTimestamp/OptionParser.hpp>
#include <Shared/GpuTimestamp/Shared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint16_t IterationsMin = 1;
      constexpr const uint16_t IterationsMax = 3000;

      constexpr uint32_t AverageEntries = 20;
      constexpr uint32_t MaxProfileDataEntries = 1;

      constexpr Color ChartColor(0xFF3488A7);    // light blue
    }

    void SetContent(UI::FmtValueLabel<float>* pTarget, const double value)
    {
      if (pTarget != nullptr)
      {
        if (value <= 10000.0)
        {
          // pTarget->SetFormatString("{:.0f}us");
          pTarget->SetFormatString(u8"{:.0f}\u03BCs");
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

  Shared::Shared(const DemoAppConfig& config, const bool chartSupported)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_data(std::make_shared<UI::ChartData>(m_uiExtension->GetDataBinding(), config.WindowMetrics.ExtentPx.Width, LocalConfig::MaxProfileDataEntries,
                                             UI::ChartData::Constraints(0, {})))
    , m_dataAverage(LocalConfig::AverageEntries)
  {
    // Configure the chart channel meta data
    m_data->SetChannelMetaData(0, LocalConfig::ChartColor);


    std::shared_ptr<OptionParser> options = config.GetOptions<OptionParser>();
    m_defaultIterations = options->GetIterations();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    m_ui = CreateUI(*uiControlFactory, m_defaultIterations, m_data, chartSupported);
    m_uiExtension->SetMainWindow(m_ui.MainWindow);

    OnConfigurationChanged(config.WindowMetrics);
    FSLLOG3_INFO("Ready");
  }


  Shared::~Shared() = default;


  void Shared::SetProfileTime(const uint64_t time)
  {
    UI::ChartDataEntry entry;
    entry.Values[0] = UncheckedNumericCast<uint32_t>(MathHelper::Clamp(time, static_cast<uint64_t>(0), static_cast<uint64_t>(0xFFFFFFFF)));
    m_data->Append(entry);
    m_dataAverage.Append(entry);
  }


  void Shared::OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    if (theEvent->GetSource() == m_ui.BtnDefault)
    {
      SetDefaultValues();
    }
    else if (theEvent->GetSource() == m_ui.BtnClearChart)
    {
      ClearGraph();
    }
  }

  void Shared::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    FSL_PARAM_NOT_USED(theEvent);
  }

  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Space:
      SetDefaultValues();
      break;
    case VirtualKey::C:
      ClearGraph();
      break;
    case VirtualKey::P:
      m_ui.SwitchPause->Toggle();
      break;
    case VirtualKey::UpArrow:
      m_ui.SliderIterations->SubValue(5);
      break;
    case VirtualKey::DownArrow:
      m_ui.SliderIterations->AddValue(5);
      break;
    default:
      break;
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }

  void Shared::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    auto res = m_dataAverage.CalcAverage();
    if (!res.empty())
    {
      const auto average = static_cast<float>(res[0]);
      SetContent(m_ui.GpuTimeFmtLabel.get(), average);
    }
  }

  void Shared::Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_uiExtension->Draw();
  }


  void Shared::SetDefaultValues()
  {
    m_ui.SliderIterations->SetValue(m_defaultIterations);
    m_ui.SwitchPause->SetIsChecked(false);
  }


  void Shared::ClearGraph()
  {
    m_data->Clear();
    m_dataAverage.Clear();
  }

  Shared::UIRecord Shared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const uint16_t iterations,
                                    const std::shared_ptr<UI::AChartData>& data, const bool chartSupported)
  {
    auto context = uiFactory.GetContext();

    auto labelIterations = uiFactory.CreateLabel("Iterations: ");
    labelIterations->SetAlignmentY(UI::ItemAlignment::Center);
    auto sliderIterations = uiFactory.CreateSliderFmtValue(
      UI::LayoutOrientation::Horizontal, ConstrainedValue<uint32_t>(iterations, LocalConfig::IterationsMin, LocalConfig::IterationsMax));
    sliderIterations->SetAlignmentY(UI::ItemAlignment::Center);

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Set defaults");
    btnDefault->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto btnClearChart = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Clear chart");
    btnClearChart->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto switchPause = uiFactory.CreateSwitch("Pause");
    switchPause->SetAlignmentX(UI::ItemAlignment::Stretch);
    switchPause->SetAlignmentY(UI::ItemAlignment::Far);

    auto gpuTime = uiFactory.CreateLabel("GPU time");
    auto gpuTimeFmtLabel = uiFactory.CreateFmtValueLabel(0.0f, "{:.2f}ms");
    gpuTimeFmtLabel->SetAlignmentX(UI::ItemAlignment::Far);
    gpuTimeFmtLabel->SetAlignmentY(UI::ItemAlignment::Far);
    auto chart = std::make_shared<UI::AreaChart>(context);
    {
      chart->SetAlignmentX(UI::ItemAlignment::Stretch);
      chart->SetAlignmentY(UI::ItemAlignment::Stretch);
      chart->SetOpaqueFillSprite(uiFactory.GetResources().GetBasicFillSprite(true));
      chart->SetTransparentFillSprite(uiFactory.GetResources().GetBasicFillSprite(false));
      chart->SetGridLines(std::make_unique<UI::ChartGridLinesFps>());
      chart->SetDataView(data);
      chart->SetFont(uiFactory.GetResources().GetDefaultSpriteFont());
      chart->SetLabelBackground(uiFactory.GetResources().GetToolTipNineSliceSprite());
      chart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);
    }

    auto bottomGrid = std::make_shared<UI::GridLayout>(context);
    bottomGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 8));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 8));
    bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, 100));
    bottomGrid->AddChild(labelIterations, 0, 0);
    bottomGrid->AddChild(sliderIterations, 2, 0);
    bottomGrid->AddChild(btnDefault, 4, 0);
    bottomGrid->AddChild(gpuTime, 0, 1);
    bottomGrid->AddChild(gpuTimeFmtLabel, 0, 1);
    if (chartSupported)
    {
      bottomGrid->AddChild(chart, 2, 1);
    }
    else
    {
      auto notSupported = uiFactory.CreateLabel("Not supported");
      notSupported->SetAlignmentX(UI::ItemAlignment::Center);
      notSupported->SetAlignmentY(UI::ItemAlignment::Center);
      bottomGrid->AddChild(notSupported, 2, 1);
    }
    bottomGrid->AddChild(btnClearChart, 4, 1);
    bottomGrid->AddChild(switchPause, 4, 1);

    return {uiFactory.CreateBottomBar(bottomGrid), sliderIterations, btnDefault, gpuTimeFmtLabel, btnClearChart, switchPause};
  }
}
