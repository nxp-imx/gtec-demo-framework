/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslSimpleUI/Base/Control/FloatSlider.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "Impl/Draw9SliceImpl.hpp"

namespace Fsl
{
  namespace UI
  {
    FloatSlider::FloatSlider(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
      , m_percentage(0)
      , m_value(0.0f)
      , m_minValue(0.0f)
      , m_maxValue(100.0f)
      , m_dragState(DragState::Idle)
      , m_dragStartPos(0)
      , m_dragOffset(0)
      , m_renderXMin(0)
      , m_renderXMax(0)
    {
      // We need to be draw enabled, accept click input and receive a notification on init
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput));
    }


    bool FloatSlider::IsIdle() const
    {
      return m_dragState == DragState::Idle;
    }


    void FloatSlider::SetBackgroundTexture(const AtlasTexture2D& value)
    {
      if (value == m_texBackground)
      {
        return;
      }
      m_texBackground = value;
      PropertyUpdated(PropertyType::Content);
    }


    void FloatSlider::SetCursorTexture(const AtlasTexture2D& value)
    {
      if (value == m_texCursor)
      {
        return;
      }
      m_texCursor = value;
      PropertyUpdated(PropertyType::Content);
    }


    void FloatSlider::SetValue(const float& value)
    {
      DoSetValue(value, 0);
    }


    bool FloatSlider::SetValueLimits(const float& min, const float& max)
    {
      if (min > max)
      {
        throw std::invalid_argument("min must be <= max");
      }

      if (min != m_minValue || max != m_maxValue)
      {
        m_minValue = min;
        m_maxValue = max;
        m_value = std::max(std::min(m_value, m_maxValue), m_minValue);
        RecalcPercentage();
        PropertyUpdated(PropertyType::Other);
        return true;
      }
      return false;
    }


    void FloatSlider::SetBackgroundPadding(const ThicknessF& value)
    {
      if (value == m_backgroundPadding)
      {
        return;
      }
      m_backgroundPadding = value;
      PropertyUpdated(PropertyType::Layout);
    }


    void FloatSlider::SetCursorPadding(const ThicknessF& value)
    {
      if (value == m_cursorPadding)
      {
        return;
      }
      m_cursorPadding = value;
      PropertyUpdated(PropertyType::Layout);
    }


    void FloatSlider::SetNineSlice(const NineSlice& value)
    {
      if (value == m_nineSlice)
      {
        return;
      }
      m_nineSlice = value;
      PropertyUpdated(PropertyType::Other);
    }


