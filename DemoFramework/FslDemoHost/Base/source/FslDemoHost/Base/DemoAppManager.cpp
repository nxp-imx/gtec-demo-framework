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
#include <FslDemoApp/Base/Host/IDemoAppFactory.hpp>
#include <FslDemoApp/Base/DemoAppFirewall.hpp>
#include <FslDemoApp/Base/Service/Events/IEventService.hpp>
#include <FslDemoApp/Base/Service/ContentMonitor/IContentMonitor.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerOverlay.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/IDemoAppControlEx.hpp>
#include <FslDemoHost/Base/Service/Profiler/IProfilerServiceControl.hpp>
#include <FslDemoHost/Base/DemoAppManager.hpp>
#include <FslDemoHost/Base/DemoAppManagerEventListener.hpp>
#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceControl.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <cassert>
#include <utility>
#include <memory>

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
                                 const bool enableContentMonitor, const bool preallocateBasic2D, const uint32_t forcedUpdateTime,
                                 const bool renderSystemOverlay)
    : m_eventListener(std::make_shared<DemoAppManagerEventListener>())
    , m_demoAppSetup(std::move(demoAppSetup))
    , m_demoAppConfig(demoAppConfig)
    , m_state(DemoState::Running)
    , m_forcedUpdateTime(forcedUpdateTime)
    , m_frameTimeConfig(1000000 / 60)
    , m_maxFrameTime(1000000 / 2)
    , m_logStatsMode(logStatsMode)
    , m_logStatsFlags(logStatsFlags)
    , m_enableStats(enableStats)
    , m_useFirewall(enableFirewall)
    , m_preallocateBasic2D(preallocateBasic2D)
  {
    if (renderSystemOverlay)
    {
      m_demoAppProfilerOverlay = std::make_unique<DemoAppProfilerOverlay>(demoAppConfig.DemoServiceProvider, logStatsFlags);
    }

    m_expectedFrameTime = m_frameTimeConfig;
    m_demoAppControl = m_demoAppConfig.DemoServiceProvider.Get<IDemoAppControlEx>();
    m_graphicsService = m_demoAppConfig.DemoServiceProvider.TryGet<IGraphicsServiceControl>();
    m_profilerServiceControl = m_demoAppConfig.DemoServiceProvider.Get<IProfilerServiceControl>();
    m_profilerService = m_demoAppConfig.DemoServiceProvider.Get<IProfilerService>();
    m_cpuStatsService = m_demoAppConfig.DemoServiceProvider.TryGet<ICpuStatsService>();

    if (enableContentMonitor)
    {
      std::shared_ptr<IContentMonitor> contentMonitor = m_demoAppConfig.DemoServiceProvider.Get<IContentMonitor>();
      contentMonitor->Enable(true);
    }

    const std::shared_ptr<IEventService> eventsService = m_demoAppConfig.DemoServiceProvider.Get<IEventService>();
    eventsService->Register(m_eventListener);

    // if (!demoAppControl)
    //  throw std::invalid_argument("demoAppControl can not be null");

    ResetTimer();
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


  bool DemoAppManager::Process(const DemoWindowMetrics& windowMetrics, const bool isConsoleBasedApp)
  {
    if (ManageExitRequests(true))
    {
      return false;
    }

    if (m_state == DemoState::Suspended)
    {
      return true;
    }

    ManageAppState(windowMetrics, isConsoleBasedApp);

    if (ManageExitRequests(false))
    {
      return false;
    }

    // Detect metrics changes
    if (windowMetrics != m_demoAppConfig.WindowMetrics)
    {
      m_demoAppConfig.UpdateWindowMetrics(windowMetrics);

      if (m_graphicsService)
      {
        m_graphicsService->SetWindowMetrics(m_demoAppConfig.WindowMetrics, true);
      }
      m_demoApp->_ConfigurationChanged(windowMetrics);
    }

    // Check if the update timer should be reset or not
    assert(m_demoAppControl);
    if (m_demoAppControl->HasUpdateTimerResetRequest())
    {
      m_demoAppControl->ClearUpdateTimerResetRequest();
      ResetTimer();
    }

    const TimeStepMode timeStepMode = m_demoAppControl->GetTimeStepMode();
    ApplyTimeStepMode(timeStepMode);

    // FIX: change the way this works depending on if we have vsync enabled or not!!

    // Standard fixed time-step algorithm
    // bool bDrawCalled = false;
    {
      // If we can get a timestamp for when the last 'display buffer swap' occurred we could make this much more precise.
      const uint64_t timeNow = m_timer.GetTime();
      m_timeStatsBeforeUpdate = timeNow;

      // Handle forced update time if enabled
      if (m_forcedUpdateTime != 0)
      {
        m_timeThen = timeNow - m_forcedUpdateTime;
      }

      uint64_t timeDiff = timeNow - m_timeThen;
      m_timeThen = timeNow;

      switch (timeStepMode)
      {
      case TimeStepMode::Paused:
        timeDiff = 0;
        m_accumulatedTime = 0;
        break;
      case TimeStepMode::Step:
        timeDiff = m_expectedFrameTime;
        m_accumulatedTime = 0;
        m_demoAppControl->SetTimeStepMode(TimeStepMode::Paused);
        break;
      default:
        break;
      }

      if (timeDiff >= m_maxFrameTime)
      {
        timeDiff = m_expectedFrameTime;
      }

      m_accumulatedTime += timeDiff;
      m_timeDiff = timeDiff;
      // if (m_accumulatedTime >= m_expectedFrameTime)
      //  FSLLOG3_INFO("Time between draws: {}", m_accumulatedTime);
      // else
      //  FSLLOG3_INFO("Time between draws: {} (Warning drawing same frame)", m_accumulatedTime);

      const uint64_t expectedUpdateCount = m_accumulatedTime / m_expectedFrameTime;
      switch (timeStepMode)
      {
      case TimeStepMode::Slow2X:
      case TimeStepMode::Slow4X:
      case TimeStepMode::Fast2X:
      case TimeStepMode::Fast4X:
        timeDiff = expectedUpdateCount * m_frameTimeConfig;
        break;
      default:
        break;
      }
      m_accumulatedTotalTime += timeDiff;
      m_currentDemoTimeUpdate = DemoTime(m_accumulatedTotalTime, timeDiff);
      m_demoApp->_PreUpdate(m_currentDemoTimeUpdate);

      // We use m_frameTimeConfig instead of m_expectedFrameTime because m_expectedFrameTime might be modified
      // by the TimeStepMode.
      DemoTime demoTimeFixedUpdate(m_accumulatedTotalTimeFixed, m_frameTimeConfig);
      int updateCount = 0;
      while (m_accumulatedTime >= m_expectedFrameTime)
      {
        m_accumulatedTotalTimeFixed += m_expectedFrameTime;
        demoTimeFixedUpdate.TotalTimeInMicroseconds = m_accumulatedTotalTimeFixed;

        m_demoApp->_FixedUpdate(demoTimeFixedUpdate);
        m_accumulatedTime -= m_expectedFrameTime;
        ++updateCount;
      }


      m_demoApp->_Update(m_currentDemoTimeUpdate);
      m_demoApp->_PostUpdate(m_currentDemoTimeUpdate);

      m_timeStatsAfterUpdate = m_timer.GetTime();
    }

    ManageExitRequests(false);

    // Let the caller know update has been called
    return true;
  }


  AppDrawResult DemoAppManager::TryDraw()
  {
    auto result = m_demoApp->_TryPrepareDraw(m_currentDemoTimeUpdate);
    if (result != AppDrawResult::Completed)
    {
      return result;
    }

    m_demoApp->_Draw(m_currentDemoTimeUpdate);

    m_timeStatsAfterDraw = m_timer.GetTime();

    if (m_enableStats && m_state == DemoState::Running && m_demoAppProfilerOverlay)
    {
      m_demoAppProfilerOverlay->Draw(m_demoAppConfig.WindowMetrics);
    }

    ManageExitRequests(false);

    return result;
  }


  AppDrawResult DemoAppManager::TryAppSwapBuffers()
  {
    if (!m_demoApp)
    {
      return AppDrawResult::Completed;
    }
    return m_demoApp->_TrySwapBuffers(m_currentDemoTimeUpdate);
  }


  void DemoAppManager::OnActivate()
  {
    if (m_demoAppControl)
    {
      m_demoAppControl->RequestUpdateTimerReset();
    }
    else
    {
      ResetTimer();
    }
  }


  void DemoAppManager::OnDeactivate()
  {
  }


  void DemoAppManager::OnFrameSwapCompleted()
  {
    if (m_state == DemoState::Running)
    {
      const uint64_t deltaTimeUpdate = m_timeStatsAfterUpdate - m_timeStatsBeforeUpdate;
      const uint64_t deltaTimeDraw = m_timeStatsAfterDraw - m_timeStatsAfterUpdate;

      m_profilerServiceControl->AddFrameTimes(deltaTimeUpdate, deltaTimeDraw, m_timeDiff);

      const auto averageTime = m_profilerService->GetAverageFrameTime();

      if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::CPU) && m_cpuStatsService)
      {
        float cpuUsage = 0.0f;
        m_cpuStatsService->TryGetApplicationCpuUsage(cpuUsage);

        if (m_logStatsFlags.IsFlagged(DemoAppStatsFlags::Frame))
        {
          // Flags: Frame | CPU
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Latest, "All: {} FPS: {} Updates: {} Draw: {} CPU: {}", m_timeDiff,
                          (1000000.0f / m_timeDiff), deltaTimeUpdate, deltaTimeDraw, cpuUsage);
          FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Average, "Average All: {} FPS: {} Updates: {} Draw: {} CPU: {}", averageTime.TotalTime,
                          (1000000.0f / averageTime.TotalTime), averageTime.UpdateTime, averageTime.DrawTime, cpuUsage);
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
        FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Latest, "All: {} FPS: {} Updates: {} Draw: {}", m_timeDiff, (1000000.0f / m_timeDiff),
                        deltaTimeUpdate, deltaTimeDraw);
        FSLLOG3_INFO_IF(m_logStatsMode == LogStatsMode::Average, "Average All: {} FPS: {} Updates: {} Draw: {}", averageTime.TotalTime,
                        (1000000.0f / averageTime.TotalTime), averageTime.UpdateTime, averageTime.DrawTime);
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


  bool DemoAppManager::ManageExitRequests(const bool bCheckExternalOnly)
  {
    assert(m_demoAppControl);

    bool bExitRightAway = !bCheckExternalOnly && !m_demoApp;

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

    if (m_demoApp && (restartRequest || CheckRestartFlags(m_demoAppSetup.CustomAppConfig.RestartFlags, windowMetrics, m_demoAppConfig.WindowMetrics)))
    {
      // Release the app
      DoShutdownAppNow();
    }

    // Check if a exit request exist (this catches the rare case where the exit occurs during a screen resolution change was detected
    // and the app was discarded above and it requested a exit during destruction) thereby allowing a fast exit.
    if (!m_demoApp && m_demoAppControl->HasExitRequest())
    {
      return;
    }

    // Handle delayed app initialization
    if (!m_demoApp)
    {
      m_demoAppConfig.UpdateWindowMetrics(windowMetrics);
      if (m_graphicsService)
      {
        m_graphicsService->SetWindowMetrics(windowMetrics, m_preallocateBasic2D);
      }
      if (!applyFirewall && ((windowMetrics.ExtentPx != PxExtent2D(0, 0)) || isConsoleBasedApp))
      {
        m_demoApp = m_demoAppSetup.Factory->Allocate(m_demoAppConfig);
      }
      else
      {
        m_demoApp = std::make_shared<DemoAppFirewall>(m_demoAppConfig, m_demoAppSetup.Factory, isConsoleBasedApp);
      }

      m_demoApp->_PostConstruct();

      m_eventListener->SetDemoApp(m_demoApp);
      assert(m_demoApp);
    }
  }


  void DemoAppManager::ResetTimer()
  {
    m_timeThen = m_timer.GetTime() - m_expectedFrameTime;
    m_accumulatedTime = 0;
  }


  void DemoAppManager::ApplyTimeStepMode(const TimeStepMode mode)
  {
    switch (mode)
    {
    case TimeStepMode::Normal:
    case TimeStepMode::Paused:
    case TimeStepMode::Step:
      m_expectedFrameTime = m_frameTimeConfig;
      break;
    case TimeStepMode::Slow2X:
      m_expectedFrameTime = m_frameTimeConfig * 2;
      break;
    case TimeStepMode::Slow4X:
      m_expectedFrameTime = m_frameTimeConfig * 4;
      break;
    case TimeStepMode::Fast2X:
      m_expectedFrameTime = m_frameTimeConfig / 2;
      break;
    case TimeStepMode::Fast4X:
      m_expectedFrameTime = m_frameTimeConfig / 4;
      break;
    default:
      FSLLOG3_WARNING("Unknown timestep mode");
      break;
    }
  }

  void DemoAppManager::DoShutdownAppNow()
  {
    if (m_demoApp)
    {
      try
      {
        m_demoApp->_PreDestruct();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Exception throw in _PreDestruct: {}", ex.what());
        m_demoApp.reset();
        throw;
      }
      m_demoApp.reset();
    }
  }
}
