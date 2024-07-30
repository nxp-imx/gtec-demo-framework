/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include "InputModule.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <cassert>
#include "../../ITreeNodeClickInputTargetLocater.hpp"
#include "../../TreeNode.hpp"
#include "../IModuleHost.hpp"
#include "HitBasedInputSender.hpp"

namespace Fsl::UI
{
  InputModule::InputModule(const std::shared_ptr<IModuleHost>& moduleHost)
    : m_targetLocater(moduleHost->GetTargetLocater())
    , m_hitBasedInputSender(std::make_shared<HitBasedInputSender>(moduleHost))
  {
  }


  InputModule::~InputModule() = default;


  bool InputModule::IsIdle() const noexcept
  {
    return !m_hitBasedInputSender || !m_hitBasedInputSender->HasActiveClickEvent();
  }


  bool InputModule::MouseMove(const MillisecondTickCount32 timestamp, const int32_t sourceId, const int32_t sourceSubId,
                              const PxPoint2& screenPositionPx, const bool isTouch)
  {
    FSL_PARAM_NOT_USED(isTouch);

    auto target = m_targetLocater->TryGetMouseOverWindow(screenPositionPx);
    auto previousTarget = m_mouseOver.Target.lock();
    if (previousTarget != target)
    {
      bool handled = false;
      if (previousTarget)
      {
        FSLLOG3_VERBOSE5("SendMouseOver end");
        // End the event
        handled = m_hitBasedInputSender->SendMouseOverEvent(timestamp, sourceId, sourceSubId, EventTransactionState::End, false, screenPositionPx,
                                                            previousTarget);
      }
      // set the new target and send the start event if the target is eligible

      // Clear the current target
      m_mouseOver.Target.reset();
      if (target)
      {
        // Dont allow mouse over while in a active click event
        if (!m_hitBasedInputSender->HasActiveClickEventThatIsNot(target))
        {
          m_mouseOver.Target = target;
          assert(target);
          FSLLOG3_VERBOSE5("SendMouseOver begin");
          return (m_hitBasedInputSender->SendMouseOverEvent(timestamp, sourceId, sourceSubId, EventTransactionState::Begin, false, screenPositionPx,
                                                            target) ||
                  handled);
        }
      }
    }
    else if (target)
    {
      FSLLOG3_VERBOSE5("SendMouseOver continue");
      // Continue the event
      return m_hitBasedInputSender->SendMouseOverEvent(timestamp, sourceId, sourceSubId, EventTransactionState::Begin, true, screenPositionPx,
                                                       target);
    }
    return false;
  }


  bool InputModule::SendClickEvent(const MillisecondTickCount32 timestamp, const int32_t sourceId, const int32_t sourceSubId,
                                   const EventTransactionState state, const bool isRepeat, const PxPoint2& screenPositionPx, const bool isTouch)
  {
    if (isTouch && state == EventTransactionState::End)
    {
      auto previousTarget = m_mouseOver.Target.lock();
      if (previousTarget)
      {
        FSLLOG3_VERBOSE5("SendMouseOver end");
        // End the event
        m_hitBasedInputSender->SendMouseOverEvent(timestamp, sourceId, sourceSubId, EventTransactionState::End, false, screenPositionPx,
                                                  previousTarget);
        // Clear the current target
        m_mouseOver.Target.reset();
      }
    }
    return m_hitBasedInputSender->SendInputClickEvent(timestamp, sourceId, sourceSubId, state, isRepeat, screenPositionPx);
  }
}
