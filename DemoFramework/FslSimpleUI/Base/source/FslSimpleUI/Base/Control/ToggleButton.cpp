/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
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
    TFactory::Create<Color, TClass, &TClass::GetFontColorChecked, &TClass::SetFontColorChecked>("FontColorChecked");
  TDef TClass::PropertyFontColorUnchecked =
    TFactory::Create<Color, TClass, &TClass::GetFontColorUnchecked, &TClass::SetFontColorChecked>("FontColorUnchecked");
  TDef TClass::PropertyFontColorDisabled =
    TFactory::Create<Color, TClass, &TClass::GetFontColorDisabled, &TClass::SetFontColorDisabled>("FontColorDisabled");
  TDef TClass::PropertyCursorColorChecked =
    TFactory::Create<Color, TClass, &TClass::GetCursorColorChecked, &TClass::SetCursorColorChecked>("CursorColorChecked");
  TDef TClass::PropertyCursorColorUnchecked =
    TFactory::Create<Color, TClass, &TClass::GetCursorColorUnchecked, &TClass::SetCursorColorUnchecked>("CursorColorUnchecked");
  TDef TClass::PropertyCursorColorCheckedDisabled =
    TFactory::Create<Color, TClass, &TClass::GetCursorColorCheckedDisabled, &TClass::SetCursorColorCheckedDisabled>("CursorColorCheckedDisabled");
  TDef TClass::PropertyCursorColorUncheckedDisabled =
    TFactory::Create<Color, TClass, &TClass::GetCursorColorUncheckedDisabled, &TClass::SetCursorColorUncheckedDisabled>(
      "CursorColorUncheckedDisabled");
  TDef TClass::PropertyBackgroundColorChecked =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorChecked, &TClass::SetBackgroundColorChecked>("BackgroundColorChecked");
  TDef TClass::PropertyBackgroundColorUnchecked =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorUnchecked, &TClass::SetBackgroundColorUnchecked>("BackgroundColorUnchecked");
  TDef TClass::PropertyBackgroundColorCheckedDisabled =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorCheckedDisabled, &TClass::SetBackgroundColorCheckedDisabled>(
      "BackgroundColorCheckedDisabled");
  TDef TClass::PropertyBackgroundColorUncheckedDisabled =
    TFactory::Create<Color, TClass, &TClass::GetBackgroundColorUncheckedDisabled, &TClass::SetBackgroundColorUncheckedDisabled>(
      "BackgroundColorUncheckedDisabled");
  TDef TClass::PropertyImageAlignment =
    TFactory::Create<ItemAlignment, TClass, &TClass::GetImageAlignment, &TClass::SetImageAlignment>("ImageAlignment");
  TDef TClass::PropertyIsChecked = TFactory::Create<bool, TClass, &TClass::IsChecked, &TClass::SetIsChecked>("Checked");
  TDef TClass::PropertyIsEnabled = TFactory::Create<bool, TClass, &TClass::IsEnabled, &TClass::SetEnabled>("Enabled");
  TDef TClass::PropertyText = TFactory::Create<StringViewLite, TClass, &TClass::GetText, &TClass::SetText>("Text");
}

namespace Fsl::UI
{
  ToggleButton::ToggleButton(const std::shared_ptr<WindowContext>& context)
    : BaseWindow(context)
    , m_windowContext(context)
    , m_font(context->TheUIContext.Get()->MeshManager, context->DefaultFont, context->UITransitionCache, DefaultAnim::ColorChangeTime,
             DefaultAnim::ColorChangeTransitionType)
    , m_cursor(context->TheUIContext.Get()->MeshManager, context->UITransitionCache, DefaultAnim::HoverOverlayTime,
               DefaultAnim::HoverOverlayTransitionType)
    , m_background(context->TheUIContext.Get()->MeshManager, context->UITransitionCache, DefaultAnim::HoverOverlayTime,
                   DefaultAnim::HoverOverlayTransitionType)
    , m_hoverOverlay(context->TheUIContext.Get()->MeshManager, context->UITransitionCache, DefaultAnim::HoverOverlayTime,
                     DefaultAnim::HoverOverlayTransitionType, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType)
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

