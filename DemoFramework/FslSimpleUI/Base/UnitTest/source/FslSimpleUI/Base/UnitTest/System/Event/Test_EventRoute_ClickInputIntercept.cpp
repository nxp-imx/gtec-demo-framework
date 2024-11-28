
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
  class TestEventRouteClickInputIntercept : public TestFixtureFslSimpleUIUITree
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
    TestEventRouteClickInputIntercept()
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_OneWindow_Direct)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, true);

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

  // Since this is a direct target event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_OneWindow_Tunnel)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, true);

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

  // Since this is a tunnel to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_OneWindow_Bubble)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, false);

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

  // Since this is a bubble to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_OneWindow_Pair_InterceptTunnel)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, true);

  m_eventRoute.SetTarget(m_nodeWindow1, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);

  // Lets try to send a eventSendTo_Begin_OneWindow_Pair_InterceptTunnel
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

  // Since this is a paired event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}


TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_OneWindow_Pair_InterceptBubble)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, false);

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

  // Since this is a paired event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Direct)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow2, true);

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

  // Since this is a direct target event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
  // Since this is a direct route the count will always be 1
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Tunnel_TargetIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow2, true);

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

  // Since this is a tunnel event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Tunnel_ParentIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, true);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Tunnel);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a tunneled event so we only expect one call to the parent window and since its intercepts the event, it stops there
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 1u);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);

  // Since this is a tunnel event and the parent intercepts, the event should be marked and the target should not get the event
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  // As the parent intercepted the route was modified to stop at the parent
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Bubble_TargetIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow2, false);

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

  // Since this is a bubble event to the window that intercepts, this should basically only mark the event and do nothing else
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Bubble_ParentIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, false);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Bubble);

  // Since this is a bubble event and the parent intercepts
  // - this marks the event
  // - sends a bubble cancel event to the original target
  // - continue the event with the parent as the new target

  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a bubble event so we expect one call to the target window with the event we send and one to the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 3u);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);

  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);

  // We expect a 'cancel' event to get generated
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TransactionState, UI::EventTransactionState::Canceled);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsHandled);

  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  // Since m_nodeWindow1 intercepted the transaction it is now the main target
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Pair_TunnelTargetIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow2, true);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 4u);

  // The tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);

  // The tunnel part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);

  // The bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsRepeat);

  // The bubble part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TransactionIsRepeat);

  // Since its the original target that intercepts we expect nothing to happen to the route
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Pair_BubbleTargetIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow2, false);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 4u);

  // The tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);

  // The tunnel part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);

  // The bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsRepeat);

  // The bubble part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TransactionIsRepeat);

  // Since its the original target that intercepts we expect nothing to happen to the route
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Pair_TunnelParentIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, true);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 2u);

  // Since the parent intercepts a begin via tunneling the original target is just cut out

  // The tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);

  // The bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);

  // Since it was intercepted by m_inputWindow we expect the route to have been updated
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Pair_BubbleParentIntercepts)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, false);
  m_eventHandler.TestAddClickEventHandleBegin(m_nodeWindow2, false);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 6u);

  // The tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsHandled);

  // The tunnel part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsHandled);

  // The bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsRepeat);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[2].TransactionIsHandled);

  // The bubble part 2
  // This is where the intercept occurs, so this will be followed by a 'tunnel cancel' + 'tunnel bubble' to m_inputWindow2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TheEvent.IsTunneling);
  // Since this window intercepts the event, that also clears the handled state set by children
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TransactionIsRepeat);
  // Since this window intercepts the event, that also clears the handled state set by children
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TransactionIsHandled);

  // Cancel tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[4].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].TransactionState, UI::EventTransactionState::Canceled);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[4].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[4].TransactionIsHandled);

  // Cancel bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].TransactionState, UI::EventTransactionState::Canceled);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TransactionIsHandled);

  // Since it was intercepted by m_nodeWindow1 we expect the route to have ben updated
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST_F(TestEventRouteClickInputIntercept, SendTo_Begin_TwoWindows_Pair_BubbleParentIntercepts2)
{
  m_eventHandler.TestAddClickEventIntercept(m_nodeWindow1, false);
  m_eventHandler.TestAddClickEventHandleBegin(m_nodeWindow1, false);
  m_eventHandler.TestAddClickEventHandleBegin(m_nodeWindow2, false);

  m_eventRoute.SetTarget(m_nodeWindow2, UI::EventRoutingStrategy::Paired);
  EXPECT_FALSE(m_eventRoute.IsEmpty());
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 2u);
  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow2);

  // Lets try to send a event
  auto theEvent = CreateInputClickEvent();
  EXPECT_TRUE(m_eventRoute.Send(&m_eventHandler, theEvent));

  // Its a paired event so we expect calls to the parent, target, target then finally the parent
  ASSERT_EQ(m_eventHandler.CallCount.HandleEvent, 6u);

  // The tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[0].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[0].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[0].TransactionIsHandled);

  // The tunnel part 2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[1].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[1].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[1].TransactionIsHandled);

  // The bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[2].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[2].TransactionIsRepeat);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[2].TransactionIsHandled);

  // The bubble part 2
  // This is where the intercept occurs, so this will be followed by a 'tunnel cancel' + 'tunnel bubble' to m_inputWindow2
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].Window, m_nodeWindow1);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TheEvent.IsTunneling);
  // Since this window intercepts the event, that also clears the handled state set by children
  EXPECT_EQ(m_eventHandler.HandleEventCalls[3].TransactionState, UI::EventTransactionState::Begin);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[3].TransactionIsRepeat);
  // Since this window intercepts the event, that also clears the handled state set by children
  // But this window also marks it as handled
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[3].TransactionIsHandled);

  // Cancel tunnel part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].TheEvent.Content, theEvent);
  EXPECT_TRUE(m_eventHandler.HandleEventCalls[4].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[4].TransactionState, UI::EventTransactionState::Canceled);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[4].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[4].TransactionIsHandled);

  // Cancel bubble part 1
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].Window, m_nodeWindow2);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].TheEvent.Content, theEvent);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TheEvent.IsTunneling);
  EXPECT_EQ(m_eventHandler.HandleEventCalls[5].TransactionState, UI::EventTransactionState::Canceled);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TransactionIsRepeat);
  EXPECT_FALSE(m_eventHandler.HandleEventCalls[5].TransactionIsHandled);

  // Since it was intercepted by m_nodeWindow1 we expect the route to have ben updated
  EXPECT_TRUE(theEvent->Intercepted());
  EXPECT_EQ(theEvent->GetInterceptionCount(), 1u);

  EXPECT_EQ(m_eventRoute.GetTarget(), m_nodeWindow1);
  EXPECT_EQ(m_eventRoute.GetWindowCount(), 1u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
