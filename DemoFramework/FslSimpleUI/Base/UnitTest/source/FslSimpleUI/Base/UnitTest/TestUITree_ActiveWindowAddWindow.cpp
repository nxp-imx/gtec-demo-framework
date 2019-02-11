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
#include <FslSimpleUI/Base/System/UITree.hpp>
#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>
#include <FslSimpleUI/Base/UnitTest/TestUITree_ActiveWindow.hpp>

using namespace Fsl;


TEST_F(TestUITree_ActiveWindow, Add)
{
  auto callIdManager = std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve);
  auto startCallId = callIdManager->GetCurrentValue();

  ASSERT_EQ(1u, m_tree->GetNodeCount());
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  window->SetCallIdManager(callIdManager);
  m_tree->AddChild(m_mainWindow, window);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  // The everything except WinInit does not called before the next update call
  auto callCount = window->GetCallCount();
  ASSERT_EQ(1u, callCount.WinInit);
  auto callId = window->GetCallId();
  ASSERT_EQ(startCallId + 1, callId.WinInit);
  CheckZeroExcept(callCount, WindowMethod::WinInit);

  const DemoTime demoTime(0, 0);
  ASSERT_EQ(2u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCount = window->GetCallCount();
  ASSERT_EQ(1u, callCount.WinInit);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.WinResolve);

  CheckZeroExcept(callCount, WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve);

  // Check that the methods got called in the expected order
  auto callIdNewWindow = window->GetCallId();
  ASSERT_EQ(startCallId + 1u, callIdNewWindow.WinInit);
  ASSERT_EQ(startCallId + 2u, callIdNewWindow.WinUpdate);
  ASSERT_EQ(startCallId + 3u, callIdNewWindow.WinResolve);
}

TEST_F(TestUITree_ActiveWindow, Add_NoFlags)
{
  auto callIdManager = std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve);
  // auto startCallId = callIdManager->GetCurrentValue();

  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  window->SetCallIdManager(callIdManager);

  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->AddChild(m_mainWindow, window);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  // The window does not called before the next update call
  auto callCount = window->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  const DemoTime demoTime(0, 0);
  ASSERT_EQ(2u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  CheckZero(callCount, WindowMethod::All);

  // Check that the methods got called in the expected order
  auto callIdNewWindow = window->GetCallId();
  ASSERT_EQ(0u, callIdNewWindow.WinInit);
  ASSERT_EQ(0u, callIdNewWindow.WinUpdate);
  ASSERT_EQ(0u, callIdNewWindow.WinResolve);
}

// Add a child window during the main windows update using the window manager directly.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);

  auto onUpdate = [newWindow, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow); };

  m_mainWindow->Callbacks.HookWinUpdate = onUpdate;

  newWindow->SetCallIdManager(callIdManager);

  auto callCount = newWindow->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1u, callCount.WinInit);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.WinResolve);

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  CheckZeroExcept(callCount, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1u, callCount.WinInit);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.WinResolve);
  ASSERT_EQ(1u, callCount.WinDraw);

  ignoreFlags |= WindowMethod::WinDraw;

  CheckZeroExcept(callCount, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow = newWindow->GetCallId();
  ASSERT_EQ(startCallId + 1u, callIdNewWindow.WinInit);
  ASSERT_EQ(startCallId + 2u, callIdNewWindow.WinUpdate);
  ASSERT_EQ(startCallId + 3u, callIdNewWindow.WinResolve);
  ASSERT_EQ(startCallId + 4u, callIdNewWindow.WinDraw);
}


// Add a child window during the main windows update using the window manager directly.
// The child window also adds a new window during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };

  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };

  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;

  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);


  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(3u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  ASSERT_EQ(startCallId + 1u, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2u, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3u, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4u, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5u, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 6u, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7u, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 8u, callIdNewWindow2.WinDraw);
}


// Add a child window during the main windows update using the window manager directly.
// The child window also adds a new window during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };

  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };

  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;

  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its resolve it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(3u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  ASSERT_EQ(startCallId + 1u, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2u, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3u, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4u, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5u, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6u, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7u, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 8u, callIdNewWindow2.WinDraw);
}

// Add a child window during the main windows Resolve using the window manager directly.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);

  auto onResolve = [newWindow, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow); };

  m_mainWindow->Callbacks.HookWinResolve = onResolve;

  newWindow->SetCallIdManager(callIdManager);

  auto callCount = newWindow->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(2u, m_tree->GetNodeCount());

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1u, callCount.WinInit);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.WinResolve);

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  CheckZeroExcept(callCount, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1u, callCount.WinInit);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.WinResolve);
  ASSERT_EQ(1u, callCount.WinDraw);

  ignoreFlags |= WindowMethod::WinDraw;

  CheckZeroExcept(callCount, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow1.WinDraw);
}


// Add a child window during the main windows update using the window manager directly.
// The child window also adds a new window during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };

  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };

  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);

  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);


  // Update the tree which adds the new window during the resolve call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(3u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinDraw);
}


// Add a child window during the main windows update using the window manager directly.
// The child window also adds a new window during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };

  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);

  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the resolve call using the above callback and
  // as the new window adds newWindow2 during its resolve it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(3u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinDraw);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Three child windows
