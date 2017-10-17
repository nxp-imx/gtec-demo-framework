#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_STATEEVENTSENDER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_STATEEVENTSENDER_HPP
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

#include "IStateEventSender.hpp"
#include "EventRoute.hpp"
#include "StateEventSenderHistory.hpp"
#include "StateEventInfo.hpp"
#include "FunctionCreateTargetWindowDeathEvent.hpp"
#include "../Modules/IModuleCallbackReceiver.hpp"

namespace Fsl
{
  namespace UI
  {

    class EventRouter;
    class IEventHandler;
    class ITreeContextInfo;
    struct WindowFlags;

    //! @brief A state event sender ensures that we always sends a Begin as the first event.
    //! @note  It also ensures that a cancel event is send if the target window dies before the End event is received, then silently consumes
    //         all future events until the real end or cancel is received
    class StateEventSender
      : public IStateEventSender
      , public IModuleCallbackReceiver
    {
      enum class State
      {
        Normal,
        BuildingHistory,
        RemovingNode,
      };

      std::shared_ptr<ITreeContextInfo> m_treeContextInfo;
      std::shared_ptr<EventRouter> m_eventRouter;
      std::shared_ptr<WindowEventPool> m_eventPool;
      std::shared_ptr<IEventHandler> m_eventHandler;
      FunctionCreateTargetWindowDeathEvent m_fnCreateTargetWindowDeathEvent;
      EventRoute m_eventRoute;
      StateEventSenderHistory m_history;
      StateEventInfo m_lastEventInfo;
      State m_state;
    public:
      StateEventSender(const std::shared_ptr<ITreeContextInfo>& treeContextInfo, const std::shared_ptr<EventRouter>& eventRouter,
        const std::shared_ptr<WindowEventPool>& eventPool,
        const std::shared_ptr<IEventHandler>& eventHandler,
        const WindowFlags& windowFlags,
        const FunctionCreateTargetWindowDeathEvent& fnCreateTargetWindowDeathEvent);
      ~StateEventSender();

      virtual const std::shared_ptr<WindowEventPool>& GetEventPool() const override { return m_eventPool; }
      virtual bool HasHistory() const override;
      virtual SendResult Send(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target) override;
      virtual SendResult Send(const StateEvent& theEvent, const Vector2& hitPosition) override;

      // From IModuleCallbackReceiver
      virtual void ModuleOnTreeNodeDispose(const std::shared_ptr<TreeNode>& node) override;
    private:
      SendResult Send(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target, const bool isHitBased, const Vector2& hitPosition);
      SendResult SendViaHistory(const StateEvent& theEvent);
      void BuildEventRoute(const StateEvent& theEvent, const std::shared_ptr<TreeNode> & target, const bool isHitBased, const Vector2& hitPosition);
      SendResult SendToEventRoute(const StateEvent& theEvent);

      class ScopedStateChange
      {
        StateEventSender& m_rObj;
        State m_oldState;

      public:
        ScopedStateChange(StateEventSender& rObj, const State scopedState)
          : m_rObj(rObj)
          , m_oldState(rObj.m_state)
        {
          m_rObj.m_state = scopedState;
        }

        ~ScopedStateChange()
        {
          m_rObj.m_state = m_oldState;
        }
      };
    };
  }
}

#endif
