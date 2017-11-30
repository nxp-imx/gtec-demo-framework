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
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/Host/IDemoAppFactory.hpp>
#include <FslDemoApp/Base/DemoAppFirewall.hpp>
#include <FslDemoApp/Base/Service/Events/IEventService.hpp>
#include <FslDemoApp/Base/Service/ContentMonitor/IContentMonitor.hpp>
#include <FslDemoApp/Base/Service/Profiler/IProfilerService.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/IDemoAppControlEx.hpp>
#include <FslDemoHost/Base/Service/Profiler/IProfilerServiceControl.hpp>
#include <FslDemoHost/Base/DemoAppManager.hpp>
#include <FslDemoHost/Base/DemoAppManagerEventListener.hpp>
#include <FslDemoHost/Base/Service/Graphics/IGraphicsServiceControl.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <cassert>
//#include <iostream>

#if 0
#define LOCAL_LOG(X) FSLLOG("DemoAppManager: " << X)
#else
#define LOCAL_LOG(X) {}
#endif

namespace Fsl
{
  DemoAppManager::DemoAppManager(const DemoAppSetup& demoAppSetup, const DemoAppConfig& demoAppConfig, const bool enableStats, const LogStatsMode logStatsMode, const bool enableFirewall, const bool enableContentMonitor, const bool preallocateBasic2D, const uint32_t forcedUpdateTime)
    : m_demoAppProfilerOverlay(demoAppSetup, demoAppConfig)
    , m_demoAppControl()
    , m_graphicsService()
    , m_profilerServiceControl()
    , m_profilerService()
    , m_eventListener(std::make_shared<DemoAppManagerEventListener>())
    , m_demoAppSetup(demoAppSetup)
    , m_demoAppConfig(demoAppConfig)
    , m_state(DemoState::Running)
    , m_hasExitRequest(false)
    , m_timer()
    , m_forcedUpdateTime(forcedUpdateTime)
    , m_frameTimeConfig(1000000 / 60)
    , m_timeThen(0)
    , m_accumulatedTime(0)
    , m_expectedFrameTime(0)
    , m_maxFrameTime(1000000 / 2)
    , m_timeStatsBeforeUpdate(0)
    , m_timeStatsAfterUpdate(0)
    , m_timeStatsAfterDraw(0)
    , m_timeStatsLast(0)
    , m_accumulatedTotalTimeFixed(0)
    , m_accumulatedTotalTime(0)
    , m_logStatsMode(logStatsMode)
    , m_enableStats(enableStats)
    , m_useFirewall(enableFirewall)
    , m_preallocateBasic2D(preallocateBasic2D)
    , m_demoApp()
  {
    m_expectedFrameTime = m_frameTimeConfig;
    m_demoAppControl = m_demoAppConfig.DemoServiceProvider.Get<IDemoAppControlEx>();
    m_graphicsService = m_demoAppConfig.DemoServiceProvider.Get<IGraphicsServiceControl>();
    m_profilerServiceControl = m_demoAppConfig.DemoServiceProvider.Get<IProfilerServiceControl>();
    m_profilerService = m_demoAppConfig.DemoServiceProvider.Get<IProfilerService>();

    if (enableContentMonitor)
    {
      std::shared_ptr<IContentMonitor> contentMonitor = m_demoAppConfig.DemoServiceProvider.Get<IContentMonitor>();
      contentMonitor->Enable(true);
    }

    const std::shared_ptr<IEventService> eventsService = m_demoAppConfig.DemoServiceProvider.Get<IEventService>();
    eventsService->Register(m_eventListener);

    //if (!demoAppControl)
    //  throw std::invalid_argument("demoAppControl can not be null");

    ResetTimer();
    m_timeStatsLast = m_timer.GetTime();
  }


  DemoAppManager::~DemoAppManager()
  {

  }


