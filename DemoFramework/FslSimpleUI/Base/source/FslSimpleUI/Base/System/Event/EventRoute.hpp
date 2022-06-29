#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTROUTE_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTROUTE_HPP
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

#include <FslSimpleUI/Base/Event/EventRoutingStrategy.hpp>
#include <FslSimpleUI/Base/WindowFlags.hpp>
#include <memory>
#include <vector>

namespace Fsl::UI
{
  class IEventHandler;
  class TreeNode;
  class WindowEvent;

  //! @brief  A event route contains the complete route that the event will traverse for a target and routing strategy.
  class EventRoute
  {
    WindowFlags m_flags;
    std::vector<std::shared_ptr<TreeNode>> m_tunnelList;
    std::vector<std::shared_ptr<TreeNode>> m_bubbleList;
    std::shared_ptr<TreeNode> m_target;
    bool m_isInitialized;

  public:
    EventRoute(const EventRoute&) = delete;
    EventRoute& operator=(const EventRoute&) = delete;

    EventRoute();
    explicit EventRoute(const WindowFlags flags);
    ~EventRoute();

    void Initialize(const WindowFlags flags);
    void Shutdown();

    WindowFlags GetFlags() const noexcept
    {
      return m_flags;
    }

    //! @brief Check if the route is empty
    bool IsEmpty() const noexcept;

    //! @brief Send the event along the route
    void Send(IEventHandler* const pEventHandler, const std::shared_ptr<WindowEvent>& theEvent);

    // @brief Clear the route to a empty state
    void Clear();

    //! @brief Get the target
    const std::shared_ptr<TreeNode>& GetTarget() const
    {
      return m_target;
    }

    //! @brief Create a route to the target using the supplied routing strategy.
    void SetTarget(const std::shared_ptr<TreeNode>& target, const EventRoutingStrategy routingStrategy);

    //! @brief Ensure that a node is removed from the routes
    //! @note This is intended to be used when a TreeNode is disposed to ensure that we don't keep a reference to it (or call it)
    void RemoveNode(const std::shared_ptr<TreeNode>& node);

    class StackScopedInit
    {
      EventRoute& m_rRoute;

    public:
      StackScopedInit(EventRoute& rRoute, const WindowFlags flags)
        : m_rRoute(rRoute)
      {
        m_rRoute.Initialize(flags);
      }
      ~StackScopedInit()
      {
        m_rRoute.Shutdown();
      }
    };

  private:
    void SendTo(IEventHandler& eventHandler, const std::vector<std::shared_ptr<TreeNode>>& nodes, const std::shared_ptr<WindowEvent>& theEvent,
                const bool isTunneling);
    void BuildTunnel(const std::shared_ptr<TreeNode>& target);
    void BuildBubble(const std::shared_ptr<TreeNode>& target);
    void BuildDirect(const std::shared_ptr<TreeNode>& target);
  };
}

#endif
