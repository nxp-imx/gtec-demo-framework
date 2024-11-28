/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/TypeConverter_Math.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Control/ToggleButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/ItemAlignmentUtil.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

// #include <FslBase/Log/Math/Pixel/FmtPxVector2.hpp>

namespace Fsl::UI
{
  using TClass = ToggleButton;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyFontColorChecked =
    TFactory::Create<UIColor, TClass, &TClass::GetFontColorChecked, &TClass::SetFontColorChecked>("FontColorChecked");
  TDef TClass::PropertyFontColorUnchecked =
    TFactory::Create<UIColor, TClass, &TClass::GetFontColorUnchecked, &TClass::SetFontColorChecked>("FontColorUnchecked");
  TDef TClass::PropertyFontColorDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetFontColorDisabled, &TClass::SetFontColorDisabled>("FontColorDisabled");
  TDef TClass::PropertyCursorColorChecked =
    TFactory::Create<UIColor, TClass, &TClass::GetCursorColorChecked, &TClass::SetCursorColorChecked>("CursorColorChecked");
  TDef TClass::PropertyCursorColorUnchecked =
    TFactory::Create<UIColor, TClass, &TClass::GetCursorColorUnchecked, &TClass::SetCursorColorUnchecked>("CursorColorUnchecked");
  TDef TClass::PropertyCursorColorCheckedDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetCursorColorCheckedDisabled, &TClass::SetCursorColorCheckedDisabled>("CursorColorCheckedDisabled");
  TDef TClass::PropertyCursorColorUncheckedDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetCursorColorUncheckedDisabled, &TClass::SetCursorColorUncheckedDisabled>(
      "CursorColorUncheckedDisabled");
  TDef TClass::PropertyBackgroundColorChecked =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorChecked, &TClass::SetBackgroundColorChecked>("BackgroundColorChecked");
  TDef TClass::PropertyBackgroundColorUnchecked =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorUnchecked, &TClass::SetBackgroundColorUnchecked>("BackgroundColorUnchecked");
  TDef TClass::PropertyBackgroundColorCheckedDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorCheckedDisabled, &TClass::SetBackgroundColorCheckedDisabled>(
      "BackgroundColorCheckedDisabled");
  TDef TClass::PropertyBackgroundColorUncheckedDisabled =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColorUncheckedDisabled, &TClass::SetBackgroundColorUncheckedDisabled>(
      "BackgroundColorUncheckedDisabled");
  TDef TClass::PropertyHoverOverlayCheckedColor =
    TFactory::Create<UIColor, TClass, &TClass::GetHoverOverlayCheckedColor, &TClass::SetHoverOverlayCheckedColor>("HoverOverlayCheckedColor");
  TDef TClass::PropertyHoverOverlayUncheckedColor =
    TFactory::Create<UIColor, TClass, &TClass::GetHoverOverlayUncheckedColor, &TClass::SetHoverOverlayUncheckedColor>("HoverOverlayUncheckedColor");
  TDef TClass::PropertyImageAlignment =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetImageAlignment, &TClass::SetImageAlignment>("ImageAlignment");
  TDef TClass::PropertyIsChecked = TFactory::Create<bool, TClass, &TClass::IsChecked, &TClass::SetIsChecked>("Checked");
  TDef TClass::PropertyIsEnabled = TFactory::Create<bool, TClass, &TClass::IsEnabled, &TClass::SetEnabled>("Enabled");
  TDef TClass::PropertyText = TFactory::Create<StringViewLite, TClass, &TClass::GetText, &TClass::SetText>("Text");
  TDef TClass::PropertyTextAlignment = TFactory::Create<ItemAlignment, TClass, &TClass::GetTextAlignment, &TClass::SetTextAlignment>("TextAlignment");
  TDef TClass::PropertyTextLocation = TFactory::Create<ItemTextLocation, TClass, &TClass::GetTextLocation, &TClass::SetTextLocation>("TextLocation");
}

