#ifndef FSLSIMPLEUI_BASE_CONTROL_SLIDERBASE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SLIDERBASE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/ThicknessF.hpp>
#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Control/SliderContentChangedReason.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderLogic.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/LayoutDirection.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>

namespace Fsl
{
  namespace UI
  {
    template <typename T>
    class SliderBase : public BaseWindow
    {
    public:
      using value_type = T;

    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      LayoutOrientation m_orientation{LayoutOrientation::Horizontal};
      LayoutDirection m_direction{LayoutDirection::NearToFar};

      SliderLogic<value_type> m_logic;

    public:
      explicit SliderBase(const std::shared_ptr<WindowContext>& context)
        : BaseWindow(context)
        , m_windowContext(context)
        , m_logic(SliderConstrainedValue<value_type>(value_type(0), value_type(0), value_type(100)))
      {
      }

      //! @brief Check if the slider is being dragged or not
      bool IsDragging() const
      {
        return m_logic.IsDragging();
      }

      LayoutOrientation GetOrientation() const
      {
        return m_orientation;
      }

      LayoutDirection GetDirection() const
      {
        return m_direction;
      }

      bool IsEnabled() const
      {
        return m_logic.IsEnabled();
      }

      value_type GetValue() const
      {
        return m_logic.GetValue();
      }

      value_type GetMinValue() const
      {
        return m_logic.Min();
      }

      value_type GetMaxValue() const
      {
        return m_logic.Max();
      }

      value_type GetTickFrequency() const
      {
        return m_logic.GetTickFrequency();
      }

      bool SetOrientation(const LayoutOrientation orientation)
      {
        if (orientation != m_orientation)
        {
          m_orientation = orientation;
          PropertyUpdated(PropertyType::Layout);
          return true;
        }
        return false;
      }

      void SetDirection(const LayoutDirection direction)
      {
        if (direction != m_direction)
        {
          m_direction = direction;
          PropertyUpdated(PropertyType::Layout);
        }
      }

      //! @brief When disabled the slider can not be edited by the user
      bool SetEnabled(const bool enabled)
      {
        auto flags = m_logic.SetEnabled(enabled);
        if (SliderResultFlagsUtil::IsFlagged(flags, SliderResultFlags::Completed))
        {
          PropertyUpdated(PropertyType::Other);
          if (SliderResultFlagsUtil::IsFlagged(flags, SliderResultFlags::DragCancelled))
          {
            DoSendWindowContentChangedEvent(SliderContentChangedReason::DragCanceled);
          }
          return true;
        }
        return false;
      }

      bool SetValue(const value_type& value)
      {
        if (m_logic.SetValue(value))
        {
          PropertyUpdated(PropertyType::Content);
          DoSendWindowContentChangedEvent(SliderContentChangedReason::Set);
          return true;
        }
        return false;
      }

      bool AddValue(const value_type& value)
      {
        return SetValue(m_logic.GetValue() + value);
      }

      bool SubValue(const value_type& value)
      {
        return SetValue(m_logic.GetValue() - value);
      }

      bool SetRange(const value_type& min, const value_type& max)
      {
        if (m_logic.SetRange(min, max))
        {
          PropertyUpdated(PropertyType::Other);
          DoSendWindowContentChangedEvent(SliderContentChangedReason::Set);
          return true;
        }
        return false;
      }

      bool SetTickFrequency(const value_type tickFrequency)
      {
        if (m_logic.SetTickFrequency(tickFrequency))
        {
          PropertyUpdated(PropertyType::Other);
          DoSendWindowContentChangedEvent(SliderContentChangedReason::Set);
          return true;
        }
        return false;
      }

    protected:
      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);

        if (!m_logic.IsEnabled())
        {
          return;
        }

        auto pos = PointFromScreen(theEvent->GetScreenPosition());
        auto offsetPx = (m_orientation == LayoutOrientation::Horizontal ? pos.X : pos.Y);
        if (!m_logic.IsDragging())
        {
          if (theEvent->IsBegin() && !theEvent->IsRepeat())
          {
            const auto renderExtent = RenderExtentPx();
            PxRectangle2D barClickRect(0, 0, renderExtent.Width, renderExtent.Height);

            if (barClickRect.Contains(pos))
            {
              if (m_logic.TryBeginDrag(offsetPx))
              {
                DoSendWindowContentChangedEvent(SliderContentChangedReason::DragBegin);
                theEvent->Handled();
              }
            }
          }
        }
        else
        {
          if (theEvent->IsBegin() && theEvent->IsRepeat())
          {
            auto initialValue = m_logic.GetValue();
            if (m_logic.TryDrag(offsetPx) && m_logic.GetValue() != initialValue)
            {
              DoSendWindowContentChangedEvent(SliderContentChangedReason::Drag);
            }
          }
          else
          {
            if (m_logic.EndDrag(offsetPx))
            {
              DoSendWindowContentChangedEvent(SliderContentChangedReason::DragEnd);
            }
          }
          theEvent->Handled();
        }
      }

      int32_t GetCursorPositionPx() const
      {
        return m_logic.GetPositionPx();
      }

      const SliderPixelSpanInfo& GetRenderInfo() const
      {
        return m_logic.GetSpanInfo();
      }

      //! @brief Allow the inheriting class to set the render information
      inline void SetSpanInfo(const SliderPixelSpanInfo& spanInfo)
      {
        m_logic.SetSpanInfo(spanInfo);
      }

    private:
      void DoSendWindowContentChangedEvent(SliderContentChangedReason reason)
      {
        if (IsReadyToSendEvents())
        {
          SendEvent(GetEventPool()->AcquireWindowContentChangedEvent(0, static_cast<int32_t>(m_logic.GetValue()), static_cast<int32_t>(reason)));
        }
      }
    };
  }
}

#endif
