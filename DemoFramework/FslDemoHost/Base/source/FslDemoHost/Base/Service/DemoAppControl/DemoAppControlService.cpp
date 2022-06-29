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
#include <FslDemoApp/Base/Service/Events/Basic/TimeStateEvent.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/DemoAppControlService.hpp>
#include <FslDemoHost/Base/Service/DemoPlatformControl/IDemoPlatformControl.hpp>
#include <FslDemoHost/Base/Service/Events/IEventPoster.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfo.hpp>
#include <FslNativeWindow/Base/INativeWindow.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeStepMode DefaultTimestepMode = TimeStepMode::Normal;
      constexpr uint16_t DefaultFixedUpdatesPerSecond = 60;
    }
  }

  DemoAppControlService::DemoAppControlService(const ServiceProvider& serviceProvider, const int defaultExitCode)
    : ThreadLocalService(serviceProvider)
    , m_eventPoster(serviceProvider.Get<IEventPoster>())
    , m_platformControl(serviceProvider.Get<IDemoPlatformControl>())
    , m_windowHostInfo(serviceProvider.TryGet<IWindowHostInfo>())
    , m_defaultExitCode(defaultExitCode)
    , m_hasScreenshotRequest(false)
    , m_hasAppRestartResetRequest(false)
    , m_hasUpdateTimerResetRequest(true)
    , m_hasExitRequest(false)
    , m_exitCode(defaultExitCode)
    , m_timestepMode(LocalConfig::DefaultTimestepMode)
    , m_captureModeEnabled(false)
    , m_fixedUpdatesPerSecond(LocalConfig::DefaultFixedUpdatesPerSecond)
  {
  }


  DemoAppControlService::~DemoAppControlService() = default;


  void DemoAppControlService::RequestScreenshot() noexcept
  {
    m_hasScreenshotRequest = true;
    RequestUpdateTimerReset();
  }


  void DemoAppControlService::RequestAppRestart() noexcept
  {
    m_hasAppRestartResetRequest = true;
    RequestUpdateTimerReset();
  }


  void DemoAppControlService::RequestUpdateTimerReset() noexcept
  {
    m_hasUpdateTimerResetRequest = true;
  }


  void DemoAppControlService::RequestExit()
  {
    DoRequestExit(m_defaultExitCode);
  }


  void DemoAppControlService::RequestExit(const int exitCode)
  {
    DoRequestExit(exitCode);
  }


  void DemoAppControlService::ChangeExitCode(const int exitCode)
  {
    if (m_hasExitRequest && m_exitCode == m_defaultExitCode)
    {
      m_exitCode = exitCode;
    }
    else
    {
      if (!m_hasExitRequest)
      {
        FSLLOG3_WARNING("No exit request exists. Request ignored.");
      }
      if (m_hasExitRequest)
      {
        FSLLOG3_WARNING("Non default exit code is already set. Request ignored.");
      }
    }
  }


  bool DemoAppControlService::HasScreenshotRequest() const noexcept
  {
    return m_hasScreenshotRequest;
  }


  bool DemoAppControlService::HasAppRestartRequest() const noexcept
  {
    return m_hasAppRestartResetRequest;
  }


  bool DemoAppControlService::HasUpdateTimerResetRequest() const noexcept
  {
    return m_hasUpdateTimerResetRequest;
  }


  bool DemoAppControlService::HasExitRequest() const noexcept
  {
    // If no exit request is active then check the signal handlers exit flag to see if a externally triggered exit
    // has been requested
    if (m_platformControl && !m_hasExitRequest && m_platformControl->HasExitRequest())
    {
      m_hasExitRequest = true;
    }
    return m_hasExitRequest;
  }


  int DemoAppControlService::GetExitCode() const noexcept
  {
    return m_exitCode;
  }


  void DemoAppControlService::SetTimeStepMode(const TimeStepMode timeStepMode)
  {
    if (timeStepMode == m_timestepMode)
    {
      return;
    }

    bool wasRealtime = m_timestepMode == TimeStepMode::Normal;

    m_timestepMode = timeStepMode;
    if (m_timestepMode == TimeStepMode::Normal)
    {
      m_eventPoster->Post(TimeStateEvent(TimeStateCommand::Realtime));
    }
    else if (wasRealtime)
    {
      m_eventPoster->Post(TimeStateEvent(TimeStateCommand::Simulated));
    }
  }


  uint32_t DemoAppControlService::GetRenderLoopFrameCounter() const noexcept
  {
    return m_renderLoopFrameCounter > 0 ? m_renderLoopFrameCounter : m_renderLoopMaxFramesInFlight;
  }

  void DemoAppControlService::SetRenderLoopFrameCounter(const uint32_t frameCount)
  {
    FSLLOG3_WARNING_IF(frameCount > m_renderLoopMaxFramesInFlight, "SetRenderLoopFrameCounter: frameCount capped to m_renderLoopMaxFramesInFlight")
    m_renderLoopFrameCounter = std::min(frameCount, m_renderLoopMaxFramesInFlight);
  }

  uint32_t DemoAppControlService::GetRenderLoopMaxFramesInFlight() const noexcept
  {
    return m_renderLoopMaxFramesInFlight;
  }

  bool DemoAppControlService::TryEnableMouseCaptureMode(const bool enabled)
  {
    if (!m_windowHostInfo)
    {
      return false;
    }

    const auto windows = m_windowHostInfo->GetWindows();
    if (windows.empty())
    {
      FSLLOG3_DEBUG_WARNING("TryEnableMouseCaptureMode did not find any active windows");
      return false;
    }
    if (windows.size() > 1)
    {
      FSLLOG3_DEBUG_WARNING("TryEnableMouseCaptureMode only support one window");
      return false;
    }
    const auto activeWindow = windows.front().lock();
    if (!activeWindow)
    {
      FSLLOG3_DEBUG_WARNING("TryEnableMouseCaptureMode did not find any un-expired windows");
      return false;
    }

    return activeWindow->TryCaptureMouse(enabled);
  }


  void DemoAppControlService::EnableMouseCaptureMode(const bool enabled)
  {
    if (!m_windowHostInfo)
    {
      throw NotSupportedException("EnableMouseCaptureMode not supported by DemoHost");
    }

    if (!TryEnableMouseCaptureMode(enabled))
    {
      throw NotSupportedException("EnableMouseCaptureMode failed");
    }
  }


  bool DemoAppControlService::GetMouseCaptureMode() const noexcept
  {
    return m_captureModeEnabled;
  }


  uint16_t DemoAppControlService::GetFixedUpdatesPerSecond() const noexcept
  {
    return m_fixedUpdatesPerSecond;
  }


  void DemoAppControlService::SetFixedUpdatesPerSecond(const uint16_t updatesPerSecond)
  {
    FSLLOG3_DEBUG_WARNING_IF(updatesPerSecond <= 0, "fixed updates per second forced to a minimum one");

    const auto cappedFixedUpdatesPerSecond = std::max(updatesPerSecond, static_cast<uint16_t>(1));
    if (cappedFixedUpdatesPerSecond != m_fixedUpdatesPerSecond)
    {
      FSLLOG3_VERBOSE3("SetFixedUpdatesPerSecond to {}", cappedFixedUpdatesPerSecond);
      m_fixedUpdatesPerSecond = cappedFixedUpdatesPerSecond;
    }
  }


  uint16_t DemoAppControlService::GetOnDemandFrameInterval() const noexcept
  {
    return m_onDemandRendering.FrameInterval;
  }


  void DemoAppControlService::SetOnDemandFrameInterval(const uint16_t frameRateInterval)
  {
    FSLLOG3_DEBUG_WARNING_IF(frameRateInterval <= 0, "frameRateInterval forced to a minimum one");
    const uint16_t cappedFrameInterval = std::max(frameRateInterval, static_cast<uint16_t>(1));
    if (cappedFrameInterval != m_onDemandRendering.FrameInterval)
    {
      FSLLOG3_VERBOSE6("SetOnDemandFrameInterval to {}", frameRateInterval);
      m_onDemandRendering.FrameInterval = frameRateInterval;
    }
  }


  void DemoAppControlService::ClearScreenshotRequestRequest() noexcept
  {
    m_hasScreenshotRequest = false;
  }


  void DemoAppControlService::ClearAppRestartRequestRequest() noexcept
  {
    m_hasAppRestartResetRequest = false;
  }


  void DemoAppControlService::ClearUpdateTimerResetRequest() noexcept
  {
    m_hasUpdateTimerResetRequest = false;
  }

  void DemoAppControlService::SetRenderLoopMaxFramesInFlight(const uint32_t maxFramesInFlight)
  {
    FSLLOG3_WARNING_IF(maxFramesInFlight <= 0u, "SetRenderLoopMaxFramesInFlight: Forcing maxFramesInFlight to be at least one")
    m_renderLoopMaxFramesInFlight = std::max(maxFramesInFlight, 1u);
    m_renderLoopFrameCounter = std::min(m_renderLoopFrameCounter, m_renderLoopMaxFramesInFlight);
  }


  void DemoAppControlService::RestoreDefaults()
  {
    FSLLOG3_VERBOSE("Restoring default settings");
    m_onDemandRendering = {};
    SetFixedUpdatesPerSecond(LocalConfig::DefaultFixedUpdatesPerSecond);
    SetTimeStepMode(LocalConfig::DefaultTimestepMode);
    if (m_captureModeEnabled)
    {
      TryEnableMouseCaptureMode(false);
    }
  }


  void DemoAppControlService::DoRequestExit(const int exitCode)
  {
    if (!m_hasExitRequest)
    {
      m_hasExitRequest = true;
      m_exitCode = exitCode;
    }
    else
    {
      FSLLOG3_WARNING("exit has already been requested with status code: {}. Request ignored.", m_exitCode);
    }
  }
}
