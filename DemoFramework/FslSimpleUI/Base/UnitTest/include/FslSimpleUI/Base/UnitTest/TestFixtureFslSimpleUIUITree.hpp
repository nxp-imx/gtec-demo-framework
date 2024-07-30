#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_TESTFIXTUREFSLSIMPLEUIUITREE_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_TESTFIXTUREFSLSIMPLEUIUITREE_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslSimpleUI/Base/Render/UIRenderSystem.hpp>
#include <FslSimpleUI/Base/System/RootWindow.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBufferEx.hpp>


namespace Fsl
{
  namespace DataBinding
  {
    class DataBindingService;
  }
  namespace UI
  {
    class BaseWindowContext;
    class ModuleCallbackRegistry;
    class RenderSystem;
    class RootWindow;
    class WindowEventPool;
    class WindowEventQueueEx;
    class WindowEventSender;
    class UIContext;
    class UITree;
  }
}

class TestFixtureFslSimpleUIUITree : public TestFixtureFslBase
{
protected:
  std::shared_ptr<Fsl::DataBinding::DataBindingService> m_dataBindingService;
  std::shared_ptr<Fsl::UI::ModuleCallbackRegistry> m_moduleCallbackRegistry;
  Fsl::UI::UIRenderSystem m_renderSystem;
  std::shared_ptr<Fsl::UI::WindowEventPool> m_eventPool;
  std::shared_ptr<Fsl::UI::WindowEventQueueEx> m_eventQueue;
  std::shared_ptr<Fsl::UI::UITree> m_tree;
  std::shared_ptr<Fsl::UI::WindowEventSender> m_windowEventSender;
  std::shared_ptr<Fsl::UI::UIContext> m_uiContext;
  std::shared_ptr<Fsl::UI::BaseWindowContext> m_windowContext;
  std::shared_ptr<Fsl::UI::RootWindow> m_rootWindow;
  Fsl::UI::DrawCommandBufferEx m_buffer;

public:
  TestFixtureFslSimpleUIUITree();
  ~TestFixtureFslSimpleUIUITree() override;

  void ForceShutdown();
};

#endif
