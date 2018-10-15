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

#include "ModuleHost.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include "../Event/EventRouter.hpp"
#include "../Event/StateEventSender.hpp"
#include "ModuleCallbackRegistry.hpp"

namespace Fsl
{
  namespace UI
  {
    ModuleHost::ModuleHost(const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry,
                           const std::shared_ptr<ITreeContextInfo>& treeContextInfo, const std::shared_ptr<TreeNode>& rootNode,
                           const std::shared_ptr<ITreeNodeClickInputTargetLocator>& clickTargetLocator,
                           const std::shared_ptr<IEventHandler>& eventHandler, const std::shared_ptr<WindowEventPool>& windowEventPool)
      : m_moduleCallbackRegistry(moduleCallbackRegistry)
      , m_treeContextInfo(treeContextInfo)
      , m_windowEventPool(windowEventPool)
      , m_eventRouter(new EventRouter(rootNode, clickTargetLocator))
      , m_eventHandler(eventHandler)
    {
      if (!m_moduleCallbackRegistry && !m_treeContextInfo && !m_windowEventPool && !m_eventRouter && !m_eventHandler)
      {
        throw std::invalid_argument("treeContextInfo can not be null");
      }
    }


    ModuleHost::~ModuleHost() = default;


    std::shared_ptr<WindowEventPool> ModuleHost::GetWindowEventPool() const
    {
      return m_windowEventPool;
    }


    std::shared_ptr<IStateEventSender> ModuleHost::CreateStateEventSender(const WindowFlags::Enum inputType,
                                                                          const FunctionCreateTargetWindowDeathEvent& fnCreateTargetWindowDeathEvent)
    {
      auto sender = std::make_shared<StateEventSender>(m_treeContextInfo, m_eventRouter, m_windowEventPool, m_eventHandler, inputType,
                                                       fnCreateTargetWindowDeathEvent);
      m_moduleCallbackRegistry->AddCallbackReceiver(sender);
      return sender;
    }
  }
}
