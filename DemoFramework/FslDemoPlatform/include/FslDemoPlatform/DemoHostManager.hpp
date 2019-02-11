#ifndef FSLDEMOPLATFORM_DEMOHOSTMANAGER_HPP
#define FSLDEMOPLATFORM_DEMOHOSTMANAGER_HPP
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

#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoHost/Base/DemoHostCaps.hpp>
#include <FslDemoHost/Base/SwapBuffersResult.hpp>
#include <FslDemoPlatform/DurationExitConfig.hpp>
#include <FslDemoPlatform/Setup/DemoSetup.hpp>
#include <memory>

namespace Fsl
{
  struct Point2;
  class DemoAppManager;
  class DemoHostManagerOptionParser;
  class IDemoHost;
  class IGraphicsServiceControl;
  class IHostInfoControl;
  class INativeWindowEventQueue;
  class INativeWindowEventSender;
  class IServiceHostLooper;
  class ITestService;
  class NativeWindowEventQueue;

  class DemoHostManager
  {
    struct State
    {
      enum Enum
      {
        Suspended = 0,
        Idle = 1,
        Activated = 2
      };
    };

    DemoSetup m_demoSetup;
    DemoHostCaps m_demoHostCaps;
    std::shared_ptr<NativeWindowEventQueue> m_eventQueue;
    std::shared_ptr<DemoAppManager> m_demoAppManager;
    std::shared_ptr<IDemoHost> m_demoHost;
    std::shared_ptr<IHostInfoControl> m_hostInfoControl;
    std::shared_ptr<IGraphicsServiceControl> m_graphicsService;
    std::shared_ptr<ITestService> m_testService;

    State::Enum m_state;
    std::shared_ptr<INativeWindowEventSender> m_nativeWindowEventSender;
    //! Provide support for exiting after a number of successfully rendered frames (if negative, we render a unlimited amount of frames)
    int32_t m_exitAfterFrame;
    DurationExitConfig m_exitAfterDuration;
    bool m_windowSizeIsDirty;
    HighResolutionTimer m_timer;
    //! Only used if m_exitAfterDuration.Enabled is true
    std::chrono::microseconds m_exitTime;

  public:
    DemoHostManager(const DemoSetup& demoSetup, const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser);
    ~DemoHostManager();

    int Run(const std::shared_ptr<IServiceHostLooper>& serviceHostLooper);

  private:
    void AppProcess(const Point2& screenResolution, const bool isConsoleBasedHost);
    SwapBuffersResult AppDrawAndSwapBuffers();
    void ProcessMessages();
    void CmdRestart();
    void CmdActivation(const bool bActivated);
    void CmdSuspend(const bool bSuspend);
  };
}

#endif