namespace Fsl::UI
{
  ToggleButton::ToggleButton(const std::shared_ptr<WindowContext>& context)
    : BaseWindow(context)
    , m_windowContext(context)
    , m_font(context->TheUIContext.Get()->MeshManager, context->DefaultFont, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType,
             context->ColorConverter)
    , m_cursor(context->TheUIContext.Get()->MeshManager, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType,
               context->ColorConverter)
    , m_background(context->TheUIContext.Get()->MeshManager, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType,
                   context->ColorConverter)
    , m_hoverOverlay(context->TheUIContext.Get()->MeshManager, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType,
                     DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType, context->ColorConverter)
  {
    assert(m_font.Mesh.GetSprite());
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput | WindowFlags::MouseOver));
    UpdateAnimationState(true);
  }


  bool ToggleButton::SetEnabled(const bool value)
  {
    const bool changed = m_propertyIsEnabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetTextAlignment(const ItemAlignment value)
  {
    const bool changed = m_propertyTextAlignment.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetTextLocation(const ItemTextLocation value)
  {
    const bool changed = m_propertyTextLocation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetImageAlignment(const ItemAlignment value)
  {
    const bool changed = m_propertyImageAlignment.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void ToggleButton::SetFont(const std::shared_ptr<SpriteFont>& value)
  {
    if (!value)
    {
      throw std::invalid_argument("font can not be null");
    }

    if (m_font.Mesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool ToggleButton::SetText(const StringViewLite strView)
  {
    const bool changed = m_propertyText.Set(ThisDependencyObject(), strView);
    if (changed)
    {
      m_font.Mesh.SetText(strView);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetText(std::string&& value)
  {
    const bool changed = m_propertyText.Set(ThisDependencyObject(), std::move(value));
    if (changed)
    {
      m_font.Mesh.SetText(m_propertyText.Get());
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetHoverOverlayCheckedColor(const UIColor value)
  {
    const bool changed = m_hoverOverlay.Checked.PrimaryColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetHoverOverlayUncheckedColor(const UIColor value)
  {
    const bool changed = m_hoverOverlay.Unchecked.PrimaryColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void ToggleButton::SetCursorCheckedPosition(const DpPoint2& valueDp)
  {
    if (valueDp != m_hoverOverlay.Checked.PositionDp)
    {
      m_hoverOverlay.Checked.PositionDp = valueDp;
      PropertyUpdated(PropertyType::Other);
    }
  }

  void ToggleButton::SetCursorUncheckedPosition(const DpPoint2& valueDp)
  {
    if (valueDp != m_hoverOverlay.Unchecked.PositionDp)
    {
      m_hoverOverlay.Unchecked.PositionDp = valueDp;
      PropertyUpdated(PropertyType::Other);
    }
  }

  void ToggleButton::SetHoverOverlaySprite(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_hoverOverlay.Mesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool ToggleButton::SetCursorColorChecked(const UIColor value)
  {
    const bool changed = m_cursor.PropertyColorChecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ToggleButton::SetCursorColorUnchecked(const UIColor value)
  {
    const bool changed = m_cursor.PropertyColorUnchecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetCursorColorCheckedDisabled(const UIColor value)
  {
    const bool changed = m_cursor.PropertyColorCheckedDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetCursorColorUncheckedDisabled(const UIColor value)
  {
    const bool changed = m_cursor.PropertyColorUncheckedDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorChecked(const UIColor value)
  {
    const bool changed = m_background.PropertyColorChecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorUnchecked(const UIColor value)
  {
    const bool changed = m_background.PropertyColorUnchecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorCheckedDisabled(const UIColor value)
  {
    const bool changed = m_background.PropertyColorCheckedDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorUncheckedDisabled(const UIColor value)
  {
    const bool changed = m_background.PropertyColorUncheckedDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorChecked(const UIColor value)
  {
    const bool changed = m_font.PropertyColorChecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorUnchecked(const UIColor value)
  {
    const bool changed = m_font.PropertyColorUnchecked.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorDisabled(const UIColor value)
  {
    const bool changed = m_font.PropertyColorDisabled.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  void ToggleButton::SetCheckedSprite(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_cursor.Mesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ToggleButton::SetUncheckedSprite(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_background.Mesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool ToggleButton::SetIsChecked(const bool value)
  {
    const bool changed = m_propertyIsChecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);

      if (IsReadyToSendEvents())
      {
        SendEvent(GetEventPool()->AcquireWindowContentChangedEvent(0));
      }
    }
    return changed;
  }


  void ToggleButton::Toggle()
  {
    SetIsChecked(!IsChecked());
  }


  void ToggleButton::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    const PxVector2 positionPxf = context.TargetRect.TopLeft();
    const PxSize2D renderSizePx = RenderSizePx();
    const UIRenderColor finalColor(GetFinalBaseColor());

    // Draw the text if it has been set
    {
      const UIRenderColor fontColor = m_font.CurrentColor.GetValue();
      if (m_font.Mesh.IsValid() && !m_propertyText.Get().empty() && fontColor.RawA() > 0)
      {
        PxValue centeredYPx;
        if (m_cachedFontMeasureInfo.MeasureSizePx.Height() < renderSizePx.Height())
        {
          // Center the round to nearest pixel
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - m_cachedFontMeasureInfo.MeasureSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(m_cachedTextOffsetPx), positionPxf.Y + PxValueF(centeredYPx));
        context.CommandBuffer.Draw(m_font.Mesh.Get(), dstPositionPxf, m_cachedFontMeasureInfo.MinimalSizePx, finalColor * fontColor,
                                   context.ClipContext);
      }
    }

    // Draw the ToggleButton
    {
      if (m_background.Mesh.IsValid())    // Background mesh
      {
        PxValue centeredYPx;
        const auto backgroundSpriteRenderSizePx = m_background.Mesh.FastGetRenderSizePx();
        if (backgroundSpriteRenderSizePx.Height() < renderSizePx.Height())
        {
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - backgroundSpriteRenderSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(m_cachedImageOffsetPx), positionPxf.Y + PxValueF(centeredYPx));

        // Draw the background mesh
        context.CommandBuffer.Draw(m_background.Mesh.Get(), dstPositionPxf, backgroundSpriteRenderSizePx,
                                   finalColor * m_background.CurrentColor.GetValue(), context.ClipContext);
      }

      if (m_cursor.Mesh.IsValid())
      {
        const PxVector2 cursorPositionPxf =
          m_windowContext->UnitConverter.ToPxVector2(TypeConverter::To<DpPoint2F>(m_hoverOverlay.CurrentPositionDp.GetValue()));

        PxValue centeredYPx;
        const PxSize2D cursorSpriteRenderSizePx = m_cursor.Mesh.FastGetRenderSizePx();
        {
          const PxSize2DF cursorOriginPxf(TypeConverter::To<PxSize2DF>(cursorSpriteRenderSizePx) / PxSize1DF::Create(2));
          const PxPoint2 adjustedCursorPositionPx =
            PxPoint2(m_cachedImageOffsetPx, PxValue()) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - cursorOriginPxf);

          const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(adjustedCursorPositionPx.X), positionPxf.Y + PxValueF(adjustedCursorPositionPx.Y));

          // Draw the cursor mesh
          context.CommandBuffer.Draw(m_cursor.Mesh.Get(), dstPositionPxf, cursorSpriteRenderSizePx, finalColor * m_cursor.CurrentColor.GetValue(),
                                     context.ClipContext);
        }

        // Draw the overlay (if enabled)
        if (m_propertyIsEnabled.Get() && (m_hoverOverlay.IsHovering || !m_hoverOverlay.CurrentColor.IsCompleted()) && m_hoverOverlay.Mesh.IsValid())
        {
          bool showOverlay = true;
          if (m_hoverOverlay.IsConstrainToGraphics)
          {
            PxPoint2 positionPx(TypeConverter::UncheckedChangeTo<PxPoint2>(positionPxf));
            PxRectangle cursorRect(PxPoint2(positionPx.X + m_cachedImageOffsetPx, positionPx.Y + centeredYPx), cursorSpriteRenderSizePx);
            showOverlay = cursorRect.Contains(m_hoverOverlay.LastScreenPositionPx);
          }
          if (showOverlay)
          {
            const PxSize2DF overlayOriginPxf(TypeConverter::To<PxSize2DF>(m_hoverOverlay.Mesh.GetSpriteObject().GetRenderSizePx()) /
                                             PxSize1DF::Create(2));
            const PxPoint2 adjustedOverlayPositionPx =
              PxPoint2(m_cachedImageOffsetPx, PxValue()) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - overlayOriginPxf);

            const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(adjustedOverlayPositionPx.X),
                                           positionPxf.Y + PxValueF(adjustedOverlayPositionPx.Y));

            // Draw the overlay mesh
            context.CommandBuffer.Draw(m_hoverOverlay.Mesh.Get(), dstPositionPxf, m_hoverOverlay.Mesh.FastGetRenderSizePx(),
                                       finalColor * m_hoverOverlay.CurrentColor.GetValue(), context.ClipContext);
          }
        }
      }
    }
  }

  void ToggleButton::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    if (m_propertyIsEnabled.Get() && !theEvent->IsHandled())
    {
      theEvent->Handled();
      if (theEvent->IsBegin())
      {
        if (!theEvent->IsRepeat())
        {
          // Begin
          const auto claimRectanglePx = TryGetButtonClaimRectangle();
          if (!claimRectanglePx.IsEmpty())
          {
            const auto localPositionPx = PointFromScreen(theEvent->GetScreenPosition());
            if (claimRectanglePx.Contains(localPositionPx))
            {
              Toggle();
              theEvent->Claimed();
              m_eventButtonState = EventButtonState::DownClaimed;
            }
          }
          if (m_eventButtonState == EventButtonState::Up)
          {
            theEvent->Handled();
            m_eventButtonState = EventButtonState::Down;
          }
        }
        else
        {
          // Continue
          if (m_eventButtonState == EventButtonState::Down)
          {
            theEvent->Handled();
          }
          else
          {
            theEvent->Claimed();
          }
        }
      }
      else if (theEvent->IsCanceled())
      {
        if (m_eventButtonState == EventButtonState::DownClaimed)
        {
          Toggle();
        }
        m_eventButtonState = EventButtonState::Up;
        theEvent->Handled();
      }
      else if (theEvent->IsEnd())
      {
        if (m_eventButtonState == EventButtonState::Down)
        {
          // Only accept the press if the mouse/finger is still on top of the button
          const auto localPositionPx = PointFromScreen(theEvent->GetScreenPosition());
          const PxRectangle hitRect(PxPoint2(), RenderSizePx());
          if (hitRect.Contains(localPositionPx.X, localPositionPx.Y))
          {
            Toggle();
          }
        }
        m_eventButtonState = EventButtonState::Up;
        theEvent->Handled();
      }
    }
  }

  void ToggleButton::OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    // We allow the m_isHovering state to be modified even when disabled as that will allow us to render the "hover overlay"
    // at the correct position if the control is enabled while the mouse was hovering.

    m_hoverOverlay.IsHovering = (theEvent->GetState() == EventTransactionState::Begin);
    if (m_hoverOverlay.IsHovering)
    {
      m_hoverOverlay.LastScreenPositionPx = theEvent->GetScreenPosition();
    }

    theEvent->Handled();
  }


  PxRectangle ToggleButton::TryGetButtonClaimRectangle() const noexcept
  {
    return {m_cachedImageOffsetPx, PxValue(0), m_cachedImageSizePx.Value(), RenderSizePx().Height()};
  }


  PxSize2D ToggleButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    // ---*-TEXT  TextLocation = right  TextAlignment = far   ImageAlignment=far
    // *----TEXT  TextLocation = right  TextAlignment = far   ImageAlignment=near
    // *-TEXT---  TextLocation = right  TextAlignment = near  ImageAlignment=near/center/far

    // TEXT-*---  TextLocation = left   TextAlignment = near  ImageAlignment=near
    // TEXT----*  TextLocation = left   TextAlignment = near  ImageAlignment=far
    // ---TEXT-*  TextLocation = left   TextAlignment = far   ImageAlignment=near/center/far


    const PxSize1D finalWidthPx = std::max(finalSizePx.Width(), DesiredSizePx().Width());
    const PxSize2D labelDesiredSizePx = m_cachedFontMeasureInfo.MeasureSizePx;
    const PxSize1D spriteWidthPx = GetMaxSpriteSize().Width();
    const PxSize1D maxTextWidthPx(finalWidthPx - spriteWidthPx);
    PxValue labelPositionPx;
    // const PxSize1D labelFinalWidthPx = labelDesiredSizePx.Width();
    PxSize1D imageAvailableWidthPx;
    if (GetTextLocation() == ItemTextLocation::Left)
    {
      switch (GetTextAlignment())
      {
      case ItemAlignment::Center:
        labelPositionPx = (maxTextWidthPx - labelDesiredSizePx.Width()) / PxSize1D::Create(2);
        break;
      case ItemAlignment::Far:
        labelPositionPx = (maxTextWidthPx - labelDesiredSizePx.Width());
        break;
      case ItemAlignment::Stretch:
      case ItemAlignment::Near:
      default:
        labelPositionPx = PxValue();
        break;
      }
      m_cachedTextOffsetPx = labelPositionPx;
      m_cachedImageOffsetPx = PxValue(labelPositionPx + labelDesiredSizePx.Width());
      imageAvailableWidthPx = PxSize1D(finalWidthPx - m_cachedImageOffsetPx);
    }
    else
    {
      switch (GetTextAlignment())
      {
      case ItemAlignment::Center:
        labelPositionPx = spriteWidthPx + (maxTextWidthPx - labelDesiredSizePx.Width()) / PxSize1D::Create(2);
        break;
      case ItemAlignment::Far:
        labelPositionPx = spriteWidthPx + (maxTextWidthPx - labelDesiredSizePx.Width());
        break;
      case ItemAlignment::Stretch:
      case ItemAlignment::Near:
      default:
        labelPositionPx = spriteWidthPx.Value();
        break;
      }
      m_cachedImageOffsetPx = PxValue::Zero();
      imageAvailableWidthPx = PxSize1D(labelPositionPx);
    }
    m_cachedTextOffsetPx = labelPositionPx;
    m_cachedImageOffsetPx += PxValue(ItemAlignmentUtil::CalcAlignmentPx(GetImageAlignment(), imageAvailableWidthPx - spriteWidthPx));
    m_cachedImageSizePx = PxSize1D::Create(imageAvailableWidthPx);
    return finalSizePx;
  }


  PxSize2D ToggleButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);

    m_cachedFontMeasureInfo = m_font.Mesh.ComplexMeasure(m_propertyText.Get());
    PxSize2D sizePx = m_cachedFontMeasureInfo.MeasureSizePx;
    const PxSize2D sizeTexPx = GetMaxSpriteSize();

    sizePx.AddWidth(sizeTexPx.Width());
    sizePx.SetHeight(std::max(sizePx.Height(), sizeTexPx.Height()));

    return sizePx;
  }

  DataBinding::DataBindingInstanceHandle ToggleButton::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_font.PropertyColorDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorChecked, m_cursor.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorUnchecked, m_cursor.PropertyColorUnchecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorCheckedDisabled, m_cursor.PropertyColorCheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorUncheckedDisabled, m_cursor.PropertyColorUncheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorChecked, m_background.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUnchecked, m_background.PropertyColorUnchecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorCheckedDisabled, m_background.PropertyColorCheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUncheckedDisabled, m_background.PropertyColorUncheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyHoverOverlayCheckedColor, m_hoverOverlay.Checked.PrimaryColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyHoverOverlayUncheckedColor, m_hoverOverlay.Unchecked.PrimaryColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyImageAlignment, m_propertyImageAlignment), DataBinding::PropLinkRefs(PropertyIsChecked, m_propertyIsChecked),
      DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled), DataBinding::PropLinkRefs(PropertyText, m_propertyText),
      DataBinding::PropLinkRefs(PropertyTextAlignment, m_propertyTextAlignment),
      DataBinding::PropLinkRefs(PropertyTextLocation, m_propertyTextLocation));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ToggleButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                       const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding,
      DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_font.PropertyColorDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorChecked, m_cursor.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorUnchecked, m_cursor.PropertyColorUnchecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorCheckedDisabled, m_cursor.PropertyColorCheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyCursorColorUncheckedDisabled, m_cursor.PropertyColorUncheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorChecked, m_background.PropertyColorChecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUnchecked, m_background.PropertyColorUnchecked.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorCheckedDisabled, m_background.PropertyColorCheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUncheckedDisabled, m_background.PropertyColorUncheckedDisabled.ExternalColor),
      DataBinding::PropLinkRefs(PropertyHoverOverlayCheckedColor, m_hoverOverlay.Checked.PrimaryColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyHoverOverlayUncheckedColor, m_hoverOverlay.Unchecked.PrimaryColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyImageAlignment, m_propertyImageAlignment), DataBinding::PropLinkRefs(PropertyIsChecked, m_propertyIsChecked),
      DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled), DataBinding::PropLinkRefs(PropertyText, m_propertyText),
      DataBinding::PropLinkRefs(PropertyTextAlignment, m_propertyTextAlignment),
      DataBinding::PropLinkRefs(PropertyTextLocation, m_propertyTextLocation));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ToggleButton::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyFontColorChecked);
    rProperties.push_back(PropertyFontColorUnchecked);
    rProperties.push_back(PropertyFontColorDisabled);
    rProperties.push_back(PropertyCursorColorChecked);
    rProperties.push_back(PropertyCursorColorUnchecked);
    rProperties.push_back(PropertyCursorColorCheckedDisabled);
    rProperties.push_back(PropertyCursorColorUncheckedDisabled);
    rProperties.push_back(PropertyBackgroundColorChecked);
    rProperties.push_back(PropertyBackgroundColorUnchecked);
    rProperties.push_back(PropertyBackgroundColorCheckedDisabled);
    rProperties.push_back(PropertyBackgroundColorUncheckedDisabled);
    rProperties.push_back(PropertyHoverOverlayCheckedColor);
    rProperties.push_back(PropertyHoverOverlayUncheckedColor);
    rProperties.push_back(PropertyImageAlignment);
    rProperties.push_back(PropertyIsChecked);
    rProperties.push_back(PropertyIsEnabled);
    rProperties.push_back(PropertyText);
    rProperties.push_back(PropertyTextAlignment);
    rProperties.push_back(PropertyTextLocation);
  }


  PxSize2D ToggleButton::GetMaxSpriteSize() const
  {
    return PxSize2D::Max(m_cursor.Mesh.GetRenderSizePx(), m_background.Mesh.GetRenderSizePx());
  }


  void ToggleButton::UpdateAnimation(const TimeSpan& timeSpan)
  {
    BaseWindow::UpdateAnimation(timeSpan);
    m_font.CurrentColor.Update(timeSpan);
    m_cursor.CurrentColor.Update(timeSpan);
    m_background.CurrentColor.Update(timeSpan);
    m_hoverOverlay.CurrentColor.Update(timeSpan);
    m_hoverOverlay.CurrentPositionDp.Update(timeSpan);
  }


  bool ToggleButton::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const bool isEnabled = IsEnabled();
    // Determine if the hover overlay should be shown or not
    const bool isChecked = m_propertyIsChecked.Get();
    const UIRenderColor hoverOverlayColor =
      isChecked ? m_hoverOverlay.Checked.PrimaryColor.InternalColor : m_hoverOverlay.Unchecked.PrimaryColor.InternalColor;
    const bool showHoverOverlay = m_hoverOverlay.IsHovering && isEnabled;
    m_hoverOverlay.CurrentColor.SetValue(showHoverOverlay ? hoverOverlayColor : UIRenderColor::ClearA(hoverOverlayColor));
    m_hoverOverlay.CurrentPositionDp.SetValue(isChecked ? TypeConverter::To<Vector2>(m_hoverOverlay.Checked.PositionDp)
                                                        : TypeConverter::To<Vector2>(m_hoverOverlay.Unchecked.PositionDp));

    m_font.CurrentColor.SetValue(isEnabled ? (isChecked ? m_font.PropertyColorChecked.InternalColor : m_font.PropertyColorUnchecked.InternalColor)
                                           : m_font.PropertyColorDisabled.InternalColor);

    m_cursor.CurrentColor.SetValue(
      isEnabled ? (isChecked ? m_cursor.PropertyColorChecked.InternalColor : m_cursor.PropertyColorUnchecked.InternalColor)
                : (isChecked ? m_cursor.PropertyColorCheckedDisabled.InternalColor : m_cursor.PropertyColorUncheckedDisabled.InternalColor));
    m_background.CurrentColor.SetValue(
      isEnabled ? (isChecked ? m_background.PropertyColorChecked.InternalColor : m_background.PropertyColorUnchecked.InternalColor)
                : (isChecked ? m_background.PropertyColorCheckedDisabled.InternalColor : m_background.PropertyColorUncheckedDisabled.InternalColor));

    if (forceCompleteAnimation)
    {
      m_font.CurrentColor.ForceComplete();
      m_cursor.CurrentColor.ForceComplete();
      m_background.CurrentColor.ForceComplete();
      m_hoverOverlay.CurrentColor.ForceComplete();
      m_hoverOverlay.CurrentPositionDp.ForceComplete();
    }

    const bool isAnimating = !m_font.CurrentColor.IsCompleted() || !m_cursor.CurrentColor.IsCompleted() || !m_background.CurrentColor.IsCompleted() ||
                             !m_hoverOverlay.CurrentColor.IsCompleted() || !m_hoverOverlay.CurrentPositionDp.IsCompleted();
    return isAnimating;
  }
}
