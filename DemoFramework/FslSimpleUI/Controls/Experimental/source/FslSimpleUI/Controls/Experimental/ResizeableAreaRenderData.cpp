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

#include <FslSimpleUI/Controls/Experimental/ResizeableAreaRenderData.hpp>

namespace Fsl::UI
{
  namespace
  {
    constexpr PxRectangle CalcDragRectangle(const PxValue xPx, const PxValue yPx, const PxSize1D sizePx) noexcept
    {
      const PxSize1D halfSizePx(sizePx / PxSize1D::Create(2));
      return {xPx - halfSizePx, yPx - halfSizePx, sizePx, sizePx};
    }
  }

  ResizeableAreaRenderData::ResizeableAreaRenderData()
  {
    DragHandles[0].Corner = ResizeableAreaDragCorner::TopLeft;
    DragHandles[1].Corner = ResizeableAreaDragCorner::TopRight;
    DragHandles[2].Corner = ResizeableAreaDragCorner::BottomLeft;
    DragHandles[3].Corner = ResizeableAreaDragCorner::BottomRight;
  }

  void ResizeableAreaRenderData::SetRenderData(const PxVector2 renderOffsetPxf, const PxRectangle& rectanglePx, const PxSize1D dragHandleSizePx,
                                               const ResizeableAreaDragHandleLocation dragHandleLocation, const bool showRenderHandles,
                                               const UIRenderColor dragHandleColor)
  {
    ShowRenderHandles = showRenderHandles;
    RenderOffsetPxf = renderOffsetPxf;
    ClipRectanglePx = rectanglePx;
    DragHandleColor = dragHandleColor;

    auto leftPx = rectanglePx.Left();
    auto rightPx = rectanglePx.Right() - PxValue(1);
    auto topPx = rectanglePx.Top();
    auto bottomPx = rectanglePx.Bottom() - PxValue(1);

    if (dragHandleLocation == ResizeableAreaDragHandleLocation::Centered)
    {
      DragHandles[0].Rectangle = CalcDragRectangle(leftPx, topPx, dragHandleSizePx);
      DragHandles[1].Rectangle = CalcDragRectangle(rightPx, topPx, dragHandleSizePx);
      DragHandles[2].Rectangle = CalcDragRectangle(leftPx, bottomPx, dragHandleSizePx);
      DragHandles[3].Rectangle = CalcDragRectangle(rightPx, bottomPx, dragHandleSizePx);
    }
    else
    {
      const auto offsetPx = dragHandleSizePx - PxSize1D::Create(1);
      DragHandles[0].Rectangle = PxRectangle(leftPx, topPx, dragHandleSizePx, dragHandleSizePx);
      DragHandles[1].Rectangle = PxRectangle(rightPx - offsetPx, topPx, dragHandleSizePx, dragHandleSizePx);
      DragHandles[2].Rectangle = PxRectangle(leftPx, bottomPx - offsetPx, dragHandleSizePx, dragHandleSizePx);
      DragHandles[3].Rectangle = PxRectangle(rightPx - offsetPx, bottomPx - offsetPx, dragHandleSizePx, dragHandleSizePx);
    }

    // Extract the final area
    if (showRenderHandles)
    {
      for (const auto& entry : DragHandles)
      {
        if (entry.Rectangle.Left() < leftPx)
        {
          leftPx = entry.Rectangle.Left();
        }
        if (entry.Rectangle.Right() > rightPx)
        {
          rightPx = entry.Rectangle.Right();
        }
        if (entry.Rectangle.Top() < topPx)
        {
          topPx = entry.Rectangle.Top();
        }
        if (entry.Rectangle.Bottom() > bottomPx)
        {
          bottomPx = entry.Rectangle.Bottom();
        }
      }
    }

    RenderRectanglePx = PxRectangle::FromLeftTopRightBottom(leftPx, topPx, rightPx, bottomPx);
  }


  int32_t ResizeableAreaRenderData::TryGetFindHitIndex(const PxPoint2 positionPx) const noexcept
  {
    int32_t i = 0;
    for (const auto& entry : DragHandles)
    {
      if (entry.Rectangle.Contains(positionPx))
      {
        return i;
      }
      ++i;
    }
    return -1;
  }
}
