/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/TypeConverter_Float.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/Stats/ChartGridLinesCpu.hpp>
#include <Shared/Stats/Shared.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t MaxDataEntries = 400;
      constexpr auto ChartWidth = UI::DpLayoutSize1D::Create(200.0f);
      constexpr auto ChartHeight = DpSize1DF::Create(100.0f);
    }

    struct AreaChartUI
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::AreaChart> Chart;

      AreaChartUI(std::shared_ptr<UI::BaseWindow> main, std::shared_ptr<UI::AreaChart> chart)
        : Main(std::move(main))
        , Chart(std::move(chart))
      {
      }
    };

    AreaChartUI CreateAreaChart(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::ChartData>& chartDataView,
                                const std::shared_ptr<UI::BaseWindow>& caption)
    {
      const auto& context = uiFactory.GetContext();
      const auto& resources = uiFactory.GetResources();

      auto chart = std::make_shared<UI::AreaChart>(context);
      chart->SetAlignmentX(UI::ItemAlignment::Stretch);
      chart->SetAlignmentY(UI::ItemAlignment::Stretch);
      chart->SetOpaqueFillSprite(resources.GetBasicFillSprite(true));
      chart->SetTransparentFillSprite(resources.GetBasicFillSprite(false));
      chart->SetFont(resources.GetDefaultSpriteFont());
      chart->SetLabelBackground(resources.GetToolTipNineSliceSprite());
      chart->SetWidth(LocalConfig::ChartWidth);
      chart->SetMinHeight(LocalConfig::ChartHeight);
      chart->SetDataView(chartDataView);

      auto layout = std::make_shared<UI::ComplexStackLayout>(context);
      layout->SetOrientation(UI::LayoutOrientation::Vertical);
      layout->SetAlignmentX(UI::ItemAlignment::Stretch);
      layout->SetAlignmentY(UI::ItemAlignment::Stretch);
      if (caption)
      {
        layout->AddChild(caption, UI::LayoutLength(UI::LayoutUnitType::Auto));
      }
      layout->AddChild(chart, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      return {layout, chart};
    }

    std::shared_ptr<UI::BaseWindow> CreateCpuAreaChart(UI::Theme::IThemeControlFactory& uiFactory,
                                                       const std::shared_ptr<UI::ChartData>& chartDataView, const std::string& strCaption)
    {
      const auto& context = uiFactory.GetContext();

      auto caption = uiFactory.CreateLabel(strCaption);

      auto fmtValueLabel = uiFactory.CreateFmtValueLabel(0.0f, "{:.1f}");
      fmtValueLabel->SetAlignmentX(UI::ItemAlignment::Far);

      auto layout = std::make_shared<UI::ComplexStackLayout>(context);
      layout->SetOrientation(UI::LayoutOrientation::Horizontal);
      layout->SetAlignmentX(UI::ItemAlignment::Stretch);
      layout->SetAlignmentY(UI::ItemAlignment::Stretch);
      layout->AddChild(caption, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(fmtValueLabel, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

      auto chartRecord = CreateAreaChart(uiFactory, chartDataView, layout);
      chartRecord.Chart->SetGridLines(std::make_shared<ChartGridLinesCpu>());
      chartRecord.Chart->SetRenderPolicy(UI::ChartRenderPolicy::Measure);

      {
        auto converterBinding = std::make_shared<Fsl::DataBinding::ConverterBinding<float, UI::ChartDataEntry>>(
          [](const UI::ChartDataEntry value) { return static_cast<float>(value.Values[0]); });

        fmtValueLabel->SetBinding(UI::FmtValueLabel<float>::PropertyContent,
                                  DataBinding::Binding(converterBinding, chartDataView->GetPropertyHandle(UI::ChartData::PropertyLatestEntry)));
      }

      return uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogNormal, chartRecord.Main);
    }

    std::shared_ptr<UI::BaseWindow> CreateRamAreaChart(UI::Theme::IThemeControlFactory& uiFactory,
                                                       const std::shared_ptr<UI::ChartData>& chartDataView, const std::string& strCaption)
    {
      const auto& context = uiFactory.GetContext();

      auto caption = uiFactory.CreateLabel(strCaption);

      auto fmtValueLabel = uiFactory.CreateFmtValueLabel(static_cast<uint64_t>(0), "{:10} KB");
      fmtValueLabel->SetAlignmentX(UI::ItemAlignment::Far);

      auto layout = std::make_shared<UI::ComplexStackLayout>(context);
      layout->SetOrientation(UI::LayoutOrientation::Horizontal);
      layout->SetAlignmentX(UI::ItemAlignment::Stretch);
      layout->SetAlignmentY(UI::ItemAlignment::Stretch);
      layout->AddChild(caption, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(fmtValueLabel, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

      auto chartRecord = CreateAreaChart(uiFactory, chartDataView, layout);
      chartRecord.Chart->SetRenderPolicy(UI::ChartRenderPolicy::FillAvailable);

      {
        auto converterBinding = std::make_shared<Fsl::DataBinding::ConverterBinding<uint64_t, UI::ChartDataEntry>>(
          [](const UI::ChartDataEntry value) { return UncheckedNumericCast<uint64_t>(value.Values[0]); });

        fmtValueLabel->SetBinding(UI::FmtValueLabel<uint64_t>::PropertyContent,
                                  DataBinding::Binding(converterBinding, chartDataView->GetPropertyHandle(UI::ChartData::PropertyLatestEntry)));
      }

      auto win = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogNormal, chartRecord.Main);
      win->SetAlignmentX(UI::ItemAlignment::Stretch);
      win->SetAlignmentY(UI::ItemAlignment::Stretch);
      return win;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_cpuStats(config.DemoServiceProvider.TryGet<ICpuStatsService>())
    , m_chartData(CreateChartData(m_uiExtension->GetDataBinding(), m_cpuStats.get()))
    , m_ui(CreateUI(*UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension), m_chartData))
  {
    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(m_ui.Main);
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& /*theEvent*/)
  {
  }

  void Shared::Update()
  {
    if (m_cpuStats)
    {
      {
        uint32_t i = 0;
        for (auto& rRecord : m_chartData.Cores)
        {
          CpuUsageRecord usage;
          if (m_cpuStats->TryGetCpuUsage(usage, i) && usage != rRecord.LastUsage)
          {
            rRecord.LastUsage = usage;
            rRecord.ChartData->Append(UI::ChartDataEntry(TypeConverter::ChangeTo<uint32_t>(usage.UsagePercentage)));
          }
          ++i;
        }
      }
      {
        CpuUsageRecord appCpuUsage;
        if (m_cpuStats->TryGetApplicationCpuUsage(appCpuUsage) && appCpuUsage != m_chartData.AppCpu.LastUsage)
        {
          m_chartData.AppCpu.LastUsage = appCpuUsage;
          m_chartData.AppCpu.ChartData->Append(UI::ChartDataEntry(TypeConverter::ChangeTo<uint32_t>(appCpuUsage.UsagePercentage)));
        }
      }
      {
        uint64_t appRamUsage = 0u;
        if (m_cpuStats->TryGetApplicationRamUsage(appRamUsage))
        {
          const auto ramUsage = UncheckedNumericCast<uint32_t>(appRamUsage / 1024);
          m_chartData.AppRam->Append(UI::ChartDataEntry(ramUsage));
        }
      }
    }
  }

  void Shared::Draw()
  {
    m_uiExtension->Draw();
  }


  Shared::ChartDataRecord Shared::CreateChartData(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
                                                  const ICpuStatsService* const pCpuStats)
  {
    auto appCpu = std::make_shared<UI::ChartData>(dataBinding, LocalConfig::MaxDataEntries, 1, UI::ChartData::Constraints(0, 100));
    auto appRam = std::make_shared<UI::ChartData>(dataBinding, LocalConfig::MaxDataEntries, 1, UI::ChartData::Constraints());

    const auto cpuCount = pCpuStats != nullptr ? pCpuStats->GetCpuCount() : 0u;
    std::vector<ChardRecord> cores(cpuCount);
    for (auto& rEntry : cores)
    {
      rEntry = ChardRecord(std::make_shared<UI::ChartData>(dataBinding, LocalConfig::MaxDataEntries, 1, UI::ChartData::Constraints(0, 100)));
    }
    return ChartDataRecord(ChardRecord(std::move(appCpu)), std::move(cores), std::move(appRam));
  }


  Shared::UIRecord Shared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData)
  {
    const auto& context = uiFactory.GetContext();

    auto appUsage = CreateAppUsageUI(uiFactory, chartData);
    auto cpuUsage = CreateCpuUsageUI(uiFactory, chartData);

    // Create the main layout
    auto mainLayout = std::make_shared<UI::StackLayout>(context);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddChild(appUsage);
    mainLayout->AddChild(cpuUsage);

    return Shared::UIRecord(std::move(mainLayout));
  }

  std::shared_ptr<UI::BaseWindow> Shared::CreateAppUsageUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData)
  {
    const auto& context = uiFactory.GetContext();

    auto chartCpu = CreateCpuAreaChart(uiFactory, chartData.AppCpu.ChartData, "App CPU");
    auto chartRam = CreateRamAreaChart(uiFactory, chartData.AppRam, "App RAM");

    auto mainLayout = std::make_shared<UI::UniformStackLayout>(context);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Center);
    mainLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    mainLayout->AddChild(chartCpu);
    mainLayout->AddChild(chartRam);

    return mainLayout;
  }


  std::shared_ptr<UI::BaseWindow> Shared::CreateCpuUsageUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData)
  {
    const auto& context = uiFactory.GetContext();

    auto mainLayout = std::make_shared<UI::UniformWrapLayout>(context);
    mainLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Center);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Center);
    std::size_t cpuIndex = 0;
    for (const auto& cpuCoreEntry : chartData.Cores)
    {
      auto coreChart = CreateCpuAreaChart(uiFactory, cpuCoreEntry.ChartData, fmt::format("CPU{}:", cpuIndex));
      mainLayout->AddChild(coreChart);
      ++cpuIndex;
    }
    return mainLayout;
  }
}
