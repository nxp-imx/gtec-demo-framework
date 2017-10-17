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

#include "EventRouter.hpp"
#include "EventRoute.hpp"
#include "../TreeNode.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/Base/Event/EventDescription.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include "../ITreeNodeClickInputTargetLocator.hpp"
#include "ScopedEventRoute.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    namespace
    {

      void CalcClipRect(Vector2& rTopLeft, Rect& rClipRect, const Vector2& parentTopLeft, const Rect& parentClipRect, const std::shared_ptr<TreeNode>& node)
      {
        Rect boundingRect = node->WinGetContentRect();
        rTopLeft = Vector2(parentTopLeft.X + boundingRect.X(), parentTopLeft.Y + boundingRect.Y());
        {
          //assert(EqualHelper::IsEqual(node->CalcScreenTopLeftCorner(), rTopLeft));
          Rect actualRect(rTopLeft.X, rTopLeft.Y, boundingRect.Width(), boundingRect.Height());

          // Clip the actual rect to the parent rect
          rClipRect = Rect::Intersect(parentClipRect, actualRect);
        }
      }

      // FIX: this method clips each node to its parent. Meaning the top children that draw outside their parents area wont get hit.
      //      A more correct and probably faster solution would be to store all hit enabled children in a oct-tree for fast searching
      std::shared_ptr<TreeNode> RecursiveLocateTopChildThatWasHit(const Vector2& currentTopLeft, const Rect& currentClipRect, const std::shared_ptr<TreeNode>& node, const Vector2& eventPos, const WindowFlags& flags)
      {
        assert(node);

        // Verify the integrity of the supplied rect by holding it up against the slow version
        //assert(EqualHelper::IsEqual(record.CalcScreenClipRect(), currentClipRect));
        //assert(EqualHelper::IsEqual(record.CalcScreenTopLeftCorner(), currentTopLeft));

        if (node->IsConsideredRunning() && currentClipRect.Contains(eventPos))
        {
          const auto& children = node->m_children;
          for (auto itr = children.rbegin(); itr != children.rend(); ++itr)
          {
            Vector2 childTopLeft;
            Rect childClipRect;
            CalcClipRect(childTopLeft, childClipRect, currentTopLeft, currentClipRect, *itr);

            // assert(EqualHelper::IsEqual(childClipRect, (*itr)->CalcScreenClipRect()));

            const std::shared_ptr<TreeNode> res = RecursiveLocateTopChildThatWasHit(childTopLeft, childClipRect, *itr, eventPos, flags);
            if (res)
              return res;
          }

          // If the record is touch enabled, it becomes our target
          if (node->GetFlags().IsFlagged(flags))
            return node;
        }
        return std::shared_ptr<TreeNode>();
      }

    }


    EventRouter::EventRouter(const std::shared_ptr<TreeNode>& rootNode, const std::shared_ptr<ITreeNodeClickInputTargetLocator>& clickTargetLocator)
      : m_rootNode(rootNode)
      , m_clickTargetLocator(clickTargetLocator)
    {
      if (!m_rootNode || !clickTargetLocator)
        throw std::invalid_argument("arguments can not be null");
    }


    EventRouter::~EventRouter()
    {
    }


    void EventRouter::CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const std::shared_ptr<TreeNode>& target)
    {
      rRoute.SetTarget(target, routingStrategy);
    }


    void EventRouter::CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const Vector2& hitPosition)
    {
      // locate the actual window that was hit
      std::shared_ptr<TreeNode> hit = LocateWindowByHit(hitPosition, rRoute.GetFlags());
      CreateRoute(rRoute, routingStrategy, hit);
    }


    std::shared_ptr<TreeNode> EventRouter::LocateWindowByHit(const Vector2& hitPosition, const WindowFlags& flags) const
    {
      if (flags.IsOnlyFlagEnabled(WindowFlags::ClickInput))
      {
        return m_clickTargetLocator->TryGet(hitPosition);
      }
      else
      {
        // locate the actual window that was hit
        Rect clipRect(m_rootNode->WinGetContentRect());
        const Vector2 topLeft = clipRect.TopLeft();
        return RecursiveLocateTopChildThatWasHit(topLeft, clipRect, m_rootNode, hitPosition, flags);
      }
    }
  }
}
