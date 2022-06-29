#ifndef FSLDEMOHOST_BASE_DEMOAPPTIMING_HPP
#define FSLDEMOHOST_BASE_DEMOAPPTIMING_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/TimeStepMode.hpp>
#include <optional>

namespace Fsl
{
  class DemoAppTiming final
  {
    struct Config
    {
      //! The number of fixed updates per second
      uint16_t FixedUpdatesPerSecond{0};
      //! Override any timing and use this as a fixed time for all updates (if 0 this is disabled)
      TimeSpan ForcedUpdateTime;
      //! The max time a frame is allowed to take before we cap it.
      TimeSpan MaxFrameTime;
      //! The time we expect a frame to take (for fast refresh rate screen this can easily be too high)
      //! But this is the time we will use when doing time-stepping
      TimeSpan ExpectedFixedFrameTime;

      Config() noexcept = default;
      Config(const uint16_t fixedUpdatesPerSecond, const TimeSpan forcedUpdateTime, const TimeSpan maxFrameTime) noexcept
        : ForcedUpdateTime(forcedUpdateTime)
        , MaxFrameTime(maxFrameTime)

      {
        SetFixedUpdatesPerSecond(fixedUpdatesPerSecond);
      }

      bool SetFixedUpdatesPerSecond(const uint16_t fixedUpdatesPerSecond) noexcept;
    };

    struct FixedTiming
    {
      //! The current expected frame time
      TimeSpan ExpectedFrameTime;
      //! The total accumulated time that has not been used by the app so far
      TimeSpan CurrentAccumulatedTime;
      //! The total fixed time that has passed since we launched (which the app knows about)
      TimeSpan AccumulatedTotalTime;
    };

    struct Timing
    {
      TimeSpan TimeThen{0u};

      //! The time that has passed since last time
      TimeSpan LastTimeDiff;
      //! The total time that has passed since we launched
      TimeSpan AccumulatedTotalTime;

      FixedTiming FixedTime;
    };

    Config m_config;
    Timing m_timing;

  public:
    //! @brief
    //! @param currentTimestamp the current timestamp.
    //! @param forcedUpdateTime the forced update time ignores any real timings (if this is zero forced update times are disabled)
    explicit DemoAppTiming(const TimeSpan currentTimestamp, const TimeSpan forcedUpdateTime);

    //! @brief Set the number of fixed updates per second
    //! @return true if the value was different and therefore changed (false if the value was equal to the existing and therefore did nothing).
    bool SetFixedUpdatesPerSecond(const uint16_t fixedUpdatesPerSecond) noexcept;

    uint16_t GetFixedUpdatesPerSecond() const noexcept
    {
      return m_config.FixedUpdatesPerSecond;
    }

    //! @brief Reset the timer to the supplied time (last timestamp will also be equal to the current time)
    //! @param currentTimestamp the current timestamp.
    void ResetTimer(const TimeSpan currentTimestamp);

    //! Advance the time by the fixed/forced timestep
    void AdvanceFixedTimeStep();


    void TimeNow(const TimeSpan currentTimestamp, const TimeStepMode timeStepMode);


    bool HasPendingFixedUpdate() const noexcept;

    //! @brief Return a DemoTime if a FixedUpdate should be scheduled, return a empty Optional if not
    std::optional<DemoTime> TryFixedUpdate();

    DemoTime GetUpdateTime() const noexcept
    {
      return {m_timing.AccumulatedTotalTime, m_timing.LastTimeDiff};
    }

  private:
    void DoTimeNow(const TimeSpan currentTimestamp, const TimeStepMode timeStepMode, const bool allowForce);
    void ConfigureTimeStepMode(const TimeStepMode mode);
    TimeSpan ApplyMaxFrameTimpCap(const TimeSpan timeDiff) const noexcept;
    void ApplyTimeStepMode(TimeSpan& rTimeDiff, TimeSpan& rAccumulatedTime, const TimeStepMode timeStepMode) const noexcept;
    TimeSpan ApplyTimeStepModeFixedTimeSteps(const TimeSpan timeDiff, const TimeStepMode timeStepMode) const noexcept;
  };
}

#endif
