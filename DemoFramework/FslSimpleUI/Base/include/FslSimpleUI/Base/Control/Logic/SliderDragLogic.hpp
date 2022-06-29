#ifndef FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERDRAGLOGIC_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERDRAGLOGIC_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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
#include <FslSimpleUI/Base/Control/Logic/SliderPixelSpanInfo.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderResultFlags.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl::UI
{
  //! @brief Contains all logic necessary to implement the drag part of a slider
  struct SliderDragLogic
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
      float StartPercentage{};
      int32_t LastPx{};
    };

    DragInfo m_dragInfo;
    SliderPixelSpanInfo m_spanInfo;
    bool m_isEnabled{true};
    float m_percentage{};

  public:
    constexpr SliderDragLogic() noexcept = default;

    constexpr explicit SliderDragLogic(const SliderPixelSpanInfo& pixelSpanInfo)
      : m_spanInfo(pixelSpanInfo)
    {
    }


    //! @brief Check if the slider is being dragged or not
    constexpr bool IsDragging() const
    {
      return m_dragInfo.State == DragState::Dragging;
    }

    constexpr bool IsEnabled() const
    {
      return m_isEnabled;
    }

    constexpr float GetPercentage() const
    {
      return m_percentage;
    }

    int32_t GetPositionPx(const float percentage) const
    {
      return m_spanInfo.CalcPercentageToPxPosition(percentage);
    }

    // constexpr const DragInfo& GetCurrentDragState() const
    //{
    //  return m_dragInfo;
    //}

    constexpr const SliderPixelSpanInfo& GetSpanInfo() const
    {
      return m_spanInfo;
    }


    //! @brief When disabled the slider can not be edited by the user
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
    inline bool SetSpanInfo(const SliderPixelSpanInfo& spanInfo)
    {
      if (spanInfo != m_spanInfo)
      {
        m_spanInfo = spanInfo;
        if (m_isEnabled && IsDragging())
        {
          // recalculate the percentage based on the new span and the last dragged position
          m_percentage = m_spanInfo.CalculatePercentage(m_dragInfo.LastPx);
        }
        return true;
      }
      return false;
    }

    //! @brief Begin a drag operation (if a existing drag operation is in progress it is canceled)
    //! @param startPositionPx the pixel position that was touched to initiate the drag (relative to this)
    bool TryBeginDrag(const int32_t startPositionPx)
    {
      if (!m_isEnabled)
      {
        return false;
      }

      if (IsDragging())
      {
        CancelDrag();
      }
      m_dragInfo.StartPercentage = m_percentage;
      m_dragInfo.State = DragState::Dragging;
      m_dragInfo.LastPx = startPositionPx;
      m_percentage = m_spanInfo.CalculatePercentage(startPositionPx);
      return true;
    }

    //! @brief Continue a drag operation
    //! @param newPositionPx the pixel position that was touched (relative to this)
    bool TryDrag(const int32_t newPositionPx)
    {
      if (!m_isEnabled || !IsDragging())
      {
        return false;
      }
      m_dragInfo.LastPx = newPositionPx;
      m_percentage = m_spanInfo.CalculatePercentage(newPositionPx);
      return true;
    }

    bool EndDrag(const int32_t positionPx)
    {
      if (!m_isEnabled || !IsDragging())
      {
        return false;
      }

      m_dragInfo = {};
      m_percentage = m_spanInfo.CalculatePercentage(positionPx);
      return true;
    }

    //! @brief Set the percentage
    //! @note  If any drags are ongoing they will be canceled.
    //! @return the flags will tell you exactly what was changed
    bool SetPercentage(const float percentage)
    {
      const auto clampedPercentage = MathHelper::Clamp(percentage, 0.0f, 1.0f);
      if (clampedPercentage != m_percentage)
      {
        m_percentage = clampedPercentage;
        return true;
      }
      return false;
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
      // Restore the original state
      m_percentage = m_dragInfo.StartPercentage;
      // Clear the drag info
      m_dragInfo = {};
      assert(!IsDragging());
      return true;
    }
  };
}

#endif
