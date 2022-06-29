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
#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <algorithm>
#include <cassert>
#include "../TreeNode.hpp"
#include "IEventHandler.hpp"

namespace Fsl::UI
{
  namespace
  {
    const std::size_t INITIAL_NODE_CAPACITY = 32u;

    inline bool Exists(const std::vector<std::shared_ptr<TreeNode>>& nodes, const std::shared_ptr<TreeNode>& node)
    {
      return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
    }


    inline void Remove(std::vector<std::shared_ptr<TreeNode>>& rNodes, const std::shared_ptr<TreeNode>& node)
    {
      auto itr = std::find(rNodes.begin(), rNodes.end(), node);
      if (itr != rNodes.end())
      {
        rNodes.erase(itr);
      }
    }
  }


  EventRoute::EventRoute()
    : m_tunnelList(INITIAL_NODE_CAPACITY)
    , m_bubbleList(INITIAL_NODE_CAPACITY)
    , m_isInitialized(false)
  {
    m_tunnelList.clear();
    m_bubbleList.clear();
  }


  EventRoute::EventRoute(const WindowFlags flags)
    : m_flags(flags)
    , m_tunnelList(INITIAL_NODE_CAPACITY)
    , m_bubbleList(INITIAL_NODE_CAPACITY)
    , m_isInitialized(true)
  {
    m_tunnelList.clear();
    m_bubbleList.clear();
  }


  EventRoute::~EventRoute() = default;


  void EventRoute::Initialize(const WindowFlags flags)
  {
    m_flags = flags;
    m_isInitialized = true;
  }


  void EventRoute::Shutdown()
  {
    assert(m_isInitialized);

    m_flags = WindowFlags();
    m_tunnelList.clear();
    m_bubbleList.clear();
    m_target.reset();
    m_isInitialized = false;
  }


  bool EventRoute::IsEmpty() const noexcept
  {
    assert(m_isInitialized);
    return (m_bubbleList.empty() && m_tunnelList.empty());
  }


  void EventRoute::Send(IEventHandler* const pEventHandler, const std::shared_ptr<WindowEvent>& theEvent)
  {
    assert(m_isInitialized);
    if (pEventHandler == nullptr)
    {
      FSLLOG3_ERROR("pEventHandler can not be null, send ignored!");
      return;
    }

    if (m_target)
    {
      SendTo(*pEventHandler, m_tunnelList, theEvent, true);
      SendTo(*pEventHandler, m_bubbleList, theEvent, false);
    }
  }


  void EventRoute::Clear()
  {
    assert(m_isInitialized);

    m_tunnelList.clear();
    m_bubbleList.clear();
    m_target.reset();
  }


  void EventRoute::SetTarget(const std::shared_ptr<TreeNode>& target, const EventRoutingStrategy routingStrategy)
  {
    assert(m_isInitialized);
    Clear();
    m_target = target;
    if (m_target)
    {
      // Build the tunnel part of the route if requested
      if (routingStrategy == EventRoutingStrategy::Paired || routingStrategy == EventRoutingStrategy::Tunnel)
      {
        BuildTunnel(target);
      }

      // Build the bubble part of the route if requested
      if (routingStrategy == EventRoutingStrategy::Paired || routingStrategy == EventRoutingStrategy::Bubble)
      {
        BuildBubble(target);
      }

      if (routingStrategy == EventRoutingStrategy::Direct)
      {
        BuildDirect(target);
      }
    }
  }


  void EventRoute::RemoveNode(const std::shared_ptr<TreeNode>& node)
  {
    assert(m_isInitialized);
    Remove(m_tunnelList, node);
    Remove(m_bubbleList, node);

    if (m_target == node)
    {
      m_target.reset();
    }
  }


  void EventRoute::SendTo(IEventHandler& eventHandler, const std::vector<std::shared_ptr<TreeNode>>& nodes,
                          const std::shared_ptr<WindowEvent>& theEvent, const bool isTunneling)
  {
    assert(m_isInitialized);
    assert(m_target);

    if (nodes.empty())
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
    for (auto itr = nodes.begin(); itr != nodes.end() && !theEvent->IsHandled(); ++itr)
    {
      if ((*itr)->IsConsideredRunning() && (*itr)->GetFlags().IsFlagged(m_flags))
      {
        eventHandler.HandleEvent(*itr, routedEvent);
      }
    }
  }


  void EventRoute::BuildTunnel(const std::shared_ptr<TreeNode>& target)
  {
    assert(m_isInitialized);
    if (target && target->IsConsideredRunning())
    {
      // ask the parents first
      BuildTunnel(target->GetParent());

      // If the record is enabled, append it to the tunnel route
      if (target->GetFlags().IsFlagged(m_flags))
      {
        assert(!Exists(m_tunnelList, target));
        m_tunnelList.push_back(target);
      }
    }
    // assert(IsValidRoute());
  }


  void EventRoute::BuildBubble(const std::shared_ptr<TreeNode>& target)
  {
    assert(m_isInitialized);
    std::shared_ptr<TreeNode> current = target;
    while (current)
    {
      // only 'bubble' the event into parents that are interested in events.
      if (current->IsConsideredRunning() && current->GetFlags().IsFlagged(m_flags))
      {
        assert(!Exists(m_bubbleList, current));
        m_bubbleList.push_back(current);
      }
      current = current->GetParent();
    }
    // assert(IsValidRoute());
  }


  void EventRoute::BuildDirect(const std::shared_ptr<TreeNode>& target)
  {
    assert(m_isInitialized);
    if (target->IsConsideredRunning() && target->GetFlags().IsFlagged(m_flags))
    {
      assert(!Exists(m_tunnelList, target));
      m_tunnelList.push_back(target);
    }
    // assert(IsValidRoute());
  }
}
