#ifndef FSLSIMPLEUI_BASE_SYSTEM_MODULES_MODULEHOST_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_MODULES_MODULEHOST_HPP
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

#include "IModuleHost.hpp"

namespace Fsl::UI
{
  class EventRouter;
  class IEventHandler;
  class ITreeContextInfo;
  class ITreeNodeClickInputTargetLocater;
  class ITreeNodeBasicInfo;
  class ModuleCallbackRegistry;
  class SimpleEventSender;
  class TreeNode;
  class WindowEventPool;

  class ModuleHost final : public IModuleHost
  {
    std::shared_ptr<ModuleCallbackRegistry> m_moduleCallbackRegistry;
    std::shared_ptr<ITreeContextInfo> m_treeContextInfo;
    std::shared_ptr<WindowEventPool> m_windowEventPool;
    std::shared_ptr<ITreeNodeClickInputTargetLocater> m_targetLocater;
    std::shared_ptr<ITreeNodeBasicInfo> m_basicInfo;
    std::shared_ptr<EventRouter> m_eventRouter;
    std::shared_ptr<IEventHandler> m_eventHandler;
    std::shared_ptr<WindowEventSender> m_eventSender;
    std::shared_ptr<SimpleEventSender> m_simpleEventSender;

  public:
    ModuleHost(std::shared_ptr<ModuleCallbackRegistry> moduleCallbackRegistry, std::shared_ptr<ITreeContextInfo> treeContextInfo,
               const std::shared_ptr<TreeNode>& rootNode, const std::shared_ptr<ITreeNodeClickInputTargetLocater>& clickTargetLocater,
               std::shared_ptr<ITreeNodeBasicInfo> basicInfo, std::shared_ptr<IEventHandler> eventHandler,
               std::shared_ptr<WindowEventPool> windowEventPool, const std::shared_ptr<WindowEventSender>& eventSender,
               const std::shared_ptr<SimpleEventSender>& simpleEventSender);
    ~ModuleHost() override;
    //! FromIModuleHost
    std::shared_ptr<WindowEventPool> GetWindowEventPool() const final;
    std::shared_ptr<ITreeNodeClickInputTargetLocater> GetTargetLocater() const final;
    std::shared_ptr<ITreeNodeBasicInfo> GetBasicInfo() const final;
    std::shared_ptr<WindowEventSender> GetWindowEventSender() const final;
    std::shared_ptr<SimpleEventSender> GetSimpleEventSender() const final;

    std::shared_ptr<IStateEventSender> CreateStateEventSender(const WindowFlags::Enum inputType,
                                                              const FunctionCreateTargetWindowDeathEvent& fnCreateTargetWindowDeathEvent) final;
  };
}

#endif
