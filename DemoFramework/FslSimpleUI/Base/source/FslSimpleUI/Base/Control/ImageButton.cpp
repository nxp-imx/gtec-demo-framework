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

#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "Impl/ImageImpl_ImageSprite.hpp"

namespace Fsl
{
  namespace UI
  {
    ImageButton::ImageButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_currentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_backgroundCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    {
      m_currentColor.SetActualValue(m_upColor);
      m_backgroundCurrentColor.SetActualValue(m_background.UpColor);
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
    }


    void ImageButton::SetContent(const std::shared_ptr<ImageSprite>& value)
    {
      if (value == m_content)
      {
        return;
      }

      m_content = value;
      PropertyUpdated(PropertyType::Content);
    }

    void ImageButton::SetContent(std::shared_ptr<ImageSprite>&& value)
    {
      if (value == m_content)
      {
        return;
      }

      m_content = std::move(value);
      PropertyUpdated(PropertyType::Content);
    }

    void ImageButton::SetBackgroundColorHoverUp(const Color& value)
    {
      if (value != m_background.HoverUpColor)
      {
        m_background.HoverUpColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void ImageButton::SetBackgroundColorUp(const Color& value)
    {
      if (value != m_background.UpColor)
      {
        m_background.UpColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void ImageButton::SetBackgroundColorDown(const Color& value)
    {
      if (value != m_background.DownColor)
      {
        m_background.DownColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void ImageButton::SetBackgroundColorDisabled(const Color& value)
    {
      if (value != m_background.DisabledColor)
      {
        m_background.DisabledColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void ImageButton::SetScalePolicy(const ItemScalePolicy value)
    {
      if (value == m_scalePolicy)
      {
        return;
      }

      m_scalePolicy = value;
      PropertyUpdated(PropertyType::ScalePolicy);
    }


    void ImageButton::SetUpColor(const Color& value)
    {
      if (value == m_upColor)
      {
        return;
      }

      m_upColor = value;
      PropertyUpdated(PropertyType::Other);
    }


    void ImageButton::SetDownColor(const Color& value)
    {
      if (value == m_downColor)
      {
        return;
      }

      m_downColor = value;
      PropertyUpdated(PropertyType::Other);
    }


    void ImageButton::SetDisabledColor(const Color& value)
    {
      if (value == m_disabledColor)
      {
        return;
      }

      m_disabledColor = value;
      PropertyUpdated(PropertyType::Other);
    }

    void ImageButton::SetBackground(const std::shared_ptr<ImageSprite>& value)
    {
      if (value != m_background.Sprite)
      {
        m_background.Sprite = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void ImageButton::SetBackgroundHover(const std::shared_ptr<ImageSprite>& value)
    {
      if (value != m_background.HoverSprite)
      {
        m_background.HoverSprite = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void ImageButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      PxSize2D renderSizePx = RenderSizePx();

      auto* backgroundSprite = (!IsEnabled() || !m_isHovering) ? m_background.Sprite.get() : m_background.HoverSprite.get();
      ImageImpl::Draw(*m_windowContext->Batch2D, backgroundSprite, context.TargetRect.Location(), renderSizePx, m_backgroundCurrentColor.GetValue());

      auto desiredImageSize = ImageImpl::ArrangeOverride(renderSizePx, m_content.get(), m_scalePolicy);

      PxPoint2 adjustPx = TypeConverter::To<PxPoint2>(renderSizePx - desiredImageSize) / 2;

      ImageImpl::Draw(*m_windowContext->Batch2D, m_content.get(), context.TargetRect.Location() + TypeConverter::UncheckedTo<Vector2>(adjustPx),
                      desiredImageSize, m_currentColor.GetValue());
    }


    PxSize2D ImageButton::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      auto desiredBackgroudSize0 = ImageImpl::ArrangeOverride(finalSizePx, m_background.Sprite.get(), m_background.ScalePolicy);
      auto desiredBackgroudSize1 = ImageImpl::ArrangeOverride(finalSizePx, m_background.HoverSprite.get(), m_background.ScalePolicy);
      auto desiredImageSize = ImageImpl::ArrangeOverride(finalSizePx, m_content.get(), m_scalePolicy);
      return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0, desiredImageSize), desiredBackgroudSize1);
    }


    PxSize2D ImageButton::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      auto desiredBackgroudSize0 = ImageImpl::MeasureOverride(availableSizePx, m_background.Sprite.get());
      auto desiredBackgroudSize1 = ImageImpl::MeasureOverride(availableSizePx, m_background.HoverSprite.get());
      auto desiredImageSize = ImageImpl::MeasureOverride(availableSizePx, m_content.get());
      return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0, desiredImageSize), desiredBackgroudSize1);
    }


    void ImageButton::OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
    {
      FSL_PARAM_NOT_USED(args);
      m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
      theEvent->Handled();
    }


    void ImageButton::UpdateAnimation(const TransitionTimeSpan& timeSpan)
    {
      BaseWindow::UpdateAnimation(timeSpan);
      m_currentColor.Update(timeSpan);
      m_backgroundCurrentColor.Update(timeSpan);
    }

    bool ImageButton::UpdateAnimationState(const bool forceCompleteAnimation)
    {
      const bool isEnabled = IsEnabled();
      const bool isUp = !IsDown();

      auto backgroundColor =
        isEnabled ? (isUp ? (!m_isHovering ? m_background.UpColor : m_background.HoverUpColor) : m_background.DownColor) : m_background.DisabledColor;
      m_backgroundCurrentColor.SetValue(backgroundColor);

      auto color = isEnabled ? (isUp ? m_upColor : m_downColor) : m_disabledColor;
      m_currentColor.SetValue(color);

      if (forceCompleteAnimation)
      {
        m_backgroundCurrentColor.ForceComplete();
        m_currentColor.ForceComplete();
      }

      const bool isAnimating = !m_currentColor.IsCompleted() || !m_backgroundCurrentColor.IsCompleted();
      return isAnimating;
    }

  }
}
