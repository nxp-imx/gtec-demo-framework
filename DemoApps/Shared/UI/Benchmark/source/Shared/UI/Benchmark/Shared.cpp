/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/AppInfo/IAppInfoService.hpp>
#include <FslDemoApp/Base/Service/DemoAppControl/IDemoAppControl.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/IBasicGpuProfiler.hpp>
#include <Shared/UI/Benchmark/OptionParser.hpp>
#include <Shared/UI/Benchmark/Persistence/AppSettings.hpp>
#include <Shared/UI/Benchmark/Persistence/AppSettingsPersistence.hpp>
#include <Shared/UI/Benchmark/Shared.hpp>
#include <algorithm>
#include "Scene/BenchResultManager.hpp"
#include "Scene/BenchmarkScene.hpp"
#include "Scene/Config/BenchSceneConfig.hpp"
#include "Scene/IScene.hpp"
#include "Scene/Input/InputRecordingManager.hpp"
#include "Scene/PlaygroundScene.hpp"
#include "Scene/RecordScene.hpp"
#include "Scene/RenderConfig.hpp"
#include "Scene/ResultScene.hpp"
#include "Scene/SceneCreateInfo.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView SettingsFilename("Settings.json");

      // We disable the benchmark UI's support for depth buffer so the test app's use wont interfere
      constexpr bool AllowDepthBuffer = false;
    }

    AppSettings LoadSettings(const IO::Path& path)
    {
      std::optional<AppSettings> settings = AppSettingsPersistence::TryLoad(path);
      if (settings.has_value())
      {
        FSLLOG3_INFO("Loaded settings from '{}'", path);
        return settings.value();
      }
      return {};
    }

    SceneAppInfo BuildAppInfo(const ServiceProvider& serviceProvider)
    {
      auto appInfo = serviceProvider.Get<IAppInfoService>();
      return {std::string(appInfo->GetAppName()), VersionInfo(1, 0), appInfo->IsDebugBuild()};
    }

    std::shared_ptr<BenchResultManager> CreateBenchResultManager(const DemoAppConfig& config, const bool appDebugMode)
    {
      auto optionParser = config.GetOptions<OptionParser>();
      return std::make_shared<BenchResultManager>(config.DemoServiceProvider, optionParser->TryGetViewReportFile(),
                                                  optionParser->TryGetCompareReportFile(), appDebugMode);
    }
  }

  Shared::Shared(const DemoAppConfig& config, std::shared_ptr<IBasicGpuProfiler> gpuProfiler)
    : m_gpuProfiler(std::move(gpuProfiler))
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlasBig/UIAtlas_160dpi",
                                                         UIDemoAppRenderCreateInfo(UI::UIColorSpace::SRGBNonLinear, UIDemoAppMaterialCreateInfo(),
                                                                                   UIDemoAppMaterialConfig(LocalConfig::AllowDepthBuffer)),
                                                         UITestPatternMode::DisabledAllowSwitching))
    , m_forwarder(std::make_shared<DemoAppExtensionForwarder>(m_uiExtension))
    , m_uiControlFactory(UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension))
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_inputRecordingManager(
        std::make_shared<InputRecordingManager>(config.DemoServiceProvider, config.GetOptions<OptionParser>()->GetRunDefaultBench()))
    , m_appInfo(BuildAppInfo(config.DemoServiceProvider))
    , m_benchResultManager(CreateBenchResultManager(config, m_appInfo.DebugMode))
    , m_mainLayout(std::make_shared<UI::FillLayout>(m_uiControlFactory->GetContext()))
    , m_sceneLayout(std::make_shared<UI::FillLayout>(m_uiControlFactory->GetContext()))
    , m_overlayFillImage(m_uiControlFactory->CreateImage(m_uiControlFactory->GetResources().GetBasicMiniFillSprite(false)))
    , m_overlayColor(TimeSpan::FromMilliseconds(240), TransitionType::Smooth)
    , m_serviceProvider(config.DemoServiceProvider)
    , m_settingsPath(IO::Path::Combine(m_serviceProvider.Get<IPersistentDataManager>()->GetPersistentDataPath(), LocalConfig::SettingsFilename))
    , m_settings(std::make_shared<AppSettings>(LoadSettings(m_settingsPath)))
  {
    // Ensure that the active render index is within the allowed range
    m_settings->Test.ActiveRenderIndex = std::clamp(m_settings->Test.ActiveRenderIndex, static_cast<uint32_t>(0), RenderConfig::GetSize());

    auto options = config.GetOptions<OptionParser>();
    {    // Command line override of the settings
      auto optionLineShowIdleEnabled = options->TryGetShowIdleEnabled();
      if (optionLineShowIdleEnabled.has_value())
      {
        m_settings->Test.ShowIdle = optionLineShowIdleEnabled.value();
      }

      const auto noChart = options->TryGetChartDisabled();
      if (noChart.has_value())
      {
        m_settings->UI.ShowChart = noChart.value();
      }
    }
    m_overlayFillImage->SetScalePolicy(UI::ItemScalePolicy::Fit);
    m_overlayFillImage->SetAlignmentX(UI::ItemAlignment::Stretch);
    m_overlayFillImage->SetAlignmentY(UI::ItemAlignment::Stretch);
    m_overlayFillImage->SetContentColor(UI::UIColors::Transparent());
    m_mainLayout->AddChild(m_sceneLayout);
    m_mainLayout->AddChild(m_overlayFillImage);
    m_uiExtension->SetMainWindow(m_mainLayout);

    SceneCreateInfo createInfo(m_appInfo, m_serviceProvider, config.WindowMetrics, m_forwarder, m_uiControlFactory, m_sceneLayout, m_settings,
                               m_uiExtension, m_gpuProfiler);
    NextSceneRecord nextSceneRecord(options->GetSceneId());
    m_sceneRecord = SelectScene(*m_demoAppControl, nextSceneRecord, createInfo, m_inputRecordingManager, m_benchResultManager);

    OnConfigurationChanged(config.WindowMetrics);
  }


  Shared::~Shared()
  {
    m_forwarder->Clear();
    m_forwarder.reset();
    m_sceneRecord.Scene.reset();

    // Store the settings
    assert(m_settings);
    try
    {
      AppSettingsPersistence::Save(m_settingsPath, *m_settings);
    }
    catch (std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to save '{}' due to '{}'", m_settingsPath, ex.what());
    }
  }


  void Shared::OnFrameSequenceBegin()
  {
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnFrameSequenceBegin();
    }
  }


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnSelect(theEvent);
    }
  }

  void Shared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnContentChanged(theEvent);
    }
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnKeyEvent(event);
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_windowMetrics = windowMetrics;
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnConfigurationChanged(windowMetrics);
    }
  }

  void Shared::Update(const DemoTime& demoTime)
  {
    if (m_sceneRecord.Scene)
    {
      const TimeSpan transitionTime(demoTime.ElapsedTime.Ticks());

      m_sceneRecord.Scene->Update(demoTime);
      auto closeRes = m_sceneRecord.Scene->TryGetNextScene();
      if (!closeRes.has_value())
      {
        m_overlayColor.SetValue(UI::UIColors::Transparent());
        m_overlayColor.Update(transitionTime);
      }
      else
      {
        assert(closeRes.has_value());
        m_overlayColor.SetValue(UI::UIColors::Black());
        m_overlayColor.Update(transitionTime);
        if (m_overlayColor.IsCompleted())
        {
          CloseScene();
          // Launch the next scene
          SceneCreateInfo createInfo(m_appInfo, m_serviceProvider, m_windowMetrics, m_forwarder, m_uiControlFactory, m_sceneLayout, m_settings,
                                     m_uiExtension, m_gpuProfiler);
          m_sceneRecord = SelectScene(*m_demoAppControl, closeRes.value(), createInfo, m_inputRecordingManager, m_benchResultManager);
        }
      }
      m_overlayFillImage->SetContentColor(m_overlayColor.GetValue());
    }
  }


  void Shared::Resolve(const DemoTime& demoTime)
  {
    bool isIdle = false;
    if (m_sceneRecord.Scene)
    {
      isIdle = m_sceneRecord.Scene->Resolve(demoTime) && m_overlayColor.IsCompleted();
    }

    const auto desiredFrameInterval = isIdle ? 60 : 1;
    m_demoAppControl->SetOnDemandFrameInterval(desiredFrameInterval);
  }


  void Shared::Draw(const DemoTime& demoTime)
  {
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->Draw(demoTime);
    }
    m_uiExtension->Draw();
  }


  void Shared::OnDrawSkipped(const FrameInfo& frameInfo)
  {
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnDrawSkipped(frameInfo);
    }
  }


  void Shared::OnFrameSequenceEnd()
  {
    if (m_sceneRecord.Scene)
    {
      m_sceneRecord.Scene->OnFrameSequenceEnd();
    }
  }


  void Shared::CloseScene()
  {
    // Destroy the scene
    m_sceneRecord.Scene.reset();
    // Clear all the shared objects just to be sure
    m_forwarder->ClearTweak();
    m_forwarder->ClearExtension1();
    m_sceneLayout->ClearChildren();
  }


  Shared::SceneRecord Shared::SelectScene(IDemoAppControl& rDemoAppControl, const NextSceneRecord& scene, const SceneCreateInfo& createInfo,
                                          const std::shared_ptr<InputRecordingManager>& inputRecordingManager,
                                          const std::shared_ptr<BenchResultManager>& benchResultManager)
  {
    // Clear all the shared objects
    createInfo.Forwarder->ClearTweak();
    createInfo.Forwarder->ClearExtension1();
    createInfo.RootLayout->ClearChildren();

    switch (scene.Id)
    {
    case SceneId::Benchmark:
      {
        BenchSceneConfig defaultSceneConfig;
        const BenchSceneConfig* pSceneConfig = nullptr;
        if (scene.Config)
        {
          pSceneConfig = dynamic_cast<const BenchSceneConfig*>(scene.Config.get());
          FSLLOG3_WARNING_IF(pSceneConfig == nullptr, "Benchmark scene started with unknown configuration type")
        }
        return {scene.Id, std::make_unique<BenchmarkScene>(createInfo, inputRecordingManager, benchResultManager,
                                                           pSceneConfig != nullptr ? *pSceneConfig : defaultSceneConfig)};
      }
    case SceneId::Playground:
      return {scene.Id, std::make_unique<PlaygroundScene>(createInfo)};
    case SceneId::Record:
      return {scene.Id, std::make_unique<RecordScene>(createInfo, inputRecordingManager)};
    case SceneId::Result:
      return {scene.Id, std::make_unique<ResultScene>(createInfo, benchResultManager)};
    default:
      throw NotSupportedException("Unknown scene");
    }
    rDemoAppControl.RequestUpdateTimerReset();
  }

}
