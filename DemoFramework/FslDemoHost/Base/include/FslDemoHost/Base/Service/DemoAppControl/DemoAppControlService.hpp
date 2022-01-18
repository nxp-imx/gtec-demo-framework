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
    struct OnDemandRenderingConfig
    {
      uint16_t FrameInterval{1};

      OnDemandRenderingConfig() noexcept = default;
      explicit OnDemandRenderingConfig(const uint16_t frameInterval) noexcept
        : FrameInterval(frameInterval >= 1 ? frameInterval : 1)
      {
      }
    };

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
    uint32_t m_renderLoopMaxFramesInFlight{1};
    uint32_t m_renderLoopFrameCounter{0};
    uint16_t m_fixedUpdatesPerSecond{1};
    OnDemandRenderingConfig m_onDemandRendering;

  public:
    DemoAppControlService(const ServiceProvider& serviceProvider, const int defaultExitCode);
    ~DemoAppControlService() final;


    // From IDemoAppControl
    void RequestScreenshot() noexcept final;
    void RequestAppRestart() noexcept final;
    void RequestUpdateTimerReset() noexcept final;
    void RequestExit() final;
    void RequestExit(const int exitCode) final;
    void ChangeExitCode(const int exitCode) final;
    bool HasScreenshotRequest() const noexcept final;
    bool HasAppRestartRequest() const noexcept final;
    bool HasUpdateTimerResetRequest() const noexcept final;
    bool HasExitRequest() const noexcept final;
    int GetExitCode() const noexcept final;
    void SetTimeStepMode(const TimeStepMode timeStepMode) final;
    TimeStepMode GetTimeStepMode() const noexcept final
    {
      return m_timestepMode;
    }
    uint32_t GetRenderLoopFrameCounter() const noexcept final;
    void SetRenderLoopFrameCounter(const uint32_t frameCount) final;
    uint32_t GetRenderLoopMaxFramesInFlight() const noexcept final;
    bool TryEnableMouseCaptureMode(const bool enabled) final;
    void EnableMouseCaptureMode(const bool enabled) final;
    bool GetMouseCaptureMode() const noexcept final;
    uint16_t GetFixedUpdatesPerSecond() const noexcept final;
    void SetFixedUpdatesPerSecond(const uint16_t updatesPerSecond) final;
    uint16_t GetOnDemandFrameInterval() const noexcept final;
    void SetOnDemandFrameInterval(const uint16_t frameRateInterval) final;

    // From IDemoAppControlServiceEx
    void ClearScreenshotRequestRequest() noexcept final;
    void ClearAppRestartRequestRequest() noexcept final;
    void ClearUpdateTimerResetRequest() noexcept final;
    void SetRenderLoopMaxFramesInFlight(const uint32_t maxFramesInFlight) final;
    void RestoreDefaults() final;

  private:
    void DoRequestExit(const int exitCode);
  };
}


#endif
