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
#include <FslSimpleUI/Base/Gesture/GestureTransactionRecord.hpp>
#include <FslSimpleUI/Base/Log/Gesture/FmtGestureTransactionMode.hpp>

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
    constexpr MovementTransactionAction SelectInitialAction(const GestureTransactionMode mode) noexcept
    {
      switch (mode)
      {
      case GestureTransactionMode::Evaluate:
        return MovementTransactionAction::Evaluate;
      case GestureTransactionMode::Handle:
        return MovementTransactionAction::Handle;
      case GestureTransactionMode::Control:
        return MovementTransactionAction::Control;
      case GestureTransactionMode::Invalid:
      case GestureTransactionMode::Ignore:
      default:
        return MovementTransactionAction::NotInterested;
      }
    }
  }

  GestureTransactionRecord::GestureTransactionRecord(const GestureFlags enabledGestures, const GestureTransactionMode mode,
                                                     const MillisecondTickCount32 downTimestamp, const DpPoint2F initialDownPositionDpf)
    : m_mode(mode)
    , m_downTimestamp(downTimestamp)
    , m_initialDownPositionDpf(initialDownPositionDpf)
    , m_allowTap(GestureFlagsUtil::IsEnabled(enabledGestures, GestureFlags::Tap))
    , m_action(SelectInitialAction(mode))
  {
  }

  void GestureTransactionRecord::DisableTap()
  {
    m_allowTap = false;

    LOCAL_LOG("DisableTap");
  }

  // void GestureTransactionRecord::MarkAsHold();
  //{
  //   m_holdDetection = DetectionState.Found;
  // }

  void GestureTransactionRecord::MarkAsInMotion()
  {
    m_inMotion = true;
    m_allowTap = false;
    // m_holdDetection = DetectionState.Disabled;
    LOCAL_LOG("MarkAsInMotion");
  }

  void GestureTransactionRecord::TakeControl()
  {
    switch (m_mode)
    {
    case GestureTransactionMode::Evaluate:
      m_action = MovementTransactionAction::InterceptAndControl;
      break;
    case GestureTransactionMode::Handle:
      m_action = MovementTransactionAction::Control;
      break;
    default:
      break;
    }
    m_mode = GestureTransactionMode::Control;

    LOCAL_LOG("Mode: {}", m_mode);
  }

  MovementTransactionAction GestureTransactionRecord::ClaimAction()
  {
    const auto action = m_action;
    if (m_action == MovementTransactionAction::InterceptAndControl)
    {
      m_action = MovementTransactionAction::Control;
    }
    return action;
  }

  MovementTransactionAction GestureTransactionRecord::ClaimCancelAction(const MovementOwnership movementOwnership)
  {
    if (movementOwnership == MovementOwnership::HandledAndControlled)
    {
      return MovementTransactionAction::NotInterested;
    }
    if (m_action == MovementTransactionAction::Evaluate)
    {
      m_action = MovementTransactionAction::NotInterested;
    }
    return m_action;
  }

  MovementTransactionAction GestureTransactionRecord::ClaimEndAction()
  {
    if (m_action == MovementTransactionAction::Evaluate)
    {
      m_action = MovementTransactionAction::NotInterested;
    }
    return m_action;
  }

}
