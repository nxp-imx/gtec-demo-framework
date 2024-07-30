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

#include "AccumulatingVelocityTrackerStrategy.hpp"

namespace Fsl::UI
{
  AccumulatingVelocityTrackerStrategy::AccumulatingVelocityTrackerStrategy(const TimeSpan horizonNanos, const bool maintainHorizonDuringAdd)
    : HorizonNanos(horizonNanos)
    , MaintainHorizonDuringAdd(maintainHorizonDuringAdd)
    , m_movements(HistorySize)
  {
  }


  void AccumulatingVelocityTrackerStrategy::AddMovement(const MillisecondTickCount32 eventTime, const DpValueF positionDpf)
  {
    const std::size_t size = m_movements.size();
    if (size > 0 && m_movements.back().EventTime == eventTime)
    {
      // When ACTION_POINTER_DOWN happens, we will first receive ACTION_MOVE with the coordinates
      // of the existing pointers, and then ACTION_POINTER_DOWN with the coordinates that include
      // the new pointer. If the eventtimes for both events are identical, just update the data
      // for this time (i.e. pop out the last element, and insert the updated movement).
      // We only compare against the last value, as it is likely that addMovement is called
      // in chronological order as events occur.
      m_movements.pop_back();
    }
    m_movements.push_back(MovementRecord(eventTime, positionDpf));

    // Clear movements that do not fall within `m_horizonNanos` of the latest movement.
    // Note that, if in the future we decide to use more movements (i.e. increase HISTORY_SIZE),
    // we can consider making this step binary-search based, which will give us some improvement.
    if (MaintainHorizonDuringAdd)
    {
      while (eventTime - m_movements.front().EventTime > HorizonNanos)
      {
        m_movements.pop_front();
      }
    }
  }


  void AccumulatingVelocityTrackerStrategy::ClearPointer() noexcept
  {
    m_movements.clear();
  }
}
