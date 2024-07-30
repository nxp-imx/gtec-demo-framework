#ifndef FSLSIMPLEUI_BASE_GESTURE_VELOCITY_STRATEGY_ACCUMULATINGVELOCITYTRACKERSTRATEGY_HPP
#define FSLSIMPLEUI_BASE_GESTURE_VELOCITY_STRATEGY_ACCUMULATINGVELOCITYTRACKERSTRATEGY_HPP
/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Simplified and adapted for use in the NXP demo framework 2024.

#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <FslSimpleUI/Base/Gesture/Velocity/Strategy/IVelocityTrackerStrategy.hpp>

namespace Fsl::UI
{
  class AccumulatingVelocityTrackerStrategy : public IVelocityTrackerStrategy
  {
  public:
    //! Number of samples to keep.
    //! If different strategies would like to maintain different history size, we can make this a protected const field.
    static constexpr uint16_t HistorySize = 20;

  protected:
    struct MovementRecord
    {
      MillisecondTickCount32 EventTime;
      DpValueF PositionDpf;

      constexpr MovementRecord() noexcept = default;
      constexpr MovementRecord(const MillisecondTickCount32 eventTime, const DpValueF positionDpf) noexcept
        : EventTime(eventTime)
        , PositionDpf(positionDpf)
      {
      }
    };

    //! Duration, in nanoseconds, since the latest movement where a movement may be considered for velocity calculation.
    const TimeSpan HorizonNanos;

    //! If true, data points outside of horizon (see `mHorizonNanos`) will be cleared after each addition of a new movement.
    const bool MaintainHorizonDuringAdd;

    CircularFixedSizeBuffer<MovementRecord> m_movements;

  public:
    AccumulatingVelocityTrackerStrategy(const TimeSpan horizonNanos, const bool maintainHorizonDuringAdd);

    void AddMovement(const MillisecondTickCount32 eventTime, const DpValueF positionDpf) final;
    void ClearPointer() noexcept final;
  };
}

#endif