  void ToggleButton::SetHoverOverlayCheckedColor(const Color& value)
  {
    if (value != m_hoverOverlay.Checked.PrimaryColor)
    {
      m_hoverOverlay.Checked.PrimaryColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }

  void ToggleButton::SetHoverOverlayUncheckedColor(const Color& value)
  {
    if (value != m_hoverOverlay.Unchecked.PrimaryColor)
    {
      m_hoverOverlay.Unchecked.PrimaryColor = value;
      PropertyUpdated(PropertyType::Other);
    }
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


  bool ToggleButton::SetCursorColorChecked(const Color value)
  {
    const bool changed = m_cursor.PropertyColorChecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ToggleButton::SetCursorColorUnchecked(const Color value)
  {
    const bool changed = m_cursor.PropertyColorUnchecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetCursorColorCheckedDisabled(const Color value)
  {
    const bool changed = m_cursor.PropertyColorCheckedDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetCursorColorUncheckedDisabled(const Color value)
  {
    const bool changed = m_cursor.PropertyColorUncheckedDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorChecked(const Color value)
  {
    const bool changed = m_background.PropertyColorChecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorUnchecked(const Color value)
  {
    const bool changed = m_background.PropertyColorUnchecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorCheckedDisabled(const Color value)
  {
    const bool changed = m_background.PropertyColorCheckedDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetBackgroundColorUncheckedDisabled(const Color value)
  {
    const bool changed = m_background.PropertyColorUncheckedDisabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorChecked(const Color value)
  {
    const bool changed = m_font.PropertyColorChecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorUnchecked(const Color value)
  {
    const bool changed = m_font.PropertyColorUnchecked.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ToggleButton::SetFontColorDisabled(const Color value)
  {
    const bool changed = m_font.PropertyColorDisabled.Set(ThisDependencyObject(), value);
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
    const Color finalColor(GetFinalBaseColor());

    // Draw the text if it has been set
    {
      const Color fontColor = m_font.CurrentColor.GetValue();
      if (m_font.Mesh.IsValid() && !m_propertyText.Get().empty() && fontColor.A() > 0)
      {
        PxValue centeredYPx;
        if (m_cachedFontMeasureInfo.MeasureSizePx.Height() < renderSizePx.Height())
        {
          // Center the round to nearest pixel
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - m_cachedFontMeasureInfo.MeasureSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X, positionPxf.Y + PxValueF(centeredYPx));
        context.CommandBuffer.Draw(m_font.Mesh.Get(), dstPositionPxf, m_cachedFontMeasureInfo.MinimalSizePx, finalColor * fontColor);
      }
    }

    // Draw the ToggleButton
    {
      PxValue offsetXPx = m_cachedFontMeasureInfo.MeasureSizePx.Width().Value();
      {
        const PxSize2D sizeTex = GetMaxSpriteSize();
        offsetXPx += ItemAlignmentUtil::CalcAlignmentPx(
          m_propertyImageAlignment.Get(), PxSize1D(renderSizePx.Width() - m_cachedFontMeasureInfo.MeasureSizePx.Width()) - sizeTex.Width());
      }

      if (m_background.Mesh.IsValid())    // Background mesh
      {
        PxValue centeredYPx;
        const auto backgroundSpriteRenderSizePx = m_background.Mesh.FastGetRenderSizePx();
        if (backgroundSpriteRenderSizePx.Height() < renderSizePx.Height())
        {
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - backgroundSpriteRenderSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(offsetXPx), positionPxf.Y + PxValueF(centeredYPx));

        // Draw the background mesh
        context.CommandBuffer.Draw(m_background.Mesh.Get(), dstPositionPxf, backgroundSpriteRenderSizePx,
                                   finalColor * m_background.CurrentColor.GetValue());
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
            PxPoint2(offsetXPx, PxValue()) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - cursorOriginPxf);

          const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(adjustedCursorPositionPx.X), positionPxf.Y + PxValueF(adjustedCursorPositionPx.Y));

          // Draw the cursor mesh
          context.CommandBuffer.Draw(m_cursor.Mesh.Get(), dstPositionPxf, cursorSpriteRenderSizePx, finalColor * m_cursor.CurrentColor.GetValue());
        }

        // Draw the overlay (if enabled)
        if (m_propertyIsEnabled.Get() && (m_hoverOverlay.IsHovering || !m_hoverOverlay.CurrentColor.IsCompleted()) && m_hoverOverlay.Mesh.IsValid())
        {
          bool showOverlay = true;
          if (m_hoverOverlay.IsConstrainToGraphics)
          {
            PxPoint2 positionPx(TypeConverter::UncheckedChangeTo<PxPoint2>(positionPxf));
            PxRectangle cursorRect(PxPoint2(positionPx.X + offsetXPx, positionPx.Y + centeredYPx), cursorSpriteRenderSizePx);
            showOverlay = cursorRect.Contains(m_hoverOverlay.LastScreenPositionPx);
          }
          if (showOverlay)
          {
            const PxSize2DF overlayOriginPxf(TypeConverter::To<PxSize2DF>(m_hoverOverlay.Mesh.GetSpriteObject().GetRenderSizePx()) /
                                             PxSize1DF::Create(2));
            const PxPoint2 adjustedOverlayPositionPx =
              PxPoint2(offsetXPx, PxValue()) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - overlayOriginPxf);

            const PxVector2 dstPositionPxf(positionPxf.X + PxValueF(adjustedOverlayPositionPx.X),
                                           positionPxf.Y + PxValueF(adjustedOverlayPositionPx.Y));

            // Draw the overlay mesh
            context.CommandBuffer.Draw(m_hoverOverlay.Mesh.Get(), dstPositionPxf, m_hoverOverlay.Mesh.FastGetRenderSizePx(),
                                       finalColor * m_hoverOverlay.CurrentColor.GetValue());
          }
        }
      }
    }
  }

  void ToggleButton::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);

    if (m_propertyIsEnabled.Get() && !theEvent->IsHandled())
    {
      theEvent->Handled();
      if (theEvent->IsBegin() && !theEvent->IsRepeat())
      {
        Toggle();
      }
    }
  }

  void ToggleButton::OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    // We allow the m_isHovering state to be modified even when disabled as that will allow us to render the "hover overlay"
    // at the correct position if the control is enabled while the mouse was hovering.

    m_hoverOverlay.IsHovering = (theEvent->GetState() == EventTransactionState::Begin);
    if (m_hoverOverlay.IsHovering)
    {
      m_hoverOverlay.LastScreenPositionPx = theEvent->GetScreenPosition();
    }

    theEvent->Handled();
  }

  PxSize2D ToggleButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
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
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_font.PropertyColorDisabled),
      DataBinding::PropLinkRefs(PropertyCursorColorChecked, m_cursor.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyCursorColorUnchecked, m_cursor.PropertyColorUnchecked),
      DataBinding::PropLinkRefs(PropertyCursorColorCheckedDisabled, m_cursor.PropertyColorCheckedDisabled),
      DataBinding::PropLinkRefs(PropertyCursorColorUncheckedDisabled, m_cursor.PropertyColorUncheckedDisabled),
      DataBinding::PropLinkRefs(PropertyBackgroundColorChecked, m_background.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUnchecked, m_background.PropertyColorUnchecked),
      DataBinding::PropLinkRefs(PropertyBackgroundColorCheckedDisabled, m_background.PropertyColorCheckedDisabled),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUncheckedDisabled, m_background.PropertyColorUncheckedDisabled),
      DataBinding::PropLinkRefs(PropertyImageAlignment, m_propertyImageAlignment), DataBinding::PropLinkRefs(PropertyIsChecked, m_propertyIsChecked),
      DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled), DataBinding::PropLinkRefs(PropertyText, m_propertyText));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ToggleButton::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                       const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyFontColorChecked, m_font.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyFontColorDisabled, m_font.PropertyColorDisabled),
      DataBinding::PropLinkRefs(PropertyCursorColorChecked, m_cursor.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyCursorColorUnchecked, m_cursor.PropertyColorUnchecked),
      DataBinding::PropLinkRefs(PropertyCursorColorCheckedDisabled, m_cursor.PropertyColorCheckedDisabled),
      DataBinding::PropLinkRefs(PropertyCursorColorUncheckedDisabled, m_cursor.PropertyColorUncheckedDisabled),
      DataBinding::PropLinkRefs(PropertyBackgroundColorChecked, m_background.PropertyColorChecked),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUnchecked, m_background.PropertyColorUnchecked),
      DataBinding::PropLinkRefs(PropertyBackgroundColorCheckedDisabled, m_background.PropertyColorCheckedDisabled),
      DataBinding::PropLinkRefs(PropertyBackgroundColorUncheckedDisabled, m_background.PropertyColorUncheckedDisabled),
      DataBinding::PropLinkRefs(PropertyImageAlignment, m_propertyImageAlignment), DataBinding::PropLinkRefs(PropertyIsChecked, m_propertyIsChecked),
      DataBinding::PropLinkRefs(PropertyIsEnabled, m_propertyIsEnabled), DataBinding::PropLinkRefs(PropertyText, m_propertyText));
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
    rProperties.push_back(PropertyImageAlignment);
    rProperties.push_back(PropertyIsChecked);
    rProperties.push_back(PropertyIsEnabled);
    rProperties.push_back(PropertyText);
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
    const Color hoverOverlayColor = isChecked ? m_hoverOverlay.Checked.PrimaryColor : m_hoverOverlay.Unchecked.PrimaryColor;
    const bool showHoverOverlay = m_hoverOverlay.IsHovering && isEnabled;
    m_hoverOverlay.CurrentColor.SetValue(showHoverOverlay ? hoverOverlayColor : Color::ClearA(hoverOverlayColor));
    m_hoverOverlay.CurrentPositionDp.SetValue(isChecked ? TypeConverter::To<Vector2>(m_hoverOverlay.Checked.PositionDp)
                                                        : TypeConverter::To<Vector2>(m_hoverOverlay.Unchecked.PositionDp));

    m_font.CurrentColor.SetValue(isEnabled ? (isChecked ? m_font.PropertyColorChecked.Get() : m_font.PropertyColorUnchecked.Get())
                                           : m_font.PropertyColorDisabled.Get());

    m_cursor.CurrentColor.SetValue(isEnabled
                                     ? (isChecked ? m_cursor.PropertyColorChecked.Get() : m_cursor.PropertyColorUnchecked.Get())
                                     : (isChecked ? m_cursor.PropertyColorCheckedDisabled.Get() : m_cursor.PropertyColorUncheckedDisabled.Get()));
    m_background.CurrentColor.SetValue(
      isEnabled ? (isChecked ? m_background.PropertyColorChecked.Get() : m_background.PropertyColorUnchecked.Get())
                : (isChecked ? m_background.PropertyColorCheckedDisabled.Get() : m_background.PropertyColorUncheckedDisabled.Get()));

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
