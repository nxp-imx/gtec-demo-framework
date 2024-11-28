/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslSimpleUI/Base/Event/EventTransactionState.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/System/Event/EventRoute.hpp>
#include <FslSimpleUI/Base/System/UITree.hpp>
#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>
#include <FslSimpleUI/Base/UnitTest/TestFixtureFslSimpleUIUITree.hpp>
#include <FslSimpleUI/Base/UnitTest/UITreeEventHandlerTest.hpp>
#include <memory>

using namespace Fsl;

namespace
{
  class TestEventRouteClickInput : public TestFixtureFslSimpleUIUITree
  {
  protected:
    MillisecondTickCount32 m_tickCounter;
    std::shared_ptr<UI::RootWindow> m_inputWindow1;
    std::shared_ptr<UI::BaseWindowTest> m_inputWindow2;
    UI::UITreeEventHandlerTest m_eventHandler;
    UI::EventRoute m_eventRoute;
    std::shared_ptr<UI::TreeNode> m_nodeWindow1;
    std::shared_ptr<UI::TreeNode> m_nodeWindow2;

  public:
    TestEventRouteClickInput()
      : m_inputWindow1(m_rootWindow)
      , m_inputWindow2(std::make_shared<UI::BaseWindowTest>(m_windowContext, UI::WindowFlags::ClickInput))
      , m_eventHandler()
      , m_eventRoute(UI::WindowFlags::ClickInput)
    {
      m_tree->AddChild(m_inputWindow1, m_inputWindow2);

      m_nodeWindow1 = m_tree->TryGet(m_inputWindow1);
      m_nodeWindow2 = m_tree->TryGet(m_inputWindow2);
    }

    std::shared_ptr<UI::WindowInputClickEvent> CreateInputClickEvent(const UI::EventTransactionState state = UI::EventTransactionState::Begin,
                                                                     const bool isRepeat = false, const PxPoint2 screenPositionPx = {})
    {
      return m_eventPool->AcquireWindowInputClickEvent(m_tickCounter, 0, 0, state, isRepeat, screenPositionPx);
    }
  };
}

TEST_F(TestEventRouteClickInput, Construct)
{
}

TEST_F(TestEventRouteClickInput, IsEmpty_true)
{
  EXPECT_TRUE(m_eventRoute.IsEmpty());
}

TEST_F(TestEventRouteClickInput, Target_empty)
{
  EXPECT_FALSE(m_eventRoute.GetTarget());
}

TEST_F(TestEventRouteClickInput, GetWindowCount_Empty)
{
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 0u);
}

TEST_F(TestEventRouteClickInput, Clear_Empty)
{
  EXPECT_TRUE(m_eventRoute.IsEmpty());
  m_eventRoute.Clear();
  EXPECT_TRUE(m_eventRoute.IsEmpty());
}

TEST_F(TestEventRouteClickInput, Clear)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  m_eventRoute.Clear();
  EXPECT_TRUE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 0u);
  EXPECT_FALSE(m_eventRoute.GetTarget());
}

TEST_F(TestEventRouteClickInput, SetTarget_NullWindow)
{
  m_eventRoute.SetTarget({}, UI::EventRoutingStrategy::Direct);
  EXPECT_TRUE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 0u);
}

TEST_F(TestEventRouteClickInput, SetTarget_NotClick)
{
  auto window = std::make_shared<UI::BaseWindowTest>(m_windowContext);
  m_tree->Add(window);
  auto node = m_tree->TryGet(window);

  m_eventRoute.SetTarget(node, UI::EventRoutingStrategy::Direct);

  // Since the window isn't a click target the event route will be considered empty
  EXPECT_TRUE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetTarget(), node);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 0u);
}

TEST_F(TestEventRouteClickInput, SetTarget_OneWindow_Direct)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Direct);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
}

TEST_F(TestEventRouteClickInput, SetTarget_TwoWindows_Direct)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Direct);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
}

TEST_F(TestEventRouteClickInput, SetTarget_OneWindow_Bubble)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
}

TEST_F(TestEventRouteClickInput, SetTarget_TwoWindow_Bubble)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
}

TEST_F(TestEventRouteClickInput, RemoveNode_Empty)
{
  EXPECT_FALSE(m_eventRoute.RemoveNode(m_nodeWindow1));
}

TEST_F(TestEventRouteClickInput, RemoveWindow_Target)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Direct);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  m_eventRoute.RemoveNode(m_nodeWindow1);
  EXPECT_TRUE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 0u);
  EXPECT_FALSE(m_eventRoute.GetTarget());
}

TEST_F(TestEventRouteClickInput, RemoveWindow_FromList)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  m_eventRoute.RemoveNode(m_nodeWindow1);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
}

// TEST_F(TestEventRouteClickInput, SendTo_Null_EventHandler)
//{
//   auto theEvent = CreateInputClickEvent();
//
//   EXPECT_THROW(m_eventRoute.Send(nullptr, theEvent), std::invalid_argument);
// }

// TEST_F(TestEventRouteClickInput, SendTo_Null_EventHandler)
//{
//   std::shared_ptr<UI::WindowEvent> theEvent;
//   EXPECT_THROW(m_eventRoute.Send(&m_eventHandler, theEvent), std::invalid_argument);
// }

TEST_F(TestEventRouteClickInput, SendTo_Empty)
{
  auto theEvent = CreateInputClickEvent();
  EXPECT_FALSE(m_eventRoute.Send(&m_eventHandler, theEvent));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_OneWindow_Direct)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Direct);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a direct event so we only expect one call to the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 1u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_OneWindow_Tunnel)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Tunnel);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a tunnel event so we only expect one call to the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 1u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_OneWindow_Bubble)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a bubble event so we only expect one call to the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 1u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_OneWindow_Pair)
{
  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect two calls to the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 2u);

  // The tunnel part
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);

  // The bubble part
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST_F(TestEventRouteClickInput, SendTo_Begin_TwoWindows_Direct)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Direct);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a direct event so we only expect one call to the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 1u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_TwoWindows_Tunnel)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Tunnel);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a tunneled event so we only expect one call to the parent window then the target window with the event we send
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 2u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_TwoWindows_Bubble)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Bubble);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a bubble event so we expect one call to the target window with the event we send and one to the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 2u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInput, SendTo_Begin_TwoWindows_Pair)
{
  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 4u);

  // The tunnel part
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);

  // The bubble part
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TheEvent.IsTunneling);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
