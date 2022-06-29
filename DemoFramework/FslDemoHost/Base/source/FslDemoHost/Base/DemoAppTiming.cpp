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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoHost/Base/DemoAppTiming.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      // The real value is not acquired from a service
      constexpr uint16_t DefaultFixedUpdatesPerSecond = 60;
      constexpr TimeSpan MaxFrameTime(TimeInfo::TicksPerSecond / 2);
    }
  }

  bool DemoAppTiming::Config::SetFixedUpdatesPerSecond(const uint16_t fixedUpdatesPerSecond) noexcept
  {
    const uint16_t cappedFixedUpdatesPerSecond = std::max(fixedUpdatesPerSecond, static_cast<uint16_t>(1));
    const bool wasChanged = cappedFixedUpdatesPerSecond != FixedUpdatesPerSecond;
    if (wasChanged)
    {
      FixedUpdatesPerSecond = cappedFixedUpdatesPerSecond;
      ExpectedFixedFrameTime =
        TimeSpan(static_cast<int64_t>(std::round(static_cast<double>(TimeInfo::TicksPerSecond) / static_cast<double>(FixedUpdatesPerSecond))));
    }
    return wasChanged;
  }


  DemoAppTiming::DemoAppTiming(const TimeSpan currentTimestamp, const TimeSpan forcedUpdateTime)
    : m_config(LocalConfig::DefaultFixedUpdatesPerSecond, forcedUpdateTime, LocalConfig::MaxFrameTime)
  {
    m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime;
    ResetTimer(currentTimestamp);
  }


  bool DemoAppTiming::SetFixedUpdatesPerSecond(const uint16_t fixedUpdatesPerSecond) noexcept
  {
    bool wasChanged = m_config.SetFixedUpdatesPerSecond(fixedUpdatesPerSecond);
    if (wasChanged)
    {
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime;
    }
    return wasChanged;
  }


  void DemoAppTiming::ResetTimer(const TimeSpan currentTimestamp)
  {
    m_timing.TimeThen = currentTimestamp;
    m_timing.FixedTime.CurrentAccumulatedTime = {};
    DoTimeNow(m_timing.TimeThen, TimeStepMode::Normal, false);
  }


  void DemoAppTiming::AdvanceFixedTimeStep()
  {
    auto oldTimeTicks = m_timing.TimeThen;
    m_timing.TimeThen -= m_timing.FixedTime.ExpectedFrameTime;
    DoTimeNow(oldTimeTicks, TimeStepMode::Normal, true);
  }


  void DemoAppTiming::TimeNow(const TimeSpan currentTimestamp, const TimeStepMode timeStepMode)
  {
    DoTimeNow(currentTimestamp, timeStepMode, true);
  }


  bool DemoAppTiming::HasPendingFixedUpdate() const noexcept
  {
    return m_timing.FixedTime.CurrentAccumulatedTime >= m_timing.FixedTime.ExpectedFrameTime;
  }


  std::optional<DemoTime> DemoAppTiming::TryFixedUpdate()
  {
    if (m_timing.FixedTime.CurrentAccumulatedTime >= m_timing.FixedTime.ExpectedFrameTime)
    {
      m_timing.FixedTime.AccumulatedTotalTime += m_timing.FixedTime.ExpectedFrameTime;
      m_timing.FixedTime.CurrentAccumulatedTime -= m_timing.FixedTime.ExpectedFrameTime;
      return {DemoTime(m_timing.FixedTime.AccumulatedTotalTime, m_timing.FixedTime.ExpectedFrameTime)};
    }
    return {};
  }


  void DemoAppTiming::DoTimeNow(const TimeSpan currentTimestamp, const TimeStepMode timeStepMode, const bool allowForce)
  {
    ConfigureTimeStepMode(timeStepMode);

    // Handle forced update time if enabled
    if (m_config.ForcedUpdateTime.Ticks() != 0 && allowForce)
    {
      m_timing.TimeThen = currentTimestamp - m_config.ForcedUpdateTime;
    }

    // Calc the difference between the last time and now, then store the 'new time as the last for next time'
    TimeSpan timeDiff = currentTimestamp - m_timing.TimeThen;
    m_timing.TimeThen = currentTimestamp;

    // Handle fixed-step and pause mode
    ApplyTimeStepMode(timeDiff, m_timing.FixedTime.CurrentAccumulatedTime, timeStepMode);

    // Ensure that we dont have to do a mega time catchup (which if it occurred would mean something else was wrong)
    timeDiff = ApplyMaxFrameTimpCap(timeDiff);

    // Apply the time-diff to the current accumulated fixed time (this will allow the TryFixedUpdate method to work)
    m_timing.FixedTime.CurrentAccumulatedTime += timeDiff;

    // Apply the various 2x, 4x timestep modes if enabled
    timeDiff = ApplyTimeStepModeFixedTimeSteps(timeDiff, timeStepMode);

    // Apply final time-diff to the total accumulated time
    m_timing.LastTimeDiff = timeDiff;
    m_timing.AccumulatedTotalTime += timeDiff;
  }


  void DemoAppTiming::ConfigureTimeStepMode(const TimeStepMode mode)
  {
    switch (mode)
    {
    case TimeStepMode::Normal:
    case TimeStepMode::Paused:
    case TimeStepMode::Step:
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime;
      break;
    case TimeStepMode::Slow2X:
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime * 2;
      break;
    case TimeStepMode::Slow4X:
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime * 4;
      break;
    case TimeStepMode::Fast2X:
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime / 2;
      break;
    case TimeStepMode::Fast4X:
      m_timing.FixedTime.ExpectedFrameTime = m_config.ExpectedFixedFrameTime / 4;
      break;
    default:
      FSLLOG3_WARNING("Unknown timestep mode");
      break;
    }
  }

  TimeSpan DemoAppTiming::ApplyMaxFrameTimpCap(const TimeSpan timeDiff) const noexcept
  {
    return timeDiff >= m_config.MaxFrameTime ? m_timing.FixedTime.ExpectedFrameTime : timeDiff;
  }

  void DemoAppTiming::ApplyTimeStepMode(TimeSpan& rTimeDiff, TimeSpan& rAccumulatedTime, const TimeStepMode timeStepMode) const noexcept
  {
    switch (timeStepMode)
    {
    case TimeStepMode::Paused:
      rTimeDiff = {};
      rAccumulatedTime = {};
      break;
    case TimeStepMode::Step:
      rTimeDiff = m_timing.FixedTime.ExpectedFrameTime;
      rAccumulatedTime = {};
      break;
    default:
      break;
    }
  }

  TimeSpan DemoAppTiming::ApplyTimeStepModeFixedTimeSteps(const TimeSpan timeDiff, const TimeStepMode timeStepMode) const noexcept
  {
    switch (timeStepMode)
    {
    case TimeStepMode::Slow2X:
    case TimeStepMode::Slow4X:
    case TimeStepMode::Fast2X:
    case TimeStepMode::Fast4X:
      {
        // When in timestep mode we apply a fixed amount of time steps
        const uint64_t expectedUpdateCount = m_timing.FixedTime.CurrentAccumulatedTime.Ticks() / m_timing.FixedTime.ExpectedFrameTime.Ticks();
        return UncheckedNumericCast<int32_t>(expectedUpdateCount) * m_config.ExpectedFixedFrameTime;
      }
    default:
      return timeDiff;
    }
  }

}
