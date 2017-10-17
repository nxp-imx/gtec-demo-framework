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
#include <FslDemoApp/Base/Service/Events/Basic/TimeStateEvent.hpp>
#include <FslDemoHost/Base/Service/Events/IEventPoster.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/DemoAppControlService.hpp>
#include <FslDemoHost/Base/Service/DemoPlatformControl/IDemoPlatformControl.hpp>
#include <cassert>

namespace Fsl
{
  DemoAppControlService::DemoAppControlService(const ServiceProvider& serviceProvider, const int defaultExitCode)
    : ThreadLocalService(serviceProvider)
    , m_eventPoster(serviceProvider.Get<IEventPoster>())
    , m_platformControl(serviceProvider.Get<IDemoPlatformControl>())
    , m_defaultExitCode(defaultExitCode)
    , m_hasScreenshotRequest(false)
    , m_hasAppRestartResetRequest(false)
    , m_hasUpdateTimerResetRequest(true)
    , m_hasExitRequest(false)
    , m_exitCode(defaultExitCode)
    , m_timestepMode(TimeStepMode::Normal)
  {
  }


  DemoAppControlService::~DemoAppControlService()
  {
  }


  void DemoAppControlService::RequestScreenshot()
  {
    m_hasScreenshotRequest = true;
    RequestUpdateTimerReset();
  }


  void DemoAppControlService::RequestAppRestart()
  {
    m_hasAppRestartResetRequest = true;
  }


  void DemoAppControlService::RequestUpdateTimerReset()
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
      m_exitCode = exitCode;
    else
    {
      if (!m_hasExitRequest)
        FSLLOG("WARNING: No exit request exists. Request ignored.");
      if (m_hasExitRequest)
        FSLLOG("WARNING: Non default exit code is already set. Request ignored.");
    }
  }


  bool DemoAppControlService::HasScreenshotRequest() const
  {
    return m_hasScreenshotRequest;
  }


  bool DemoAppControlService::HasAppRestartRequest() const
  {
    return m_hasAppRestartResetRequest;
  }


  bool DemoAppControlService::HasUpdateTimerResetRequest() const
  {
    return m_hasUpdateTimerResetRequest;
  }


  bool DemoAppControlService::HasExitRequest() const
  {
    // If no exit request is active then check the signal handlers exit flag to see if a externally triggered exit
    // has been requested
    if (m_platformControl && !m_hasExitRequest && m_platformControl->HasExitRequest())
      m_hasExitRequest = true;
    return m_hasExitRequest;
  }


  int DemoAppControlService::GetExitCode() const
  {
    return m_exitCode;
  }


  void DemoAppControlService::SetTimeStepMode(const TimeStepMode timeStepMode)
  {
    if (timeStepMode == m_timestepMode)
      return;

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


  TimeStepMode DemoAppControlService::GetTimeStepMode() const
  {
    return m_timestepMode;
  }


  void DemoAppControlService::ClearScreenshotRequestRequest()
  {
    m_hasScreenshotRequest = false;
  }


  void DemoAppControlService::ClearAppRestartRequestRequest()
  {
    m_hasAppRestartResetRequest = false;
  }


  void DemoAppControlService::ClearUpdateTimerResetRequest()
  {
    m_hasUpdateTimerResetRequest = false;
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
      FSLLOG_WARNING("WARNING: exit has already been requested with status code: " << (int)m_exitCode << ". Request ignored.");
    }
  }

}
