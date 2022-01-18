#ifndef FSLDEMOAPP_BASE_SERVICE_DEMOAPPCONTROL_IDEMOAPPCONTROL_HPP
#define FSLDEMOAPP_BASE_SERVICE_DEMOAPPCONTROL_IDEMOAPPCONTROL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoApp/Base/TimeStepMode.hpp>

namespace Fsl
{
  class IDemoAppControl
  {
  public:
    virtual ~IDemoAppControl() = default;

    //! @brief Request a screenshot (this causes a update timer reset to occur as well, to prevent the screenshot from affecting the animation too
    //! much)
    virtual void RequestScreenshot() noexcept = 0;

    //! @brief Request that the app restarts.
    virtual void RequestAppRestart() noexcept = 0;

    //! @brief Reset the update timers (do this if a lengthy operation has taken place to prevent a lot FixedUpdate catchup calls)
    virtual void RequestUpdateTimerReset() noexcept = 0;

    //! @brief Request that the demo exits with the default exit code.
    virtual void RequestExit() = 0;

    //! @brief Request that the demo exits with the provided exit code.
    virtual void RequestExit(const int exitCode) = 0;

    //! @brief Changes the exit code if one has been set.
    //!        Will ignore the change request if not in 'requested exit' mode and log a warning.
    //!        Will ignore the change request if the current exit code isn't defaultExitCode and log a warning
    virtual void ChangeExitCode(const int exitCode) = 0;

    virtual bool HasScreenshotRequest() const noexcept = 0;
    virtual bool HasAppRestartRequest() const noexcept = 0;
    virtual bool HasUpdateTimerResetRequest() const noexcept = 0;
    virtual bool HasExitRequest() const noexcept = 0;
    virtual int GetExitCode() const noexcept = 0;

    virtual void SetTimeStepMode(const TimeStepMode timeStepMode) = 0;
    virtual TimeStepMode GetTimeStepMode() const noexcept = 0;

    //! @brief This is the max used number send to BeginFrame as a frame index and it will be (1 >= returnedValue <= GetRenderLoopMaxFramesInFlight())
    virtual uint32_t GetRenderLoopFrameCounter() const noexcept = 0;

    //! @brief This is the max used number send to BeginFrame as a frame index
    virtual void SetRenderLoopFrameCounter(const uint32_t frameCount) = 0;

    //! @brief This is the max allowed number send to BeginFrame as a frame index
    //!        This number is the fixed upper bound used by the app and will not change while the app is running.
    //! @return the Value will always be >= 1
    virtual uint32_t GetRenderLoopMaxFramesInFlight() const noexcept = 0;

    //! @brief Enable enable/disable mouse capture
    //! @return true if the set succeeded, false if it failed
    //! @note  When mouse capture is enabled the mouse pointer is also hidden automatically.
    virtual bool TryEnableMouseCaptureMode(const bool enabled) = 0;

    //! @brief Enable enable/disable mouse capture
    //! @note  When mouse capture is enabled the mouse pointer is also hidden automatically.
    //! @throws NotSupportedException if mouse capture is unsupported on this platform.
    virtual void EnableMouseCaptureMode(const bool enabled) = 0;

    //! @return true if mouse capture is enabled else false
    virtual bool GetMouseCaptureMode() const noexcept = 0;

    //! @return the number of fixed updates per second.
    virtual uint16_t GetFixedUpdatesPerSecond() const noexcept = 0;

    //! @brief Set the number of fixed updates per second
    //! @param updates per second (will be clamped to >= 1)
    virtual void SetFixedUpdatesPerSecond(const uint16_t updatesPerSecond) = 0;

    //! @brief Get the on demand frame rate interval (used when the app is idle)
    //! @return The frame interval which will always be >= 1
    virtual uint16_t GetOnDemandFrameInterval() const noexcept = 0;

    //! @brief Set the OnDemandRendering frame interval
    //! @param frameRateInterval the frameRateInterval to use (valid range is >= 1, if less than one it will be set to 1)
    virtual void SetOnDemandFrameInterval(const uint16_t frameRateInterval) = 0;
  };
}

#endif
