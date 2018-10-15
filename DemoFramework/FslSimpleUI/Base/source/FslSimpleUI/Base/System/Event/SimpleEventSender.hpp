#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_SIMPLEEVENTSENDER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_SIMPLEEVENTSENDER_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslSimpleUI/Base/Event/EventRoutingStrategy.hpp>
#include "EventRouter.hpp"
#include "EventRoute.hpp"

namespace Fsl
{
  namespace UI
  {
    class ITreeContextInfo;
    class ITreeNodeClickInputTargetLocator;
    class TreeNode;
    class WindowEvent;
    class WindowEventPool;

    class SimpleEventSender : private Noncopyable
    {
      std::shared_ptr<ITreeContextInfo> m_treeContextInfo;
      std::shared_ptr<WindowEventPool> m_windowEventPool;
      std::shared_ptr<IEventHandler> m_eventHandler;
      EventRoute m_eventRoute;
      EventRouter m_eventRouter;

    public:
      SimpleEventSender(const std::shared_ptr<ITreeContextInfo>& treeContextInfo, const std::shared_ptr<WindowEventPool>& windowEventPool,
                        const std::shared_ptr<IEventHandler>& eventHandler, const std::shared_ptr<TreeNode>& rootNode,
                        const std::shared_ptr<ITreeNodeClickInputTargetLocator>& clickTargetLocator);
      ~SimpleEventSender();

      //! @brief Send a event using the given routing strategy to the window at the given hitPosition.
      //! @param theEvent the event to send.
      //! @param target the intended target of the event.
      //! @param manageEvent if true the event will be returned to the pool on completion of this call.
      //! @warning Do not send TransactionEvents via this method!! Use CreateRoute instead and use the route for the entire transaction.
      void Send(const std::shared_ptr<WindowEvent>& theEvent, const std::shared_ptr<TreeNode>& target, const bool manageEvent);

      //! @brief Send a event using the given routing strategy to the window at the given hitPosition.
      //! @param theEvent the event to send.
      //! @param screenHitPosition the area on screen that was targeted.
      //! @param manageEvent if true the event will be returned to the pool on completion of this call.
      //! @warning Do not send TransactionEvents via this method!! Use CreateRoute instead and use the route for the entire transaction.
      void Send(const std::shared_ptr<WindowEvent>& theEvent, const Vector2& screenHitPosition, const bool manageEvent);
    };
  }
}

#endif
