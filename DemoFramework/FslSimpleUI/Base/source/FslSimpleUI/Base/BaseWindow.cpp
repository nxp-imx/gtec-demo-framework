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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {

    struct RoutedEventArgs
    {
    };

    namespace
    {
      inline float CalcAlignment(const ItemAlignment alignment, const float delta)
      {
        switch (alignment)
        {
        case ItemAlignment::Center:
          return delta / 2.0f;
        case ItemAlignment::Far:
          return delta;
        case ItemAlignment::Near:
        default:
          return 0;
        }
      }

      template<typename T, typename Y>
      inline std::shared_ptr<T> SafeDynamicPointerCast(const std::shared_ptr<Y>& ptr)
      {
        auto converted = std::dynamic_pointer_cast<T>(ptr);
        if (!converted)
          throw UsageErrorException("Internal error the object is not of the expected type");
        return converted;
      }
    }


    BaseWindow::BaseWindow(const std::shared_ptr<WindowContext>& context)
      : m_context(context)
      , m_size(-1, -1)
      , m_margin()
      , m_alignmentX(ItemAlignment::Near)
      , m_alignmentY(ItemAlignment::Near)
      , m_flags()
      , m_layoutCache()
    {
    }


    BaseWindow::~BaseWindow()
    {
    }


    bool BaseWindow::WinMarkLayoutAsDirty()
    {
      if (m_flags.IsEnabled(BaseWindowFlags::LayoutDirty))
        return false;
      m_flags.Enable(BaseWindowFlags::LayoutDirty);
      return true;
    }


    void BaseWindow::WinHandleEvent(const RoutedEvent& routedEvent)
    {
      RoutedEventArgs routedEventArgs;
      switch (routedEvent.Content->GetEventTypeId())
      {
      case EventTypeId::InputClick:
      {
        auto event = SafeDynamicPointerCast<WindowInputClickEvent>(routedEvent.Content);
        if (routedEvent.IsTunneling)
          OnClickInputPreview(routedEventArgs, event);
        else
          OnClickInput(routedEventArgs, event);
      }
      break;
      case EventTypeId::Select:
      {
        auto event = SafeDynamicPointerCast<WindowSelectEvent>(routedEvent.Content);
        assert(!routedEvent.IsTunneling);
        OnSelect(routedEventArgs, event);
      }
      break;
      case EventTypeId::ContentChanged:
      {
        auto event = SafeDynamicPointerCast<WindowContentChangedEvent>(routedEvent.Content);
        assert(!routedEvent.IsTunneling);
        OnContentChanged(routedEventArgs, event);
      }
      break;
      }
    }


    void BaseWindow::Arrange(const Rect& finalRect)
    {
      if (IsLayoutDirty() || !EqualHelper::IsAlmostEqual(finalRect, m_layoutCache.ArrangeLastFinalRect))
      {
        m_layoutCache.ArrangeLastFinalRect = finalRect;

        MarkLayoutArrangeBegin();
        try
        {
          float marginWidth = m_margin.SumX();
          float marginHeight = m_margin.SumY();

          float finalWidth = (m_alignmentX != ItemAlignment::Stretch ? m_layoutCache.DesiredSize.X : finalRect.Width());
          float finalHeight = (m_alignmentY != ItemAlignment::Stretch ? m_layoutCache.DesiredSize.Y : finalRect.Height());

          // Calculate the actual available space.
          const float availW = finalWidth - marginWidth;
          const float availH = finalHeight - marginHeight;
          const float finalW = finalRect.Width() - marginWidth;
          const float finalH = finalRect.Height() - marginHeight;

          const Vector2 availableSize((availW >= 0.0f ? availW : 0.0f), (availH >= 0.0f ? availH : 0.0f));
          const Vector2 actualAvailableSize((finalW >= 0.0f ? finalW : 0.0f), (finalH >= 0.0f ? finalH : 0.0f));
          Vector2 renderSize = ArrangeOverride(availableSize);

          // validate the result
          assert(!isnan(renderSize.X));
          assert(!isnan(renderSize.Y));
          assert(!isinf(renderSize.X));
          assert(!isinf(renderSize.Y));
          assert(renderSize.X >= 0);
          assert(renderSize.Y >= 0);

          // cap it
          renderSize.X = std::max(renderSize.X, 0.0f);
          renderSize.Y = std::max(renderSize.Y, 0.0f);

          const float dx = (actualAvailableSize.X - renderSize.X);
          const float dy = (actualAvailableSize.Y - renderSize.Y);
          const float alignmentOffsetX = CalcAlignment(m_alignmentX, dx);
          const float alignmentOffsetY = CalcAlignment(m_alignmentY, dy);

          m_layoutCache.ContentRect = Rect((finalRect.Left() + m_margin.Left() + alignmentOffsetX), (finalRect.Top() + m_margin.Top() + alignmentOffsetY), renderSize.X, renderSize.Y);
          m_layoutCache.ClippedContentRect = Rect::Intersect(m_layoutCache.ContentRect, finalRect);
          m_layoutCache.RenderSize = Vector2(m_layoutCache.ContentRect.Width(), m_layoutCache.ContentRect.Height());
          MarkLayoutArrangeEnd();
        }
        catch (...)
        {
          MarkLayoutArrangeEnd();
          throw;
        }
        SetLayoutDirty(false);
      }
    }


    void BaseWindow::Measure(const Vector2& availableSize)
    {
      //if (IsLayoutDirty() || !EqualHelper::IsAlmostEqual(availableSize, m_layoutCache.MeasureLastAvailableSize))
      {
        m_layoutCache.MeasureLastAvailableSize = availableSize;

        MarkLayoutMeasureBegin();
        try
        {
          const float marginWidth = m_margin.SumX();
          const float marginHeight = m_margin.SumY();

          // calc local available size by removing required margin
          const float availX = availableSize.X - marginWidth;
          const float availY = availableSize.Y - marginHeight;
          Vector2 localAvailableSpace(std::max(availX, 0.0f), std::max(availY, 0.0f));

          // Calc the minimum desired content size.
          Vector2 minContentSize = MeasureOverride(localAvailableSpace);

          assert(minContentSize.X >= 0.0f);
          assert(minContentSize.Y >= 0.0f);
          assert(!isnan(minContentSize.X));
          assert(!isnan(minContentSize.Y));
          assert(!isinf(minContentSize.X));
          assert(!isinf(minContentSize.Y));

          // Apply fixed width and height if set
          minContentSize.X = (m_size.X < 0.0f ? minContentSize.X : m_size.X);
          minContentSize.Y = (m_size.Y < 0.0f ? minContentSize.Y : m_size.Y);

          // Reapply margin to the desired space
          const float width = minContentSize.X + marginWidth;
          const float height = minContentSize.Y + marginHeight;

          const Vector2 desiredSize(std::max(width, 0.0f), std::max(height, 0.0f));

          // Validate the desired size
          {
            assert(!isnan(desiredSize.X));
            assert(!isnan(desiredSize.Y));
            assert(!isinf(desiredSize.X));
            assert(!isinf(desiredSize.Y));

            // handle invalid measure results.
            if (isnan(desiredSize.X) || isnan(desiredSize.Y))
              throw UsageErrorException("MeasureOverride returned NaN!");
            if (isinf(desiredSize.X) || isinf(desiredSize.Y))
              throw UsageErrorException("MeasureOverride returned infinity!");
          }

          m_layoutCache.DesiredSize = desiredSize;
          MarkLayoutMeasureEnd();
        }
        catch (...)
        {
          MarkLayoutMeasureEnd();
          throw;
        }
      }
    }


    void BaseWindow::SetWidth(const float value)
    {
      if (!EqualHelper::IsAlmostEqual(value, m_size.X))
      {
        m_size.X = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BaseWindow::SetWidth(const int32_t value)
    {
      const float floatValue = static_cast<float>(value);
      if (floatValue != m_size.X)
      {
        m_size.X = floatValue;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BaseWindow::SetHeight(const float value)
    {
      if (!EqualHelper::IsAlmostEqual(value, m_size.Y))
      {
        m_size.Y = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BaseWindow::SetHeight(const int32_t value)
    {
      const float floatValue = static_cast<float>(value);
      if (floatValue != m_size.Y)
      {
        m_size.Y = floatValue;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BaseWindow::SetMargin(const ThicknessF& value)
    {
      if (value != m_margin)
      {
        m_margin = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BaseWindow::SetAlignmentX(const ItemAlignment& value)
    {
      if (value != m_alignmentX)
      {
        m_alignmentX = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void BaseWindow::SetAlignmentY(const ItemAlignment& value)
    {
      if (value != m_alignmentY)
      {
        m_alignmentY = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    Vector2 BaseWindow::PointFromScreen(const Vector2& screenPoint) const
    {
      return m_context->WindowManager->PointFromScreen(this, screenPoint);
    }


    Vector2 BaseWindow::PointFrom(const IWindowId*const pFromWin, const Vector2& point) const
    {
      auto screenPoint = m_context->WindowManager->PointToScreen(pFromWin, point);
      return m_context->WindowManager->PointFromScreen(this, screenPoint);
    }


    Vector2 BaseWindow::PointTo(const IWindowId*const pToWin, const Vector2& point) const
    {
      auto screenPoint = m_context->WindowManager->PointToScreen(this, point);
      return m_context->WindowManager->PointFromScreen(pToWin, screenPoint);
    }


    void BaseWindow::SendEvent(const std::shared_ptr<WindowEvent>& event)
    {
      m_context->EventSender->SendEvent(event, this);
    }


    const std::shared_ptr<WindowEventPool>& BaseWindow::GetEventPool() const
    {
      return m_context->EventSender->EventPool;
    }


    void BaseWindow::Enable(const WindowFlags& flags)
    {
      if (!m_flags.IsEnabled(flags))
      {
        m_flags.Enable(flags);
        if (m_context)
          m_context->WindowManager->TrySetWindowFlags(this, flags);
      }
    }


    void BaseWindow::PropertyUpdated(const PropertyTypeFlags& flags)
    {
      // Validate that the caller supplied valid flags
      assert((flags.Value & (~PropertyTypeFlags::MASK_PropertyTypeFlags)) == 0);

      if (!m_flags.IsEnabled(BaseWindowFlags::InBatchPropertyUpdate))
      {
        // Check if any of the layout related flags were set and if somebody marked or layout as dirty or not
        if (flags.IsLayoutRelated() && !IsLayoutDirty())
        {
          // A layout related flag was set and we are not marked as dirty
          // -> so lets flag this window and its parents
          SetLayoutDirty(true);
        }

        // Let a control know a flag was changed
        OnPropertiesUpdated(flags);
      }
      else
        m_flags.Value |= flags.GetSafeValue();
    }


    void BaseWindow::SetLayoutDirty(const bool isDirty)
    {
      if (isDirty != m_flags.IsEnabled(BaseWindowFlags::LayoutDirty))
      {
        if (isDirty)
        {
          m_context->WindowManager->TrySetWindowFlags(this, WindowFlags::LayoutDirty);
        }
        else
          m_flags.Disable(BaseWindowFlags::LayoutDirty);
      }
    }
  }
}
