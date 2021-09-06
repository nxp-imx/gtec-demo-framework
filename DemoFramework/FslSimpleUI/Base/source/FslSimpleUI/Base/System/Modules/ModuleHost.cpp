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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <utility>
#include "../Event/EventRouter.hpp"
#include "../Event/StateEventSender.hpp"
#include "../Event/SimpleEventSender.hpp"
#include "ModuleCallbackRegistry.hpp"

namespace Fsl
{
  namespace UI
  {
    ModuleHost::ModuleHost(std::shared_ptr<ModuleCallbackRegistry> moduleCallbackRegistry, std::shared_ptr<ITreeContextInfo> treeContextInfo,
                           const std::shared_ptr<TreeNode>& rootNode, const std::shared_ptr<ITreeNodeClickInputTargetLocater>& clickTargetLocater,
                           std::shared_ptr<ITreeNodeBasicInfo> basicInfo, std::shared_ptr<IEventHandler> eventHandler,
                           std::shared_ptr<WindowEventPool> windowEventPool, const std::shared_ptr<WindowEventSender>& eventSender,
                           const std::shared_ptr<SimpleEventSender>& simpleEventSender)
      : m_moduleCallbackRegistry(std::move(moduleCallbackRegistry))
      , m_treeContextInfo(std::move(treeContextInfo))
      , m_windowEventPool(std::move(windowEventPool))
      , m_targetLocater(clickTargetLocater)
      , m_basicInfo(std::move(basicInfo))
      , m_eventRouter(new EventRouter(rootNode, clickTargetLocater))
      , m_eventHandler(std::move(eventHandler))
      , m_eventSender(eventSender)
      , m_simpleEventSender(simpleEventSender)
    {
      if (!m_moduleCallbackRegistry || !m_treeContextInfo || !m_windowEventPool || !m_eventRouter || !m_eventHandler || !eventSender ||
          !simpleEventSender)
      {
        throw std::invalid_argument("treeContextInfo can not be null");
      }
    }


    ModuleHost::~ModuleHost() = default;


    std::shared_ptr<ITreeNodeClickInputTargetLocater> ModuleHost::GetTargetLocater() const
    {
      return m_targetLocater;
    }


    std::shared_ptr<ITreeNodeBasicInfo> ModuleHost::GetBasicInfo() const
    {
      return m_basicInfo;
    }

    std::shared_ptr<WindowEventPool> ModuleHost::GetWindowEventPool() const
    {
      return m_windowEventPool;
    }


    std::shared_ptr<WindowEventSender> ModuleHost::GetWindowEventSender() const
    {
      return m_eventSender;
    }


    std::shared_ptr<SimpleEventSender> ModuleHost::GetSimpleEventSender() const
    {
      return m_simpleEventSender;
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
