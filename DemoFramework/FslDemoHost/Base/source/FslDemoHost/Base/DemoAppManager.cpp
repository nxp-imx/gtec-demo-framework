/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslDemoApp/Base/DemoAppFirewall.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslDemoApp/Base/Host/IDemoAppFactory.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerOverlay.hpp>
#include <FslDemoApp/Base/Service/ContentMonitor/IContentMonitor.hpp>
#include <FslDemoApp/Base/Service/Events/IEventService.hpp>
#include <FslDemoHost/Base/DemoAppManager.hpp>
#include <FslDemoHost/Base/DemoAppManagerEventListener.hpp>
#include <FslDemoHost/Base/Service/AppInfo/IAppInfoControlService.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/IDemoAppControlEx.hpp>
#include <FslDemoHost/Base/Service/Profiler/IProfilerServiceControl.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceControl.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <cassert>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace
  {
    constexpr inline bool CheckRestartFlags(const CustomDemoAppConfigRestartFlags restartFlags, const DemoWindowMetrics& newWindowMetrics,
                                            const DemoWindowMetrics& oldWindowMetrics)
    {
      return (CustomDemoAppConfigRestartFlagsUtil::IsFlagged(restartFlags, CustomDemoAppConfigRestartFlags::Resize) &&
              newWindowMetrics.ExtentPx != oldWindowMetrics.ExtentPx) ||
             (CustomDemoAppConfigRestartFlagsUtil::IsFlagged(restartFlags, CustomDemoAppConfigRestartFlags::DpiChange) &&
              !newWindowMetrics.IsEqualDpi(oldWindowMetrics));
    }
  }

  DemoAppManager::DemoAppManager(DemoAppSetup demoAppSetup, const DemoAppConfig& demoAppConfig, const bool enableStats,
                                 const LogStatsMode logStatsMode, const DemoAppStatsFlags& logStatsFlags, const bool enableFirewall,
                                 const bool enableContentMonitor, const TimeSpan& forcedUpdateTime, const bool renderSystemOverlay)
    : m_eventListener(std::make_shared<DemoAppManagerEventListener>())
    , m_demoAppSetup(std::move(demoAppSetup))
    , m_demoAppConfig(demoAppConfig)
    , m_state(DemoState::Running)
    , m_appTiming(m_timer.GetTimestamp(), forcedUpdateTime)
    , m_logStatsMode(logStatsMode)
    , m_logStatsFlags(logStatsFlags)
    , m_enableStats(enableStats)
    , m_useFirewall(enableFirewall)
  {
    if (renderSystemOverlay)
    {
      m_demoAppProfilerOverlay = std::make_unique<DemoAppProfilerOverlay>(demoAppConfig.DemoServiceProvider, logStatsFlags);
    }
    m_demoAppControl = m_demoAppConfig.DemoServiceProvider.Get<IDemoAppControlEx>();
    m_graphicsService = m_demoAppConfig.DemoServiceProvider.TryGet<IGraphicsServiceControl>();
    m_profilerServiceControl = m_demoAppConfig.DemoServiceProvider.Get<IProfilerServiceControl>();
    m_profilerService = m_demoAppConfig.DemoServiceProvider.Get<IProfilerService>();
    m_cpuStatsService = m_demoAppConfig.DemoServiceProvider.TryGet<ICpuStatsService>();
    auto appInfo = m_demoAppConfig.DemoServiceProvider.Get<IAppInfoControlService>();
    appInfo->SetAppName(StringViewLiteUtil::AsStringViewLite(m_demoAppSetup.ApplicationName));

    m_demoAppControl->SetRenderLoopMaxFramesInFlight(m_demoAppSetup.CustomAppConfig.MaxFramesInFlight);

    if (enableContentMonitor)
    {
      std::shared_ptr<IContentMonitor> contentMonitor = m_demoAppConfig.DemoServiceProvider.Get<IContentMonitor>();
      contentMonitor->Enable(true);
    }

    const std::shared_ptr<IEventService> eventsService = m_demoAppConfig.DemoServiceProvider.Get<IEventService>();
    eventsService->Register(m_eventListener);

    // if (!demoAppControl)
    //  throw std::invalid_argument("demoAppControl can not be null");

    m_demoAppControl->RequestUpdateTimerReset();
  }


  DemoAppManager::~DemoAppManager()
  {
    try
    {
      DoShutdownAppNow();
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("DoShutdownAppNow failed: {}", ex.what());
      std::terminate();
    }
  }


  void DemoAppManager::Suspend(const bool bSuspend)
  {
    if (bSuspend)
    {
      m_state = DemoState::Suspended;
      // Ensure that the app is released when we enter suspended state
      DoShutdownAppNow();
    }
    else
    {
      m_state = DemoState::Running;
    }
  }


  DemoState DemoAppManager::GetState() const
  {
    return m_state;
  }


  DemoAppManagerProcessResult DemoAppManager::Process(const DemoWindowMetrics& windowMetrics, const bool isConsoleBasedApp)
  {
    if (ManageExitRequests(true))
    {
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::SkipDraw);
    }

    if (m_state == DemoState::Suspended)
    {
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::Draw);
    }

    ManageAppState(windowMetrics, isConsoleBasedApp);

    if (ManageExitRequests(false))
    {
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::SkipDraw);
    }

    m_record.DemoApp->_Begin();

    // Detect metrics changes
    if (windowMetrics != m_demoAppConfig.WindowMetrics)
    {
      m_demoAppConfig.UpdateWindowMetrics(windowMetrics);

      if (m_graphicsService)
      {
        m_graphicsService->SetWindowMetrics(m_demoAppConfig.WindowMetrics);
      }
      m_record.DemoApp->_ConfigurationChanged(windowMetrics);
    }

    // Check if the update timer should be reset or not
    assert(m_demoAppControl);
    if (m_demoAppControl->HasUpdateTimerResetRequest())
    {
      m_demoAppControl->ClearUpdateTimerResetRequest();
      ResetTimer();
    }

    {
      if (m_cachedState.CachedTimeStepMode == TimeStepMode::Step)
      {
        m_demoAppControl->SetTimeStepMode(TimeStepMode::Paused);
      }
      const DemoTime currentUpdateTime = m_appTiming.GetUpdateTime();

      m_stats.TimeBeforeUpdate = m_timer.GetTimestamp();
      {
        m_record.DemoApp->_PreUpdate(currentUpdateTime);

        {    // Run all missing fixed updates
          std::optional<DemoTime> fixedTime = m_appTiming.TryFixedUpdate();
          while (fixedTime.has_value())
          {
            m_record.DemoApp->_FixedUpdate(fixedTime.value());
            fixedTime = m_appTiming.TryFixedUpdate();
          }
        }

        if (m_graphicsService)
        {
          m_graphicsService->PreUpdate();
        }

        m_record.DemoApp->_Update(currentUpdateTime);
        m_record.DemoApp->_PostUpdate(currentUpdateTime);
        m_record.DemoApp->_Resolve(currentUpdateTime);
      }
      m_stats.TimeAfterUpdate = m_timer.GetTimestamp();
    }

    ManageExitRequests(false);
    CacheState();

    // Let the caller know update has been called
    return ProcessOnDemandRendering();
  }

  AppDrawResult DemoAppManager::TryDraw()
  {
    FrameInfo frameInfo(m_record.FrameIndex, m_currentDemoTimeDraw);

    auto result = m_record.DemoApp->_TryPrepareDraw(frameInfo);
    if (result != AppDrawResult::Completed)
    {
      return result;
    }

    m_record.DemoApp->_BeginDraw(frameInfo);
    try
    {
      m_record.DemoApp->_Draw(frameInfo);
      m_record.DemoApp->_EndDraw(frameInfo);
    }
    catch (std::exception& ex)
    {
      FSLLOG3_ERROR("Exception during draw: {}", ex.what());
      m_record.DemoApp->_EndDraw(frameInfo);
      throw;
    }

    m_stats.TimeAfterDraw = m_timer.GetTimestamp();

    if (m_enableStats && m_state == DemoState::Running && m_demoAppProfilerOverlay)
    {
      m_demoAppProfilerOverlay->Draw(m_demoAppConfig.WindowMetrics);
    }

    ManageExitRequests(false);

    return result;
  }


  void DemoAppManager::OnDrawSkipped()
  {
    if (m_record.DemoApp)
    {
      const FrameInfo frameInfo(m_record.FrameIndex, m_currentDemoTimeDraw);
      m_record.DemoApp->_OnDrawSkipped(frameInfo);
    }
  }


  AppDrawResult DemoAppManager::TryAppSwapBuffers()
  {
    if (!m_record.DemoApp)
    {
      return AppDrawResult::Completed;
    }
    FrameInfo frameInfo(m_record.FrameIndex, m_currentDemoTimeDraw);

    AppDrawResult result = m_record.DemoApp->_TrySwapBuffers(frameInfo);

    if (result == AppDrawResult::Completed)
    {    // Increase the frame index
      ++m_record.FrameIndex;
      m_record.FrameIndex %= m_demoAppControl->GetRenderLoopFrameCounter();
      assert(m_record.FrameIndex < m_demoAppSetup.CustomAppConfig.MaxFramesInFlight);
    }
    return result;
  }


  void DemoAppManager::OnActivate()
  {
    assert(m_demoAppControl);
    m_demoAppControl->RequestUpdateTimerReset();
  }


  void DemoAppManager::OnDeactivate()
  {
  }


  void DemoAppManager::OnFrameSwapCompleted()
  {
    if (m_state == DemoState::Running)
    {
      UpdateAppTimers();

      const auto deltaTimeUpdate = m_stats.TimeAfterUpdate - m_stats.TimeBeforeUpdate;
      const auto deltaTimeDraw = m_stats.TimeAfterDraw - m_stats.TimeAfterUpdate;

      const auto timeNow = m_timer.GetTimestamp();
      const auto deltaFrameSwapCompletedTime = timeNow - m_stats.LastFrameSwapCompletedTime;
      m_stats.LastFrameSwapCompletedTime = timeNow;

      m_profilerServiceControl->AddFrameTimes(TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeUpdate),
                                              TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeDraw),
                                              TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaFrameSwapCompletedTime));

      const auto averageTime = m_profilerService->GetAverageFrameTime();
      const auto averageTotalTime = TimeSpanUtil::FromMicroseconds(averageTime.TotalTime);
      const float frameFps = deltaFrameSwapCompletedTime.Ticks() > 0
                               ? (static_cast<float>(TimeInfo::TicksPerSecond) / static_cast<float>(deltaFrameSwapCompletedTime.Ticks()))
                               : 0.0f;
      const float averageFps =
        averageTotalTime.Ticks() > 0 ? (static_cast<float>(TimeInfo::TicksPerSecond) / static_cast<float>(averageTotalTime.Ticks())) : 0.0f;

      if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::CPU) && m_cpuStatsService)
      {
        float cpuUsage = 0.0f;
        m_cpuStatsService->TryGetApplicationCpuUsage(cpuUsage);

        if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::Frame))
        {
          // Flags: Frame | CPU
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Latest, "All: {} FPS: {} Updates: {} Draw: {} CPU: {}",
                          TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaFrameSwapCompletedTime), frameFps,
                          TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeUpdate), TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeDraw),
                          cpuUsage);
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Average, "Average All: {} FPS: {} Updates: {} Draw: {} CPU: {}",
                          TimeSpanUtil::ToClampedMicrosecondsUInt64(averageTotalTime), averageFps, averageTime.UpdateTime, averageTime.DrawTime,
                          cpuUsage);
        }
        else
        {
          // Flags: CPU
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Latest, "CPU: {}", cpuUsage);
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Average, "CPU: {}", cpuUsage);
        }
      }
      else
      {
        // Flags: Frame
        FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Latest, "All: {} FPS: {} Updates: {} Draw: {}",
                        TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaFrameSwapCompletedTime), frameFps,
                        TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeUpdate), TimeSpanUtil::ToClampedMicrosecondsUInt64(deltaTimeDraw));
        FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Average, "Average All: {} FPS: {} Updates: {} Draw: {}", averageTime.TotalTime, averageFps,
                        averageTime.UpdateTime, averageTime.DrawTime);
      }
    }
  }


  void DemoAppManager::OnDemandDrawSkipped()
  {
    if (m_state == DemoState::Running)
    {
      UpdateAppTimers();
    }
  }


  void DemoAppManager::ProcessDone()
  {
    if (m_state == DemoState::Running)
    {
      if (m_record.DemoApp)
      {
        m_record.DemoApp->_End();
      }
    }
  }


  void DemoAppManager::RequestExit()
  {
    m_hasExitRequest = true;
  }


  bool DemoAppManager::HasExitRequest() const
  {
    return m_hasExitRequest;
  }

  bool DemoAppManager::HasRestartRequest() const
  {
    return m_demoAppControl && m_demoAppControl->HasAppRestartRequest();
  }


  int DemoAppManager::CloseApp()
  {
    // Ensure that the app has been marked as requesting a exit
    if (!m_demoAppControl->HasExitRequest())
    {
      m_demoAppControl->RequestExit();
    }

    // Free the app
    DoShutdownAppNow();
    return m_demoAppControl->GetExitCode();
  }

  void DemoAppManager::CacheState()
  {
    m_cachedState.CachedTimeStepMode = m_demoAppControl->GetTimeStepMode();
  }


  void DemoAppManager::UpdateAppTimers()
  {
    // Apply any changes that might have occurred to the fixed update per seconds setting
    m_appTiming.SetFixedUpdatesPerSecond(m_demoAppControl->GetFixedUpdatesPerSecond());

    // If we can get a timestamp for when the last 'display buffer swap' occurred we could make this much more precise.
    m_appTiming.TimeNow(m_timer.GetTimestamp(), m_cachedState.CachedTimeStepMode);
  }


  DemoAppManagerProcessResult DemoAppManager::ProcessOnDemandRendering()
  {
    // FSLLOG3_VERBOSE("{}", m_currentDemoTimeUpdate.DeltaTimeInMicroseconds);
    const uint16_t onDemandFrameInterval = m_demoAppControl->GetOnDemandFrameInterval();
    if (onDemandFrameInterval <= 1)
    {
      m_currentDemoTimeDraw = m_appTiming.GetUpdateTime();
      m_onDemandRendering = {};
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::Draw);
    }
    // FIX: we need to ensure that at least one frame is currently visible, we need more info from the 'owner' as the present could have failed
    if (onDemandFrameInterval != m_onDemandRendering.LastOnDemandFrameInterval)
    {
      double wait = 60.0 / onDemandFrameInterval;
      double waitTime = wait > 0 ? 1000000.0 / wait : 1000000.0;

      // Render the first frame after its been enabled
      auto waitTimeInMicroseconds = NumericCast<uint64_t>(static_cast<int64_t>(std::round(waitTime)));
      m_onDemandRendering = OnDemandRendering{onDemandFrameInterval, waitTimeInMicroseconds, 0};
      m_currentDemoTimeDraw = m_appTiming.GetUpdateTime();
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::Draw);
    }
    if (m_onDemandRendering.ForceRenderNextFrame)
    {
      // We do this to ensure that any app changes that might be frame delayed gets rendered (this is nice for a UI that tries to show the active
      // OnDemandFrameInterval)
      m_onDemandRendering.ForceRenderNextFrame = false;
      m_currentDemoTimeDraw = m_appTiming.GetUpdateTime();
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::Draw);
    }

    m_onDemandRendering.TimeInTicks += m_appTiming.GetUpdateTime().ElapsedTime.Ticks();
    if (m_onDemandRendering.TimeInTicks >= m_onDemandRendering.WaitTimeInTicks)
    {
      // const uint64_t overshoot = m_onDemandRendering.TimeInMicroseconds - m_onDemandRendering.WaitTimeInMicroseconds;
      // FSLLOG3_INFO("DRAW target time {}, time passed {}, diff: {}, skipped: {}", m_onDemandRendering.WaitTimeInMicroseconds,
      //             m_onDemandRendering.TimeInMicroseconds, overshoot, m_onDemandRendering.SkipCount);
      m_currentDemoTimeDraw = m_appTiming.GetUpdateTime();
      m_onDemandRendering.TimeInTicks = 0;
      m_onDemandRendering.SkipCount = 0;
      return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::Draw);
    }

    constexpr uint64_t defaultFramesPerSecond = 60;
    const uint64_t waitTimeLeft = m_onDemandRendering.WaitTimeInTicks - m_onDemandRendering.TimeInTicks;
    const uint64_t defaultFrameTime = 1000000 / defaultFramesPerSecond;
    const uint64_t sleepTime = defaultFrameTime <= waitTimeLeft ? defaultFrameTime : waitTimeLeft;
    ++m_onDemandRendering.SkipCount;
    return DemoAppManagerProcessResult(DemoAppManagerProcessResult::Command::SkipDrawSleep, sleepTime);
  }


  bool DemoAppManager::ManageExitRequests(const bool bCheckExternalOnly)
  {
    assert(m_demoAppControl);

    bool bExitRightAway = !bCheckExternalOnly && !m_record.DemoApp;

    if (!m_hasExitRequest && m_demoAppControl->HasExitRequest())
    {
      m_hasExitRequest = true;
    }
    return bExitRightAway;
  }


  void DemoAppManager::ManageAppState(const DemoWindowMetrics& windowMetrics, const bool isConsoleBasedApp)
  {
    assert(m_demoAppControl);

    bool applyFirewall = m_useFirewall;

    const bool restartRequest = HasRestartRequest();
    if (restartRequest)
    {
      m_demoAppControl->ClearAppRestartRequestRequest();
      applyFirewall = true;
    }

    if (m_record.DemoApp &&
        (restartRequest || CheckRestartFlags(m_demoAppSetup.CustomAppConfig.RestartFlags, windowMetrics, m_demoAppConfig.WindowMetrics)))
    {
      // Release the app
      DoShutdownAppNow();
    }

    // Check if a exit request exist (this catches the rare case where the exit occurs during a screen resolution change was detected
    // and the app was discarded above and it requested a exit during destruction) thereby allowing a fast exit.
    if (!m_record.DemoApp && m_demoAppControl->HasExitRequest())
    {
      return;
    }

    // Handle delayed app initialization
    if (!m_record.DemoApp)
    {
      m_demoAppConfig.UpdateWindowMetrics(windowMetrics);
      if (m_graphicsService)
      {
        m_graphicsService->SetWindowMetrics(windowMetrics);
      }
      if (!applyFirewall && ((windowMetrics.ExtentPx != PxExtent2D(0, 0)) || isConsoleBasedApp))
      {
        m_record = AppRecord(m_demoAppSetup.Factory->Allocate(m_demoAppConfig));
      }
      else
      {
        m_record = AppRecord(std::make_shared<DemoAppFirewall>(m_demoAppConfig, m_demoAppSetup.Factory, isConsoleBasedApp));
      }

      m_record.DemoApp->_PostConstruct();

      m_eventListener->SetDemoApp(m_record.DemoApp);
      assert(m_record.DemoApp);
    }
  }


  void DemoAppManager::ResetTimer()
  {
    // Apply any changes that might have occurred to the fixed update per seconds setting
    m_appTiming.SetFixedUpdatesPerSecond(m_demoAppControl->GetFixedUpdatesPerSecond());

    auto currentTime = m_timer.GetTimestamp();
    m_appTiming.ResetTimer(currentTime);
    m_appTiming.AdvanceFixedTimeStep();
    m_onDemandRendering = {};
    m_stats = {};
    m_stats.LastFrameSwapCompletedTime = currentTime;
  }


  void DemoAppManager::DoShutdownAppNow()
  {
    if (m_record.DemoApp)
    {
      try
      {
        m_record.DemoApp->_PreDestruct();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Exception throw in _PreDestruct: {}", ex.what());
        m_record = {};
        throw;
      }
      m_record = {};

      assert(m_demoAppControl);
      m_demoAppControl->RestoreDefaults();
    }
  }
}
