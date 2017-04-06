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

#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/DemoAppConfig.hpp>
#include <FslDemoHost/ADemoHost.hpp>
#include <FslDemoHost/DemoAppManager.hpp>
#include <FslDemoHost/IDemoHostFactory.hpp>
#include <FslDemoHost/Service/Graphics/IGraphicsServiceControl.hpp>
#include <FslDemoHost/Service/Host/IHostInfoControl.hpp>
#include <FslDemoHost/Service/ServiceProviderImpl.hpp>
#include <FslDemoHost/Service/ServiceRegistryImpl.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalServiceRegistryImpl.hpp>
#include <FslDemoHost/Service/Test/ITestService.hpp>
#include <FslDemoHost/Service/NativeWindowEvents/INativeWindowEventSender.hpp>
#include <FslNativeWindow/NativeWindowEventQueue.hpp>
#include <FslDemoPlatform/DemoHostManager.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <FslDemoPlatform/Service/MMDCStats/IMMDCStatsService.hpp>
#include <cassert>

#if 0
#define LOCAL_LOG(X) FSLLOG("DemoHostManager: " << X)
#else
#define LOCAL_LOG(X) {}
#endif

#if 1
#define VERBOSE_LOG(X) FSLLOG_IF( m_demoSetup.Verbose, "DemoHostManager: " << X)
#else
#define VERBOSE_LOG(X) {}
#endif

namespace Fsl
{

