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

#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/UnitTest/UITreeEventHandlerTest.hpp>

namespace Fsl::UI
{
  void UITreeEventHandlerTest::HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent)
  {
    ++CallCount.HandleEvent;
    if (routedEvent.IsTunneling)
    {
      ++CallCount.HandleEventTunnel;
    }
    else
    {
      ++CallCount.HandleEventBubble;
    }

    auto clickEvent = std::dynamic_pointer_cast<WindowInputClickEvent>(routedEvent.Content);
    if (clickEvent)
    {
      auto itrFindIntercept = m_clickEventIntercept.find(target);
      if (itrFindIntercept != m_clickEventIntercept.end())
      {
        if (routedEvent.IsTunneling == itrFindIntercept->second)
        {
          clickEvent->Intercept();
        }
      }
      if (clickEvent->GetState() == EventTransactionState::Begin && !clickEvent->IsRepeat())
      {
        auto itrFindHandle = m_clickEventHandleBegin.find(target);
        if (itrFindHandle != m_clickEventHandleBegin.end())
        {
          if (routedEvent.IsTunneling == itrFindHandle->second)
          {
            clickEvent->Handled();
          }
        }
      }
    }

    if (TestMarkEventsAsHandled)
    {
      routedEvent.Content->Handled();
    }

    HandleEventCalls.emplace_back(target, routedEvent);
  }

  void UITreeEventHandlerTest::TestAddClickEventIntercept(const std::shared_ptr<TreeNode>& window, const bool tunnel)
  {
    m_clickEventIntercept.emplace(window, tunnel);
  }

  bool UITreeEventHandlerTest::TestRemoveClickEventIntercept(const std::shared_ptr<TreeNode>& window)
  {
    auto itrFind = m_clickEventIntercept.find(window);
    if (itrFind == m_clickEventIntercept.end())
    {
      return false;
    }
    m_clickEventIntercept.erase(itrFind);
    return true;
  }

  void UITreeEventHandlerTest::TestAddClickEventHandleBegin(const std::shared_ptr<TreeNode>& window, const bool tunnel)
  {
    m_clickEventHandleBegin.emplace(window, tunnel);
  }

  bool UITreeEventHandlerTest::TestRemoveClickEventHandleBegin(const std::shared_ptr<TreeNode>& window)
  {
    auto itrFind = m_clickEventHandleBegin.find(window);
    if (itrFind == m_clickEventHandleBegin.end())
    {
      return false;
    }
    m_clickEventHandleBegin.erase(itrFind);
    return true;
  }

  void UITreeEventHandlerTest::ClearCallCount()
  {
    CallCount = {};
    HandleEventCalls.clear();
  }
}
