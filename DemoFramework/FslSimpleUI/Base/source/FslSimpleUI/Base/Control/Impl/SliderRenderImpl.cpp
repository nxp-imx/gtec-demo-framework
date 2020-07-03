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

#include <FslSimpleUI/Base/Control/Impl/SliderRenderImpl.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include "ImageImpl_ImageSprite.hpp"
#include "ImageImpl_NineSliceSprite.hpp"

namespace Fsl
{
  namespace UI
  {
    SliderRenderImpl::SliderRenderImpl(TransitionCache& rTransitionCache)
      : m_cursorOverlay(DefaultColor::Slider::HoverOverlay, rTransitionCache, DefaultAnim::HoverOverlayTime, DefaultAnim::HoverOverlayTransitionType)
    {
    }


    SliderPixelSpanInfo SliderRenderImpl::WinDraw(INativeBatch2D& batch, const Vector2 dstPositionPxf, const PxSize2D& renderSizePx,
                                                  const LayoutOrientation orientation, const LayoutDirection layoutDirection, const bool isEnabled,
                                                  const int32_t cursorPositionPx, const bool isDragging,
                                                  const SpriteUnitConverter& spriteUnitConverter)
    {
      FSL_PARAM_NOT_USED(isDragging);

      PxThickness backgroundContentMarginPx;
      {
        {
          const NineSliceSprite* const pSprite = m_background.Sprite.get();
          if (pSprite != nullptr)
          {
            const auto& backgroundColor = (isEnabled ? m_background.EnabledColor : m_background.DisabledColor);
            backgroundContentMarginPx = pSprite->GetRenderInfo().ScaledContentMarginPx;
            if (!m_verticalGraphicsRotationEnabled)
            {
              ImageImpl::Draw(batch, pSprite, dstPositionPxf, renderSizePx, backgroundColor);
            }
            else
            {
              // coverity[swapped_arguments]
              backgroundContentMarginPx = PxThickness(backgroundContentMarginPx.Bottom(), backgroundContentMarginPx.Left(),
                                                      backgroundContentMarginPx.Top(), backgroundContentMarginPx.Right());
              ImageImpl::DrawRotated90CW(batch, pSprite, dstPositionPxf, renderSizePx, backgroundColor);
            }
          }
        }

        SliderPixelSpanInfo spanInfo;
        {
          const ImageSprite* const pSprite = m_cursor.Sprite.get();
          if (pSprite != nullptr)
          {
            const ImageSpriteInfo& info = pSprite->GetInfo();

            const auto& cursorCulor = (isEnabled ? m_cursor.EnabledColor : m_cursor.DisabledColor);
            const PxPoint2 cursorSizePx(spriteUnitConverter.ToPxPoint2(m_cursor.SizeDp));
            const PxPoint2 cursorOriginPx(spriteUnitConverter.ToPxPoint2(m_cursor.OriginDp));
            const bool reverseDirection = (layoutDirection != LayoutDirection::NearToFar);

            if (orientation == LayoutOrientation::Horizontal)
            {
              const int32_t virtualCursorLengthPx = (cursorSizePx.X > 0 ? cursorSizePx.X : info.RenderInfo.ScaledSizePx.Width());
              const int32_t startPx = (virtualCursorLengthPx / 2) + backgroundContentMarginPx.Left();

              spanInfo =
                SliderPixelSpanInfo(startPx, renderSizePx.Width() - virtualCursorLengthPx - backgroundContentMarginPx.SumX(), reverseDirection);

              Vector2 cursorPositionPxf(dstPositionPxf.X + static_cast<float>(cursorPositionPx - cursorOriginPx.X), dstPositionPxf.Y);

              ImageImpl::Draw(batch, pSprite, cursorPositionPxf, cursorCulor);

              // Draw the overlay (if enabled)
              if (m_cursorOverlay.Sprite && m_cursorOverlay.CurrentColor.GetValue().PackedValue() > 0)
              {
                ImageImpl::Draw(batch, m_cursorOverlay.Sprite.get(), cursorPositionPxf, m_cursorOverlay.CurrentColor.GetValue());
              }
            }
            else
            {
              const int32_t virtualCursorLengthPx = (cursorSizePx.Y > 0 ? cursorSizePx.Y : info.RenderInfo.ScaledSizePx.Height());
              const int32_t startPx = (virtualCursorLengthPx / 2) + backgroundContentMarginPx.Top();

              spanInfo =
                SliderPixelSpanInfo(startPx, renderSizePx.Height() - virtualCursorLengthPx - backgroundContentMarginPx.SumY(), !reverseDirection);

              Vector2 cursorPositionPxf(dstPositionPxf.X, dstPositionPxf.Y + static_cast<float>(cursorPositionPx - cursorOriginPx.Y));

              ImageImpl::Draw(batch, pSprite, cursorPositionPxf, cursorCulor);

              // Draw the overlay (if enabled)
              if (m_cursorOverlay.Sprite && m_cursorOverlay.CurrentColor.GetValue().PackedValue() > 0)
              {
                ImageImpl::Draw(batch, m_cursorOverlay.Sprite.get(), cursorPositionPxf, m_cursorOverlay.CurrentColor.GetValue());
              }
            }
          }
        }
        return spanInfo;
      }
    }

    void SliderRenderImpl::OnMouseOver(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
    {
      // We allow the m_isHovering state to be modified even when disabled as that will allow us to render the "hover overlay"
      // at the correct position if the control is enabled while the mouse was hovering.
      m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
    }

    PxSize2D SliderRenderImpl::Measure(const PxAvailableSize& availableSizePx)
    {
      return ImageImpl::MeasureOverride(availableSizePx, m_background.Sprite.get());
    }

    void SliderRenderImpl::UpdateAnimation(const TransitionTimeSpan& timeSpan)
    {
      m_cursorOverlay.CurrentColor.Update(timeSpan);
    }

    bool SliderRenderImpl::UpdateAnimationState(const bool forceCompleteAnimation, const bool isEnabled, const bool isDragging)
    {
      const bool showHoverOverlay = isEnabled && (m_isHovering || isDragging);
      m_cursorOverlay.CurrentColor.SetValue(showHoverOverlay ? m_cursorOverlay.EnabledColor : Color::Transparent());

      if (forceCompleteAnimation)
      {
        m_cursorOverlay.CurrentColor.ForceComplete();
      }

      return !m_cursorOverlay.CurrentColor.IsCompleted();
    }
  }
}
