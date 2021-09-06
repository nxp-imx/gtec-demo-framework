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
#include <FslSimpleUI/Base/System/UITree.hpp>
#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>
#include <FslSimpleUI/Base/UnitTest/TestUITree_Window.hpp>

using namespace Fsl;


TEST_F(TestUITree_Window, Window_Add)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);

  auto callCount = window->GetCallCount();

  CheckZero(callCount, WindowMethod::All);
}

TEST_F(TestUITree_Window, AddThenDispose)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);

  ForceShutdown();

  auto callCount = window->GetCallCount();
  CheckZero(callCount, WindowMethod::All);

  // Release the 'last' reference to the window and check that it was indeed released
  std::weak_ptr<UI::BaseWindowTest> weakWindow = window;
  window.reset();

  // Since the UI tree has been shutdown and we release our own reference to the window the weak pointer should be expired.
  // If its not we have a circular reference somewhere!
  ASSERT_TRUE(weakWindow.expired());
}


TEST_F(TestUITree_Window, Window_Add_WinInit)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::WinInit);
  m_tree->Add(window);

  auto callCount = window->GetCallCount();

  ASSERT_EQ(1u, callCount.WinInit);
  CheckZeroExcept(callCount, WindowMethod::WinInit);
}


TEST_F(TestUITree_Window, Window_Update)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);
  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}


TEST_F(TestUITree_Window, Window_Update_UpdateEnabled)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::UpdateEnabled);
  m_tree->Add(window);
  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.WinUpdate);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::WinUpdate | WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}


// As resolve is disabled by default we dont expect it to be called on a newly added window
TEST_F(TestUITree_Window, Window_Resolve)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);
  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}


// As resolve was enabled for the new window it should be called
TEST_F(TestUITree_Window, Window_Resolve_ResolveEnabled)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::ResolveEnabled);
  m_tree->Add(window);
  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.WinResolve);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::WinResolve | WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}

TEST_F(TestUITree_Window, Window_Update2X_LayoutCacheCheck)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);
  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);
  const auto ignoreFlags = WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride;
  CheckZeroExcept(callCount, ignoreFlags);


  m_tree->Update(TransitionTimeSpan(0, TransitionTimeUnit::Microseconds));

  // Check that the layout was cached
  callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);
  CheckZeroExcept(callCount, ignoreFlags);
}


TEST_F(TestUITree_Window, Window_Draw)
{
  // Update must be called before draw
  const TransitionTimeSpan timeSpan(0, TransitionTimeUnit::Microseconds);

  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);
  m_tree->Update(timeSpan);
  m_tree->Draw(this->m_buffer);

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  // As draw isn't enabled by default we expect zero
  ASSERT_EQ(0u, callCount.WinDraw);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}


TEST_F(TestUITree_Window, Window_Draw_DrawEnabled)
{
  // Update must be called before draw
  const TransitionTimeSpan timeSpan(0, TransitionTimeUnit::Microseconds);

  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::DrawEnabled);
  m_tree->Add(window);
  m_tree->Update(timeSpan);
  m_tree->Draw(this->m_buffer);

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  // We enabled the draw flag so we should get a draw call
  ASSERT_EQ(1u, callCount.WinDraw);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, WindowMethod::WinDraw | WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride);
}


TEST_F(TestUITree_Window, Window_Draw2x_DrawEnabled)
{
  // Update must be called before draw
  const TransitionTimeSpan timeSpan(0, TransitionTimeUnit::Microseconds);

  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::DrawEnabled);
  m_tree->Add(window);
  m_tree->Update(timeSpan);
  m_tree->Draw(this->m_buffer);

  auto callCount = window->GetCallCount();

  // ASSERT_EQ(1u, callCount.WinGetContentRect);
  // We enabled the draw flag so we should get a draw call
  ASSERT_EQ(1u, callCount.WinDraw);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  const auto ignoreFlags = WindowMethod::WinDraw | WindowMethod::ArrangeOverride | WindowMethod::MeasureOverride;
  CheckZeroExcept(callCount, ignoreFlags);

  m_tree->Draw(this->m_buffer);

  callCount = window->GetCallCount();

  ASSERT_EQ(2u, callCount.WinDraw);
  ASSERT_EQ(1u, callCount.ArrangeOverride);
  ASSERT_EQ(1u, callCount.MeasureOverride);

  CheckZeroExcept(callCount, ignoreFlags);
}
