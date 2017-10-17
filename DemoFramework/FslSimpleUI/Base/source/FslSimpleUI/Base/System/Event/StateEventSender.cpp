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

#include "StateEventSender.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Log/Log.hpp>
#include "EventRouter.hpp"
#include "StateEvent.hpp"
#include "../ITreeContextInfo.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {

    StateEventSender::StateEventSender(const std::shared_ptr<ITreeContextInfo>& treeContextInfo,
      const std::shared_ptr<EventRouter>& eventRouter,
      const std::shared_ptr<WindowEventPool>& eventPool,
      const std::shared_ptr<IEventHandler>& eventHandler,
      const WindowFlags& windowFlags,
      const FunctionCreateTargetWindowDeathEvent& fnCreateTargetWindowDeathEvent)
      : m_treeContextInfo(treeContextInfo)
      , m_eventRouter(eventRouter)
      , m_eventPool(eventPool)
      , m_eventHandler(eventHandler)
      , m_fnCreateTargetWindowDeathEvent(fnCreateTargetWindowDeathEvent)
      , m_eventRoute(windowFlags)
      , m_history()
      , m_lastEventInfo()
      , m_state(State::Normal)
    {
      if (!m_treeContextInfo)
        throw std::invalid_argument("treeContextInfo can not be null");
      if (!m_eventRouter)
        throw std::invalid_argument("eventRouter can not be null");
      if (!m_eventPool)
        throw std::invalid_argument("eventPool can not be null");
      if (!m_eventHandler)
        throw std::invalid_argument("eventHandler can not be null");
      if (!m_fnCreateTargetWindowDeathEvent)
        throw std::invalid_argument("fnCreateTargetWindowDeathEvent can not be null");
    }


    StateEventSender::~StateEventSender()
    {
    }


    bool StateEventSender::HasHistory() const
    {
      return m_history.UseHistory();
    }


    SendResult StateEventSender::Send(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target)
    {
      return Send(theEvent, target, false, Vector2());
    }


    SendResult StateEventSender::Send(const StateEvent& theEvent, const Vector2& hitPosition)
    {
      return Send(theEvent, std::shared_ptr<TreeNode>(), true, hitPosition);
    }


    void StateEventSender::ModuleOnTreeNodeDispose(const std::shared_ptr<TreeNode>& node)
    {
      assert(m_state == State::Normal);

      if (m_history.UseHistory() && !m_eventRoute.IsEmpty())
      {
        const auto routeTarget = m_eventRoute.GetTarget();
        m_eventRoute.RemoveNode(node);
        // If the node that is being removed is the target of the event history
        // then create a fake end event and send it right away and we then keep
        // capturing and ignoring events until the real end event arrive.
        if (routeTarget == node)
        {
          StateEvent lastEvent;
          { // Callback to get a custom event for 'target window death'
            ScopedStateChange scopedChange(*this, State::RemovingNode);
            lastEvent = m_fnCreateTargetWindowDeathEvent(m_lastEventInfo, m_eventPool);
            FSLLOG_WARNING_IF(!lastEvent.Info().IsCancel(), "The WindowDeathEvent is expected to be a cancel event");
          }

          if (lastEvent.Content())
          {
            try
            {
              // We lock the history so that it wont get freed by the fake 'up'
              // This ensures that the StateEvent captures the rest of the real event series.
              m_history.Lock();
              SendViaHistory(lastEvent);
              m_history.Unlock();
              m_history.MarkReceiverAsDead();
            }
            catch (...)
            {
              m_history.Unlock();
              m_history.MarkReceiverAsDead();
            }
          }
        }
      }
    }


    SendResult StateEventSender::Send(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target, const bool isHitBased, const Vector2& hitPosition)
    {
      // This check ensures that we don't end up in some weird re-entrancy situations
      if (!m_treeContextInfo->IsInSystemContext())
        throw UsageErrorException("Method called from a invalid context.");

      assert(m_state == State::Normal);
      // Validate the event according to the received input.
      if ((theEvent.Info().IsBegin() && !theEvent.Info().IsRepeat()) && m_history.UseHistory())
      {
        FSLLOG_ERROR("Received double begin that isn't a repeat, ignoring it.");
        return SendResult::Error;
      }
      if (!m_history.UseHistory() && ((theEvent.Info().IsBegin() && theEvent.Info().IsRepeat()) || theEvent.Info().IsCancel() || theEvent.Info().IsEnd()))
      {
        FSLLOG_ERROR_IF(theEvent.Info().IsBegin(), "Received begin repeat without a initial press, ignoring it.");
        FSLLOG_ERROR_IF(theEvent.Info().IsCancel(), "Received cancel without a initial press, ignoring it.");
        FSLLOG_ERROR_IF(theEvent.Info().IsEnd(), "Received end without a initial press, ignoring it.");
        return SendResult::Error;
      }

      // This warning indicates that we have multiple sources (multi touch/keyboard) which currently is unsupported by our routing system.
      FSLLOG_WARNING_IF(m_history.UseHistory() && m_history.LastSourceId() != theEvent.Info().SourceId(), "Received a related input event from a different source than the initial one (multi touch/keyboard is unsupported).");

      SendResult sendResult = SendResult::Error;

      // If we don't have a history -> locate the window that is being hit
      // else if the receiver isn't dead send it through the windows in exactly the same order.
      // else ignore the event (since a fake up event has been sent).
      m_lastEventInfo = theEvent.Info();
      if (!m_history.UseHistory())
      {
        BuildEventRoute(theEvent, target, isHitBased, hitPosition);
        assert(HasHistory());
      }

      if (!m_history.IsReceiverDead())
        sendResult = SendToEventRoute(theEvent);

      // Clear the history on release
      if ((theEvent.Info().IsEnd() || theEvent.Info().IsCancel()) && !m_history.IsLocked())
      {
        m_eventRoute.Clear();
        m_history.End();
        m_lastEventInfo.Clear();
      }
      return sendResult;
    }



    SendResult StateEventSender::SendViaHistory(const StateEvent& theEvent)
    {
      assert(m_history.UseHistory());
      return Send(theEvent, std::shared_ptr<TreeNode>(), false, Vector2());
    }


    void StateEventSender::BuildEventRoute(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target, const bool isHitBased, const Vector2& hitPosition)
    {
      // Since we verified the event according to our history we can assume that if the history is invalid
      // this must be a pressed event which we need to create a history record for
      assert(theEvent.Info().IsBegin());
      assert(!theEvent.Info().IsRepeat());

      // start a new history
      ScopedStateChange scopedChange(*this, State::BuildingHistory);
      m_history.Begin(theEvent.Info().SourceId(), theEvent.Info().SourceSubId());

      if (isHitBased)
        m_eventRouter->CreateRoute(m_eventRoute, theEvent.Content()->GetDescription().RoutingStrategy, hitPosition);
      else
        m_eventRouter->CreateRoute(m_eventRoute, theEvent.Content()->GetDescription().RoutingStrategy, target);
    }


    SendResult StateEventSender::SendToEventRoute(const StateEvent& theEvent)
    {
      assert(m_history.UseHistory());

      m_eventRoute.Send(m_eventHandler.get(), theEvent.Content());

      // FIX: Let our owner know that we completed sending a event
      // m_creator.OnSendCompleted(this, theEvent);

      return (theEvent.Content()->IsHandled() ? SendResult::Handled : SendResult::Unhandled);
    }
  }
}
