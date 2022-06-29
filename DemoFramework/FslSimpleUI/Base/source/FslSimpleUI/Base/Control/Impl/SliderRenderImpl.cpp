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

#include <FslGraphics/Sprite/IContentSprite.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/Control/Impl/SliderRenderImpl.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>

namespace Fsl::UI
{
  SliderRenderImpl::SliderRenderImpl(const std::shared_ptr<IMeshManager>& meshManager, TransitionCache& rTransitionCache)
    : m_background(meshManager, DefaultColor::Palette::Primary, DefaultColor::Palette::PrimaryDisabled, rTransitionCache,
                   DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    , m_cursor(meshManager, DefaultColor::Palette::Primary, DefaultColor::Palette::PrimaryDisabled, rTransitionCache, DefaultAnim::ColorChangeTime,
               DefaultAnim::ColorChangeTransitionType)
    , m_cursorOverlay(meshManager, DefaultColor::Slider::HoverOverlay, rTransitionCache, DefaultAnim::HoverOverlayTime,
                      DefaultAnim::HoverOverlayTransitionType)
  {
  }

  bool SliderRenderImpl::SetCursorSprite(const std::shared_ptr<ISizedSprite>& value)
  {
    return m_cursor.Sprite.SetSprite(value);
  }

  bool SliderRenderImpl::SetCursorOverlaySprite(const std::shared_ptr<ISizedSprite>& value)
  {
    return m_cursorOverlay.Sprite.SetSprite(value);
  }


  bool SliderRenderImpl::SetBackgroundSprite(const std::shared_ptr<IContentSprite>& value)
  {
    return m_background.Sprite.SetSprite(value);
  }


  void SliderRenderImpl::Draw(DrawCommandBuffer& commandBuffer, const PxVector2 dstPositionPxf, const Color finalColor,
                              const int32_t cursorPositionPx, const bool isDragging, const SpriteUnitConverter& spriteUnitConverter)
  {
    FSL_PARAM_NOT_USED(isDragging);

    if (m_background.Sprite.IsValid())
    {
      const auto backgroundColor = m_background.CurrentColor.GetValue();
      if (!m_verticalGraphicsRotationEnabled)
      {
        // ImageImpl::Draw(batch, pSprite, dstPositionPxf, renderSizePx, backgroundColor);
        commandBuffer.Draw(m_background.Sprite.Get(), dstPositionPxf, m_arrangeCache.RenderSizePx, finalColor * backgroundColor);
      }
      else
      {
        // ImageImpl::DrawRotated90CW(batch, pSprite, dstPositionPxf, renderSizePx, backgroundColor);
        commandBuffer.DrawRotated90CW(m_background.Sprite.Get(), dstPositionPxf, m_arrangeCache.RenderSizePx, finalColor * backgroundColor);
      }
    }

    if (m_cursor.Sprite.IsValid())
    {
      const PxSize2D cursorRenderSizePx = m_cursor.Sprite.FastGetRenderSizePx();

      const auto cursorColor = m_cursor.CurrentColor.GetValue();
      const PxPoint2 cursorOriginPx(spriteUnitConverter.ToPxPoint2(m_cursor.OriginDp));

      if (m_arrangeCache.Orientation == LayoutOrientation::Horizontal)
      {
        PxVector2 cursorPositionPxf(dstPositionPxf.X + static_cast<float>(cursorPositionPx - cursorOriginPx.X), dstPositionPxf.Y);

        // ImageImpl::Draw(batch, pSprite, cursorPositionPxf, cursorCulor);
        commandBuffer.Draw(m_cursor.Sprite.Get(), cursorPositionPxf, cursorRenderSizePx, finalColor * cursorColor);

        // Draw the overlay (if enabled)
        if (m_cursorOverlay.Sprite.IsValid() && m_cursorOverlay.CurrentColor.GetValue().A() > 0)
        {
          commandBuffer.Draw(m_cursorOverlay.Sprite.Get(), cursorPositionPxf, m_cursorOverlay.Sprite.FastGetRenderSizePx(),
                             finalColor * m_cursorOverlay.CurrentColor.GetValue());
        }
      }
      else
      {
        PxVector2 cursorPositionPxf(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(cursorPositionPx - cursorOriginPx.Y));

        // ImageImpl::Draw(batch, pSprite, cursorPositionPxf, cursorCulor);
        commandBuffer.Draw(m_cursor.Sprite.Get(), cursorPositionPxf, cursorRenderSizePx, finalColor * cursorColor);

        // Draw the overlay (if enabled)
        if (m_cursorOverlay.Sprite.IsValid() && m_cursorOverlay.CurrentColor.GetValue().A() > 0)
        {
          commandBuffer.Draw(m_cursorOverlay.Sprite.Get(), cursorPositionPxf, m_cursorOverlay.Sprite.FastGetRenderSizePx(),
                             finalColor * m_cursorOverlay.CurrentColor.GetValue());
        }
      }
    }
  }

  void SliderRenderImpl::OnMouseOver(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowMouseOverEvent>& theEvent, const bool isEnabled)
  {
    // We allow the m_isHovering state to be modified even when disabled as that will allow us to render the "hover overlay"
    // at the correct position if the control is enabled while the mouse was hovering.
    m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
    if (isEnabled)
    {
      theEvent->Handled();
    }
  }

  PxSize2D SliderRenderImpl::Measure(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    return m_background.Sprite.Measure();
  }

  SliderPixelSpanInfo SliderRenderImpl::Arrange(const PxSize2D finalSizePx, const LayoutOrientation orientation,
                                                const LayoutDirection layoutDirection, const SpriteUnitConverter& spriteUnitConverter)
  {
    const PxPoint2 cursorSizePx(spriteUnitConverter.ToPxPoint2(m_cursor.SizeDp));
    const PxSize2D cursorRenderSizePx = m_cursor.Sprite.FastGetRenderSizePx();
    const bool reverseDirection = (layoutDirection != LayoutDirection::NearToFar);

    PxThickness backgroundContentMarginPx;
    if (m_background.Sprite.IsValid())
    {
      backgroundContentMarginPx = m_background.Sprite.GetRenderContentMarginPx();
      if (m_verticalGraphicsRotationEnabled)
      {
        // coverity[swapped_arguments]
        backgroundContentMarginPx = PxThickness(backgroundContentMarginPx.Bottom(), backgroundContentMarginPx.Left(), backgroundContentMarginPx.Top(),
                                                backgroundContentMarginPx.Right());
      }
    }

    SliderPixelSpanInfo spanInfo;
    if (orientation == LayoutOrientation::Horizontal)
    {
      const int32_t virtualCursorLengthPx = (cursorSizePx.X > 0 ? cursorSizePx.X : cursorRenderSizePx.Width());
      const int32_t startPx = (virtualCursorLengthPx / 2) + backgroundContentMarginPx.Left();

      spanInfo = SliderPixelSpanInfo(startPx, finalSizePx.Width() - virtualCursorLengthPx - backgroundContentMarginPx.SumX(), reverseDirection);
    }
    else
    {
      const int32_t virtualCursorLengthPx = (cursorSizePx.Y > 0 ? cursorSizePx.Y : cursorRenderSizePx.Height());
      const int32_t startPx = (virtualCursorLengthPx / 2) + backgroundContentMarginPx.Top();

      spanInfo = SliderPixelSpanInfo(startPx, finalSizePx.Height() - virtualCursorLengthPx - backgroundContentMarginPx.SumY(), !reverseDirection);
    }
    m_arrangeCache = ArrangeCache(finalSizePx, orientation, layoutDirection, spanInfo);
    return spanInfo;
  }

  void SliderRenderImpl::UpdateAnimation(const TimeSpan& timeSpan)
  {
    m_background.CurrentColor.Update(timeSpan);
    m_cursor.CurrentColor.Update(timeSpan);
    m_cursorOverlay.CurrentColor.Update(timeSpan);
  }

  bool SliderRenderImpl::UpdateAnimationState(const bool forceCompleteAnimation, const bool isEnabled, const bool isDragging)
  {
    const bool showHoverOverlay = isEnabled && (m_isHovering || isDragging);
    m_background.CurrentColor.SetValue(isEnabled ? m_background.EnabledColor : m_background.DisabledColor);
    m_cursor.CurrentColor.SetValue(isEnabled ? m_cursor.EnabledColor : m_cursor.DisabledColor);
    m_cursorOverlay.CurrentColor.SetValue(showHoverOverlay ? m_cursorOverlay.EnabledColor : Color::ClearA(m_cursorOverlay.EnabledColor));

    if (forceCompleteAnimation)
    {
      m_background.CurrentColor.ForceComplete();
      m_cursor.CurrentColor.ForceComplete();
      m_cursorOverlay.CurrentColor.ForceComplete();
    }

    return !m_background.CurrentColor.IsCompleted() || !m_cursor.CurrentColor.IsCompleted() || !m_cursorOverlay.CurrentColor.IsCompleted();
  }
}