// 000
// 001
// 010
// 011
// 100
// 101
// 110
// 111
// ---------------------------------------------------------------------------------------------------------------------------------------------------


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_UpdateAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnUpdate = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;
  newWindow2->Callbacks.HookWinUpdate = newWindow2OnUpdate;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 6, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 7, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Resolve'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_UpdateAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnResolve = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };
  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;
  newWindow2->Callbacks.HookWinResolve = newWindow2OnResolve;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 8, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_ResolveAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnUpdate = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;
  newWindow2->Callbacks.HookWinUpdate = newWindow2OnUpdate;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Resolve'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainUpdateAddChild_ResolveAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnUpdate = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnResolve = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinUpdate = mainWindowOnUpdate;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;
  newWindow2->Callbacks.HookWinResolve = newWindow2OnResolve;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 8, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_UpdateAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnUpdate = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;
  newWindow2->Callbacks.HookWinUpdate = newWindow2OnUpdate;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 6, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 7, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Resolve'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_UpdateAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnUpdate = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnResolve = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };
  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinUpdate = newWindowOnUpdate;
  newWindow2->Callbacks.HookWinResolve = newWindow2OnResolve;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 5, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 8, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Update'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainesolveAddChild_ResolveAddChild_UpdateAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnUpdate = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;
  newWindow2->Callbacks.HookWinUpdate = newWindow2OnUpdate;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 8, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}


// Add a child window1 during the main windows update using the window manager directly.
// The child window1 also adds a new window2 during its 'Update'.
// The child window2 also adds a new window3 during its 'Resolve'.
// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveWindow, MainResolveAddChild_ResolveAddChild_ResolveAddChild_NoLayout)
{
  auto callIdManager =
    std::make_shared<WindowCallIdManager>(WindowMethod::WinInit | WindowMethod::WinUpdate | WindowMethod::WinResolve | WindowMethod::WinDraw);
  auto startCallId = callIdManager->GetCurrentValue();

  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow1 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow2 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  auto newWindow3 = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);
  newWindow1->SetCallIdManager(callIdManager);
  newWindow2->SetCallIdManager(callIdManager);
  newWindow3->SetCallIdManager(callIdManager);

  auto mainWindowOnResolve = [newWindow1, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow1); };
  auto newWindowOnResolve = [newWindow2, newWindow1, tree](const DemoTime& demoTime) { tree->AddChild(newWindow1, newWindow2); };
  auto newWindow2OnResolve = [newWindow3, newWindow2, tree](const DemoTime& demoTime) { tree->AddChild(newWindow2, newWindow3); };

  m_mainWindow->Callbacks.HookWinResolve = mainWindowOnResolve;
  newWindow1->Callbacks.HookWinResolve = newWindowOnResolve;
  newWindow2->Callbacks.HookWinResolve = newWindow2OnResolve;

  auto callCountNewWindow = newWindow1->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow2->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);
  callCountNewWindow = newWindow3->GetCallCount();
  CheckZero(callCountNewWindow, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback and
  // as the new window adds newWindow2 during its update it should also be called
  const DemoTime demoTime(0, 0);
  ASSERT_EQ(1u, m_tree->GetNodeCount());
  m_tree->Update(demoTime);
  ASSERT_EQ(4u, m_tree->GetNodeCount());

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCountNewWindow = newWindow1->GetCallCount();

  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();

  ignoreFlags |= WindowMethod::WinDraw;

  callCountNewWindow = newWindow1->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow2->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  callCountNewWindow = newWindow3->GetCallCount();
  ASSERT_EQ(1u, callCountNewWindow.WinInit);
  ASSERT_EQ(1u, callCountNewWindow.WinUpdate);
  ASSERT_EQ(1u, callCountNewWindow.WinResolve);
  ASSERT_EQ(1u, callCountNewWindow.WinDraw);
  CheckZeroExcept(callCountNewWindow, ignoreFlags);

  // Check that the methods got called in the expected order
  auto callIdNewWindow1 = newWindow1->GetCallId();
  auto callIdNewWindow2 = newWindow2->GetCallId();
  auto callIdNewWindow3 = newWindow3->GetCallId();
  ASSERT_EQ(startCallId + 1, callIdNewWindow1.WinInit);
  ASSERT_EQ(startCallId + 2, callIdNewWindow1.WinUpdate);
  ASSERT_EQ(startCallId + 3, callIdNewWindow1.WinResolve);
  ASSERT_EQ(startCallId + 4, callIdNewWindow2.WinInit);
  ASSERT_EQ(startCallId + 5, callIdNewWindow2.WinUpdate);
  ASSERT_EQ(startCallId + 6, callIdNewWindow2.WinResolve);
  ASSERT_EQ(startCallId + 7, callIdNewWindow3.WinInit);
  ASSERT_EQ(startCallId + 8, callIdNewWindow3.WinUpdate);
  ASSERT_EQ(startCallId + 9, callIdNewWindow3.WinResolve);
  ASSERT_EQ(startCallId + 10, callIdNewWindow1.WinDraw);
  ASSERT_EQ(startCallId + 11, callIdNewWindow2.WinDraw);
  ASSERT_EQ(startCallId + 12, callIdNewWindow3.WinDraw);
}
