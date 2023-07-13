#ifndef FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERLOGIC_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERLOGIC_HPP
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

#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderConstrainedValue.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderDragLogic.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderResultFlags.hpp>

namespace Fsl::UI
{
  //! @brief Contains all logic necessary to implement a dragable slider
  template <typename TValue>
  struct SliderLogic
  {
    using value_type = TValue;

  private:
    SliderConstrainedValue<value_type> m_value;
    SliderDragLogic m_drag;
    value_type m_preDragValue{};

  public:
    SliderLogic() noexcept = default;

    explicit SliderLogic(const SliderConstrainedValue<value_type>& value)
      : SliderLogic(value, SliderPixelSpanInfo())
    {
    }

    explicit SliderLogic(const SliderConstrainedValue<value_type>& value, const SliderPixelSpanInfo& spanInfo)
      : m_value(value)
      , m_drag(spanInfo)
    {
      m_drag.SetPercentage(m_value.GetPercentage());
    }

    //! @brief get the minimum value
    value_type Min() const
    {
      return m_value.Min();
    }

    //! @brief get the maximum value
    value_type Max() const
    {
      return m_value.Max();
    }

    value_type GetTickFrequency() const
    {
      return m_value.TickFrequency();
    }

    //! @brief Check if the slider is being dragged or not
    bool IsDragging() const
    {
      return m_drag.IsDragging();
    }

    //! @brief Check if we are enabled or disabled
    bool IsEnabled() const
    {
      return m_drag.IsEnabled();
    }

    //! @brief Get the current percentage
    float GetPercentage() const
    {
      return m_value.GetPercentage();
    }

    //! @brief Get the pixel position inside the 'span'
    PxValue GetPositionPx() const
    {
      return m_drag.GetPositionPx(m_value.GetPercentage());
    }

    //! @brief Get information about the currently configured span
    const SliderPixelSpanInfo& GetSpanInfo() const
    {
      return m_drag.GetSpanInfo();
    }

    //! @brief get the current value
    value_type GetValue() const
    {
      return m_value.Get();
    }

    //! @brief Set the value
    bool SetValue(const value_type value)
    {
      if (m_value.Set(value))
      {
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    bool SetTickFrequency(const value_type tickFrequency)
    {
      if (m_value.SetTickFrequency(tickFrequency))
      {
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    //! @brief Set the range.
    //! @note  Setting the range does not cancel drag operations
    bool SetRange(const value_type min, const value_type max)
    {
      if (m_value.SetRange(min, max))
      {
        m_value.SetPercentage(m_drag.GetPercentage());
        return true;
      }
      return false;
    }


    //! @brief Set the value using the percentage
    bool SetPercentage(const float percentage)
    {
      if (m_value.SetPercentage(percentage))
      {
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    //! @brief Enable/disable this control
    SliderResultFlags SetEnabled(const bool enabled)
    {
      auto flags = m_drag.SetEnabled(enabled);
      if (SliderResultFlagsUtil::IsFlagged(flags, SliderResultFlags::DragCancelled))
      {
        // Drag was canceled, so restoring the exact pre-drag value
        RestorePreDragValue();
      }
      return flags;
    }

    //! @brief Allow the inheriting class to set the render information
    bool SetSpanInfo(const SliderPixelSpanInfo& spanInfo)
    {
      if (m_drag.SetSpanInfo(spanInfo))
      {
        // Since the percentage might have been changed we need to reapply it to the value and then the drag (due to snap)
        m_value.SetPercentage(m_drag.GetPercentage());
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    bool TryBeginDrag(const PxValue startPositionPx)
    {
      if (!m_drag.IsDragging())
      {
        StorePreDragValue();
      }
      if (m_drag.TryBeginDrag(startPositionPx))
      {
        m_value.SetPercentage(m_drag.GetPercentage());
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    bool TryDrag(const PxValue newPositionPx)
    {
      if (m_drag.TryDrag(newPositionPx))
      {
        m_value.SetPercentage(m_drag.GetPercentage());
        m_drag.SetPercentage(m_value.GetPercentage());
        return true;
      }
      return false;
    }

    bool EndDrag(const PxValue positionPx)
    {
      if (m_drag.EndDrag(positionPx))
      {
        m_value.SetPercentage(m_drag.GetPercentage());
        m_drag.SetPercentage(m_value.GetPercentage());
        // Drag completed, so clear
        ClearPreDragValue();
        return true;
      }
      return false;
    }

    bool CancelDrag()
    {
      if (m_drag.CancelDrag())
      {
        RestorePreDragValue();
        return true;
      }
      return false;
    }


  private:
    inline void StorePreDragValue()
    {
      // Update the pre-drag value
      m_preDragValue = m_value.Get();
    }

    inline void ClearPreDragValue()
    {
      m_preDragValue = m_value.Min();
    }

    inline void RestorePreDragValue()
    {
      // Drag was canceled, so restoring the exact pre-drag value
      m_value.Set(m_preDragValue);
      m_drag.SetPercentage(m_value.GetPercentage());
      ClearPreDragValue();
    }
  };
}

#endif
