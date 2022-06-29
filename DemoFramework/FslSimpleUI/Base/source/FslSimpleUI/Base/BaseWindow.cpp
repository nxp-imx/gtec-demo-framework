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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/EqualHelper_DpValueF.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/RoutedEventArgs.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/ItemAlignmentUtil.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>


namespace Fsl::UI
{
  using TClass = BaseWindow;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyWidthDp = TFactory::Create<DpLayoutSize1D, TClass, &TClass::GetWidth, &TClass::SetWidth>("Width");
  TDef TClass::PropertyHeightDp = TFactory::Create<DpLayoutSize1D, TClass, &TClass::GetHeight, &TClass::SetHeight>("Height");
  TDef TClass::PropertyMarginDpf = TFactory::Create<DpThicknessF, TClass, &TClass::GetMargin, &TClass::SetMargin>("Margin");
  TDef TClass::PropertyAlignmentX = TFactory::Create<ItemAlignment, TClass, &TClass::GetAlignmentX, &TClass::SetAlignmentX>("AlignmentX");
  TDef TClass::PropertyAlignmentY = TFactory::Create<ItemAlignment, TClass, &TClass::GetAlignmentY, &TClass::SetAlignmentY>("AlignmentY");
  TDef TClass::PropertyBaseColor = TFactory::Create<Color, TClass, &TClass::GetBaseColor, &TClass::SetBaseColor>("BaseColor");
}

namespace Fsl::UI
{
  namespace
  {
    template <typename T, typename Y>
    inline std::shared_ptr<T> SafeDynamicPointerCast(const std::shared_ptr<Y>& ptr)
    {
      auto converted = std::dynamic_pointer_cast<T>(ptr);
      if (!converted)
      {
        throw UsageErrorException("Internal error the object is not of the expected type");
      }
      return converted;
    }
  }


  BaseWindow::BaseWindow(const std::shared_ptr<BaseWindowContext>& context, const WindowFlags initialFlags)
    : DataBinding::DependencyObject(context->UIDataBindingService)
    , m_context(context)
    , m_propertyAlignmentX(ItemAlignment::Near)
    , m_propertyAlignmentY(ItemAlignment::Near)
    , m_propertyBaseColor(Color::White())
    , m_flags(initialFlags)
  {
    if (!context)
    {
      throw std::invalid_argument("context can not be null");
    }
  }


  BaseWindow::~BaseWindow() = default;


  void BaseWindow::WinResolutionChanged(const ResolutionChangedInfo& /*info*/)
  {
    m_flags.Enable(BaseWindowFlags::LayoutDirty);
  }

