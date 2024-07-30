#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_UITREEEVENTHANDLERTEST_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_UITREEEVENTHANDLERTEST_HPP
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

#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowTransactionEvent.hpp>
#include <FslSimpleUI/Base/System/Event/IEventHandler.hpp>
#include <map>
#include <utility>
#include <vector>

namespace Fsl::UI
{
  class UITreeEventHandlerTest : public IEventHandler
  {
  public:
    struct TheCallCount
    {
      uint32_t HandleEvent;
      uint32_t HandleEventTunnel;
      uint32_t HandleEventBubble;
    };

    struct TestHandleInfo
    {
      std::shared_ptr<TreeNode> Window;
      RoutedEvent TheEvent;
      std::shared_ptr<WindowTransactionEvent> TransactionEvent;
      EventTransactionState TransactionState{};
      bool TransactionIsRepeat{};
      bool TransactionIsHandled{};

      TestHandleInfo(std::shared_ptr<TreeNode> window, const RoutedEvent& theEvent)
        : Window(std::move(window))
        , TheEvent(theEvent)
        , TransactionEvent(std::dynamic_pointer_cast<WindowTransactionEvent>(theEvent.Content))
        , TransactionState(TransactionEvent ? TransactionEvent->GetState() : EventTransactionState::Begin)
        , TransactionIsRepeat(TransactionEvent ? TransactionEvent->IsRepeat() : false)
        , TransactionIsHandled(TransactionEvent ? TransactionEvent->IsHandled() : false)
      {
      }
    };

    TheCallCount CallCount;

    std::vector<TestHandleInfo> HandleEventCalls;

    bool TestMarkEventsAsHandled;

  private:
    std::map<std::shared_ptr<TreeNode>, bool> m_clickEventIntercept;
    std::map<std::shared_ptr<TreeNode>, bool> m_clickEventHandleBegin;


  public:
    void HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent) final;

    void TestAddClickEventIntercept(const std::shared_ptr<TreeNode>& window, const bool tunnel);
    bool TestRemoveClickEventIntercept(const std::shared_ptr<TreeNode>& window);
    void TestAddClickEventHandleBegin(const std::shared_ptr<TreeNode>& window, const bool tunnel);
    bool TestRemoveClickEventHandleBegin(const std::shared_ptr<TreeNode>& window);

    void ClearCallCount();
  };
}

#endif