  DemoHostManager::DemoHostManager(const DemoSetup& demoSetup, const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    : m_demoSetup(demoSetup)
    , m_eventQueue(new NativeWindowEventQueue())
    , m_demoAppManager()
    , m_demoHost()
    , m_state(State::Idle)
    , m_nativeWindowEventSender()
    , m_exitAfterFrame(demoHostManagerOptionParser->GetExitAfterFrame())
    , m_windowSizeIsDirty(true)
  {
    // Acquire the various services
    ServiceProvider serviceProvider(demoSetup.ServiceProvider);
    m_graphicsService = serviceProvider.Get<IGraphicsServiceControl>();
    m_nativeWindowEventSender = serviceProvider.Get<INativeWindowEventSender>();
    m_hostInfoControl = serviceProvider.Get<IHostInfoControl>();

    // Set the config we are using for the app host so it can be retrieved from IHostInfo
    m_hostInfoControl->SetAppHostConfig(demoSetup.App.AppHostConfig);

    // Acquire and configure the test service
    m_testService = serviceProvider.Get<ITestService>();
    m_testService->SetScreenshotFrequency(demoHostManagerOptionParser->GetScreenshotFrequency());
    m_testService->SetScreenshotNameScheme(demoHostManagerOptionParser->GetScreenshotNameScheme());

    // Get the demo host up and running
    CmdRestart();

    VERBOSE_LOG("Starting demoAppManager");
    // Lets prepare the app manager.
    const DemoAppConfig demoAppConfig(demoSetup.App.AppSetup.OptionParser, m_demoHost->GetScreenResolution(), serviceProvider);
    m_demoAppManager = std::make_shared<DemoAppManager>(demoSetup.App.AppSetup, demoAppConfig, demoHostManagerOptionParser->IsStatsEnabled(), demoHostManagerOptionParser->GetLogStatsMode(), demoHostManagerOptionParser->IsAppFirewallEnabled(), demoHostManagerOptionParser->IsContentMonitorEnabled(), demoHostManagerOptionParser->IsBasic2DPreallocEnabled(), demoHostManagerOptionParser->GetForceUpdateTime());

    VERBOSE_LOG("Processing messages");

    // Allow the pending messages that was created during setup to be processed as part of the 'host setup'
    ProcessMessages();

    VERBOSE_LOG("Created");
  }


  DemoHostManager::~DemoHostManager()
  {
    // Close the app first
    m_demoAppManager.reset();

    // Kill the graphics service resources
    m_graphicsService->Reset();

    // then the demo host
    m_demoHost.reset();
  }


  int DemoHostManager::Run()
  {
    VERBOSE_LOG("Running");
    Point2 screenResolution = m_demoHost->GetScreenResolution();

    // Event loop
    while (m_demoHost->ProcessNativeMessages(m_state == State::Suspended) && !m_demoAppManager->HasExitRequest())
    {
      ProcessMessages();
      // Allow the services to react to the incoming messages before we process the app
      m_demoSetup.ServiceProvider->Update();

      if (m_state == State::Activated)
      {
        if (m_demoAppManager->GetState() == DemoState::Running)
        {
          if (m_windowSizeIsDirty)
          {
            screenResolution = m_demoHost->GetScreenResolution();
            m_windowSizeIsDirty = false;
          }

          if (m_demoAppManager->Process(screenResolution))
          {
            assert(m_demoHost);
            if (m_demoHost->SwapBuffers())
            {
              m_demoAppManager->OnFrameSwapCompleted();
              m_testService->OnFrameSwapCompleted();

              // Provide support for exiting after a number of successfully rendered frames
              if (m_exitAfterFrame >= 0)
              {
                --m_exitAfterFrame;
                if (m_exitAfterFrame <= 0)
                  m_demoAppManager->RequestExit();
              }
            }
            else
            {
              // The swap failed, lets try to restart the demo host
              VERBOSE_LOG("Swap buffers failed, trying to restart");
              CmdRestart();
            }
          }
        }
      }
    }
    return m_demoAppManager->CloseApp();
  }


  void DemoHostManager::ProcessMessages()
  {
    NativeWindowEvent event;
    while (m_eventQueue->TryDequeue(event))
    {
      VERBOSE_LOG("Event: " << static_cast<int32_t>(event.Type) << " arg1: " << event.Arg1 << " arg2: " << event.Arg2 << " arg3: " << event.Arg3);
      switch (event.Type)
      {
      case NativeWindowEventType::WindowActivation:
        VERBOSE_LOG("WindowActivation: " << event.Arg1);
        CmdActivation(event.Arg1 != 0);
        break;
      case NativeWindowEventType::WindowSuspend:
        VERBOSE_LOG("WindowSuspend: " << event.Arg1);
        CmdSuspend(event.Arg1 != 0);
        break;
      case NativeWindowEventType::LowMemory:
        VERBOSE_LOG("LowMemory");
        // For now we ignore this
        break;
      case NativeWindowEventType::WindowResized:
        VERBOSE_LOG("WindowResized");
        m_windowSizeIsDirty = true;
        break;
      case NativeWindowEventType::WindowDPIChanged:
        VERBOSE_LOG("WindowDPIChanged");
        // For now we ignore this
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
      m_demoAppManager->Suspend(true);

    m_graphicsService->Reset();
    m_demoHost.reset();

    const DemoHostConfig demoHostConfig(m_demoSetup.Host.OptionParser, m_eventQueue, m_demoSetup.App, m_demoSetup.ServiceProvider);
    m_demoHost = m_demoSetup.Host.Factory->Allocate(demoHostConfig);

    VERBOSE_LOG("Demo host started");
    const auto activeAPI = m_demoHost->GetActiveAPI();
    m_hostInfoControl->SetActiveAPI(activeAPI);
    m_graphicsService->Configure(activeAPI);
    VERBOSE_LOG("Graphics service configured");

    if (m_demoAppManager)
      m_demoAppManager->Suspend(false);
  }


  void DemoHostManager::CmdActivation(const bool bActivated)
  {
    if (bActivated)
    {
      if (m_state != State::Idle)
      {
        FSLLOG_WARNING("Received activation(1) from unexpected state: " << m_state << " expected state: " << State::Idle);
        return;
      }

      assert(m_state == State::Idle);
      m_state = State::Activated;
      m_demoHost->OnActivate();
      m_demoAppManager->OnActivate();
      VERBOSE_LOG("Activated");
    }
    else
    {
      if (m_state != State::Activated)
      {
        FSLLOG_WARNING("Received activation(0) from unexpected state: " << m_state << " expected state: " << State::Idle);
        return;
      }

      assert(m_state == State::Activated);
      m_state = State::Idle;
      m_demoAppManager->OnDeactivate();
      m_demoHost->OnDeactivate();
      VERBOSE_LOG("Deactivated");
    }
  }


  void DemoHostManager::CmdSuspend(const bool bSuspend)
  {
    if (bSuspend)
    {
      // Quick exit if we are in the correct state
      if (m_state == State::Suspended)
      {
        FSLLOG_WARNING("Already in suspended state, ignoring.");
        return;
      }

      if (m_state == State::Activated)
      {
        FSLLOG_WARNING("Forcing window deactivation on suspend (missing activation)");
        CmdActivation(false);
      }

      assert(m_state == State::Idle);
      m_state = State::Suspended;
      m_demoAppManager->Suspend(true);
      m_demoHost->OnSuspend();
      VERBOSE_LOG("Suspended");
    }
    else
    {
      // Quick exit if we are in the correct state
      if (m_state != State::Suspended)
      {
        FSLLOG_WARNING("Not in suspended state, ignoring.");
        return;
      }

      assert(m_state == State::Suspended);
      m_state = State::Idle;
      m_demoHost->OnResume();
      m_demoAppManager->Suspend(false);
      VERBOSE_LOG("Resumed");
    }
 }


}
