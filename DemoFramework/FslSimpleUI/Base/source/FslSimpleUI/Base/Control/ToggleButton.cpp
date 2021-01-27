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

#include <FslSimpleUI/Base/Control/ToggleButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Dp/TypeConverter_Math.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "Impl/ImageImpl_ImageSprite.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      inline int32_t AddAlignmentPx(const ItemAlignment alignment, const int32_t availableSpace, const int32_t itemSize)
      {
        switch (alignment)
        {
        case ItemAlignment::Center:
          return (availableSpace - itemSize) / 2;
        case ItemAlignment::Far:
          return availableSpace - itemSize;
        case ItemAlignment::Near:
        default:
          return 0;
        }
      }
    }

    ToggleButton::ToggleButton(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
      , m_font(context->DefaultFont, context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_cursor(context->UITransitionCache, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType)
      , m_background(context->UITransitionCache, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType)
      , m_hoverOverlay(context->UITransitionCache, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType,
                       DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType)
    {
      assert(m_font.Font);
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

      if (value != m_font.Font)
      {
        m_font.Font = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void ToggleButton::SetText(const StringViewLite& strView)
    {
      if (strView != m_text)
      {
        StringViewLiteUtil::Set(m_text, strView);
        PropertyUpdated(PropertyType::Content);
      }
    }

    void ToggleButton::SetText(std::string&& value)
    {
      if (value != m_text)
      {
        m_text = std::move(value);
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

    void ToggleButton::SetCursorCheckedPosition(const DpPoint& valueDp)
    {
      if (valueDp != m_hoverOverlay.Checked.PositionDp)
      {
        m_hoverOverlay.Checked.PositionDp = valueDp;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void ToggleButton::SetCursorUncheckedPosition(const DpPoint& valueDp)
    {
      if (valueDp != m_hoverOverlay.Unchecked.PositionDp)
      {
        m_hoverOverlay.Unchecked.PositionDp = valueDp;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void ToggleButton::SetHoverOverlaySprite(const std::shared_ptr<ImageSprite>& value)
    {
      if (value != m_hoverOverlay.Sprite)
      {
        m_hoverOverlay.Sprite = value;
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

    void ToggleButton::SetCheckedSprite(const std::shared_ptr<ImageSprite>& value)
    {
      if (value != m_cursor.Sprite)
      {
        m_cursor.Sprite = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void ToggleButton::SetUncheckedSprite(const std::shared_ptr<ImageSprite>& value)
    {
      if (value != m_background.Sprite)
      {
        m_background.Sprite = value;
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

      const auto batch = m_windowContext->Batch2D;

      const PxVector2 positionPxf = context.TargetRect.TopLeft();
      const PxSize2D renderSizePx = RenderSizePx();

      // Draw the text if it has been set
      PxSize2D measuredTextPx;
      {
        const auto* const pFont = m_font.Font.get();
        const Color fontColor = m_font.CurrentColor.GetValue();
        if (!m_text.empty() && pFont != nullptr && fontColor.PackedValue() > 0)
        {
          const auto& fontInfo = pFont->GetInfo();
          measuredTextPx = pFont->MeasureString(StringViewLiteUtil::AsStringViewLite(m_text));
          measuredTextPx.SetHeight(fontInfo.ScaledLineSpacingPx);

          int32_t centeredYPx = 0;
          if (measuredTextPx.Height() < renderSizePx.Height())
          {
            centeredYPx = (renderSizePx.Height() - measuredTextPx.Height()) / 2;
          }
          batch->ChangeTo(static_cast<BlendState>(pFont->GetInfo().MaterialInfo.NativeMaterialFlags));
          batch->DrawString(*pFont, m_text, Vector2(positionPxf.X, positionPxf.Y + float(centeredYPx)), fontColor);
        }
      }

      // Draw the ToggleButton
      const ImageSprite* const pCursorSprite = m_cursor.Sprite.get();
      const ImageSprite* const pBackgroundSprite = m_background.Sprite.get();
      {
        int32_t offsetXPx = measuredTextPx.Width();
        {
          const PxSize2D sizeTex = GetMaxSpriteSize();
          offsetXPx += AddAlignmentPx(m_imageAlignment, std::max(renderSizePx.Width() - measuredTextPx.Width(), 0), sizeTex.Width());
        }

        if (pBackgroundSprite != nullptr)
        {
          int32_t centeredYPx = 0;
          const auto& backgroundSpriteInfo = pBackgroundSprite->GetInfo();
          if (backgroundSpriteInfo.RenderInfo.ScaledSizePx.Height() < renderSizePx.Height())
          {
            centeredYPx = (renderSizePx.Height() - backgroundSpriteInfo.RenderInfo.ScaledSizePx.Height()) / 2;
          }
          ImageImpl::Draw(*batch, pBackgroundSprite, PxVector2(positionPxf.X + float(offsetXPx), positionPxf.Y + float(centeredYPx)),
                          m_background.CurrentColor.GetValue());
        }

        if (pCursorSprite != nullptr)
        {
          const PxVector2 cursorPositionPxf =
            m_windowContext->UnitConverter.ToPxVector2(TypeConverter::To<DpPointF>(m_hoverOverlay.CurrentPositionDp.GetValue()));

          int32_t centeredYPx = 0;
          const auto& cursorSpriteInfo = pCursorSprite->GetInfo();
          {
            const PxSize2DF cursorOriginPxf(TypeConverter::To<PxSize2DF>(cursorSpriteInfo.RenderInfo.ScaledSizePx) / 2);
            const PxPoint2 adjustedCursorPositionPx =
              PxPoint2(offsetXPx, 0) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - cursorOriginPxf);

            ImageImpl::Draw(*batch, pCursorSprite,
                            PxVector2(positionPxf.X + float(adjustedCursorPositionPx.X), positionPxf.Y + float(adjustedCursorPositionPx.Y)),
                            m_cursor.CurrentColor.GetValue());
          }

          // Draw the overlay (if enabled)
          const ImageSprite* const pHoverSprite = m_hoverOverlay.Sprite.get();
          if (m_isEnabled && (m_hoverOverlay.IsHovering || !m_hoverOverlay.CurrentColor.IsCompleted()) && pHoverSprite != nullptr)
          {
            bool showOverlay = true;
            if (m_hoverOverlay.IsConstrainToGraphics)
            {
              PxPoint2 positionPx(TypeConverter::UncheckedChangeTo<PxPoint2>(positionPxf));
              PxRectangle cursorRect(PxPoint2(positionPx.X + offsetXPx, positionPx.Y + centeredYPx), cursorSpriteInfo.RenderInfo.ScaledSizePx);
              showOverlay = cursorRect.Contains(m_hoverOverlay.LastScreenPositionPx);
            }
            if (showOverlay)
            {
              const PxSize2DF overlayOriginPxf(TypeConverter::To<PxSize2DF>(pHoverSprite->GetInfo().RenderInfo.ScaledSizePx) / 2);
              const PxPoint2 adjustedOverlayPositionPx =
                PxPoint2(offsetXPx, 0) + TypeConverter::UncheckedChangeTo<PxPoint2>(cursorPositionPxf - overlayOriginPxf);

              ImageImpl::Draw(*batch, pHoverSprite,
                              PxVector2(positionPxf.X + float(adjustedOverlayPositionPx.X), positionPxf.Y + float(adjustedOverlayPositionPx.Y)),
                              m_hoverOverlay.CurrentColor.GetValue());
            }
          }
        }
      }
    }

    void ToggleButton::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      FSL_PARAM_NOT_USED(args);

      if (m_isEnabled && !theEvent->IsHandled() && theEvent->IsBegin() && !theEvent->IsRepeat())
      {
        theEvent->Handled();
        SetIsChecked(!m_isChecked);
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

      PxSize2D sizePx;
      if (!m_text.empty())
      {
        const auto* pFont = m_font.Font.get();
        if (pFont != nullptr)
        {
          const auto& fontInfo = m_font.Font->GetInfo();
          auto measuredPx = m_font.Font->MeasureString(StringViewLiteUtil::AsStringViewLite(m_text));
          sizePx = PxSize2D(measuredPx.Width(), fontInfo.ScaledLineSpacingPx);
        }
      }

      const PxSize2D sizeTexPx = GetMaxSpriteSize();

      sizePx.AddWidth(sizeTexPx.Width());
      sizePx.SetHeight(std::max(sizePx.Height(), sizeTexPx.Height()));

      return sizePx;
    }

    PxSize2D ToggleButton::GetMaxSpriteSize() const
    {
      auto maxSize = m_cursor.Sprite ? m_cursor.Sprite->GetInfo().RenderInfo.ScaledSizePx : PxSize2D();
      maxSize.SetMax(m_background.Sprite ? m_background.Sprite->GetInfo().RenderInfo.ScaledSizePx : PxSize2D());
      return maxSize;
    }


    void ToggleButton::UpdateAnimation(const TransitionTimeSpan& timeSpan)
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
      m_hoverOverlay.CurrentColor.SetValue(showHoverOverlay ? hoverOverlayColor : Color::Transparent());
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

      const bool isAnimating = !m_font.CurrentColor.IsCompleted() || !m_cursor.CurrentColor.IsCompleted() ||
                               !m_background.CurrentColor.IsCompleted() || !m_hoverOverlay.CurrentColor.IsCompleted() ||
                               !m_hoverOverlay.CurrentPositionDp.IsCompleted();
      return isAnimating;
    }
  }
}
