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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/TypeConverter_Math.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
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

//#include <FslBase/Log/Math/Pixel/FmtPxVector2.hpp>


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


  bool ToggleButton::SetEnabled(const bool enabled)
  {
    if (enabled != m_isEnabled)
    {
      m_isEnabled = enabled;
      PropertyUpdated(PropertyType::Content);
      return true;
    }
    return false;
  }

  bool ToggleButton::SetImageAlignment(const ItemAlignment alignment)
  {
    if (alignment != m_imageAlignment)
    {
      m_imageAlignment = alignment;
      PropertyUpdated(PropertyType::Other);
      return true;
    }
    return false;
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


  void ToggleButton::SetText(const StringViewLite& strView)
  {
    if (m_font.Mesh.SetText(strView))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ToggleButton::SetText(std::string&& value)
  {
    if (m_font.Mesh.SetText(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
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


  void ToggleButton::SetCursorCheckedColor(const Color& value)
  {
    if (value != m_cursor.CheckedColor)
    {
      m_cursor.CheckedColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetCursorCheckedDisabledColor(const Color& value)
  {
    if (value != m_cursor.CheckedDisabledColor)
    {
      m_cursor.CheckedDisabledColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }

  void ToggleButton::SetCursorUncheckedColor(const Color& value)
  {
    if (value != m_cursor.UncheckedColor)
    {
      m_cursor.UncheckedColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetCursorUncheckedDisabledColor(const Color& value)
  {
    if (value != m_cursor.UncheckedDisabledColor)
    {
      m_cursor.UncheckedDisabledColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetBackgroundCheckedColor(const Color& value)
  {
    if (value != m_background.CheckedColor)
    {
      m_background.CheckedColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetBackgroundCheckedDisabledColor(const Color& value)
  {
    if (value != m_background.CheckedDisabledColor)
    {
      m_background.CheckedDisabledColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetBackgroundUncheckedColor(const Color& value)
  {
    if (value != m_background.UncheckedColor)
    {
      m_background.UncheckedColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetBackgroundUncheckedDisabledColor(const Color& value)
  {
    if (value != m_background.UncheckedDisabledColor)
    {
      m_background.UncheckedDisabledColor = value;
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ToggleButton::SetFontCheckedkColor(const Color& value)
  {
    if (value != m_font.CheckedColor)
    {
      m_font.CheckedColor = value;
      PropertyUpdated(PropertyType::Content);
    }
  }


  void ToggleButton::SetFontUncheckColor(const Color& value)
  {
    if (value != m_font.UncheckedColor)
    {
      m_font.UncheckedColor = value;
      PropertyUpdated(PropertyType::Content);
    }
  }


  void ToggleButton::SetFontDisabledColor(const Color& value)
  {
    if (value != m_font.DisabledColor)
    {
      m_font.DisabledColor = value;
      PropertyUpdated(PropertyType::Content);
    }
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


  void ToggleButton::SetIsChecked(const bool value)
  {
    if (value != m_isChecked)
    {
      m_isChecked = value;
      PropertyUpdated(PropertyType::Content);

      if (IsReadyToSendEvents())
      {
        SendEvent(GetEventPool()->AcquireWindowContentChangedEvent(0));
      }
    }
  }

  void ToggleButton::Toggle()
  {
    SetIsChecked(!m_isChecked);
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
      if (m_font.Mesh.IsValid() && !m_font.Mesh.GetText().empty() && fontColor.A() > 0)
      {
        int32_t centeredYPx = 0;
        if (m_cachedFontMeasureInfo.MeasureSizePx.Height() < renderSizePx.Height())
        {
          // Center the round to nearest pixel
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - m_cachedFontMeasureInfo.MeasureSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X, positionPxf.Y + static_cast<float>(centeredYPx));
        context.CommandBuffer.Draw(m_font.Mesh.Get(), dstPositionPxf, m_cachedFontMeasureInfo.MinimalSizePx, finalColor * fontColor);
      }
    }

    // Draw the ToggleButton
    {
      int32_t offsetXPx = m_cachedFontMeasureInfo.MeasureSizePx.Width();
      {
        const PxSize2D sizeTex = GetMaxSpriteSize();
        offsetXPx += ItemAlignmentUtil::CalcAlignmentPx(
          m_imageAlignment, std::max(renderSizePx.Width() - m_cachedFontMeasureInfo.MeasureSizePx.Width(), 0) - sizeTex.Width());
      }

      if (m_background.Mesh.IsValid())    // Background mesh
      {
        int32_t centeredYPx = 0;
        const auto backgroundSpriteRenderSizePx = m_background.Mesh.FastGetRenderSizePx();
        if (backgroundSpriteRenderSizePx.Height() < renderSizePx.Height())
        {
          centeredYPx = ItemAlignmentUtil::CenterPx(renderSizePx.Height() - backgroundSpriteRenderSizePx.Height());
        }
        const PxVector2 dstPositionPxf(positionPxf.X + static_cast<float>(offsetXPx), positionPxf.Y + static_cast<float>(centeredYPx));

        // Draw the background mesh
        context.CommandBuffer.Draw(m_background.Mesh.Get(), dstPositionPxf, backgroundSpriteRenderSizePx,
                                   finalColor * m_background.CurrentColor.GetValue());
      }

      if (m_cursor.Mesh.IsValid())
      {
        const PxVector2 cursorPositionPxf =
          m_windowContext->UnitConverter.ToPxVector2(TypeConverter::To<DpPoint2F>(m_hoverOverlay.CurrentPositionDp.GetValue()));

        int32_t centeredYPx = 0;
        const PxSize2D cursorSpriteRenderSizePx = m_cursor.Mesh.FastGetRenderSizePx();
        {
          const PxSize2DF cursorOriginPxf(TypeConverter::To<PxSize2DF>(cursorSpriteRenderSizePx) / 2);
          const PxPoint2 adjustedCursorPositionPx =
            PxPoint2(offsetXPx, 0) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - cursorOriginPxf);

          const PxVector2 dstPositionPxf(positionPxf.X + static_cast<float>(adjustedCursorPositionPx.X),
                                         positionPxf.Y + static_cast<float>(adjustedCursorPositionPx.Y));

          // Draw the cursor mesh
          context.CommandBuffer.Draw(m_cursor.Mesh.Get(), dstPositionPxf, cursorSpriteRenderSizePx, finalColor * m_cursor.CurrentColor.GetValue());
        }

        // Draw the overlay (if enabled)
        if (m_isEnabled && (m_hoverOverlay.IsHovering || !m_hoverOverlay.CurrentColor.IsCompleted()) && m_hoverOverlay.Mesh.IsValid())
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
            const PxSize2DF overlayOriginPxf(TypeConverter::To<PxSize2DF>(m_hoverOverlay.Mesh.GetSpriteObject().GetRenderSizePx()) / 2);
            const PxPoint2 adjustedOverlayPositionPx =
              PxPoint2(offsetXPx, 0) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - overlayOriginPxf);

            const PxVector2 dstPositionPxf(positionPxf.X + static_cast<float>(adjustedOverlayPositionPx.X),
                                           positionPxf.Y + static_cast<float>(adjustedOverlayPositionPx.Y));

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

    if (m_isEnabled && !theEvent->IsHandled())
    {
      theEvent->Handled();
      if (theEvent->IsBegin() && !theEvent->IsRepeat())
      {
        SetIsChecked(!m_isChecked);
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

    m_cachedFontMeasureInfo = m_font.Mesh.ComplexMeasure();
    PxSize2D sizePx = m_cachedFontMeasureInfo.MeasureSizePx;
    const PxSize2D sizeTexPx = GetMaxSpriteSize();

    sizePx.AddWidth(sizeTexPx.Width());
    sizePx.SetHeight(std::max(sizePx.Height(), sizeTexPx.Height()));

    return sizePx;
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
    const Color hoverOverlayColor = m_isChecked ? m_hoverOverlay.Checked.PrimaryColor : m_hoverOverlay.Unchecked.PrimaryColor;
    const bool showHoverOverlay = m_hoverOverlay.IsHovering && isEnabled;
    m_hoverOverlay.CurrentColor.SetValue(showHoverOverlay ? hoverOverlayColor : Color::ClearA(hoverOverlayColor));
    m_hoverOverlay.CurrentPositionDp.SetValue(m_isChecked ? TypeConverter::To<Vector2>(m_hoverOverlay.Checked.PositionDp)
                                                          : TypeConverter::To<Vector2>(m_hoverOverlay.Unchecked.PositionDp));

    m_font.CurrentColor.SetValue(isEnabled ? (m_isChecked ? m_font.CheckedColor : m_font.UncheckedColor) : m_font.DisabledColor);

    m_cursor.CurrentColor.SetValue(isEnabled ? (m_isChecked ? m_cursor.CheckedColor : m_cursor.UncheckedColor)
                                             : (m_isChecked ? m_cursor.CheckedDisabledColor : m_cursor.UncheckedDisabledColor));
    m_background.CurrentColor.SetValue(isEnabled ? (m_isChecked ? m_background.CheckedColor : m_background.UncheckedColor)
                                                 : (m_isChecked ? m_background.CheckedDisabledColor : m_background.UncheckedDisabledColor));


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
