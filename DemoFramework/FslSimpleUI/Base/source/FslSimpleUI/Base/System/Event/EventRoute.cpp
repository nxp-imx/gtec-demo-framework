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

#include "EventRoute.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowTransactionEvent.hpp>
#include <FslSimpleUI/Base/System/Event/IEventHandler.hpp>
#include <algorithm>
#include <cassert>
#include "../../Event/ScopedWindowTransactionEventPatch.hpp"
#include "../TreeNode.hpp"

namespace Fsl::UI
{
  namespace
  {
    constexpr std::size_t InitialNodeCapacity = 32u;

    inline bool Exists(const std::vector<std::shared_ptr<TreeNode>>& nodes, const std::shared_ptr<TreeNode>& node)
    {
      return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
    }


    inline bool Remove(std::vector<std::shared_ptr<TreeNode>>& rNodes, const std::shared_ptr<TreeNode>& node)
    {
      auto itr = std::find(rNodes.begin(), rNodes.end(), node);
      const bool found = itr != rNodes.end();
      if (found)
      {
        rNodes.erase(itr);
      }
      return found;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void SendCancelEventsViaTunnel(IEventHandler& eventHandler, ReadOnlySpan<std::shared_ptr<TreeNode>> nodeSpan,
                                   const std::shared_ptr<WindowTransactionEvent>& theEvent)
    {
      RoutedEvent routedEvent(theEvent, true);
      Internal::ScopedWindowTransactionEventPatch scopedStateChange(*theEvent, EventTransactionState::Canceled, false, false);
      for (const auto& entry : nodeSpan)
      {
        if (entry->IsConsideredRunning())
        {
          eventHandler.HandleEvent(entry, routedEvent);
        }
      }
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void SendCancelEventsViaBubble(IEventHandler& eventHandler, ReadOnlySpan<std::shared_ptr<TreeNode>> nodeSpan,
                                   const std::shared_ptr<WindowTransactionEvent>& theEvent)
    {
      RoutedEvent routedEvent(theEvent, false);
      Internal::ScopedWindowTransactionEventPatch scopedStateChange(*theEvent, EventTransactionState::Canceled, false, false);
      // Temporarily patch the event so it becomes a cancel event
      for (std::size_t i = nodeSpan.size(); i > 0; --i)
      {
        const auto& entry = nodeSpan[i - 1u];
        if (entry->IsConsideredRunning())
        {
          eventHandler.HandleEvent(entry, routedEvent);
        }
      }
    }

    void SendToViaTunnel(IEventHandler& eventHandler, std::vector<std::shared_ptr<TreeNode>>& rNodes, const RoutedEvent& routedEvent,
                         const bool paired)
    {
      std::shared_ptr<WindowTransactionEvent> windowTransactionEvent = std::dynamic_pointer_cast<UI::WindowTransactionEvent>(routedEvent.Content);

      uint32_t interceptionCount = 0;
      bool allowIntercept = false;
      if (windowTransactionEvent)
      {
        interceptionCount = windowTransactionEvent->GetInterceptionCount();
        // Intercept is only valid for begin and end events during tunnel
        allowIntercept =
          windowTransactionEvent->GetState() == EventTransactionState::Begin || windowTransactionEvent->GetState() == EventTransactionState::End;
        windowTransactionEvent->SYS_SetAllowIntercept(allowIntercept);
      }
      for (std::size_t i = 0; i < rNodes.size(); ++i)
      {
        // The check is not needed as it would not get on the list if it was not interested at the time the list was build
        // Doing the check causes problems with FocusLoss events during 'hiding' of windows where certain flags are disabled.
        if (rNodes[i]->IsConsideredRunning())
        {
          eventHandler.HandleEvent(rNodes[i], routedEvent);
          if (windowTransactionEvent && windowTransactionEvent->GetInterceptionCount() != interceptionCount)
          {
            interceptionCount = windowTransactionEvent->GetInterceptionCount();

            if (allowIntercept)
            {
              std::size_t removeIndex = i + 1;
              if (removeIndex < rNodes.size())
              {
                FSLLOG3_VERBOSE3("Intercepting transaction event");
                const std::size_t removeCount = rNodes.size() - removeIndex;
                if (windowTransactionEvent->IsRepeat() || windowTransactionEvent->GetState() == EventTransactionState::End)
                {
                  // Since this is either a 'begin + repeat' or a end event we need to send a cancel event to the windows we are about to remove from
                  // the transaction
                  SendCancelEventsViaTunnel(eventHandler, SpanUtil::AsReadOnlySpan(rNodes, removeIndex, removeCount), windowTransactionEvent);
                  if (paired)
                  {
                    SendCancelEventsViaBubble(eventHandler, SpanUtil::AsReadOnlySpan(rNodes, removeIndex, removeCount), windowTransactionEvent);
                  }
                }

                {    // Remove all following windows from the route
                  auto itrRemoveBegin = std::next(rNodes.begin(), UncheckedNumericCast<std::ptrdiff_t>(removeIndex));
                  auto itrRemoveEnd = std::next(itrRemoveBegin, UncheckedNumericCast<std::ptrdiff_t>(removeCount));
                  rNodes.erase(itrRemoveBegin, itrRemoveEnd);
                }
              }
              else
              {
                FSLLOG3_VERBOSE3("A event intercept was requested from the main target, request ignored");
              }
            }
            else
            {
              FSLLOG3_ERROR("Only begin and end state events can be intercepted during tunnel, request ignored");
            }
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void SendViaBubble(IEventHandler& eventHandler, std::vector<std::shared_ptr<TreeNode>>& rNodes, const RoutedEvent& routedEvent, const bool paired)
    {
      std::shared_ptr<WindowTransactionEvent> windowTransactionEvent = std::dynamic_pointer_cast<UI::WindowTransactionEvent>(routedEvent.Content);

      uint32_t interceptionCount = 0;
      bool allowIntercept = false;
      if (windowTransactionEvent)
      {
        interceptionCount = windowTransactionEvent->GetInterceptionCount();
        // Intercept is only valid for begin events during bubble
        allowIntercept = windowTransactionEvent->GetState() == EventTransactionState::Begin;
        windowTransactionEvent->SYS_SetAllowIntercept(allowIntercept);
      }
      for (std::size_t i = rNodes.size(); i > 0; --i)
      {
        const auto& node = rNodes[i - 1];

        // The check is not needed as it would not get on the list if it was not interested at the time the list was build
        // Doing the check causes problems with FocusLoss events during 'hiding' of windows where certain flags are disabled.
        if (node->IsConsideredRunning())
        {
          eventHandler.HandleEvent(node, routedEvent);

          if (windowTransactionEvent && windowTransactionEvent->GetInterceptionCount() != interceptionCount)
          {
            interceptionCount = windowTransactionEvent->GetInterceptionCount();

            if (allowIntercept)
            {
              // since i is already one ahead
              const std::size_t removeIndex = i;
              if (removeIndex < rNodes.size())
              {
                FSLLOG3_VERBOSE3("Intercepting transaction event");

                const std::size_t removeCount = rNodes.size() - removeIndex;
                // For bubble events that are intercepted at a parent we always need to send a cancel to the children
                if (paired)
                {
                  SendCancelEventsViaTunnel(eventHandler, SpanUtil::AsReadOnlySpan(rNodes, removeIndex, removeCount), windowTransactionEvent);
                }
                SendCancelEventsViaBubble(eventHandler, SpanUtil::AsReadOnlySpan(rNodes, removeIndex, removeCount), windowTransactionEvent);

                {    // Remove all following windows from the route
                  auto itrRemoveBegin = std::next(rNodes.begin(), UncheckedNumericCast<std::ptrdiff_t>(removeIndex));
                  auto itrRemoveEnd = std::next(itrRemoveBegin, UncheckedNumericCast<std::ptrdiff_t>(removeCount));
                  rNodes.erase(itrRemoveBegin, itrRemoveEnd);
                }
              }
              else
              {
                FSLLOG3_VERBOSE3("A event intercept was requested from the main target, request ignored");
              }
            }
            else
            {
              FSLLOG3_ERROR("Only begin state events can be intercepted during bubble, request ignored");
            }
          }
        }
      }
    }
  }


  EventRoute::EventRoute()
    : m_windowList(InitialNodeCapacity)
    , m_isInitialized(false)
  {
    m_windowList.clear();
  }


  EventRoute::EventRoute(const WindowFlags flags)
    : m_flags(flags)
    , m_windowList(InitialNodeCapacity)
    , m_isInitialized(true)
  {
    m_windowList.clear();
  }


  EventRoute::~EventRoute() = default;


  void EventRoute::Initialize(const WindowFlags flags)
  {
    // if (m_isInitialized)
    //{
    //   throw UsageErrorException("already initialized");
    // }
    m_flags = flags;
    m_isInitialized = true;
  }


  void EventRoute::Shutdown()
  {
    m_flags = WindowFlags();
    m_windowList.clear();
    m_target.reset();
    m_isInitialized = false;
  }


  bool EventRoute::IsEmpty() const noexcept
  {
    assert(m_isInitialized);
    return m_windowList.empty();
  }


  bool EventRoute::Send(IEventHandler* const pEventHandler, const std::shared_ptr<WindowEvent>& theEvent)
  {
    assert(m_isInitialized);
    if (pEventHandler == nullptr)
    {
      FSLLOG3_ERROR("pEventHandler can not be null, send ignored!");
      return false;
    }

    bool send = false;
    if (m_target)
    {
      switch (m_strategy)
      {
      case EventRoutingStrategy::Direct:
        SendTo(*pEventHandler, m_windowList, theEvent, true);
        break;
      case EventRoutingStrategy::Tunnel:
        SendTo(*pEventHandler, m_windowList, theEvent, true);
        break;
      case EventRoutingStrategy::Bubble:
        SendTo(*pEventHandler, m_windowList, theEvent, false);
        break;
      case EventRoutingStrategy::Paired:
        SendTo(*pEventHandler, m_windowList, theEvent, true, true);
        SendTo(*pEventHandler, m_windowList, theEvent, false, true);
        break;
      default:
        return false;
      }

      send = GetWindowCount() > 0u;
    }
    return send;
  }


  void EventRoute::Clear()
  {
    assert(m_isInitialized);

    m_windowList.clear();
    m_strategy = EventRoutingStrategy::Direct;
    m_target.reset();
  }


  void EventRoute::SetTarget(const std::shared_ptr<TreeNode>& target, const EventRoutingStrategy routingStrategy)
  {
    assert(m_isInitialized);
    Clear();
    m_strategy = routingStrategy;
    m_target = target;
    if (m_target)
    {
      // Build the tunnel part of the route if requested
      // Build the tunnel part of the route if requested
      switch (routingStrategy)
      {
      case EventRoutingStrategy::Bubble:
      case EventRoutingStrategy::Tunnel:
      case EventRoutingStrategy::Paired:
        BuildParent(m_target);
        break;
      case EventRoutingStrategy::Direct:
        BuildDirect(m_target);
        break;
      default:
        throw NotSupportedException(fmt::format("Unsupported routing strategy: {}", fmt::underlying(routingStrategy)));
      }
    }
  }


  bool EventRoute::RemoveNode(const std::shared_ptr<TreeNode>& node)
  {
    assert(m_isInitialized);
    bool removed = false;
    removed |= Remove(m_windowList, node);

    if (m_target == node)
    {
      m_target.reset();
    }
    return removed;
  }


  void EventRoute::SendTo(IEventHandler& eventHandler, std::vector<std::shared_ptr<TreeNode>>& rNodes, const std::shared_ptr<WindowEvent>& theEvent,
                          const bool isTunneling, const bool paired)
  {
    assert(m_isInitialized);
    assert(m_target);

    if (rNodes.empty())
    {
      return;
    }

    if (!theEvent->GetOriginalSource())
    {
      // Setup the various things in the event here.
      theEvent->SYS_SetOriginalSource(m_target->GetWindow());
    }

    // Continue to send the event while the event isn't marked as handled.
    RoutedEvent routedEvent(theEvent, isTunneling);
    if (isTunneling)
    {
      SendToViaTunnel(eventHandler, rNodes, routedEvent, paired);
      UpdateTargetIfNecessary(SpanUtil::AsReadOnlySpan(rNodes));
    }
    else
    {
      SendViaBubble(eventHandler, rNodes, routedEvent, paired);
      UpdateTargetIfNecessary(SpanUtil::AsReadOnlySpan(rNodes));
    }
  }

  void EventRoute::UpdateTargetIfNecessary(ReadOnlySpan<std::shared_ptr<TreeNode>> nodeSpan)
  {
    if (m_target && !nodeSpan.empty() && nodeSpan.back() != m_target)
    {
      FSLLOG3_VERBOSE3("Updating EventRoute target as list target was changed");
      m_target = nodeSpan.back();
    }
  }

  void EventRoute::BuildParent(const std::shared_ptr<TreeNode>& target)
  {
    assert(m_isInitialized);
    if (!target || !target->IsConsideredRunning())
    {
      return;
    }

    // ask the parents first
    BuildParent(target->GetParent());

    // If the record is enabled, append it to the tunnel route
    if (target->GetFlags().IsFlagged(m_flags))
    {
      // The window should not be present in the list
      assert(!Exists(m_windowList, target));
      m_windowList.push_back(target);
    }
  }


  void EventRoute::BuildDirect(const std::shared_ptr<TreeNode>& target)
  {
    assert(m_isInitialized);
    if (target && target->IsConsideredRunning() && target->GetFlags().IsFlagged(m_flags))
    {
      // The window should not be present in the list
      assert(!Exists(m_windowList, target));
      m_windowList.push_back(target);
    }
  }
}
