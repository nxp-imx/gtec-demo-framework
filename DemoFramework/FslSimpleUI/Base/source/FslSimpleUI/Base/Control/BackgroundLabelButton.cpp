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

#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/ItemAlignmentUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    BackgroundLabelButton::BackgroundLabelButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_fontMesh(context->TheUIContext.Get()->MeshManager, context->DefaultFont)
      , m_backgroundNormal(context->TheUIContext.Get()->MeshManager)
      , m_backgroundHover(context->TheUIContext.Get()->MeshManager)
      , m_backgroundHoverOverlay(context->TheUIContext.Get()->MeshManager)
      , m_backgroundCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_backgroundCurrentHoverOverlayColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_fontCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
      UpdateAnimationState(true);
    }


    void BackgroundLabelButton::SetContentAlignmentX(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentX)
      {
        m_contentAlignmentX = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void BackgroundLabelButton::SetContentAlignmentY(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentY)
      {
        m_contentAlignmentY = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void BackgroundLabelButton::SetPadding(const DpThickness& valueDp)
    {
      if (valueDp != m_paddingDp)
      {
        m_paddingDp = valueDp;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void BackgroundLabelButton::SetBackground(const std::shared_ptr<IContentSprite>& value)
    {
      if (m_backgroundNormal.SetSprite(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }

    void BackgroundLabelButton::SetBackgroundHover(const std::shared_ptr<IContentSprite>& value)
    {
      if (m_backgroundHover.SetSprite(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }

    void BackgroundLabelButton::SetBackgroundHoverOverlay(const std::shared_ptr<IContentSprite>& value)
    {
      if (m_backgroundHoverOverlay.SetSprite(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }


    void BackgroundLabelButton::SetContent(const StringViewLite& strView)
    {
      if (m_fontMesh.SetText(strView))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }

    void BackgroundLabelButton::SetContent(std::string&& value)
    {
      if (m_fontMesh.SetText(std::move(value)))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }


    void BackgroundLabelButton::SetFont(const std::shared_ptr<SpriteFont>& value)
    {
      if (m_fontMesh.SetSprite(value))
      {
        PropertyUpdated(PropertyType::Content);
      }
    }


    void BackgroundLabelButton::SetBackgroundHoverOverlayColorUp(const Color& value)
    {
      if (value != m_backgroundColorHoverOverlayUp)
      {
        m_backgroundColorHoverOverlayUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::SetBackgroundHoverOverlayColorDown(const Color& value)
    {
      if (value != m_backgroundColorHoverOverlayDown)
      {
        m_backgroundColorHoverOverlayDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void BackgroundLabelButton::SetBackgroundColorHoverUp(const Color& value)
    {
      if (value != m_backgroundColorHoverUp)
      {
        m_backgroundColorHoverUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::SetBackgroundColorUp(const Color& value)
    {
      if (value != m_backgroundColorUp)
      {
        m_backgroundColorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::SetBackgroundColorDown(const Color& value)
    {
      if (value != m_backgroundColorDown)
      {
        m_backgroundColorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::SetBackgroundColorDisabled(const Color& value)
    {
      if (value != m_backgroundColorDisabled)
      {
        m_backgroundColorDisabled = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void BackgroundLabelButton::SetFontColorUp(const Color& value)
    {
      if (value != m_fontColorUp)
      {
        m_fontColorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void BackgroundLabelButton::SetFontColorDown(const Color& value)
    {
      if (value != m_fontColorDown)
      {
        m_fontColorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::SetFontColorDisabled(const Color& value)
    {
      if (value != m_fontColorDisabled)
      {
        m_fontColorDisabled = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundLabelButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      const PxSize2D renderSizePx = RenderSizePx();

      const Color finalColor = GetFinalBaseColor();
      if (m_backgroundNormal.IsValid())
      {
        if (!IsDown() && m_isHovering && m_backgroundHover.IsValid() && IsEnabled())
        {
          context.CommandBuffer.Draw(m_backgroundHover.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                     finalColor * m_backgroundCurrentColor.GetValue());
        }
        else
        {
          context.CommandBuffer.Draw(m_backgroundNormal.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                     finalColor * m_backgroundCurrentColor.GetValue());
        }
      }

      {
        const Color backgroundHoverOverlayColor = m_backgroundCurrentHoverOverlayColor.GetValue();
        if (backgroundHoverOverlayColor.A() > 0)
        {
          context.CommandBuffer.Draw(m_backgroundHoverOverlay.Get(), context.TargetRect.TopLeft(), renderSizePx,
                                     finalColor * backgroundHoverOverlayColor);
        }
      }


      if (m_fontMesh.IsValid() && !m_fontMesh.GetText().empty())
      {
        const PxThickness contentMarginPx =
          PxThickness::Max(m_backgroundHover.GetRenderContentMarginPx(), m_backgroundNormal.GetRenderContentMarginPx());

        const PxThickness paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_paddingDp);
        const int32_t widthPx = renderSizePx.Width() - contentMarginPx.SumX() - paddingPx.SumX() - m_labelMeasureInfo.MeasureSizePx.Width();
        const int32_t heightPx = renderSizePx.Height() - contentMarginPx.SumY() - paddingPx.SumY() - m_labelMeasureInfo.MeasureSizePx.Height();
        const int32_t offsetXPx = ItemAlignmentUtil::CalcAlignmentPx(m_contentAlignmentX, widthPx);
        const int32_t offsetYPx = ItemAlignmentUtil::CalcAlignmentPx(m_contentAlignmentY, heightPx);

        auto posPxf = context.TargetRect.TopLeft();
        posPxf.X += static_cast<float>(contentMarginPx.Left() + paddingPx.Left() + offsetXPx);
        posPxf.Y += static_cast<float>(contentMarginPx.Top() + paddingPx.Top() + offsetYPx);

        context.CommandBuffer.Draw(m_fontMesh.Get(), posPxf, m_labelMeasureInfo.MinimalSizePx, finalColor * m_fontCurrentColor.GetValue());
      }
    }


    void BackgroundLabelButton::OnMouseOver(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
    {
      m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
      theEvent->Handled();
    }

    PxSize2D BackgroundLabelButton::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return finalSizePx;
    }


    PxSize2D BackgroundLabelButton::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      FSL_PARAM_NOT_USED(availableSizePx);

      auto paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_paddingDp);
      m_labelMeasureInfo = m_fontMesh.ComplexMeasure();
      PxSize2D desiredSizePx = paddingPx.Sum() + m_labelMeasureInfo.MeasureSizePx;

      const auto backgroundHoverInfo = m_backgroundHover.GetRenderContentInfo();
      const auto backgroundNormalInfo = m_backgroundNormal.GetRenderContentInfo();
      const auto backgroundHoverOverlayInfo = m_backgroundHoverOverlay.GetRenderContentInfo();

      // Apply the max content padding
      desiredSizePx += PxSize2D::Max(backgroundHoverInfo.RenderContentMarginPx.Sum(), backgroundNormalInfo.RenderContentMarginPx.Sum(),
                                     backgroundHoverOverlayInfo.RenderContentMarginPx.Sum());
      // Chose the largest size
      desiredSizePx =
        PxSize2D::Max(desiredSizePx, backgroundHoverInfo.RenderSizePx, backgroundNormalInfo.RenderSizePx, backgroundHoverOverlayInfo.RenderSizePx);
      return desiredSizePx;
    }

    void BackgroundLabelButton::UpdateAnimation(const TransitionTimeSpan& timeSpan)
    {
      ButtonBase::UpdateAnimation(timeSpan);
      m_backgroundCurrentColor.Update(timeSpan);
      m_backgroundCurrentHoverOverlayColor.Update(timeSpan);
      m_fontCurrentColor.Update(timeSpan);
    }

    bool BackgroundLabelButton::UpdateAnimationState(const bool forceCompleteAnimation)
    {
      const bool isEnabled = IsEnabled();
      const bool isDown = IsDown();

      // Determine background color

      const auto& backgroundColor = GetBackgroundColor(isEnabled, isDown, m_isHovering);
      m_backgroundCurrentColor.SetValue(backgroundColor);

      const auto& overlayBaseColor = !isDown ? m_backgroundColorHoverOverlayUp : m_backgroundColorHoverOverlayDown;
      const auto overlayColor = isEnabled && m_isHovering ? overlayBaseColor : Color::ClearA(overlayBaseColor);
      m_backgroundCurrentHoverOverlayColor.SetValue(overlayColor);

      const auto& fontColor = (isEnabled ? (!isDown ? m_fontColorUp : m_fontColorDown) : m_fontColorDisabled);
      m_fontCurrentColor.SetValue(fontColor);

      if (forceCompleteAnimation)
      {
        m_backgroundCurrentColor.ForceComplete();
        m_backgroundCurrentHoverOverlayColor.ForceComplete();
        m_fontCurrentColor.ForceComplete();
      }

      const bool isAnimating =
        !m_backgroundCurrentColor.IsCompleted() || !m_backgroundCurrentHoverOverlayColor.IsCompleted() || !m_fontCurrentColor.IsCompleted();

      // Toggle animation on/off
      return isAnimating;
    }

    Color BackgroundLabelButton::GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const
    {
      if (isEnabled)
      {
        if (!isDown)
        {
          return !isHovering ? m_backgroundColorUp : m_backgroundColorHoverUp;
        }
        return m_backgroundColorDown;
      }
      return m_backgroundColorDisabled;
    }

  }
}
