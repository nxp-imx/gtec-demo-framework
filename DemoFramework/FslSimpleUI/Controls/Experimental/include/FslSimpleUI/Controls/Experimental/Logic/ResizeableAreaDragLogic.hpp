#ifndef FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_LOGIC_RESIZEABLEAREADRAGLOGIC_HPP
#define FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_LOGIC_RESIZEABLEAREADRAGLOGIC_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderResultFlags.hpp>
#include <FslSimpleUI/Controls/Experimental/Logic/ResizeableAreaDragAxis.hpp>
#include <FslSimpleUI/Controls/Experimental/Logic/ResizeableAreaDragCorner.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl::UI
{
  //! @brief Contains all logic necessary to implement the drag part of a resizeable area
  struct ResizeableAreaDragLogic
  {
  private:
    enum class DragState
    {
      Idle,
      Dragging
    };

    struct DragInfo
    {
      DragState State{DragState::Idle};
      ResizeableAreaDragCorner Corner{ResizeableAreaDragCorner::TopLeft};
      ResizeableAreaDragAxis Axis{ResizeableAreaDragAxis::Disabled};
      PxPoint2 StartPositionPx;
      PxPoint2 LastPx;
    };

    DragInfo m_dragInfo;
    PxRectangle m_clampAreaPx;
    PxRectangle m_areaPx;
    bool m_isEnabled{true};
    bool m_isAreaInitialized{false};

  public:
    constexpr ResizeableAreaDragLogic() noexcept = default;

    constexpr explicit ResizeableAreaDragLogic(const PxRectangle& clampAreaRectanglePx, const PxRectangle& areaRectanglePx)
      : m_clampAreaPx(clampAreaRectanglePx)
      , m_areaPx(PxRectangle::Intersect(m_clampAreaPx, areaRectanglePx))
      , m_isAreaInitialized(true)
    {
    }


    //! @brief Check if the slider is being dragged or not
    constexpr bool IsDragging() const noexcept
    {
      return m_dragInfo.State == DragState::Dragging;
    }

    constexpr bool IsEnabled() const noexcept
    {
      return m_isEnabled;
    }

    constexpr PxRectangle GetAreaRectangle() const noexcept
    {
      return !IsDragging() ? m_areaPx : CalculateDragRectangle(m_dragInfo, m_clampAreaPx, m_areaPx);
    }

    constexpr const PxRectangle& GetClampAreaRectangle() const noexcept
    {
      return m_clampAreaPx;
    }

    //! @brief When disabled the area can not be edited by the user
    //! @note  If dragging when disabled the drag will be canceled
    //! @return true if the enabled state was changed
    SliderResultFlags SetEnabled(const bool enabled)
    {
      if (enabled == m_isEnabled)
      {
        return SliderResultFlags::NoChange;
      }
      auto flags = SliderResultFlags::Completed;
      if (m_isEnabled && IsDragging())
      {
        flags |= (CancelDrag() ? SliderResultFlags::DragCancelled : SliderResultFlags::NoChange);
      }
      m_isEnabled = enabled;
      return flags;
    }


    //! @brief Set the current span
    //! @note beware this might change the GetPercentage value
    inline bool SetAreaRectangle(const PxRectangle& valuePx)
    {
      if (valuePx != m_areaPx)
      {
        m_areaPx = PxRectangle::Intersect(m_clampAreaPx, valuePx);
        m_isAreaInitialized = true;
        if (IsDragging())
        {
          CancelDrag();
        }
        return true;
      }
      return false;
    }

    //! @brief Set the current span
    //! @note beware this might change the GetPercentage value
    inline bool SetClampAreaRectangle(const PxRectangle& valuePx)
    {
      if (valuePx != m_clampAreaPx)
      {
        m_clampAreaPx = valuePx;
        if (m_isAreaInitialized)
        {
          m_areaPx = PxRectangle::Intersect(m_clampAreaPx, m_areaPx);
        }
        else
        {
          m_areaPx = m_clampAreaPx;
          m_isAreaInitialized = true;
        }
        if (IsDragging())
        {
          CancelDrag();
        }
        return true;
      }
      return false;
    }

    //! @brief Begin a drag operation (if a existing drag operation is in progress it is canceled)
    //! @param startPositionPx the pixel position that was touched to initiate the drag (relative to this)
    bool TryBeginDrag(const PxPoint2 startPositionPx, const ResizeableAreaDragCorner corner,
                      const ResizeableAreaDragAxis axis = ResizeableAreaDragAxis::XY)
    {
      if (!m_isEnabled || axis == ResizeableAreaDragAxis::Disabled)
      {
        return false;
      }

      if (IsDragging())
      {
        CancelDrag();
      }
      m_dragInfo.State = DragState::Dragging;
      m_dragInfo.Corner = corner;
      m_dragInfo.Axis = axis;
      m_dragInfo.StartPositionPx = startPositionPx;
      m_dragInfo.LastPx = startPositionPx;
      return true;
    }

    //! @brief Continue a drag operation
    //! @param newPositionPx the pixel position that was touched (relative to this)
    bool TryDrag(const PxPoint2 newPositionPx)
    {
      if (!m_isEnabled || !IsDragging())
      {
        return false;
      }
      m_dragInfo.LastPx = newPositionPx;
      return true;
    }

    bool EndDrag(const PxPoint2 positionPx)
    {
      if (!m_isEnabled || !IsDragging())
      {
        return false;
      }
      m_dragInfo.LastPx = positionPx;
      m_areaPx = CalculateDragRectangle(m_dragInfo, m_clampAreaPx, m_areaPx);
      m_dragInfo = {};
      return true;
    }

    //! @brief If a drag is ongoing its canceled and the percentage state is rolled back to its initial value at drag start.
    //!        If not dragging this does nothing.
    //! @return true if a drag was canceled, false if not.
    bool CancelDrag()
    {
      if (!IsDragging())
      {
        return false;
      }
      // Clear the drag info
      m_dragInfo = {};
      assert(!IsDragging());
      return true;
    }

  private:
    static PxRectangle CalculateDragRectangle(const DragInfo& dragInfo, const PxRectangle& clampAreaPx, const PxRectangle& originalAreaPx) noexcept
    {
      PxPoint2 finalMovementPx = dragInfo.LastPx - dragInfo.StartPositionPx;
      if (!ResizeableAreaDragAxisUtil::IsFlagged(dragInfo.Axis, ResizeableAreaDragAxis::X))
      {
        finalMovementPx.X = PxValue();
      }
      if (!ResizeableAreaDragAxisUtil::IsFlagged(dragInfo.Axis, ResizeableAreaDragAxis::Y))
      {
        finalMovementPx.Y = PxValue();
      }

      PxValue leftPx = originalAreaPx.Left();
      PxValue rightPx = originalAreaPx.Right();
      PxValue topPx = originalAreaPx.Top();
      PxValue bottomPx = originalAreaPx.Bottom();

      switch (dragInfo.Corner)
      {
      case ResizeableAreaDragCorner::TopLeft:
        leftPx += finalMovementPx.X;
        topPx += finalMovementPx.Y;
        if (leftPx >= rightPx)
        {
          leftPx = rightPx;
        }
        if (topPx >= bottomPx)
        {
          topPx = bottomPx;
        }
        break;
      case ResizeableAreaDragCorner::TopRight:
        rightPx += finalMovementPx.X;
        topPx += finalMovementPx.Y;
        if (rightPx <= leftPx)
        {
          rightPx = leftPx;
        }
        if (topPx >= bottomPx)
        {
          topPx = bottomPx;
        }
        break;
      case ResizeableAreaDragCorner::BottomLeft:
        leftPx += finalMovementPx.X;
        bottomPx += finalMovementPx.Y;
        if (leftPx >= rightPx)
        {
          leftPx = rightPx;
        }
        if (bottomPx <= topPx)
        {
          bottomPx = topPx;
        }
        break;
      case ResizeableAreaDragCorner::BottomRight:
        rightPx += finalMovementPx.X;
        bottomPx += finalMovementPx.Y;
        if (rightPx <= leftPx)
        {
          rightPx = leftPx;
        }
        if (bottomPx <= topPx)
        {
          bottomPx = topPx;
        }
        break;
      }
      return PxRectangle::Intersect(clampAreaPx, PxRectangle::FromLeftTopRightBottom(leftPx, topPx, rightPx, bottomPx));
    }
  };
}

#endif
