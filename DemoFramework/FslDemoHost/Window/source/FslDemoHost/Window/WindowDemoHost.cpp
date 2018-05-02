/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <FslDemoApp/Base/Host/DemoAppHostConfigWindow.hpp>
#include <FslDemoApp/Base/Host/DemoHostCustomWindowSystemSetup.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfoControl.hpp>
#include <FslDemoHost/Window/WindowDemoHost.hpp>
#include <FslDemoHost/Window/WindowDemoHostOptionParser.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/INativeWindowSystem.hpp>
#include <FslNativeWindow/Base/INativeWindow.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowProcessMessagesArgs.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystemFactory.hpp>
#include <cassert>
#include <iostream>
#include <vector>

#if 0
#define LOCAL_LOG(X) FSLLOG("VulkanDemoHost: " << X)
#else
#define LOCAL_LOG(X) {}
#endif

#if 1
#define LOCAL_LOG_WARNING(X) FSLLOG_WARNING("WindowDemoHost: " << X)
#else
#define LOCAL_LOG_WARNING(X) {}
#endif

namespace Fsl
{
  WindowDemoHost::WindowDemoHost(const DemoHostConfig& demoHostConfig)
    : ADemoHost(demoHostConfig)
    , m_demoHostConfig(demoHostConfig)
    , m_isActivated(true)
    , m_activeApi(DemoHostFeatureName::Window, 0)
    , m_options(demoHostConfig.GetOptions<WindowDemoHostOptionParser>())
    , m_windowHostInfoControl(demoHostConfig.GetServiceProvider().Get<IWindowHostInfoControl>())
    , m_nativeWindowSetup()
    , m_customWindowSystem()
    , m_windowSystem()
    , m_window()
  {
    auto demoHostAppSetup = demoHostConfig.GetDemoHostAppSetup();
    const auto demoAppHostConfigWindow = demoHostAppSetup.GetDemoAppHostConfig<DemoAppHostConfigWindow>();

    // Get the native window system from the app, if none was supplied use the Platform one.
    auto customWindowSystemAllocator = demoAppHostConfigWindow->TryGetCustomWindowSystemAllocator();


    // Prepare the native window setup
    const NativeWindowSystemSetup nativeWindowSystemSetup(demoHostConfig.GetEventQueue(), demoHostConfig.GetVerbosityLevel(),
                                                          m_options->GetNativeWindowConfig(), m_options->GetNativeWindowTag());

    if (customWindowSystemAllocator)
    {
      const DemoHostCustomWindowSystemSetup customSetup(demoHostConfig.GetServiceProvider(), nativeWindowSystemSetup,
                                                        demoAppHostConfigWindow->TryGetCustomDemoAppHostConfigBase());
      m_customWindowSystem = customWindowSystemAllocator(customSetup);
      m_windowSystem = m_customWindowSystem;
    }
    if (!m_windowSystem)
      m_windowSystem = PlatformNativeWindowSystemFactory::Allocate(nativeWindowSystemSetup);

    // Set the window system in the host service so that any services or app that is interested will be able to access it
    m_windowHostInfoControl->SetWindowSystem(m_windowSystem);


    const DemoHostAppSetup hostAppSetup = demoHostConfig.GetDemoHostAppSetup();
    if (hostAppSetup.DemoHostFeatures->size() <= 0)
      throw NotSupportedException("Must use at least one feature");

    m_activeApi = hostAppSetup.DemoHostFeatures->front();

    m_nativeWindowSetup.reset(new NativeWindowSetup(demoHostConfig.GetDemoHostAppSetup().AppSetup.ApplicationName, demoHostConfig.GetEventQueue(),
                                                    m_options->GetNativeWindowConfig(), demoHostConfig.GetVerbosityLevel()));
    Init();
  }


  WindowDemoHost::~WindowDemoHost()
  {
    m_windowHostInfoControl->ClearWindowSystem();
    Shutdown();
  }


  void WindowDemoHost::OnActivate()
  {
    m_isActivated = true;
  }


  void WindowDemoHost::OnDeactivate()
  {
    m_isActivated = false;
  }


  void WindowDemoHost::OnSuspend()
  {
    Shutdown();
  }


  void WindowDemoHost::OnResume()
  {
    Init();
  }


  DemoHostFeature WindowDemoHost::GetActiveAPI() const
  {
    return m_activeApi;
  }


  Point2 WindowDemoHost::GetScreenResolution() const
  {
    Point2 size;
    if (m_window && m_window->TryGetSize(size))
      return size;

    FSLLOG_DEBUG_WARNING("Failed to GetScreenResolution, returning zero");
    return Point2();
  }


  bool WindowDemoHost::SwapBuffers()
  {
    if (!m_isActivated)
      return true;

    if (m_window)
    {
      // Give the custom window system a chance to react and request a app restart if needed.
      if (m_customWindowSystem && ! m_customWindowSystem->TryPostSwapModifications())
        return false;
    }

    // Validate that we are not suspended
    return true;
  }


  bool WindowDemoHost::ProcessNativeMessages(const bool allowBlock)
  {
    // This will be called while suspended
    // Allow the native window to process messages
    NativeWindowProcessMessagesArgs args(allowBlock);
    return m_windowSystem->ProcessMessages(args);
  }


  void WindowDemoHost::Init()
  {
    LOCAL_LOG("Init");
    try
    {
      m_window = m_windowSystem->CreateNativeWindow(*m_nativeWindowSetup);
      m_windowHostInfoControl->AddWindow(m_window);
    }
    catch (const std::exception&)
    {
      LOCAL_LOG("Init failed with exception: " << ex.what());
      Shutdown();
      throw;
    }
  }


  void WindowDemoHost::Shutdown()
  {
    LOCAL_LOG("Shutdown");
    m_windowHostInfoControl->RemoveWindow(m_window);
    m_window.reset();
  }

}
