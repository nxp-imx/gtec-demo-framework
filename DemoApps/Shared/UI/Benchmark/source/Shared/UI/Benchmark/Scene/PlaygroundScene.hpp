#ifndef SHARED_UI_BENCHMARK_SCENE_PLAYGROUNDSCENE_HPP
#define SHARED_UI_BENCHMARK_SCENE_PLAYGROUNDSCENE_HPP
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

#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
//#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Render/Base/RenderOptionFlags.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemInfo.hpp>
#include <Shared/UI/Benchmark/Chart/AreaChartData.hpp>
#include <Shared/UI/Benchmark/Chart/AverageData.hpp>
#include <Shared/UI/Benchmark/Persistence/AppRenderMethod.hpp>
#include <Shared/UI/Benchmark/Persistence/AppSettings.hpp>
#include <Shared/UI/Benchmark/RenderMethodInfo.hpp>
#include <Shared/UI/Benchmark/Scene/Control/RenderOptionControls.hpp>
#include "Control/CpuDetailedLegendRecord.hpp"
#include "IScene.hpp"
#include <memory>
#include <future>

namespace Fsl
{
  struct AppBenchSettings;
  class DemoAppExtensionForwarder;
  struct SceneCreateInfo;
  class TestAppHost;
  class UIDemoAppExtension;