  void DemoAppManager::Suspend(const bool bSuspend)
  {
    if (bSuspend)
    {
      m_state = DemoState::Suspended;
      // Ensure that the app is released when we enter suspended state
      m_demoApp.reset();
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


  bool DemoAppManager::Process(const Point2& screenResolution)
  {
    if (ManageExitRequests(true))
      return false;

    if (m_state == DemoState::Suspended)
      return true;

    ManageAppState(screenResolution);

    if (ManageExitRequests(false))
      return false;

    // Detect resolution changes
    if (screenResolution != m_demoAppConfig.ScreenResolution)
    {
      m_demoAppConfig.ScreenResolution = screenResolution;
      m_graphicsService->SetScreenResolution(screenResolution, true);
      m_demoApp->_Resized(m_demoAppConfig.ScreenResolution);
    }

    // Check if the update timer should be reset or not
    assert(m_demoAppControl);
    if (m_demoAppControl->HasUpdateTimerResetRequest() )
    {
      m_demoAppControl->ClearUpdateTimerResetRequest();
      ResetTimer();
    }

    const TimeStepMode timeStepMode = m_demoAppControl->GetTimeStepMode();
    ApplyTimeStepMode(timeStepMode);

    // FIX: change the way this works depending on if we have vsync enabled or not!!

    // Standard fixed time-step algorithm
    //bool bDrawCalled = false;
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
        timeDiff = m_expectedFrameTime;

      m_accumulatedTime += timeDiff;
      //if (m_accumulatedTime >= m_expectedFrameTime)
      //  FSLLOG("Time between draws: " << m_accumulatedTime);
      //else
      //  FSLLOG("Time between draws: " << m_accumulatedTime << "(Warning drawing same frame)");

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
      const DemoTime demoTimeUpdate(m_accumulatedTotalTime, timeDiff);
      m_demoApp->_PreUpdate(demoTimeUpdate);

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


      m_demoApp->_Update(demoTimeUpdate);
      m_demoApp->_PostUpdate(demoTimeUpdate);

      m_timeStatsAfterUpdate = m_timer.GetTime();

      m_demoApp->_Draw(demoTimeUpdate);

      m_timeStatsAfterDraw = m_timer.GetTime();

      if (m_enableStats && m_state == DemoState::Running)
      {
        m_demoAppProfilerOverlay.Draw(screenResolution);
      }
    }

    ManageExitRequests(false);
    // Returning true to let the caller know we called draw.
    return true;
  }


  void DemoAppManager::OnActivate()
  {
    if (m_demoAppControl)
      m_demoAppControl->RequestUpdateTimerReset();
    else
      ResetTimer();
  }


  void DemoAppManager::OnDeactivate()
  {

  }


  void DemoAppManager::OnFrameSwapCompleted()
  {
    if (m_state == DemoState::Running)
    {
      const uint64_t currentTime = m_timer.GetTime();
      const uint64_t deltaTime = currentTime - m_timeStatsLast;
      const uint64_t deltaTimeUpdate = m_timeStatsAfterUpdate - m_timeStatsBeforeUpdate;
      const uint64_t deltaTimeDraw = m_timeStatsAfterDraw - m_timeStatsAfterUpdate;

      m_timeStatsLast = currentTime;

      m_profilerServiceControl->AddFrameTimes(deltaTimeUpdate, deltaTimeDraw, deltaTime);

      const auto averageTime = m_profilerService->GetAverageFrameTime();

      FSLLOG_IF(m_logStatsMode == LogStatsMode::Latest, "All: " << deltaTime << " FPS: " << (1000000.0f / deltaTime) << " Updates: " << deltaTimeUpdate << " Draw: " << deltaTimeDraw);
      FSLLOG_IF(m_logStatsMode == LogStatsMode::Average, "Average All: " << averageTime.TotalTime << " FPS: " << (1000000.0f / averageTime.TotalTime) << " Updates: " << averageTime.UpdateTime << " Draw: " << averageTime.DrawTime);
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


  int DemoAppManager::CloseApp()
  {
    // Ensure that the app has been marked as requesting a exit
    if (! m_demoAppControl->HasExitRequest())
      m_demoAppControl->RequestExit();

    // Free the app
    m_demoApp.reset();
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


  void DemoAppManager::ManageAppState(const Point2& screenResolution)
  {
    assert(m_demoAppControl);

    bool applyFirewall = m_useFirewall;

    const bool restartRequest = m_demoAppControl->HasAppRestartRequest();
    if (restartRequest)
    {
      m_demoAppControl->ClearAppRestartRequestRequest();
      applyFirewall = true;
    }

    if (m_demoApp && (restartRequest || (m_demoAppSetup.CustomAppConfig.RestartOnResize && screenResolution != m_demoAppConfig.ScreenResolution)))
    {
      // Release the app
      m_demoApp.reset();
    }

    // Check if a exit request exist (this catches the rare case where the exit occurs during a screen resolution change was detected
    // and the app was discarded above and it requested a exit during destruction) thereby allowing a fast exit.
    if (!m_demoApp && m_demoAppControl->HasExitRequest())
      return;

    // Handle delayed app initialization
    if (! m_demoApp)
    {
      m_graphicsService->SetScreenResolution(screenResolution, m_preallocateBasic2D);
      m_demoAppConfig.ScreenResolution = screenResolution;
      if ( ! applyFirewall )
        m_demoApp = m_demoAppSetup.Factory->Allocate(m_demoAppConfig);
      else
        m_demoApp = std::make_shared<DemoAppFirewall>(m_demoAppConfig, m_demoAppSetup.Factory);

      m_demoApp->_PostConstruct();

      m_eventListener->SetDemoApp(m_demoApp);
      assert(m_demoApp);
    }
  }


  void DemoAppManager::ResetTimer()
  {
    m_timeThen = m_timer.GetTime() - m_expectedFrameTime;
    m_accumulatedTime = 0;
    m_timeStatsLast = m_timeThen;
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
      FSLLOG_WARNING("Unknown timestep mode");
      break;
    }
  }

}
