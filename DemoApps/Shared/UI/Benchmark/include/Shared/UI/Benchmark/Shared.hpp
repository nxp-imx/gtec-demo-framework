#ifndef SHARED_UI_BENCHMARK_SHARED_HPP
#define SHARED_UI_BENCHMARK_SHARED_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIColor.hpp>
#include <Shared/UI/Benchmark/DemoAppExtensionForwarder.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkScene.hpp>
#include <Shared/UI/Benchmark/SceneAppInfo.hpp>
#include <Shared/UI/Benchmark/SceneId.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    class Image;
    class Layout;
    namespace Theme
    {
      class IThemeControlFactory;
    }
  }

  struct AppSettings;
  class BenchResultManager;
  struct FrameInfo;
  class IBasicGpuProfiler;
  class IDemoAppControl;
  class InputRecordingManager;
  class IScene;
  struct NextSceneRecord;
  struct SceneCreateInfo;

  class Shared final : public UI::EventListener
  {
    struct SceneRecord
    {
      SceneId CurrentId{SceneId::Playground};
      std::unique_ptr<IScene> Scene;
    };

    std::shared_ptr<IBasicGpuProfiler> m_gpuProfiler;
    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;
    DemoWindowMetrics m_windowMetrics;
    std::shared_ptr<DemoAppExtensionForwarder> m_forwarder;
    std::shared_ptr<UI::Theme::IThemeControlFactory> m_uiControlFactory;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    std::shared_ptr<InputRecordingManager> m_inputRecordingManager;
    SceneAppInfo m_appInfo;
    std::shared_ptr<BenchResultManager> m_benchResultManager;

    std::shared_ptr<UI::Layout> m_mainLayout;
    std::shared_ptr<UI::Layout> m_sceneLayout;
    //! We use this overlay for a 'brute-force' fade to black and fade in
    std::shared_ptr<UI::Image> m_overlayFillImage;
    UI::TransitionUIColor m_overlayColor;
    ServiceProvider m_serviceProvider;
    IO::Path m_settingsPath;
    std::shared_ptr<AppSettings> m_settings;
    AppBenchmarkScene m_benchmarkScene{AppBenchmarkScene::Scene0};

    SceneRecord m_sceneRecord;

  public:
    explicit Shared(const DemoAppConfig& config, std::shared_ptr<IBasicGpuProfiler> gpuProfiler = {});
    ~Shared() override;

    std::shared_ptr<IDemoAppExtension> GetDemoAppExtension() const
    {
      return m_forwarder;
    }

    void OnFrameSequenceBegin();


    // From EventListener
    void OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;


    void OnKeyEvent(const KeyEvent& event);
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& demoTime);
    void Resolve(const DemoTime& demoTime);
    void Draw(const DemoTime& demoTime);
    void OnDrawSkipped(const FrameInfo& frameInfo);
    void OnFrameSequenceEnd();

  private:
    void CloseScene();
    static SceneRecord SelectScene(IDemoAppControl& rDemoAppControl, const NextSceneRecord& scene, const SceneCreateInfo& createInfo,
                                   const std::shared_ptr<InputRecordingManager>& inputRecordingManager,
                                   const std::shared_ptr<BenchResultManager>& benchResultManager);
  };
}

#endif
