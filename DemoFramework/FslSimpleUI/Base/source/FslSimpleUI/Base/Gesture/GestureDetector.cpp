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
#include <FslSimpleUI/Base/Gesture/GestureDetector.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan MaxTapTime = TimeSpan::FromMilliseconds(500);
      constexpr TimeSpan HoldTimeout = TimeSpan::FromMilliseconds(900);
      constexpr DpValueF FlickVelocityThresholdX(50.0f);
      constexpr DpValueF FlickVelocityThresholdY(50.0f);
      constexpr DpValueF MinFlingVelocityDpf(50.0f);
      constexpr DpValueF MaxFlingVelocityDpf(8000.0f);
      constexpr DpValueF TapDistThresholdSqr(4);
    }

    constexpr DpPoint2F ClampMagnitude(const DpPoint2F srcValue, const DpValueF minValue, const DpValueF maxValue)
    {
      assert(minValue.Value >= 0.0);
      assert(maxValue.Value >= 0.0 && maxValue.Value >= minValue.Value);

      const DpValueF valueSquared = srcValue.LengthSquared();
      if (valueSquared < (minValue * minValue))
      {
        const DpValueF lengthDpf(std::sqrt(valueSquared.Value));
        return lengthDpf != DpValueF::Zero() ? (srcValue / lengthDpf) * minValue : DpPoint2F::Zero();
      }
      if (valueSquared > (maxValue * maxValue))
      {
        const DpValueF lengthDpf(std::sqrt(valueSquared.Value));
        return lengthDpf != DpValueF::Zero() ? (srcValue / lengthDpf) * maxValue : DpPoint2F::Zero();
      }
      return srcValue;
    }
  }


  GestureDetector& GestureDetector::operator=(GestureDetector&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_velocityTracker = std::move(other.m_velocityTracker);
      m_isDown = other.m_isDown;
      m_hasTapCandidate = other.m_hasTapCandidate;
      m_gotHold = other.m_gotHold;
      m_inMotion = other.m_inMotion;
      m_downTimestamp = other.m_downTimestamp;
      m_initialDownPositionDpf = other.m_initialDownPositionDpf;
      m_currentlyPressed = other.m_currentlyPressed;
      m_gestureQueue = std::move(other.m_gestureQueue);

      // Remove the data from other
      other.m_isDown = false;
      other.m_hasTapCandidate = false;
      other.m_gotHold = false;
      other.m_inMotion = false;
      other.m_downTimestamp = {};
      other.m_initialDownPositionDpf = {};
      other.m_currentlyPressed = false;
    }
    return *this;
  }

  GestureDetector::GestureDetector(GestureDetector&& other) noexcept
    : m_velocityTracker(std::move(other.m_velocityTracker))
    , m_isDown(other.m_isDown)
    , m_hasTapCandidate(other.m_hasTapCandidate)
    , m_gotHold(other.m_gotHold)
    , m_inMotion(other.m_inMotion)
    , m_downTimestamp(other.m_downTimestamp)
    , m_initialDownPositionDpf(other.m_initialDownPositionDpf)
    , m_currentlyPressed(other.m_currentlyPressed)
    , m_gestureQueue(std::move(other.m_gestureQueue))
  {
    // Remove the data from other
    other.m_isDown = false;
    other.m_hasTapCandidate = false;
    other.m_gotHold = false;
    other.m_inMotion = false;
    other.m_downTimestamp = {};
    other.m_initialDownPositionDpf = {};
    other.m_currentlyPressed = false;
  }

  GestureDetector::GestureDetector() = default;


  bool GestureDetector::IsGestureAvailable() const noexcept
  {
    return !m_gestureQueue.empty();
  }


  void GestureDetector::Clear()
  {
    Reset();
  }

  bool GestureDetector::TryReadGesture(GestureRecord& rRecord) noexcept
  {
    if (!IsGestureAvailable())
    {
      rRecord = {};
      return false;
    }

    rRecord = m_gestureQueue.front();
    m_gestureQueue.pop();
    return true;
  }


  void GestureDetector::AddMovement(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf, const bool isPressed)
  {
    if (m_isDown && m_hasTapCandidate)
    {
      const auto sessionDuration = timestamp - m_downTimestamp;
      // Update tap candidate
      if (sessionDuration > LocalConfig::MaxTapTime)
      {
        assert(m_isDown);
        assert(m_hasTapCandidate);
        m_hasTapCandidate = false;
      }
      if (!m_gotHold && sessionDuration > LocalConfig::HoldTimeout)
      {
        m_gotHold = true;
        m_gestureQueue.emplace(GestureType::Hold, m_initialDownPositionDpf, DpPoint2F());
      }
    }

    // pointer motion event
    if (isPressed)
    {
      if (!m_currentlyPressed)
      {
        m_velocityTracker.Clear();
      }
      m_velocityTracker.AddMovement(timestamp, screenPositionDpf);
    }
    else if (m_currentlyPressed)
    {
      // Register the last position when the button was released as movement as well.
      m_velocityTracker.AddMovement(timestamp, screenPositionDpf);
    }

    ProcessMovement(timestamp, screenPositionDpf, isPressed, isPressed ? m_currentlyPressed : false);
    m_currentlyPressed = isPressed;
  }


  void GestureDetector::Reset() noexcept
  {
    m_velocityTracker.Clear();
    m_isDown = false;
    m_hasTapCandidate = false;
    m_gotHold = false;
    m_inMotion = false;
    m_downTimestamp = {};
    m_initialDownPositionDpf = {};
    m_currentlyPressed = false;

    while (!m_gestureQueue.empty())
    {
      m_gestureQueue.pop();
    }
  }


  void GestureDetector::ProcessMovement(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf, const bool isPressed,
                                        const bool isRepeat)
  {
    if (isPressed)
    {
      if (isRepeat)
      {
        if (m_isDown && !m_gotHold)
        {
          const DpPoint2F deltaPosDp = screenPositionDpf - m_initialDownPositionDpf;

          if (!m_inMotion)
          {
            const auto distSqrDp = deltaPosDp.LengthSquared();
            if (distSqrDp > LocalConfig::TapDistThresholdSqr)
            {
              m_inMotion = true;
              m_hasTapCandidate = false;

              m_gestureQueue.emplace(GestureType::FreeDrag, m_initialDownPositionDpf, DpPoint2F());
              m_gestureQueue.emplace(GestureType::FreeDrag, screenPositionDpf, DpPoint2F());
            }
          }
          else
          {
            m_gestureQueue.emplace(GestureType::FreeDrag, screenPositionDpf, DpPoint2F());
          }
        }
      }
      else
      {
        // pointer down event
        m_isDown = true;
        m_hasTapCandidate = true;
        m_inMotion = false;
        m_initialDownPositionDpf = screenPositionDpf;
        m_gotHold = false;

        // Store this so we know when the down occurred
        m_downTimestamp = timestamp;
      }
    }
    else
    {
      // pointer up event
      if (!m_gotHold)
      {
        if (m_hasTapCandidate)
        {
          m_gestureQueue.emplace(GestureType::Tap, screenPositionDpf, DpPoint2F());
        }
        else if (m_inMotion)
        {
          // Include a flick if above the threshold else just send a simple DragComplete
          DpPoint2F flingVelocityDpf;
          m_velocityTracker.TryGetVelocity(flingVelocityDpf);

          const float velocityMagXDp = std::abs(flingVelocityDpf.X.Value);
          const float velocityMagYDp = std::abs(flingVelocityDpf.Y.Value);

          const auto finalDeltaDpf = screenPositionDpf - m_initialDownPositionDpf;

          if (velocityMagXDp > LocalConfig::FlickVelocityThresholdX.Value || velocityMagYDp > LocalConfig::FlickVelocityThresholdY.Value)
          {
            const auto clampedFlingVelocityDpf = ClampMagnitude(flingVelocityDpf, LocalConfig::MinFlingVelocityDpf, LocalConfig::MaxFlingVelocityDpf);
            // FSLLOG3_INFO("flingVelocityDpf {} clampedFlingVelocityDpf {}", flingVelocityDpf, clampedFlingVelocityDpf);
            m_gestureQueue.emplace(GestureType::DragComplete, finalDeltaDpf, clampedFlingVelocityDpf);
          }
          else
          {
            m_gestureQueue.emplace(GestureType::DragComplete, finalDeltaDpf, DpPoint2F());
          }
        }
      }

      m_isDown = false;
    }
  }
}