    void FloatSlider::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);


      const Vector2 renderSize = context.TargetRect.GetSize();
      const Vector2 dstPos = context.TargetRect.TopLeft();

      // -> RenderSize.X                 |--------------------|
      //    xBackground, dxBackground    .|------------------|.
      //    xCursor, dxCursor            ..|----------------|..


      const float dxBackground = renderSize.X - m_backgroundPadding.SumX();
      const float dyBackground = renderSize.Y - m_backgroundPadding.SumY();
      const float xBackground = dstPos.X + m_backgroundPadding.Left();
      const float yBackground = dstPos.Y + m_backgroundPadding.Top();

      const float dxCursor = dxBackground - m_cursorPadding.SumX();
      // const float dyCursor = dyBackground - m_cursorPadding.SumY();
      const float xCursor = xBackground + m_cursorPadding.Left();
      const float yCursor = yBackground + m_cursorPadding.Top();
      float cursorX = xCursor;
      float cursorY = yCursor + (dyBackground / 2);
      float renderXMin = xCursor;
      float renderXMax = xCursor + dxCursor;

      // Add current value
      {
        float currentPercentage = (m_dragState != DragState::Dragging ? m_percentage : 0.0f);
        cursorX += dxCursor * currentPercentage;
      }

      if (m_dragState == DragState::Dragging)
      {
        cursorX = dstPos.X + m_dragOffset;
        if (cursorX <= renderXMin)
        {
          cursorX = renderXMin;
        }
        else if (cursorX >= renderXMax)
        {
          cursorX = renderXMax;
        }
      }

      // Scale the cursor to fit the height of the Slider (keeping the aspect ratio)
      const Point2 cursorSize = m_texCursor.IsValid() ? m_texCursor.GetSize() : Point2();
      const float scale = (cursorSize.Y > 0 ? renderSize.Y / cursorSize.Y : 1.0f);
      const float texWidth = cursorSize.X * scale;
      Rect cursorRect(cursorX - (texWidth * 0.5f), cursorY - (renderSize.Y * 0.5f), texWidth, renderSize.Y);
      Rect dstRect(xBackground, yBackground, dxBackground, dyBackground);

      // make the positions relative to the window
      m_renderXMin = renderXMin - dstPos.X;
      m_renderXMax = renderXMax - dstPos.X;
      // m_barClickRect = Rect(dstRect.X() - dstPos.X, dstRect.Y() - dstPos.Y, dstRect.Width(), dstRect.Height());
      m_barClickRect = Rect(0, 0, renderSize.X, renderSize.Y);

      // Do the actual drawing
      {
        const auto batch = m_windowContext->Batch2D;
        if (m_texBackground.IsValid())
        {
          Draw9SliceImpl::WinDraw(batch, dstRect, m_texBackground, m_nineSlice, ThicknessF(), Color::White());
        }

        if (m_texCursor.IsValid())
        {
          batch->Draw(m_texCursor, cursorRect, Color::White());
        }
      }
    }


    void FloatSlider::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      auto pos = PointFromScreen(theEvent->GetScreenPosition());
      if (m_dragState == DragState::Idle)
      {
        if (theEvent->IsBegin() && !theEvent->IsRepeat())
        {
          if (m_barClickRect.Contains(pos))
          {
            SetValueBasedOnPosition(pos.X);
            m_dragState = DragState::Dragging;
            m_dragOffset = pos.X;
            m_dragStartPos = pos.X;
            theEvent->Handled();
          }
        }
      }
      else
      {
        if (theEvent->IsBegin() && theEvent->IsRepeat())
        {
          m_dragOffset = pos.X;
          SetValueBasedOnPosition(pos.X);
        }
        else
        {
          m_dragState = DragState::Idle;
          SetValueBasedOnPosition(pos.X);
        }
        theEvent->Handled();
      }
    }


    Vector2 FloatSlider::ArrangeOverride(const Vector2& finalSize)
    {
      return finalSize;
    }


    Vector2 FloatSlider::MeasureOverride(const Vector2& availableSize)
    {
      if (!m_texBackground.IsValid())
      {
        return Vector2();
      }
      return Vector2(m_texBackground.GetSize().X, m_texBackground.GetSize().Y);
    }


    void FloatSlider::RecalcPercentage()
    {
      const auto delta = m_maxValue - m_minValue;
      const auto value = m_value - m_minValue;
      m_percentage = delta != 0.0f ? value / delta : 0.0f;
    }


    void FloatSlider::SetValueBasedOnPosition(const float position)
    {
      if (position < m_renderXMin || EqualHelper::IsAlmostEqual(position, m_renderXMin))
      {
        DoSetValue(m_minValue, 1);
      }
      else if (position > m_renderXMax || EqualHelper::IsAlmostEqual(position, m_renderXMax))
      {
        DoSetValue(m_maxValue, 1);
      }
      else
      {
        const float value = position - m_renderXMin;
        const float percentage = value / (m_renderXMax - m_renderXMin);
        const auto delta = (m_maxValue - m_minValue);
        DoSetValue(std::min(std::max((delta * percentage) + m_minValue, m_minValue), m_maxValue), 1);
      }
    }


    void FloatSlider::DoSetValue(const float& value, const int32_t reason)
    {
      const float cappedValue = std::max(std::min(value, m_maxValue), m_minValue);
      if (!EqualHelper::IsAlmostEqual(cappedValue, m_value))
      {
        m_value = cappedValue;
        RecalcPercentage();
        PropertyUpdated(PropertyType::Other);
        if (IsReadyToSendEvents())
        {
          SendEvent(GetEventPool()->AcquireWindowContentChangedEvent(0, 0, reason));
        }
      }
    }
  }
}
