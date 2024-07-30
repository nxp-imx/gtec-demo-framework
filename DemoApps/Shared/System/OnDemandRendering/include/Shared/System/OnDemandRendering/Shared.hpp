#ifndef SHARED_SYSTEM_ONDEMANDRENDERING_SHARED_HPP
#define SHARED_SYSTEM_ONDEMANDRENDERING_SHARED_HPP
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
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Controls/Charts/Common/AverageData.hpp>
#include <Shared/System/OnDemandRendering/JankDetector.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class BasicImageSprite;
  class IBasicRenderSystem;
  class ICpuStatsService;
  class IDemoAppControl;
  class IProfilerService;
  class INativeBatch2D;

  namespace UI
  {
    class ChartData;
    class AChartData;
    class Image;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class Shared final : public UI::EventListener
  {
    struct StatsOverlayUI
    {
      std::shared_ptr<UI::BaseWindow> MainOverlay;
      std::shared_ptr<UI::Image> ImageIdle;
      std::shared_ptr<UI::FmtValueLabel<float>> LabelFramerate;
      std::shared_ptr<UI::FmtValueLabel<float>> LabelAverageFramerate;
      std::shared_ptr<UI::FmtValueLabel<uint32_t>> LabelFrameInterval;
      std::shared_ptr<UI::FmtValueLabel<float>> OptionalCpuUsage;

      StatsOverlayUI() = default;

      StatsOverlayUI(std::shared_ptr<UI::BaseWindow> mainOverlay, std::shared_ptr<UI::Image> imageIdle,
                     std::shared_ptr<UI::FmtValueLabel<float>> labelFramerate, std::shared_ptr<UI::FmtValueLabel<float>> labelAverageFramerate,
                     std::shared_ptr<UI::FmtValueLabel<uint32_t>> labelFrameInterval, std::shared_ptr<UI::FmtValueLabel<float>> optionalCpuUsage)
        : MainOverlay(std::move(mainOverlay))
        , ImageIdle(std::move(imageIdle))
        , LabelFramerate(std::move(labelFramerate))
        , LabelAverageFramerate(std::move(labelAverageFramerate))
        , LabelFrameInterval(std::move(labelFrameInterval))
        , OptionalCpuUsage(std::move(optionalCpuUsage))
      {
      }
    };

    struct PlayUI
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
    };

    struct DeltaTimeUI
    {
      std::shared_ptr<UI::BaseWindow> MainLayout;
      std::shared_ptr<UI::Switch> SwitchAnimate;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderRefreshRate;
      std::shared_ptr<UI::BackgroundLabelButton> BtnReset;
    };

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> MainWindow;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderIdleFrameInterval;
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> SliderFixedUpdatesPerSecond;
      std::shared_ptr<UI::BackgroundLabelButton> BtnDefault;
      std::shared_ptr<UI::FmtValueLabel<float>> UpdateFmtLabel;
      std::shared_ptr<UI::FmtValueLabel<float>> DrawFmtLabel;
      std::shared_ptr<UI::FmtValueLabel<float>> FixedUpdateFmtLabel;
      std::shared_ptr<UI::BackgroundLabelButton> BtnClearChart;
      std::shared_ptr<UI::Switch> SwitchOnDemand;

      PlayUI Play;
      DeltaTimeUI Time;
      StatsOverlayUI Stats;
    };

    struct DefaultValues
    {
      uint16_t FixedUpdatesPerSecond{1};
      uint16_t OnDemandFrameInterval{1};

      explicit DefaultValues(const uint16_t fixedUpdatesPerSecond, const uint16_t onDemandFrameInterval)
        : FixedUpdatesPerSecond(fixedUpdatesPerSecond)
        , OnDemandFrameInterval(onDemandFrameInterval)
      {
      }
    };

    struct CachedState
    {
      bool WasUIIdle{false};
    };

    struct OnDemandState
    {
      bool Enabled{false};
      uint16_t IdleFrameInterval{1};
      uint16_t DisabledFrameInterval{1};

      OnDemandState(const bool enabled, const uint16_t idleFrameInterval, const uint16_t disabledFrameInterval)
        : Enabled(enabled)
        , IdleFrameInterval(idleFrameInterval)
        , DisabledFrameInterval(disabledFrameInterval)
      {
      }

      uint16_t CurrentInterval() const noexcept
      {
        return Enabled ? IdleFrameInterval : DisabledFrameInterval;
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    std::shared_ptr<IProfilerService> m_profilerService;
    std::shared_ptr<ICpuStatsService> m_cpuStatsService;
    std::shared_ptr<INativeBatch2D> m_nativeBatch;
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;
    DefaultValues m_defaults;

    std::shared_ptr<UI::ChartData> m_dataUpdate;
    UI::AverageData m_dataUpdateAverage;
    std::shared_ptr<UI::ChartData> m_dataDraw;
    UI::AverageData m_dataDrawAverage;
    std::shared_ptr<UI::ChartData> m_dataFixedUpdate;
    UI::AverageData m_dataFixedUpdateAverage;

    UIRecord m_ui;
    DemoWindowMetrics m_cachedWindowMetrics;
    std::shared_ptr<BasicImageSprite> m_fillSprite;
    AtlasTexture2D m_fillTexture;

    uint32_t m_fixedUpdateCount{0};

    OnDemandState m_onDemandRendering;
    CachedState m_cachedState;

    JankDetector m_jankDetector;

  public:
    explicit Shared(const DemoAppConfig& config);
    ~Shared() override;

    std::shared_ptr<UIDemoAppExtension> GetUIDemoAppExtension() const
    {
      return m_uiExtension;
    }

    void OnKeyEvent(const KeyEvent& event);

    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void FixedUpdate(const DemoTime& demoTime);
    void Update(const DemoTime& demoTime);
    void Resolve(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);
    void OnFrameSequenceEnd();

  private:
    static void DrawTest(INativeBatch2D& batch, const AtlasTexture2D& fillTexture, const DemoWindowMetrics& windowMetrics,
                         const JankDetector::AnimationRecord0& record0, const JankDetector::AnimationRecord1& record1,
                         const ReadOnlySpan<JankDetector::TimingRecords> timingRecords, const JankDetector::AnimationRecord1& record2);

    void SetDefaultValues();
    void SetAnimDefaultValues();
    void ResetAnim();

    void AddSpeedIdleFrameInterval(const uint32_t value);
    void SubSpeedIdleFrameInterval(const uint32_t value);
    void AddFixedUpdatesPerSecond(const uint32_t value);
    void SubFixedUpdatesPerSecond(const uint32_t value);
    void AddAnimationFramesPerSecond(const uint32_t value);
    void SubAnimationFramesPerSecond(const uint32_t value);
    void ToggleAnimation();
    void ToggleOnDemandRendering();
    void SetOnDemandRendering(const bool enabled);

    void ClearCharts();
    static UIRecord CreateUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::AChartData>& dataUpdate,
                             const std::shared_ptr<UI::AChartData>& dataDraw, const std::shared_ptr<UI::AChartData>& dataFixedUpdate,
                             const uint16_t fixedUpdatesPerSecond, const OnDemandState& onDemandRendering, const bool hasCpuStats);
    static PlayUI CreatePlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context);
    static DeltaTimeUI CreateDeltaTimeUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context);
    static StatsOverlayUI CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context,
                                               const bool hasCpuStats);
  };
}

#endif
