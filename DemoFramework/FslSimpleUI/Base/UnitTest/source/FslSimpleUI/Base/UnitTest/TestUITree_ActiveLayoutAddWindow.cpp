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
#include <FslSimpleUI/Base/UnitTest/TestUITree_ActiveLayout.hpp>

using namespace Fsl;


TEST_F(TestUITree_ActiveLayout, Add)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->AddChild(m_mainWindow, window);

  auto callCount = window->GetCallCount();

  CheckZero(callCount, WindowMethod::All);
}


// When adding a window to a existing window without participating in its layout the layout functions will not be called
TEST_F(TestUITree_ActiveLayout, UpdateAddChild_NoLayout)
{
  auto mainWindow = m_mainWindow;
  auto tree = m_tree;
  auto newWindow = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);

  auto onUpdate = [newWindow, mainWindow, tree](const DemoTime& demoTime) { tree->AddChild(mainWindow, newWindow); };

  m_mainWindow->Callbacks.HookWinUpdate = onUpdate;

  auto callCount = newWindow->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  // Update the tree which adds the new window during the update call using the above callback
  const DemoTime demoTime(0, 0);
  m_tree->Update(demoTime);

  // Since the child was added directly on the window manager this returns zero
  ASSERT_EQ(0, m_mainWindow->GetChildCount());

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1, callCount.WinInit);
  ASSERT_EQ(1, callCount.WinUpdate);
  ASSERT_EQ(1, callCount.WinResolve);

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve;

  CheckZeroExcept(callCount, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1, callCount.WinInit);
  ASSERT_EQ(1, callCount.WinUpdate);
  ASSERT_EQ(1, callCount.WinResolve);
  ASSERT_EQ(1, callCount.WinDraw);

  ignoreFlags |= WindowMethod::WinDraw;
  CheckZeroExcept(callCount, ignoreFlags);
}

// When adding a window to a existing layout and participating in its layout the layout functions will be called
TEST_F(TestUITree_ActiveLayout, Window_UpdateAddChild)
{
  auto mainWindow = m_mainWindow;
  auto newWindow = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::Enum::All);

  m_mainWindow->Callbacks.HookWinUpdate = [newWindow, mainWindow](const DemoTime& demoTime) { mainWindow->AddChild(newWindow); };

  auto callCount = newWindow->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  ASSERT_EQ(1, m_tree->GetNodeCount());

  // Update the tree which adds the new window during the update call using the above callback
  const DemoTime demoTime(0, 0);
  m_tree->Update(demoTime);

  ASSERT_EQ(1, m_mainWindow->GetChildCount());

  // Now verify that both WinInit and WinUpdate has been called (this ensures that the window appears the same frame it was added)
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1, callCount.WinInit);
  ASSERT_EQ(1, callCount.WinUpdate);
  ASSERT_EQ(1, callCount.WinResolve);
  ASSERT_EQ(1, callCount.ArrangeOverride);
  ASSERT_EQ(1, callCount.MeasureOverride);

  auto ignoreFlags = WindowMethod::WinInit | WindowMethod::WinGetContentRect | WindowMethod::WinUpdate | WindowMethod::WinResolve |
                     WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride;
  CheckZeroExcept(callCount, ignoreFlags);

  // When draw is called the new window should also be drawn
  m_tree->Draw();
  callCount = newWindow->GetCallCount();

  ASSERT_EQ(1, callCount.WinInit);
  ASSERT_EQ(1, callCount.WinUpdate);
  ASSERT_EQ(1, callCount.WinResolve);
  ASSERT_EQ(1, callCount.WinDraw);
  ASSERT_EQ(1, callCount.ArrangeOverride);
  ASSERT_EQ(1, callCount.MeasureOverride);

  ignoreFlags |= WindowMethod::WinDraw;

  CheckZeroExcept(callCount, ignoreFlags);
}
