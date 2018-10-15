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

  class DemoAppControlService
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
    ~DemoAppControlService() override;


    // From IDemoAppControl
    void RequestScreenshot() override;
    void RequestAppRestart() override;
    void RequestUpdateTimerReset() override;
    void RequestExit() override;
    void RequestExit(const int exitCode) override;
    void ChangeExitCode(const int exitCode) override;
    bool HasScreenshotRequest() const override;
    bool HasAppRestartRequest() const override;
    bool HasUpdateTimerResetRequest() const override;
    bool HasExitRequest() const override;
    int GetExitCode() const override;
    void SetTimeStepMode(const TimeStepMode timeStepMode) override;
    TimeStepMode GetTimeStepMode() const override;
    bool TryEnableMouseCaptureMode(const bool enabled) override;
    void EnableMouseCaptureMode(const bool enabled) override;
    bool GetMouseCaptureMode() override;

    // From IDemoAppControlServiceEx
    void ClearScreenshotRequestRequest() override;
    void ClearAppRestartRequestRequest() override;
    void ClearUpdateTimerResetRequest() override;

  private:
    void DoRequestExit(const int exitCode);
  };
}

#endif
