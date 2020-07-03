#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTROUTER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTROUTER_HPP
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
#include <memory>

namespace Fsl
{
  struct PxPoint2;

  namespace UI
  {
    class EventRoute;
    class ITreeNodeClickInputTargetLocater;
    class TreeNode;
    struct WindowFlags;

    class EventRouter
    {
      std::shared_ptr<TreeNode> m_rootNode;
      std::shared_ptr<ITreeNodeClickInputTargetLocater> m_clickTargetLocater;

    public:
      EventRouter(const EventRouter&) = delete;
      EventRouter& operator=(const EventRouter&) = delete;

      EventRouter(std::shared_ptr<TreeNode> rootNode, const std::shared_ptr<ITreeNodeClickInputTargetLocater>& clickTargetLocater);
      ~EventRouter();

      //! @brief Create a route for multiple uses.
      void CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const std::shared_ptr<TreeNode>& target);

      //! @brief Create a route for multiple uses.
      void CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const PxPoint2& hitPositionPx);

    private:
      //! @brief Locate a window by hitting
      std::shared_ptr<TreeNode> LocateWindowByHit(const PxPoint2& hitPositionPx, const WindowFlags& flags) const;
    };
  }
}

#endif