  bool BaseWindow::WinMarkLayoutAsDirty()
  {
    if (m_flags.IsEnabled(BaseWindowFlags::LayoutDirty))
    {
      return false;
    }
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
        {
          OnClickInputPreview(routedEventArgs, event);
        }
        else
        {
          OnClickInput(routedEventArgs, event);
        }
        break;
      }
    case EventTypeId::MouseOver:
      {
        auto event = SafeDynamicPointerCast<WindowMouseOverEvent>(routedEvent.Content);
        if (routedEvent.IsTunneling)
        {
          OnMouseOverPreview(routedEventArgs, event);
        }
        else
        {
          OnMouseOver(routedEventArgs, event);
        }
        break;
      }
    case EventTypeId::Select:
      {
        auto event = SafeDynamicPointerCast<WindowSelectEvent>(routedEvent.Content);
        assert(!routedEvent.IsTunneling);
        OnSelect(routedEventArgs, event);
        break;
      }
    case EventTypeId::ContentChanged:
      {
        auto event = SafeDynamicPointerCast<WindowContentChangedEvent>(routedEvent.Content);
        assert(!routedEvent.IsTunneling);
        OnContentChanged(routedEventArgs, event);
        break;
      }
    default:
      FSLLOG3_WARNING("Unhandled eventTypeId: {}", int32_t(routedEvent.Content->GetEventTypeId()));
      break;
    }
    SetAnimationUpdate(UpdateAnimationState(false));
  }

  void BaseWindow::WinUpdate(const TimeSpan& timespan)
  {
    UpdateAnimation(timespan);
    SetAnimationUpdate(UpdateAnimationState(false));
  }

  void BaseWindow::Arrange(const PxRectangle& finalRectPx)
  {
    if (IsLayoutDirty() || finalRectPx != m_layoutCache.ArrangeLastFinalRectPx)
    {
      m_layoutCache.ArrangeLastFinalRectPx = finalRectPx;

      MarkLayoutArrangeBegin();
      try
      {
        if (m_flags.GetVisibility() != ItemVisibility::Collapsed)
        {
          const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

          const PxThickness marginThicknessPx = unitConverter.ToPxThickness(m_propertyMarginDpf.Get());
          const PxSize2D marginPx(marginThicknessPx.Sum());

          const ItemAlignment alignmentX = m_propertyAlignmentX.Get();
          const ItemAlignment alignmentY = m_propertyAlignmentY.Get();
          const PxSize2D spacePx(alignmentX != ItemAlignment::Stretch ? m_layoutCache.DesiredSizePx.Width() : finalRectPx.Width(),
                                 alignmentY != ItemAlignment::Stretch ? m_layoutCache.DesiredSizePx.Height() : finalRectPx.Height());

          // Calculate the actual available space.
          const PxSize2D arrangeSizePx(PxSize2D::Subtract(spacePx, marginPx));

          const PxSize2D renderSizePx = ArrangeOverride(arrangeSizePx);

          // validate the result (even though PxLayoutSize promises to be >= 0)
          assert(renderSizePx.Width() >= 0);
          assert(renderSizePx.Height() >= 0);

          // Calc alignment
          const PxSize2D actualAvailableSizePx(finalRectPx.Width() - marginPx.Width(), finalRectPx.Height() - marginPx.Height());
          const PxPoint2 deltaPx(actualAvailableSizePx.Width() - renderSizePx.Width(), actualAvailableSizePx.Height() - renderSizePx.Height());
          const int32_t alignmentOffsetXPx = ItemAlignmentUtil::CalcAlignmentPx(alignmentX, deltaPx.X);
          const int32_t alignmentOffsetYPx = ItemAlignmentUtil::CalcAlignmentPx(alignmentY, deltaPx.Y);

          const PxPoint2 posPx(finalRectPx.X() + marginThicknessPx.Left() + alignmentOffsetXPx,
                               finalRectPx.Y() + marginThicknessPx.Top() + alignmentOffsetYPx);

          m_layoutCache.ContentRectPx = PxRectangle(posPx.X, posPx.Y, renderSizePx.Width(), renderSizePx.Height());
          m_layoutCache.ClippedContentRectPx = PxRectangle::Intersect(m_layoutCache.ContentRectPx, finalRectPx);
          m_layoutCache.RenderSizePx = renderSizePx;
        }
        else
        {
          m_layoutCache.ContentRectPx = {};
          m_layoutCache.ClippedContentRectPx = {};
          m_layoutCache.RenderSizePx = {};
        }
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


  void BaseWindow::Measure(const PxAvailableSize& availableSizePx)
  {
    if (IsLayoutDirty() || availableSizePx != m_layoutCache.MeasureLastAvailableSizePx)
    {
      m_layoutCache.MeasureLastAvailableSizePx = availableSizePx;

      MarkLayoutMeasureBegin();
      try
      {
        if (m_flags.GetVisibility() != ItemVisibility::Collapsed)
        {
          const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

          const PxThickness marginThicknessPx = unitConverter.ToPxThickness(m_propertyMarginDpf.Get());
          const PxSize2D marginSizePx(marginThicknessPx.Sum());

          // calc local available size by removing required margin (the subtract has build in clamping so it cant become negative
          const PxAvailableSize localAvailableSpacePx(PxAvailableSize::Subtract(availableSizePx, marginSizePx));

          // Calc the minimum desired content size.
          PxSize2D minContentSizePx = MeasureOverride(localAvailableSpacePx);

          // Apply fixed width and height if set
          const DpLayoutSize1D widthDp = m_propertyWidthDp.Get();
          const DpLayoutSize1D heightDp = m_propertyHeightDp.Get();
          if (widthDp.HasValue())
          {
            minContentSizePx.SetWidth(unitConverter.ToPxInt32(widthDp.Value()), OptimizationCheckFlag::NoCheck);
          }
          if (heightDp.HasValue())
          {
            minContentSizePx.SetHeight(unitConverter.ToPxInt32(heightDp.Value()), OptimizationCheckFlag::NoCheck);
          }

          // Reapply margin to the desired space (Add ensures it wont be negative)
          PxSize2D desiredSizePx(PxSize2D::Add(minContentSizePx, marginSizePx));
          m_layoutCache.DesiredSizePx = desiredSizePx;
        }
        else
        {
          m_layoutCache.DesiredSizePx = {};
        }
        MarkLayoutMeasureEnd();
      }
      catch (...)
      {
        MarkLayoutMeasureEnd();
        throw;
      }
    }
  }


  bool BaseWindow::SetWidth(const DpLayoutSize1D value)
  {
    const bool changed = m_propertyWidthDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool BaseWindow::SetHeight(const DpLayoutSize1D value)
  {
    const bool changed = m_propertyHeightDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool BaseWindow::SetMargin(const DpThicknessF value)
  {
    const bool changed = m_propertyMarginDpf.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool BaseWindow::SetAlignmentX(const ItemAlignment value)
  {
    const bool changed = m_propertyAlignmentX.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  bool BaseWindow::SetAlignmentY(const ItemAlignment value)
  {
    const bool changed = m_propertyAlignmentY.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Alignment);
    }
    return changed;
  }


  bool BaseWindow::SetBaseColor(const Color value)
  {
    const bool changed = m_propertyBaseColor.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_cachedBaseColor = m_propertyBaseColor.Get() * m_parentBaseColor;
      PropertyUpdated(PropertyType::BaseColor);
    }
    return changed;
  }


  void BaseWindow::SetVisibility(const ItemVisibility value)
  {
    if (m_flags.GetVisibility() != value)
    {
      m_flags.SetVisibility(value);
      PropertyUpdated(PropertyType::Layout);

      // Inform the window manager about the change
      auto uiContext = GetContext()->TheUIContext.Get();
      uiContext->WindowManager->TrySetWindowVisibility(this, value);
    }
  }


  PxPoint2 BaseWindow::PointFromScreen(const PxPoint2& screenPointPx) const
  {
    auto uiContext = GetContext()->TheUIContext.Get();
    return uiContext->WindowManager->PointFromScreen(this, screenPointPx);
  }


  PxPoint2 BaseWindow::PointFrom(const IWindowId* const pFromWin, const PxPoint2& pointPx) const
  {
    auto uiContext = GetContext()->TheUIContext.Get();
    const auto& windowManager = uiContext->WindowManager;
    PxPoint2 screenPointPx = windowManager->PointToScreen(pFromWin, pointPx);
    return windowManager->PointFromScreen(this, screenPointPx);
  }


  PxPoint2 BaseWindow::PointTo(const IWindowId* const pToWin, const PxPoint2& pointPx) const
  {
    auto uiContext = GetContext()->TheUIContext.Get();
    const auto& windowManager = uiContext->WindowManager;
    PxPoint2 screenPoint = windowManager->PointToScreen(this, pointPx);
    return windowManager->PointFromScreen(pToWin, screenPoint);
  }


  void BaseWindow::SYS_SetParentBaseColor(const Color color)
  {
    if (color != m_parentBaseColor)
    {
      m_parentBaseColor = color;
      m_cachedBaseColor = m_propertyBaseColor.Get() * m_parentBaseColor;
      PropertyUpdated(PropertyType::BaseColor);
    }
  }


  bool BaseWindow::IsReadyToSendEvents()
  {
    if (m_flags.IsEnabled(BaseWindowFlags::CachedEventReady))
    {
      return true;
    }


    auto uiContext = GetContext()->TheUIContext.Get();
    if (!uiContext->WindowManager->Exists(this))
    {
      return false;
    }

    m_flags.Set(BaseWindowFlags::CachedEventReady, true);
    return true;
  }


  void BaseWindow::SendEvent(const std::shared_ptr<WindowEvent>& event)
  {
    auto uiContext = GetContext()->TheUIContext.Get();
    uiContext->EventSender->SendEvent(event, this);
  }

  // Disabled for now as its easy to forget to release the event back to the pool on failure
  // bool BaseWindow::TrySendEvent(const std::shared_ptr<WindowEvent>& event)
  //{
  //  return m_context->EventSender->TrySendEvent(event, this);
  //}


  const std::shared_ptr<WindowEventPool>& BaseWindow::GetEventPool() const
  {
    auto uiContext = GetContext()->TheUIContext.Get();
    return uiContext->EventSender->EventPool;
  }


  void BaseWindow::Enable(const WindowFlags flags)
  {
    if (!m_flags.IsEnabled(flags))
    {
      m_flags.Enable(flags);

      auto uiContext = GetContext()->TheUIContext.Get();
      uiContext->WindowManager->TrySetWindowFlags(this, flags, true);
    }
  }


  void BaseWindow::Disable(const WindowFlags flags)
  {
    if (m_flags.IsEnabled(flags))
    {
      m_flags.Disable(flags);

      auto uiContext = GetContext()->TheUIContext.Get();
      uiContext->WindowManager->TrySetWindowFlags(this, flags, false);
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

      // Update animation state
      SetAnimationUpdate(UpdateAnimationState(false));
    }
    else
    {
      m_flags.Value |= flags.GetSafeValue();
    }
  }


  void BaseWindow::SetLayoutDirty(const bool isDirty)
  {
    if (isDirty != m_flags.IsEnabled(BaseWindowFlags::LayoutDirty))
    {
      if (isDirty)
      {
        auto uiContext = GetContext()->TheUIContext.Get();
        uiContext->WindowManager->TrySetWindowFlags(this, WindowFlags::LayoutDirty, true);
      }
      else
      {
        m_flags.Disable(BaseWindowFlags::LayoutDirty);
      }
    }
  }

  void BaseWindow::SetAnimationUpdate(const bool isAnimating)
  {
    if (isAnimating)
    {
      if (!IsUpdateEnabled())
      {
        Enable(WindowFlags::UpdateEnabled);
        // FSLLOG3_INFO("Enabling animation"); // LOG FOR NOW
      }
    }
    else
    {
      if (IsUpdateEnabled())
      {
        Disable(WindowFlags::UpdateEnabled);
        // FSLLOG3_INFO("Disabling animation");  // LOG FOR NOW
      }
    }
  }


  DataBinding::DataBindingInstanceHandle BaseWindow::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyWidthDp, m_propertyWidthDp),
      DataBinding::PropLinkRefs(PropertyHeightDp, m_propertyHeightDp), DataBinding::PropLinkRefs(PropertyMarginDpf, m_propertyMarginDpf),
      DataBinding::PropLinkRefs(PropertyAlignmentX, m_propertyAlignmentX), DataBinding::PropLinkRefs(PropertyAlignmentY, m_propertyAlignmentY),
      DataBinding::PropLinkRefs(PropertyBaseColor, m_propertyBaseColor));
    return res.IsValid() ? res : DataBinding::DependencyObject::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult BaseWindow::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                     const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyWidthDp, m_propertyWidthDp),
      DataBinding::PropLinkRefs(PropertyHeightDp, m_propertyHeightDp), DataBinding::PropLinkRefs(PropertyMarginDpf, m_propertyMarginDpf),
      DataBinding::PropLinkRefs(PropertyAlignmentX, m_propertyAlignmentX), DataBinding::PropLinkRefs(PropertyAlignmentY, m_propertyAlignmentY),
      DataBinding::PropLinkRefs(PropertyBaseColor, m_propertyBaseColor));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : DataBinding::DependencyObject::TrySetBindingNow(targetDef, binding);
  }


  void BaseWindow::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    DataBinding::DependencyObject::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyWidthDp);
    rProperties.push_back(PropertyHeightDp);
    rProperties.push_back(PropertyMarginDpf);
    rProperties.push_back(PropertyAlignmentX);
    rProperties.push_back(PropertyAlignmentY);
    rProperties.push_back(PropertyBaseColor);
  }


}