  namespace UI
  {
    class ActivityStack;
    class FrameAnalysisDialogActivity;
    struct RenderSystemStats;
    class SlidingPanel;

    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  class PlaygroundScene final : public IScene
  {
    enum class InputState
    {
      Playground,
      SettingsSubActivity,
      FrameAnalysisSubActivity,
      BenchConfigSubActivity,
      Closing
    };

    struct StatsOverlayUI
    {
      std::array<std::shared_ptr<UI::FmtValueLabel<uint32_t>>, 8> Entries;
      std::shared_ptr<UI::Background> MainLayout;
    };

    struct RenderMethodUI
    {
      std::vector<std::shared_ptr<UI::RadioButton>> Methods;
    };

    struct UISwitchButtons
    {
      std::shared_ptr<UI::Switch> SwitchStats;
      std::shared_ptr<UI::Switch> SwitchChart;
    };

    struct OptionBarUI
    {
      std::shared_ptr<UI::Layout> Layout;
      UISwitchButtons UI;
      RenderMethodUI RenderMethod;
      RenderOptionControls RenderOptions;
      // UI options
      std::shared_ptr<UI::Switch> SwitchSdfFont;
      std::shared_ptr<UI::Switch> SwitchEmulateDpi;
      std::shared_ptr<UI::SliderAndFmtValueLabel<int32_t>> SliderEmulatedDpi;
      std::shared_ptr<UI::ButtonBase> ButtonConfig;
      std::shared_ptr<UI::ButtonBase> ButtonFrameAnalysis;
      std::shared_ptr<UI::ButtonBase> ButtonRecord;
      std::shared_ptr<UI::ButtonBase> ButtonBench;
      std::shared_ptr<UI::Image> ImageIdle;
    };

    struct BottomBarUI
    {
      bool Enabled{false};
      std::shared_ptr<UI::BaseWindow> Main;
      CpuDetailedLegendRecord CpuLegendUI;
      std::shared_ptr<UI::BackgroundLabelButton> ButtonClear;
    };

    struct ProfileUI
    {
      std::shared_ptr<UI::SlidingPanel> BottomSlidingPanel;
      BottomBarUI BottomBar;
      StatsOverlayUI StatsOverlay;
      StatsOverlayUI StatsOverlay2;
      OptionBarUI OptionsBar;
      std::shared_ptr<UI::Layout> MainLayout;
      std::shared_ptr<UI::Layout> AppLayout;
      std::shared_ptr<UI::ActivityStack> ActivityStack;
      std::shared_ptr<UI::BaseWindow> ContentArea;
    };

    struct AnimRecord
    {
      TransitionColor OverlayColorStatsApp;
      TransitionColor OverlayColorStats;

      explicit AnimRecord(TransitionCache& rTransitionCache, const TransitionTimeSpan timespan)
        : OverlayColorStatsApp(rTransitionCache, timespan, TransitionType::Smooth)
        , OverlayColorStats(rTransitionCache, timespan, TransitionType::Smooth)
      {
      }

      void Update(const TransitionTimeSpan& deltaTime)
      {
        OverlayColorStatsApp.Update(deltaTime);
        OverlayColorStats.Update(deltaTime);
      }

      bool IsBusy() const
      {
        return !OverlayColorStatsApp.IsCompleted() || !OverlayColorStats.IsCompleted();
      }
    };
    struct CachedState
    {
      bool WasTestAppUIIdle{false};
      bool WasAppUIIdle{false};
    };

    std::shared_ptr<DemoAppExtensionForwarder> m_forwarder;
    std::shared_ptr<UI::Theme::IThemeControlFactory> m_uiControlFactory;
    DemoWindowMetrics m_windowMetrics;
    std::shared_ptr<const UIDemoAppExtension> m_uiExtension;
    bool m_gpuProfilerSupported{false};

    std::vector<RenderMethodInfo> m_renderRecords;
    uint32_t m_activeRenderIndex{0};

    std::shared_ptr<UI::AreaChartData> m_data;
    UI::AverageData m_dataAverage;

    ProfileUI m_uiProfile;
    AnimRecord m_anim;

    std::shared_ptr<TestAppHost> m_testAppHost;

    std::shared_ptr<AppSettings> m_settings;
    CachedState m_cachedState;

    InputState m_inputState{InputState::Playground};

    std::shared_ptr<UI::FrameAnalysisDialogActivity> m_frameAnalysisDialog;

    std::shared_ptr<AppTestSettings> m_settingsResult;
    std::shared_ptr<AppBenchSettings> m_benchResult;
    std::future<bool> m_configDialogPromise;

    Optional<NextSceneRecord> m_nextSceneRecord;

  public:
    explicit PlaygroundScene(const SceneCreateInfo& createInfo);
    ~PlaygroundScene() override;

    // std::shared_ptr<DemoAppExtension> GetDemoAppExtension() const
    //{
    //  return m_forwarder;
    //}


    Optional<NextSceneRecord> TryGetNextScene() const final
    {
      return m_nextSceneRecord;
    }

    void OnFrameSequenceBegin() final;
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnKeyEvent(const KeyEvent& event) final;
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void Draw(const DemoTime& demoTime) final;
    void OnFrameSequenceEnd() final;

  private:
    void UpdateSettingsSubActivityState();
    void UpdateFrameAnalysisSubActivityState();
    void UpdateBenchConfigSubActivityState();

    void SetDefaultValues();
    void ClearGraph();
    void RestoreUISettings(const RenderMethodInfo& renderMethodInfo);

    uint32_t FrameAnalysisGetCurrentMaxDrawCalls() const;
    void FrameAnalysisSetMaxDrawCalls(const uint32_t maxDrawCalls);
    static void UpdateStats(StatsOverlayUI& overlay, const UI::RenderSystemStats& stats);

    void RestartTestApp(const bool storeSettingsBeforeRestart);

    static ProfileUI CreateProfileUI(UI::Theme::IThemeControlFactory& uiFactory, const uint16_t currentDensityDpi,
                                     const std::shared_ptr<UI::AreaChartData>& data, const ReadOnlySpan<RenderMethodInfo> renderRecordSpan,
                                     const uint32_t activeRenderIndex, const AppUISettings& settings);
    static OptionBarUI CreateOptionBarUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context,
                                         const uint16_t currentDensityDpi, const ReadOnlySpan<RenderMethodInfo> renderRecordSpan,
                                         const uint32_t activeRenderIndex, const AppUISettings& settings);
    static UISwitchButtons CreateUISwitchButtons(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context,
                                                 const AppUISettings& settings);
    static BottomBarUI CreateBottomBar(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context,
                                       const std::shared_ptr<UI::AreaChartData>& data);
    static StatsOverlayUI CreateStatsOverlayUI(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& context);

    void SetDpi(const uint16_t densityDpi);

    void BeginClose(const SceneId nextSceneId, std::shared_ptr<ISceneConfig> sceneConfig = {});
  };
}

#endif
