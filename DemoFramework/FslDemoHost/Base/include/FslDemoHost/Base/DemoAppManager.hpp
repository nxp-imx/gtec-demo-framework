#ifndef FSLDEMOHOST_BASE_DEMOAPPMANAGER_HPP
#define FSLDEMOHOST_BASE_DEMOAPPMANAGER_HPP
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

#include <memory>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Base/IDemoApp.hpp>
#include <FslDemoApp/Base/TimeStepMode.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoHost/Base/DemoAppProfilerOverlay.hpp>
#include <FslDemoHost/Base/DemoState.hpp>
#include <FslDemoHost/Base/LogStatsMode.hpp>

namespace Fsl
{
  class DemoAppManagerEventListener;
  class IDemoAppControlEx;
  class IGraphicsServiceControl;
  class IProfilerService;
  class IProfilerServiceControl;

  class DemoAppManager
  {
    DemoAppProfilerOverlay m_demoAppProfilerOverlay;
    std::shared_ptr<IDemoAppControlEx> m_demoAppControl;
    std::shared_ptr<IGraphicsServiceControl> m_graphicsService;
    std::shared_ptr<IProfilerServiceControl> m_profilerServiceControl;
    std::shared_ptr<IProfilerService> m_profilerService;
    std::shared_ptr<DemoAppManagerEventListener> m_eventListener;
    DemoAppSetup m_demoAppSetup;
    DemoAppConfig m_demoAppConfig;
    DemoState m_state;
    bool m_hasExitRequest;
    HighResolutionTimer m_timer;
    uint32_t m_forcedUpdateTime;
    uint64_t m_frameTimeConfig;
    uint64_t m_timeThen;
    uint64_t m_accumulatedTime;
    uint64_t m_expectedFrameTime;
    uint64_t m_maxFrameTime;
    uint64_t m_timeStatsBeforeUpdate;
    uint64_t m_timeStatsAfterUpdate;
    uint64_t m_timeStatsAfterDraw;
    uint64_t m_timeStatsLast;
    uint64_t m_accumulatedTotalTimeFixed;
    uint64_t m_accumulatedTotalTime;
    bool m_enableLogStats;
    LogStatsMode m_logStatsMode;
    bool m_enableStats;
    bool m_useFirewall;
    bool m_preallocateBasic2D;
    std::shared_ptr<IDemoApp> m_demoApp;
  public:
    DemoAppManager(const DemoAppSetup& demoAppSetup, const DemoAppConfig& demoAppConfig, const bool enableStats, const LogStatsMode logStatsMode, const bool enableFirewall, const bool enableContentMonitor, const bool preallocateBasic2D, const uint32_t forcedUpdateTime);
    virtual ~DemoAppManager();

    void Suspend(const bool bSuspend);

    DemoState GetState() const;

    bool Process(const Point2& screenResolution);

    void OnActivate();
    void OnDeactivate();
    //! @brief Should be called after a buffer swap has been completed
    void OnFrameSwapCompleted();

    void RequestExit();
    bool HasExitRequest() const;
    int CloseApp();
  private:
    //! @brief Manage exit requests
    //! @return true if exit should occur right away
    bool ManageExitRequests(const bool bCheckExternalOnly);
      //! @brief manage the app state
    void ManageAppState(const Point2& screenResolution);
    void ResetTimer();
    void ApplyTimeStepMode(const TimeStepMode mode);
  };
}

#endif
