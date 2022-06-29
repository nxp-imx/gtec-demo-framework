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
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  ImageButton::ImageButton(const std::shared_ptr<WindowContext>& context)
    : ButtonBase(context)
    , m_windowContext(context)
    , m_content(context->TheUIContext.Get()->MeshManager)
    , m_background(context->TheUIContext.Get()->MeshManager)
    , m_currentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    , m_backgroundCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
  {
    m_currentColor.SetActualValue(m_upColor);
    m_backgroundCurrentColor.SetActualValue(m_background.UpColor);
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
  }


  void ImageButton::SetContent(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::SetContent(std::shared_ptr<ISizedSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
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

  void ImageButton::SetBackground(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_background.Sprite.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::SetBackgroundHover(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_background.HoverSprite.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void ImageButton::WinDraw(const UIDrawContext& context)
  {
    ButtonBase::WinDraw(context);

    PxSize2D renderSizePx = RenderSizePx();
    const Color finalColor(GetFinalBaseColor());
    {
      // auto* backgroundSprite = (!IsEnabled() || !m_isHovering) ? m_background.Sprite.get() : m_background.HoverSprite.get();
      // ImageImpl::Draw(*m_windowContext->Batch2D, backgroundSprite, context.TargetRect.Location(), renderSizePx,
      // m_backgroundCurrentColor.GetValue());
      const auto hMesh = (!IsEnabled() || !m_isHovering) ? m_background.Sprite.Get() : m_background.HoverSprite.Get();
      context.CommandBuffer.Draw(hMesh, context.TargetRect.Location(), renderSizePx, finalColor * m_backgroundCurrentColor.GetValue());
    }

    {
      auto desiredImageSizePx = m_content.Measure(renderSizePx, m_scalePolicy);

      PxPoint2 adjustPx = (renderSizePx - desiredImageSizePx) / 2;
      PxVector2 dstPositionPxf = context.TargetRect.Location() + TypeConverter::To<PxVector2>(adjustPx);
      // ImageImpl::Draw(*m_windowContext->Batch2D, m_content.get(), dstPositionPxf, desiredImageSize, m_currentColor.GetValue());
      context.CommandBuffer.Draw(m_content.Get(), dstPositionPxf, desiredImageSizePx, finalColor * m_currentColor.GetValue());
    }
  }


  PxSize2D ImageButton::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    PxSize2D desiredBackgroudSize0Px = m_background.Sprite.Measure(finalSizePx, m_background.ScalePolicy);
    PxSize2D desiredBackgroudSize1Px = m_background.HoverSprite.Measure(finalSizePx, m_background.ScalePolicy);
    PxSize2D desiredImageSizePx = m_content.Measure(finalSizePx, m_scalePolicy);
    return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0Px, desiredImageSizePx), desiredBackgroudSize1Px);
  }


  PxSize2D ImageButton::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);

    auto desiredBackgroudSize0Px = m_background.Sprite.Measure();
    auto desiredBackgroudSize1Px = m_background.HoverSprite.Measure();
    auto desiredImageSizePx = m_content.Measure();
    return PxSize2D::Max(PxSize2D::Max(desiredBackgroudSize0Px, desiredImageSizePx), desiredBackgroudSize1Px);
  }


  void ImageButton::OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);
    m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
    theEvent->Handled();
  }


  void ImageButton::UpdateAnimation(const TimeSpan& timeSpan)
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
