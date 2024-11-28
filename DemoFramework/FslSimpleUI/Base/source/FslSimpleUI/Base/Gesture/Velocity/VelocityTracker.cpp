/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Time/FmtMillisecondTickCount32.hpp>
#include <FslSimpleUI/Base/Gesture/Velocity/VelocityTracker.hpp>
#include "Strategy/LeastSquaresVelocityTrackerStrategy.hpp"

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      //! This is basically a threshold for determining if movement has stopped.
      constexpr TimeSpan AssumeMovementStoppedTime = TimeSpan::FromMilliseconds(40);
    }
  }

  VelocityTracker& VelocityTracker::operator=(VelocityTracker&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_lastMovementTimestamp = other.m_lastMovementTimestamp;
      m_strategyX = std::move(other.m_strategyX);
      m_strategyY = std::move(other.m_strategyY);

      // Remove the data from other
      other.m_lastMovementTimestamp = {};
    }
    return *this;
  }

  VelocityTracker::VelocityTracker(VelocityTracker&& other) noexcept
    : m_lastMovementTimestamp(other.m_lastMovementTimestamp)
    , m_strategyX(std::move(other.m_strategyX))
    , m_strategyY(std::move(other.m_strategyY))
  {
    // Remove the data from other
    other.m_lastMovementTimestamp = {};
  }


  VelocityTracker::VelocityTracker()
    : m_strategyX(std::make_unique<LeastSquaresVelocityTrackerStrategy>(2, LeastSquaresVelocityTrackerStrategy::Weighting::Disabled))
    , m_strategyY(std::make_unique<LeastSquaresVelocityTrackerStrategy>(2, LeastSquaresVelocityTrackerStrategy::Weighting::Disabled))
  {
  }


  bool VelocityTracker::HasVelocityEntries() const noexcept
  {
    return !m_strategyX->IsEmpty() || !m_strategyY->IsEmpty();
  }


  void VelocityTracker::Clear()
  {
    FSLLOG3_VERBOSE5("AddMovement clear");
    m_strategyX->ClearPointer();
    m_strategyY->ClearPointer();
  }


  void VelocityTracker::AddMovement(const MillisecondTickCount32 timestamp, const DpPoint2F positionDpf)
  {
    FSLLOG3_VERBOSE5("AddMovement(timestamp: {}, positionDpf: {})", timestamp, positionDpf);
    if ((timestamp - m_lastMovementTimestamp) >= LocalConfig::AssumeMovementStoppedTime)
    {
      FSLLOG3_VERBOSE5("AddMovement cleared since the movement appear to have stopped");
      m_strategyX->ClearPointer();
      m_strategyY->ClearPointer();
    }
    m_lastMovementTimestamp = timestamp;

    m_strategyX->AddMovement(timestamp, positionDpf.X);
    m_strategyY->AddMovement(timestamp, positionDpf.Y);
  }


  bool VelocityTracker::TryGetVelocity(DpPoint2F& rValue) const noexcept
  {
    DpValueF velocityX;
    DpValueF velocityY;
    bool hasX = m_strategyX->TryGetVelocity(velocityX);
    bool hasY = m_strategyY->TryGetVelocity(velocityY);
    rValue = DpPoint2F(velocityX, velocityY);
    return hasX || hasY;
  }


  void VelocityTracker::Reset() noexcept
  {
    m_lastMovementTimestamp = {};
    m_strategyX.reset();
    m_strategyY.reset();
  }
}
