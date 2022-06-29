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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslSimpleUI/Base/Event/EventDescription.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <cassert>
#include <utility>
#include "../ITreeNodeClickInputTargetLocater.hpp"
#include "../TreeNode.hpp"
#include "EventRoute.hpp"
#include "ScopedEventRoute.hpp"

namespace Fsl::UI
{
  namespace
  {
    void CalcClipRect(PxPoint2& rTopLeftPx, PxClipRectangle& rClipRectPx, const PxPoint2& parentTopLeftPx, const PxClipRectangle& parentClipRectPx,
                      const std::shared_ptr<TreeNode>& node)
    {
      PxRectangle boundingRectPx = node->WinGetContentRectanglePx();
      rTopLeftPx = PxPoint2(parentTopLeftPx.X + boundingRectPx.X(), parentTopLeftPx.Y + boundingRectPx.Y());
      {
        // assert(EqualHelper::IsEqual(node->CalcScreenTopLeftCornerPx(), rTopLeft));
        PxClipRectangle actualRect(rTopLeftPx.X, rTopLeftPx.Y, boundingRectPx.Width(), boundingRectPx.Height());

        // Clip the actual rect to the parent rect
        rClipRectPx = PxClipRectangle::Intersect(parentClipRectPx, actualRect);
      }
    }

    // FIX: this method clips each node to its parent. Meaning the top children that draw outside their parents area wont get hit.
    //      A more correct and probably faster solution would be to store all hit enabled children in a oct-tree for fast searching
    std::shared_ptr<TreeNode> RecursiveLocateTopChildThatWasHit(const PxPoint2& currentTopLeftPx, const PxClipRectangle& currentClipRectPx,
                                                                const std::shared_ptr<TreeNode>& node, const PxPoint2& eventPosPx,
                                                                const WindowFlags flags)
    {
      assert(node);

      // Verify the integrity of the supplied rect by holding it up against the slow version
      // assert(EqualHelper::IsEqual(record.CalcScreenClipRect(), currentClipRect));
      // assert(EqualHelper::IsEqual(record.CalcScreenTopLeftCornerPx(), currentTopLeft));

      if (node->IsConsideredRunning() && currentClipRectPx.Contains(eventPosPx.X, eventPosPx.Y))
      {
        const auto& children = node->m_children;
        for (auto itr = children.rbegin(); itr != children.rend(); ++itr)
        {
          PxPoint2 childTopLeftPx;
          PxClipRectangle childClipRectPx;
          CalcClipRect(childTopLeftPx, childClipRectPx, currentTopLeftPx, currentClipRectPx, *itr);

          // assert(EqualHelper::IsEqual(childClipRect, (*itr)->CalcScreenClipRect()));

          std::shared_ptr<TreeNode> res = RecursiveLocateTopChildThatWasHit(childTopLeftPx, childClipRectPx, *itr, eventPosPx, flags);
          if (res)
          {
            return res;
          }
        }

        // If the record is touch enabled, it becomes our target
        if (node->GetFlags().IsFlagged(flags))
        {
          return node;
        }
      }
      return {};
    }
  }


  EventRouter::EventRouter(std::shared_ptr<TreeNode> rootNode, const std::shared_ptr<ITreeNodeClickInputTargetLocater>& clickTargetLocater)
    : m_rootNode(std::move(rootNode))
    , m_clickTargetLocater(clickTargetLocater)
  {
    if (!m_rootNode || !clickTargetLocater)
    {
      throw std::invalid_argument("arguments can not be null");
    }
  }


  EventRouter::~EventRouter() = default;


  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  void EventRouter::CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const std::shared_ptr<TreeNode>& target)

  {
    rRoute.SetTarget(target, routingStrategy);
  }


  void EventRouter::CreateRoute(EventRoute& rRoute, const EventRoutingStrategy routingStrategy, const PxPoint2& hitPositionPx)
  {
    // locate the actual window that was hit
    std::shared_ptr<TreeNode> hit = LocateWindowByHit(hitPositionPx, rRoute.GetFlags());
    CreateRoute(rRoute, routingStrategy, hit);
  }


  std::shared_ptr<TreeNode> EventRouter::LocateWindowByHit(const PxPoint2& hitPositionPx, const WindowFlags flags) const
  {
    if (flags.IsOnlyFlagEnabled(WindowFlags::ClickInput))
    {
      return m_clickTargetLocater->TryGetClickInputWindow(hitPositionPx);
    }

    // locate the actual window that was hit
    PxClipRectangle clipRectPx(TypeConverter::UncheckedTo<PxClipRectangle>(m_rootNode->WinGetContentRectanglePx()));
    const PxPoint2 topLeftPx = clipRectPx.TopLeft();
    return RecursiveLocateTopChildThatWasHit(topLeftPx, clipRectPx, m_rootNode, hitPositionPx, flags);
  }
}
