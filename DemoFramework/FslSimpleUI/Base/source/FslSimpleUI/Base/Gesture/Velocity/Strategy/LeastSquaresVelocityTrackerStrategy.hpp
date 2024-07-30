#ifndef FSLSIMPLEUI_BASE_GESTURE_VELOCITY_STRATEGY_LEASTSQUARESVELOCITYTRACKERSTRATEGY_HPP
#define FSLSIMPLEUI_BASE_GESTURE_VELOCITY_STRATEGY_LEASTSQUARESVELOCITYTRACKERSTRATEGY_HPP
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

#include <array>
#include "AccumulatingVelocityTrackerStrategy.hpp"

namespace Fsl::UI
{
  class LeastSquaresVelocityTrackerStrategy final : public AccumulatingVelocityTrackerStrategy
  {
  public:
    enum class Weighting
    {
      //! No weights applied.  All data points are equally reliable.
      Disabled,
      //! Weight by time delta.  Data points clustered together are weighted less.
      Delta,
      //! Weight such that points within a certain horizon are weighed more than those
      //! outside of that horizon.
      Central,
      //! Weight such that points older than a certain amount are weighed less.
      Recent,
    };


  private:
    struct Record
    {
      DpValueF Position;
      float W{0};
      float Time{0};

      Record() = default;
      Record(const DpValueF position, const float w, const float time)
      {
        Position = position;
        W = w;
        Time = time;
      }
    };

    const uint32_t Degree{0};
    const Weighting DesiredWeighting;

    mutable std::array<Record, AccumulatingVelocityTrackerStrategy::HistorySize> m_scratchpad{};

  public:
    explicit LeastSquaresVelocityTrackerStrategy(const uint32_t degree, const Weighting weighting = Weighting::Disabled);

    bool TryGetVelocity(DpValueF& rVelocityDpf) const noexcept final;

  private:
    float ChooseWeight(const std::size_t index) const noexcept;

    //! An optimized least-squares solver for degree 2 and no weight(i.e. `Weighting.NONE`).
    //! The provided container of movements shall NOT be empty, and shall have the movements in chronological order.
    static bool TrySolveUnweightedLeastSquaresDeg2(const CircularFixedSizeBuffer<MovementRecord>& movements, DpValueF& rResultDpf) noexcept;

    static bool TrySolveLeastSquares(const ReadOnlySpan<Record> span, const uint32_t n, DpValueF& rResultDpf) noexcept;
  };
}

#endif
