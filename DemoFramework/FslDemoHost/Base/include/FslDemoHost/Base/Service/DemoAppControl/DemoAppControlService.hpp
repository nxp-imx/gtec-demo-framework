#ifndef FSLDEMOHOST_BASE_SERVICE_DEMOAPPCONTROL_DEMOAPPCONTROLSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_DEMOAPPCONTROL_DEMOAPPCONTROLSERVICE_HPP
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

#include <FslDemoHost/Base/Service/DemoAppControl/IDemoAppControlEx.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>

namespace Fsl
{
  class IEventPoster;
  class IDemoPlatformControl;
  class IWindowHostInfo;
  class INativeWindow;

  class DemoAppControlService final
    : public ThreadLocalService
    , public IDemoAppControlEx
  {
    std::shared_ptr<IEventPoster> m_eventPoster;
    std::shared_ptr<IDemoPlatformControl> m_platformControl;
    std::shared_ptr<IWindowHostInfo> m_windowHostInfo;
    int m_defaultExitCode;
    bool m_hasScreenshotRequest;
    bool m_hasAppRestartResetRequest;
    bool m_hasUpdateTimerResetRequest;
    mutable bool m_hasExitRequest;
    int m_exitCode;
    TimeStepMode m_timestepMode;
    bool m_captureModeEnabled;

  public:
    DemoAppControlService(const ServiceProvider& serviceProvider, const int defaultExitCode);
    ~DemoAppControlService() final;


    // From IDemoAppControl
    void RequestScreenshot() final;
    void RequestAppRestart() final;
    void RequestUpdateTimerReset() final;
    void RequestExit() final;
    void RequestExit(const int exitCode) final;
    void ChangeExitCode(const int exitCode) final;
    bool HasScreenshotRequest() const final;
    bool HasAppRestartRequest() const final;
    bool HasUpdateTimerResetRequest() const final;
    bool HasExitRequest() const final;
    int GetExitCode() const final;
    void SetTimeStepMode(const TimeStepMode timeStepMode) final;
    TimeStepMode GetTimeStepMode() const final;
    bool TryEnableMouseCaptureMode(const bool enabled) final;
    void EnableMouseCaptureMode(const bool enabled) final;
    bool GetMouseCaptureMode() final;

    // From IDemoAppControlServiceEx
    void ClearScreenshotRequestRequest() final;
    void ClearAppRestartRequestRequest() final;
    void ClearUpdateTimerResetRequest() final;

  private:
    void DoRequestExit(const int exitCode);
  };
}

#endif
