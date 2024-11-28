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
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslSimpleUI/Base/Gesture/GestureDetector.hpp>
#include <FslSimpleUI/Base/Log/Gesture/FmtGestureType.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

// #define LOCAL_LOG_ENABLED
#ifdef LOCAL_LOG_ENABLED
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG(...) FSLLOG3_INFO(__VA_ARGS__)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG(...) \
  {                    \
  }
#endif

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan MaxTapTime = TimeSpan::FromMilliseconds(500);
      // constexpr TimeSpan HoldTimeout = TimeSpan::FromMilliseconds(900);
      constexpr DpValueF FlickVelocityThresholdX(50.0f);
      constexpr DpValueF FlickVelocityThresholdY(50.0f);
      constexpr DpValueF MinFlingVelocityDpf(50.0f);
      constexpr DpValueF MaxFlingVelocityDpf(8000.0f);
      constexpr DpValueF TapDistThresholdSqr(4);
    }

    constexpr DpPoint2F ClampToAxis(const DpPoint2F deltaPosDpf, const GestureAxis axisFlags) noexcept
    {
      if (axisFlags == GestureAxis::XY)
      {
        return deltaPosDpf;
      }
      return GestureAxisUtil::IsEnabled(axisFlags, GestureAxis::X) ? DpPoint2F(deltaPosDpf.X, DpValueF(0.0f))
                                                                   : DpPoint2F(DpValueF(0.0f), deltaPosDpf.Y);
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

    constexpr DpValueF ClampMagnitude(DpValueF src, DpValueF min, DpValueF max)
    {
      const auto length = src.Value >= 0.0f ? src.Value : -src.Value;
      auto clampedLength = MathHelper::Clamp(length, min.Value, max.Value);
      return DpValueF(src.Value >= 0 ? clampedLength : -clampedLength);
    }

    constexpr DpPoint2F ClampMagnitude(const DpPoint2F srcValue, const DpValueF minValue, const DpValueF maxValue, const GestureAxis axisFlags)
    {
      if (GestureAxisUtil::IsEnabled(axisFlags, GestureAxis::XY))
      {
        return ClampMagnitude(srcValue, minValue, maxValue);
      }
      if (GestureAxisUtil::IsEnabled(axisFlags, GestureAxis::X))
      {
        return {ClampMagnitude(srcValue.X, minValue, maxValue), DpValueF()};
      }
      return {DpValueF(), ClampMagnitude(srcValue.Y, minValue, maxValue)};
    }

  }


  GestureDetector& GestureDetector::operator=(GestureDetector&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_enabledGestures = other.m_enabledGestures;
      m_velocityTracker = std::move(other.m_velocityTracker);
      m_axisFlags = other.m_axisFlags;
      m_transactionRecord = other.m_transactionRecord;
      m_gestureQueue = std::move(other.m_gestureQueue);

      // Remove the data from other
      other.m_enabledGestures = GestureFlags::NotDefined;
      other.m_axisFlags = GestureAxis::NotDefined;
      other.m_transactionRecord = {};
    }
    return *this;
  }

  GestureDetector::GestureDetector(GestureDetector&& other) noexcept
    : m_enabledGestures(other.m_enabledGestures)
    , m_velocityTracker(std::move(other.m_velocityTracker))
    , m_axisFlags(other.m_axisFlags)
    , m_transactionRecord(other.m_transactionRecord)
    , m_gestureQueue(std::move(other.m_gestureQueue))
  {
    // Remove the data from other
    other.m_enabledGestures = GestureFlags::NotDefined;
    other.m_axisFlags = GestureAxis::NotDefined;
    other.m_transactionRecord = {};
  }

  GestureDetector::GestureDetector(const GestureFlags enabledGestures, const GestureAxis axisFlags)
    : m_enabledGestures(enabledGestures)
    , m_axisFlags(axisFlags)
  {
  }


  bool GestureDetector::IsGestureAvailable() const noexcept
  {
    return !m_gestureQueue.empty();
  }


  bool GestureDetector::InMomementTransaction() const noexcept
  {
    return !m_transactionRecord.IsDefault();
  }


  GestureAxis GestureDetector::GetGestureAxis() const noexcept
  {
    return m_axisFlags;
  }


  void GestureDetector::SetGestureAxis(const GestureAxis value)
  {
    m_axisFlags = value;
  }


  bool GestureDetector::HasVelocityEntries() const noexcept
  {
    return m_velocityTracker.HasVelocityEntries();
  }


  void GestureDetector::Clear()
  {
    // m_enabledGestures = GestureFlags::NotDefined;
    // m_axisFlags = GestureAxis::NotDefined;

    // Basically a reset but without clearing the enabled gestures and axis flags
    m_velocityTracker.Clear();
    m_transactionRecord = {};
    while (!m_gestureQueue.empty())
    {
      m_gestureQueue.pop();
    }
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


  MovementTransactionAction GestureDetector::AddMovement(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf,
                                                         const EventTransactionState state, const bool isRepeat,
                                                         const MovementOwnership movementOwnership)
  {
    EventTransactionState currentState = state;
    const bool isBeginTransaction = currentState == EventTransactionState::Begin && !isRepeat;
    if (!m_transactionRecord.IsDefault() && isBeginTransaction)
    {
      FSLLOG3_WARNING("Got a begin transaction while already in a transaction, cancelling existing transaction");
      CancelTransaction(timestamp, screenPositionDpf, movementOwnership);
    }
    else if (m_transactionRecord.Mode() == GestureTransactionMode::Ignore)
    {
      return MovementTransactionAction::Handle;
    }

    if (m_transactionRecord.IsDefault())
    {
      if (isBeginTransaction)
      {
        // A request to start a new transaction
        return BeginTransaction(timestamp, screenPositionDpf, movementOwnership);
      }
      return MovementTransactionAction::NotInterested;
    }
    if (movementOwnership == MovementOwnership::HandledAndControlled)    //  && m_transactionRecord.Mode != TransactionMode.Control)
    {
      LOCAL_LOG("Ownership claimed, cancelling transaction");
      currentState = EventTransactionState::Canceled;
    }

    // We are current in a transaction, so determine how to handle it
    switch (currentState)
    {
    case EventTransactionState::Begin:
      // Continue a existing transaction
      assert(!isBeginTransaction);
      return ContinueTransaction(timestamp, screenPositionDpf, movementOwnership);
    case EventTransactionState::Canceled:
      return CancelTransaction(timestamp, screenPositionDpf, movementOwnership);
    case EventTransactionState::End:
      if (m_transactionRecord.Mode() == GestureTransactionMode::Evaluate)
      {
        // For now we cancel the transaction if we are still in evaluate mode.
        // To allow Evaluate to 'intercept' during EndTransaction we would need to hook into the tunnel preview events
        return CancelTransaction(timestamp, screenPositionDpf, movementOwnership);
      }
      return EndTransaction(timestamp, screenPositionDpf);
    }
    FSLLOG3_WARNING("Unknown transaction state, cancelling transaction: {}", fmt::underlying(currentState));
    return CancelTransaction(timestamp, screenPositionDpf, movementOwnership);
  }


  MovementTransactionAction GestureDetector::BeginTransaction(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf,
                                                              const MovementOwnership movementOwnership)
  {
    if (movementOwnership == MovementOwnership::HandledAndControlled)
    {
      // Someone else is controlling this, so dont start a new transaction
      return MovementTransactionAction::NotInterested;
    }

    LOCAL_LOG("BeginTransaction: {0} {1} {2}", timestamp, screenPositionDpf, fmt::underlying(movementOwnership));

    auto mode = GestureTransactionMode::Handle;
    if (movementOwnership == MovementOwnership::Handled)
    {
      mode = GestureTransactionMode::Evaluate;
    }

    m_velocityTracker.Clear();
    m_velocityTracker.AddMovement(timestamp, screenPositionDpf);

    m_transactionRecord = GestureTransactionRecord(m_enabledGestures, mode, timestamp, screenPositionDpf);
    return m_transactionRecord.ClaimAction();
  }

  MovementTransactionAction GestureDetector::ContinueTransaction(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf,
                                                                 const MovementOwnership movementOwnership)
  {
    assert(!m_transactionRecord.IsDefault());
    assert(m_transactionRecord.Mode() == GestureTransactionMode::Control || m_transactionRecord.Mode() == GestureTransactionMode::Handle ||
           m_transactionRecord.Mode() == GestureTransactionMode::Evaluate);

    LOCAL_LOG("ContinueTransaction: {} {} {}", timestamp, screenPositionDpf, fmt::underlying(movementOwnership));

    UpdateTransactionState(timestamp, screenPositionDpf, true);

    if (!m_transactionRecord.IsHold())
    {
      DpPoint2F deltaPosDpf = screenPositionDpf - m_transactionRecord.GetInitialDownPositionDpf();
      if (!m_transactionRecord.InMotion())
      {
        deltaPosDpf = ClampToAxis(deltaPosDpf, m_axisFlags);
        auto distSqrDpf = deltaPosDpf.LengthSquared();
        if (distSqrDpf > LocalConfig::TapDistThresholdSqr)
        {
          m_transactionRecord.MarkAsInMotion();
          if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
          {
            EnqueueGesture(GestureRecord(GestureType::FreeDrag, m_transactionRecord.GetInitialDownPositionDpf(), DpPoint2F()));
            EnqueueGesture(GestureRecord(GestureType::FreeDrag, screenPositionDpf, DpPoint2F()));
          }
        }
      }
      else if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
      {
        EnqueueGesture(GestureRecord(GestureType::FreeDrag, screenPositionDpf, DpPoint2F()));
      }
    }
    return m_transactionRecord.ClaimAction();
  }

  MovementTransactionAction GestureDetector::EndTransaction(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf)
  {
    assert(!m_transactionRecord.IsDefault());
    assert(m_transactionRecord.Mode() == GestureTransactionMode::Control || m_transactionRecord.Mode() == GestureTransactionMode::Handle ||
           m_transactionRecord.Mode() == GestureTransactionMode::Evaluate);

    LOCAL_LOG("EndTransaction: {0} {1}", timestamp, screenPositionDpf);

    UpdateTransactionState(timestamp, screenPositionDpf, true);

    // Detect a motion that consist of a initial begin followed by a end
    if (!m_transactionRecord.IsHold() && !m_transactionRecord.InMotion())
    {
      const DpPoint2F deltaPosDpf = screenPositionDpf - m_transactionRecord.GetInitialDownPositionDpf();
      const auto distSqrDpf = deltaPosDpf.LengthSquared();
      if (distSqrDpf > LocalConfig::TapDistThresholdSqr)
      {
        m_transactionRecord.MarkAsInMotion();

        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
        {
          EnqueueGesture(GestureRecord(GestureType::FreeDrag, m_transactionRecord.GetInitialDownPositionDpf(), DpPoint2F()));
        }
      }
    }

    if (!m_transactionRecord.IsHold())
    {
      if (m_transactionRecord.CanBeTap())
      {
        assert(GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Tap));
        EnqueueGesture(GestureRecord(GestureType::Tap, screenPositionDpf, DpPoint2F()));
      }
      else if (m_transactionRecord.InMotion() && GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
      {
        // Include a flick if above the threshold else just send a simple DragComplete
        DpPoint2F flingVelocityDpf;
        m_velocityTracker.TryGetVelocity(flingVelocityDpf);

        float velocityMagXDp = std::abs(flingVelocityDpf.X.Value);
        float velocityMagYDp = std::abs(flingVelocityDpf.Y.Value);

        if (velocityMagXDp > LocalConfig::FlickVelocityThresholdX.Value || velocityMagYDp > LocalConfig::FlickVelocityThresholdY.Value)
        {
          auto clampedFlingVelocityDpf =
            ClampMagnitude(flingVelocityDpf, LocalConfig::MinFlingVelocityDpf, LocalConfig::MaxFlingVelocityDpf, m_axisFlags);
          // g_logger.Info("flingVelocityDpf {0} clampedFlingVelocityDpf {1}", flingVelocityDpf, clampedFlingVelocityDpf);
          EnqueueGesture(GestureRecord(GestureType::DragComplete, screenPositionDpf, clampedFlingVelocityDpf));
        }
        else
        {
          EnqueueGesture(GestureRecord(GestureType::DragComplete, screenPositionDpf, DpPoint2F()));
        }
      }
    }
    m_velocityTracker.Clear();

    const auto action = m_transactionRecord.ClaimEndAction();
    m_transactionRecord = {};
    return action;
  }


  //! This does not clear any pending gestures
  MovementTransactionAction GestureDetector::CancelTransaction(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf,
                                                               const MovementOwnership movementOwnership)
  {
    assert(!m_transactionRecord.IsDefault());
    assert(m_transactionRecord.Mode() == GestureTransactionMode::Control || m_transactionRecord.Mode() == GestureTransactionMode::Handle ||
           m_transactionRecord.Mode() == GestureTransactionMode::Evaluate);

    LOCAL_LOG("CancelTransaction: {} {}", timestamp, screenPositionDpf);

    UpdateTransactionState(timestamp, screenPositionDpf, false);

    m_velocityTracker.Clear();

    if (m_transactionRecord.InMotion() && GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
    {
      EnqueueGesture(GestureRecord(GestureType::DragCanceled, screenPositionDpf, DpPoint2F()));
    }

    const auto action = m_transactionRecord.ClaimCancelAction(movementOwnership);
    m_transactionRecord = {};
    return action;
  }


  void GestureDetector::UpdateTransactionState(const MillisecondTickCount32 timestamp, const DpPoint2F screenPositionDpf, const bool allowNewGestures)
  {
    if (m_transactionRecord.IsDefault())
    {
      return;
    }

    m_velocityTracker.AddMovement(timestamp, screenPositionDpf);

    auto sessionDuration = timestamp - m_transactionRecord.GetDownTimestamp();
    // Update tap candidate
    if (m_transactionRecord.CanBeTap() && sessionDuration > LocalConfig::MaxTapTime)
    {
      // Tap timed out -> so it can no longer trigger for this event type
      m_transactionRecord.DisableTap();
    }

    // if (m_transactionRecord.CanBeHold() && sessionDuration > LocalConfig::HoldTimeout)
    //{
    //   m_transactionRecord.MarkAsHold();
    //   EnqueueGesture(GestureRecord(GestureType::Hold, m_transactionRecord.IsHold(), DpPoint2F()));
    // }
  }


  void GestureDetector::EnqueueGesture(const GestureRecord record)
  {
    LOCAL_LOG("EnqueueGesture: {} {} {}", record.Gesture, record.PositionDpf, record.VelocityDpf);

    assert(!m_transactionRecord.IsDefault());
    m_gestureQueue.push(record);

    m_transactionRecord.TakeControl();
  }


  void GestureDetector::Reset() noexcept
  {
    m_enabledGestures = GestureFlags::NotDefined;
    m_velocityTracker.Clear();
    m_axisFlags = GestureAxis::NotDefined;
    m_transactionRecord = {};
    while (!m_gestureQueue.empty())
    {
      m_gestureQueue.pop();
    }
  }
}
