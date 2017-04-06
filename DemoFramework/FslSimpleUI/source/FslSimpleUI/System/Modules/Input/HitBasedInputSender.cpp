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

#include "HitBasedInputSender.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Event/WindowEventPool.hpp>
#include "../IModuleHost.hpp"
#include "../../Event/StateEvent.hpp"
#include "../../Event/StateEventSender.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      StateEvent Convert(const std::shared_ptr<WindowInputClickEvent>& theEvent)
      {
        assert(theEvent);
        StateEventInfo info(theEvent->GetSourceId(), theEvent->GetSourceSubId(), theEvent->GetState(), theEvent->IsRepeat());
        info.SetParam1(theEvent->GetScreenPosition());
        return StateEvent(theEvent, info);
      }

      //void OnSendCompleted(const std::shared_ptr<ISESOnCompleted>& stateEventSender, const StateEvent& theEvent, const std::shared_ptr<IWindowEventQueue> m_eventQueue)
      //{
      //  assert(stateEventSender);
      //  stateEventSender->ISESOnCompleted_HandleAutoFocus(m_eventQueue); // , theEvent.Content.TimeStamp);
      //}
      //

      StateEvent CreateTargetWindowDeathEvent(const StateEventInfo& lastKnownInfo, const std::shared_ptr<WindowEventPool>& windowEventPool)
      {
        assert(windowEventPool);
        auto fakeEvent = windowEventPool->AcquireWindowInputClickEvent(lastKnownInfo.SourceId(), lastKnownInfo.SourceSubId(), EventTransactionState::Canceled, false, lastKnownInfo.Param1());
        return Convert(fakeEvent);
      }
    }


    HitBasedInputSender::HitBasedInputSender(const std::shared_ptr<IModuleHost>& moduleHost)
      : m_stateEventSender(moduleHost->CreateStateEventSender(WindowFlags::ClickInput, CreateTargetWindowDeathEvent))
    {
    }


    HitBasedInputSender::~HitBasedInputSender()
    {
    }


    bool HitBasedInputSender::Send(const int32_t senderId, const int32_t senderSubId, const EventTransactionState state, const bool isRepeat, const Vector2& screenPosition)
    {
      assert(m_stateEventSender);
      auto pool = m_stateEventSender->GetEventPool();
      auto theEvent = pool->AcquireWindowInputClickEvent(senderId, senderSubId, state, isRepeat, screenPosition);
      auto result = m_stateEventSender->Send(Convert(theEvent), screenPosition);
      pool->Release(theEvent);
      return result == SendResult::Handled;
    }
  }
}
