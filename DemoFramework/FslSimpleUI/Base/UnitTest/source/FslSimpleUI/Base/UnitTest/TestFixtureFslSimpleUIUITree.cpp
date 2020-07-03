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

#include <FslSimpleUI/Base/UnitTest/TestFixtureFslSimpleUIUITree.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/System/Event/WindowEventQueueEx.hpp>
#include <FslSimpleUI/Base/System/Modules/ModuleCallbackRegistry.hpp>
#include <FslSimpleUI/Base/System/RootWindow.hpp>
#include <FslSimpleUI/Base/System/UITree.hpp>
#include <FslSimpleUI/Base/UIContext.hpp>
#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>


using namespace Fsl;


TestFixtureFslSimpleUIUITree::TestFixtureFslSimpleUIUITree()
  : m_moduleCallbackRegistry(std::make_shared<UI::ModuleCallbackRegistry>())
  , m_eventPool(std::make_shared<UI::WindowEventPool>())
  , m_eventQueue(std::make_shared<UI::WindowEventQueueEx>())
  , m_tree(std::make_shared<UI::UITree>(m_moduleCallbackRegistry, m_eventPool, m_eventQueue))
  , m_windowEventSender(std::make_shared<UI::WindowEventSender>(m_eventQueue, m_eventPool, m_tree))
  , m_uiContext(std::make_shared<UI::UIContext>(m_tree, m_windowEventSender))
  , m_windowContext(std::make_shared<UI::BaseWindowContext>(m_uiContext, SpriteDpConfig::BaseDpi))
  , m_rootWindow(std::make_shared<UI::RootWindow>(m_windowContext, PxExtent2D(800, 600), 160))
{
  m_tree->Init(m_rootWindow);
}


TestFixtureFslSimpleUIUITree::~TestFixtureFslSimpleUIUITree() = default;


void TestFixtureFslSimpleUIUITree::ForceShutdown()
{
  if (m_tree)
  {
    m_tree->Shutdown();
  }

  m_windowContext.reset();
  m_uiContext.reset();
  m_windowEventSender.reset();
  m_tree.reset();
  m_eventQueue.reset();
  m_eventPool.reset();
  m_moduleCallbackRegistry.reset();
}
