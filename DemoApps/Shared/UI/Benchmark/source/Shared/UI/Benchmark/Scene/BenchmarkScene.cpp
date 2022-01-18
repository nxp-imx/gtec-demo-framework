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

#include "BenchmarkScene.hpp"
#include <FslBase/NumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslSimpleUI/App/DemoPerformanceCapture.hpp>
#include <Shared/UI/Benchmark/App/TestAppFactory.hpp>
#include <Shared/UI/Benchmark/App/TestAppHost.hpp>
#include <Shared/UI/Benchmark/IBasicGpuProfiler.hpp>
#include <Shared/UI/Benchmark/Persistence/AppBenchSettings.hpp>
#include "BenchResultManager.hpp"
#include "ISceneTestDemoControl.hpp"
#include "Config/BenchSceneConfig.hpp"
#include "Input/InputRecordingManager.hpp"
#include "RenderSystemRuntimeSettingsUtil.hpp"
#include "SceneCreateInfo.hpp"
#include "TypeConverter.hpp"
#include "TestHostForwarderTweak.hpp"
#include <ctime>
#include <utility>

namespace Fsl
{
  namespace
  {
    AppBenchmarkInfo ExtractInfo(const SceneCreateInfo& createInfo)
    {
      return {createInfo.AppInfo.Name,
              createInfo.AppInfo.Version,
              createInfo.AppInfo.DebugMode,
              AppBenchmarkScene::Scene0,
              VersionInfo(1, 0),
              createInfo.WindowMetrics.GetSizePx(),
              NumericCast<uint16_t>(createInfo.WindowMetrics.DensityDpi)};
    }
  }


  BenchmarkScene::BenchmarkScene(const SceneCreateInfo& createInfo, std::shared_ptr<InputRecordingManager> inputRecordingManager,
                                 std::shared_ptr<BenchResultManager> benchResultManager, const BenchSceneConfig& sceneConfig)
    : BasicTestScene(createInfo)
    , m_info(ExtractInfo(createInfo))
    , m_inputRecordingManager(std::move(inputRecordingManager))
    , m_benchResultManager(std::move(benchResultManager))
    , m_gpuProfiler(createInfo.GpuProfiler)
  {
    if (m_gpuProfiler)
    {
      m_gpuProfiler->SetEnabled(sceneConfig.BenchSettings.BasicOptions.GpuTimestamps);
    }

    m_renderInfo = RestartTestApp(sceneConfig.BenchSettings);

    if (TryLoad())
    {
      const auto& demoControl = GetDemoControl();
      const bool hasRecording = demoControl.FrameCount() > 0;
      if (hasRecording)
      {
        auto& rDemoControl = GetDemoControl();
        assert(!rDemoControl.IsPlaying());
        rDemoControl.PlayBegin();
      }
      m_cpuEntries.resize(demoControl.FrameCount());
      m_gpuEntries.resize(demoControl.FrameCount());
    }
    else
    {
      FSLLOG3_WARNING("Failed to load input recording");
    }
  }


  BenchmarkScene::~BenchmarkScene() = default;


  void BenchmarkScene::Update(const DemoTime& demoTime)
  {
    if (m_state == State::Playing)
    {
      const auto& demoControl = GetDemoControl();
      if (!demoControl.IsPlaying())
      {
        // For now we just close, this is the point where we would show the results.
        ScheduleClose(SceneId::Result);
        m_state = State::Closing;

        TrySaveResult();
      }
    }
    BasicTestScene::Update(demoTime);
  }


  bool BenchmarkScene::Resolve(const DemoTime& demoTime)
  {
    BasicTestScene::Resolve(demoTime);
    return m_renderInfo.BasicOptions.UseOnDemandRendering ? m_testAppHost->IsUIIdle() : false;
  }


  void BenchmarkScene::Draw(const DemoTime& demoTime)
  {
    if (m_gpuProfiler && m_gpuProfiler->IsEnabled())
    {
      m_gpuProfiler->BeginTimestamp();
    }

    BasicTestScene::Draw(demoTime);

    if (m_gpuProfiler && m_gpuProfiler->IsEnabled())
    {
      m_gpuProfiler->EndTimestamp();
    }
  }


  void BenchmarkScene::OnDrawSkipped(const FrameInfo& frameInfo)
  {
    BasicTestScene::OnDrawSkipped(frameInfo);
  }


