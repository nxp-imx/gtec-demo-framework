#ifndef SHARED_STATS_SHARED_HPP
#define SHARED_STATS_SHARED_HPP
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

#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoService/CpuStats/CpuUsageRecord.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/System/CallbackEventListenerScope.hpp>
#include <FslSimpleUI/Base/System/EventListener.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <fmt/format.h>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  class ICpuStatsService;
  class IGraphicsService;

  class Shared : public UI::EventListener
  {
    struct ChardRecord
    {
      std::shared_ptr<UI::ChartData> ChartData;
      CpuUsageRecord LastUsage;

      ChardRecord() = default;
      explicit ChardRecord(std::shared_ptr<UI::ChartData> chartData)
        : ChartData(std::move(chartData))
      {
      }
    };

    struct ChartDataRecord
    {
      ChardRecord AppCpu;

      std::vector<ChardRecord> Cores;
      std::shared_ptr<UI::ChartData> AppRam;


      explicit ChartDataRecord(ChardRecord appCpu, std::vector<ChardRecord>&& cores, std::shared_ptr<UI::ChartData>&& appRam)
        : AppCpu(std::move(std::move(appCpu)))
        , Cores(std::move(cores))
        , AppRam(std::move(appRam))
      {
      }
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;

      explicit UIRecord(std::shared_ptr<UI::BaseWindow>&& main)
        : Main(std::move(main))
      {
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IGraphicsService> m_graphics;
    std::shared_ptr<ICpuStatsService> m_cpuStats;

    ChartDataRecord m_chartData;
    UIRecord m_ui;

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void Update();
    void Draw();

  private:
    static ChartDataRecord CreateChartData(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
                                           const ICpuStatsService* const pCpuStats);
    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData);
    static std::shared_ptr<UI::BaseWindow> CreateAppUsageUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData);
    static std::shared_ptr<UI::BaseWindow> CreateCpuUsageUI(UI::Theme::IThemeControlFactory& uiFactory, const ChartDataRecord& chartData);
  };
}

#endif
