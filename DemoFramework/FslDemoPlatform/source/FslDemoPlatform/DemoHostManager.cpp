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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoHost/Base/ADemoHost.hpp>
#include <FslDemoHost/Base/DemoAppManager.hpp>
#include <FslDemoHost/Base/IDemoHostFactory.hpp>
#include <FslDemoHost/Base/Service/Host/IHostInfoControl.hpp>
#include <FslDemoHost/Base/Service/Test/ITestService.hpp>
#include <FslDemoHost/Base/Service/NativeWindowEvents/INativeWindowEventSender.hpp>
#include <FslDemoPlatform/DemoHostManager.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceControl.hpp>
#include <FslService/Impl/Threading/IServiceHostLooper.hpp>
#include <FslNativeWindow/Base/NativeWindowEventQueue.hpp>
#include <cassert>
#include <thread>

namespace Fsl
{
  DemoHostManager::DemoHostManager(const DemoSetup& demoSetup, const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    : m_demoSetup(demoSetup)
    , m_demoHostCaps(m_demoSetup.Host.Factory->GetCaps())
    , m_eventQueue(new NativeWindowEventQueue())
    , m_state(State::Idle)
    , m_basic2DPreallocEnabled(demoHostManagerOptionParser->IsBasic2DPreallocEnabled())
    , m_exitAfterFrame(demoHostManagerOptionParser->GetExitAfterFrame())
    , m_exitAfterDuration(demoHostManagerOptionParser->GetDurationExitConfig())
    , m_exitTime(std::chrono::microseconds(m_timer.GetTime()) + std::chrono::microseconds(m_exitAfterDuration.Duration))
  {
    // Acquire the various services
    ServiceProvider serviceProvider(demoSetup.ServiceProvider);
    m_graphicsService = serviceProvider.TryGet<IGraphicsServiceControl>();
    m_nativeWindowEventSender = serviceProvider.Get<INativeWindowEventSender>();
    m_hostInfoControl = serviceProvider.Get<IHostInfoControl>();

    HostConfig hostConfig(demoHostManagerOptionParser->IsAppFirewallEnabled(), demoHostManagerOptionParser->IsContentMonitorEnabled(),
                          demoHostManagerOptionParser->IsStatsEnabled(), m_basic2DPreallocEnabled, demoHostManagerOptionParser->GetAppStatsFlags());
    m_hostInfoControl->SetConfig(hostConfig);

    // Set the config we are using for the app host so it can be retrieved from IHostInfo
    m_hostInfoControl->SetAppHostConfig(demoSetup.App.AppHostConfig);

    // Acquire and configure the test service
    m_testService = serviceProvider.Get<ITestService>();
    m_testService->SetScreenshotConfig(demoHostManagerOptionParser->GetScreenshotConfig());

    // Get the demo host up and running
    CmdRestart();

    FSLLOG3_VERBOSE("DemoHostManager: Starting demoAppManager");

    // Lets prepare the app manager.
    auto customAppConfig = demoSetup.App.AppSetup.CustomAppConfig;
    const DemoAppConfig demoAppConfig(demoSetup.App.AppSetup.OptionParser, demoSetup.ExceptionFormatter, m_demoHost->GetWindowMetrics(),
                                      serviceProvider, customAppConfig);

    m_demoAppManager = std::make_shared<DemoAppManager>(
      demoSetup.App.AppSetup, demoAppConfig, hostConfig.StatOverlay, demoHostManagerOptionParser->GetLogStatsMode(),
      demoHostManagerOptionParser->GetAppStatsFlags(), hostConfig.AppFirewall, hostConfig.ContentMonitor,
      demoHostManagerOptionParser->GetForceUpdateTime(), !m_demoHostCaps.IsEnabled(DemoHostCaps::Flags::AppRenderedSystemOverlay));

    FSLLOG3_VERBOSE("DemoHostManager: Processing messages");

    // Allow the pending messages that was created during setup to be processed as part of the 'host setup'
    ProcessMessages();

    FSLLOG3_VERBOSE("DemoHostManager: Created");
  }


  DemoHostManager::~DemoHostManager()
  {
    // Close the app first
    m_demoAppManager.reset();

    // Kill the graphics service resources if present
    if (!m_demoHostCaps.IsEnabled(DemoHostCaps::Flags::HostControlGraphicsServiceApi) && m_graphicsService)
    {
      m_graphicsService->ClearActiveApi();
    }

    // then the demo host
    m_demoHost.reset();
  }


  int DemoHostManager::Run(const std::shared_ptr<IServiceHostLooper>& serviceHostLooper)
  {
    FSLLOG3_VERBOSE("DemoHostManager: Running");
    auto windowMetrics = m_demoHost->GetWindowMetrics();

    const auto isConsoleBasedHost = m_demoHost->IsConsoleBaseHost();
    // Event loop
    while (m_demoHost->ProcessNativeMessages(m_state == State::Suspended) && !m_demoAppManager->HasExitRequest())
    {
      ProcessMessages();
      // Allow the services to react to the incoming messages before we process the app
      serviceHostLooper->ProcessMessages();

      if (m_state == State::Activated)
      {
        if (m_demoAppManager->GetState() == DemoState::Running)
        {
          if (m_windowMetricsDirty || m_demoAppManager->HasRestartRequest())
          {
            windowMetrics = m_demoHost->GetWindowMetrics();
            m_windowMetricsDirty = false;
          }
          AppProcess(windowMetrics, isConsoleBasedHost);
        }
      }
    }
    return m_demoAppManager->CloseApp();
  }

  void DemoHostManager::AppProcess(const DemoWindowMetrics& windowMetrics, const bool isConsoleBasedHost)
  {
    if (m_demoAppManager->Process(windowMetrics, isConsoleBasedHost))
    {
      auto swapBuffersResult = AppDrawAndSwapBuffers();
      switch (swapBuffersResult)
      {
      case SwapBuffersResult::Completed:
        m_demoAppManager->OnFrameSwapCompleted();
        m_testService->OnFrameSwapCompleted();

        // Provide support for exiting after a number of successfully rendered frames
        if (m_exitAfterFrame >= 0)
        {
          --m_exitAfterFrame;
          if (m_exitAfterFrame <= 0)
          {
            m_demoAppManager->RequestExit();
          }
        }
        break;
      case SwapBuffersResult::NotReady:
        // While the swapchain is invalid we sleep during draw to prevent the app from consuming 100% CPU time busy waiting
        std::this_thread::sleep_for(std::chrono::duration<uint64_t, std::milli>(8));
        break;
      case SwapBuffersResult::Failed:
        // The swap failed, lets try to restart the demo host
        FSLLOG3_VERBOSE("DemoHostManager: Swap buffers failed, trying to restart");
        CmdRestart();
        break;
      case SwapBuffersResult::AppControlled:
      default:
        throw NotSupportedException("Unsupported state");
      }

      if (m_exitAfterDuration.Enabled)
      {
        if (std::chrono::microseconds(m_timer.GetTime()) >= m_exitTime)
        {
          m_demoAppManager->RequestExit();
        }
      }
    }
  }

  SwapBuffersResult DemoHostManager::AppDrawAndSwapBuffers()
  {
    const uint32_t MAX_RETRY_COUNT = 5;
    uint32_t retryCount = 0;
    auto result = AppDrawResult::Retry;
    while (result == AppDrawResult::Retry && retryCount < MAX_RETRY_COUNT)
    {
      result = m_demoAppManager->TryDraw();
      if (result == AppDrawResult::Completed)
      {
        assert(m_demoHost);
        auto swapBuffersResult = m_demoHost->TrySwapBuffers();
        if (swapBuffersResult != SwapBuffersResult::AppControlled)
        {
          //  The swap buffer operation is not app controlled, so use a quick exit.
          m_demoAppManager->EndFrameSequence();
          return swapBuffersResult;
        }
        // the swap is app controlled so delegate it to the app
        result = m_demoAppManager->TryAppSwapBuffers();
      }
      ++retryCount;
    }

    switch (result)
    {
    case AppDrawResult::Completed:
      // everything is ok
      m_demoAppManager->EndFrameSequence();
      return SwapBuffersResult::Completed;
    case AppDrawResult::Retry:
      FSLLOG3_WARNING("RetryDraw limit exceeded -> failing");
      return SwapBuffersResult::Failed;
    case AppDrawResult::NotReady:
      return SwapBuffersResult::NotReady;
    case AppDrawResult::Failed:
      return SwapBuffersResult::Failed;
    default:
      throw NotSupportedException("Unsupported result");
    }
  }


  void DemoHostManager::ProcessMessages()
  {
    NativeWindowEvent event;
    while (m_eventQueue->TryDequeue(event))
    {
      FSLLOG3_VERBOSE6("Event: {} arg1: {} arg2: {} arg3: {}", static_cast<int32_t>(event.Type), event.Arg1, event.Arg2, event.Arg3);
      switch (event.Type)
      {
      case NativeWindowEventType::WindowActivation:
        FSLLOG3_VERBOSE("DemoHostManager: WindowActivation: {}", event.Arg1);
        CmdActivation(event.Arg1 != 0);
        break;
      case NativeWindowEventType::WindowSuspend:
        FSLLOG3_VERBOSE("DemoHostManager: WindowSuspend: {}", event.Arg1);
        CmdSuspend(event.Arg1 != 0);
        break;
      case NativeWindowEventType::LowMemory:
        FSLLOG3_VERBOSE("DemoHostManager: LowMemory");
        // For now we ignore this
        break;
      case NativeWindowEventType::WindowResized:
        FSLLOG3_VERBOSE("DemoHostManager: WindowResized");
        m_windowMetricsDirty = true;
        break;
      case NativeWindowEventType::WindowConfigChanged:
        FSLLOG3_VERBOSE("DemoHostManager: WindowConfigChanged");
        m_windowMetricsDirty = true;
        break;
      default:
        break;
      }

      m_nativeWindowEventSender->SendEvent(event);
    }
  }


  void DemoHostManager::CmdRestart()
  {
    if (m_demoAppManager)
    {
      m_demoAppManager->Suspend(true);
    }

    if (!m_demoHostCaps.IsEnabled(DemoHostCaps::Flags::HostControlGraphicsServiceApi) && m_graphicsService)
    {
      m_graphicsService->ClearActiveApi();
    }

    m_demoHost.reset();

    const uint32_t verbosityLevel = m_demoSetup.VerbosityLevel;
    const DemoHostConfig demoHostConfig(m_demoSetup.Host.OptionParser, m_eventQueue, m_demoSetup.App, m_demoSetup.ServiceProvider,
                                        m_basic2DPreallocEnabled, verbosityLevel);

    m_demoHost = m_demoSetup.Host.Factory->Allocate(demoHostConfig);
    // Allow a bit of post construction processing
    m_demoHost->OnConstructed();

    FSLLOG3_VERBOSE("DemoHostManager: Demo host started");
    m_hostInfoControl->SetIsConsoleBasedHost(m_demoHost->IsConsoleBaseHost());
    const auto activeAPI = m_demoHost->GetActiveAPI();
    m_hostInfoControl->SetActiveAPI(activeAPI);
    if (!m_demoHostCaps.IsEnabled(DemoHostCaps::Flags::HostControlGraphicsServiceApi) && m_graphicsService)
    {
      m_graphicsService->SetActiveApi(activeAPI);
      FSLLOG3_VERBOSE("DemoHostManager: Graphics service API configured");
    }

    if (m_demoAppManager)
    {
      m_demoAppManager->Suspend(false);
    }
  }


  void DemoHostManager::CmdActivation(const bool bActivated)
  {
    if (bActivated)
    {
      if (m_state != State::Idle)
      {
        FSLLOG3_WARNING("Received activation(1) from unexpected state: {} expected state: {}", static_cast<uint32_t>(m_state),
                        static_cast<uint32_t>(State::Idle));
        return;
      }

      assert(m_state == State::Idle);
      m_state = State::Activated;
      m_demoHost->OnActivate();
      m_demoAppManager->OnActivate();
      FSLLOG3_VERBOSE("DemoHostManager: Activated");
    }
    else
    {
      if (m_state != State::Activated)
      {
        FSLLOG3_WARNING("Received activation(0) from unexpected state: {} expected state: ", static_cast<uint32_t>(m_state),
                        static_cast<uint32_t>(State::Idle));
        return;
      }

      assert(m_state == State::Activated);
      m_state = State::Idle;
      m_demoAppManager->OnDeactivate();
      m_demoHost->OnDeactivate();
      FSLLOG3_VERBOSE("DemoHostManager: Deactivated");
    }
  }


  void DemoHostManager::CmdSuspend(const bool bSuspend)
  {
    if (bSuspend)
    {
      // Quick exit if we are in the correct state
      if (m_state == State::Suspended)
      {
        FSLLOG3_WARNING("Already in suspended state, ignoring.");
        return;
      }

      if (m_state == State::Activated)
      {
        FSLLOG3_WARNING("Forcing window deactivation on suspend (missing activation)");
        CmdActivation(false);
      }

      assert(m_state == State::Idle);
      m_state = State::Suspended;
      m_demoAppManager->Suspend(true);
      m_demoHost->OnSuspend();
      FSLLOG3_VERBOSE("DemoHostManager: Suspended");
    }
    else
    {
      // Quick exit if we are in the correct state
      if (m_state != State::Suspended)
      {
        FSLLOG3_WARNING("Not in suspended state, ignoring.");
        return;
      }

      assert(m_state == State::Suspended);
      m_state = State::Idle;
      m_demoHost->OnResume();
      m_demoAppManager->Suspend(false);
      FSLLOG3_VERBOSE("DemoHostManager: Resumed");
    }
  }
}