  void BenchmarkScene::OnFrameSequenceEnd()
  {
    if (m_state == State::Playing)
    {
      const uint64_t gpuMicroseconds = m_gpuProfiler ? m_gpuProfiler->GetResult() : 0;

      // record UI rendering CPU KPI's.
      const DemoPerformanceCapture& profiler = m_testAppHost->GetProfiler();

      int64_t valUIProcessEvents = profiler.GetResult(DemoPerformanceCaptureId::UIProcessEvents);
      int64_t valUIUpdate = profiler.GetResult(DemoPerformanceCaptureId::UIUpdate);
      int64_t valUIDraw = profiler.GetResult(DemoPerformanceCaptureId::UIDraw);
      int64_t valUIDrawPreprocess = profiler.GetResult(DemoPerformanceCaptureId::UIDrawPreprocessDrawCommands);
      int64_t valUIDrawGenMesh = profiler.GetResult(DemoPerformanceCaptureId::UIDrawGenerateMeshes);
      int64_t valUIDrawFillBuffers = profiler.GetResult(DemoPerformanceCaptureId::UIDrawUpdateBuffers);
      int64_t valUIDrawSchedule = profiler.GetResult(DemoPerformanceCaptureId::UIDrawScheduleDraw);

      valUIDraw -= valUIDrawPreprocess;
      valUIDraw -= valUIDrawGenMesh;
      valUIDraw -= valUIDrawFillBuffers;
      valUIDraw -= valUIDrawSchedule;

      const AppBenchmarkCpuRecord entry(static_cast<uint32_t>(MathHelper::Clamp(valUIProcessEvents, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIUpdate, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIDraw, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIDrawPreprocess, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIDrawGenMesh, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIDrawFillBuffers, int64_t(0), int64_t(0xFFFFFFFF))),
                                        static_cast<uint32_t>(MathHelper::Clamp(valUIDrawSchedule, int64_t(0), int64_t(0xFFFFFFFF))));

      const AppBenchmarkGpuRecord gpuEntry(static_cast<uint32_t>(MathHelper::Clamp(gpuMicroseconds, uint64_t(0), uint64_t(0xFFFFFFFF))));

      if (m_entryCount >= m_cpuEntries.size())
      {
        throw InternalErrorException("Out of room (cpu)");
      }
      if (m_entryCount >= m_gpuEntries.size())
      {
        throw InternalErrorException("Out of room (gpu)");
      }
      m_cpuEntries[m_entryCount] = entry;
      m_gpuEntries[m_entryCount] = gpuEntry;
      ++m_entryCount;
    }
  }

  AppBenchmarkRenderInfo BenchmarkScene::RestartTestApp(const AppBenchSettings& benchSettings)
  {
    Optional<AppRenderOptions> renderOptions = benchSettings.TryGetRenderOptions(benchSettings.RenderMethod);
    UI::RenderSystemInfo systemInfo = TestAppFactory::GetRenderSystemInfo(benchSettings.RenderMethod);
    if (!renderOptions.HasValue())
    {
      FSLLOG3_WARNING("Could not find render options for render method, defaulting to the default settings for the render-system");
      const auto defaultSettings = systemInfo.DefaultSettings;
      AppRenderOptions defaultRenderOptions(benchSettings.RenderMethod,
                                            UI::RenderOptionFlagsUtil::IsEnabled(defaultSettings, UI::RenderOptionFlags::Batch),
                                            UI::RenderOptionFlagsUtil::IsEnabled(defaultSettings, UI::RenderOptionFlags::FillBuffers),
                                            UI::RenderOptionFlagsUtil::IsEnabled(defaultSettings, UI::RenderOptionFlags::DepthBuffer),
                                            UI::RenderOptionFlagsUtil::IsEnabled(defaultSettings, UI::RenderOptionFlags::DrawReorder),
                                            UI::RenderOptionFlagsUtil::IsEnabled(defaultSettings, UI::RenderOptionFlags::MeshCaching));
      renderOptions = Optional<AppRenderOptions>(defaultRenderOptions);
    }

    {    // restart the test app with the new render code
      m_testAppHost->StopTestApp();
      TestAppFactory appFactory(benchSettings.RenderMethod);

      UIDemoAppMaterialCreateInfo materialCreateInfo(benchSettings.BasicOptions.NoOpaqueMaterials, true);
      UIDemoAppMaterialConfig materialConfig(benchSettings.BasicOptions.UseSdfFonts, renderOptions.Value().DepthBuffer);
      m_testAppHost->StartTestApp(appFactory, materialCreateInfo, materialConfig);
    }

    // Finally apply the runtime options by applying them to the system
    systemInfo.Set(TypeConverter::To<UI::RenderOptionFlags>(renderOptions.Value()));
    RenderSystemRuntimeSettingsUtil::Apply(*m_testAppHost, systemInfo.Settings, benchSettings.BasicOptions.UseSdfFonts);

    return {benchSettings.BasicOptions, renderOptions.Value()};
  }


  bool BenchmarkScene::TryLoad()
  {
    const Optional<AppInputCommandList> res = m_inputRecordingManager->TryGetRecording();
    if (res.HasValue())
    {
      GetDemoControl().SetRecording(res.Value().AsSpan(), res.Value().GetFrameCount());
    }
    return res.HasValue();
  }


  bool BenchmarkScene::TrySaveResult()
  {
    auto currentTime = std::time(nullptr);

    assert(m_benchResultManager);

    const bool gpuProfilingEnabled = m_gpuProfiler && m_gpuProfiler->IsEnabled();
    Optional<AppBenchmarkGpuData> gpuData =
      gpuProfilingEnabled ? Optional<AppBenchmarkGpuData>(AppBenchmarkGpuData(ReadOnlySpanUtil::AsSpan(m_gpuEntries, 0, m_entryCount)))
                          : Optional<AppBenchmarkGpuData>();

    Optional<AppBenchmarkRenderInfo> renderInfo(m_renderInfo);

    AppBenchmarkData benchmarkData(m_info, AppBenchmarkCpuData(ReadOnlySpanUtil::AsSpan(m_cpuEntries, 0, m_entryCount)), gpuData, renderInfo,
                                   currentTime);
    return m_benchResultManager->TrySave(benchmarkData);
  }

}
