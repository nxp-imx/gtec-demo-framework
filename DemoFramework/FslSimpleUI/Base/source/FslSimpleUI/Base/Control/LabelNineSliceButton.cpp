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

#include <FslSimpleUI/Base/Control/LabelNineSliceButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/Util/Draw9SliceUtil.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "Impl/ImageImpl_NineSliceSprite.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      inline int32_t CalcAlignmentPx(const ItemAlignment alignment, const int32_t deltaPx)
      {
        switch (alignment)
        {
        case ItemAlignment::Center:
          return deltaPx / 2;
        case ItemAlignment::Far:
          return deltaPx;
        case ItemAlignment::Near:
        default:
          return 0;
        }
      }
    }

    LabelNineSliceButton::LabelNineSliceButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_font(context->DefaultFont)
      , m_backgroundCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_backgroundCurrentHoverOverlayColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
      , m_fontCurrentColor(context->UITransitionCache, DefaultAnim::ColorChangeTime, DefaultAnim::ColorChangeTransitionType)
    {
      assert(m_font);
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::MouseOver));
      UpdateAnimationState(true);
    }


    void LabelNineSliceButton::SetContentAlignmentX(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentX)
      {
        m_contentAlignmentX = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void LabelNineSliceButton::SetContentAlignmentY(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentY)
      {
        m_contentAlignmentY = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void LabelNineSliceButton::SetPadding(const DpThickness& valueDp)
    {
      if (valueDp != m_paddingDp)
      {
        m_paddingDp = valueDp;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void LabelNineSliceButton::SetBackground(const std::shared_ptr<NineSliceSprite>& value)
    {
      if (value != m_backgroundNormal)
      {
        m_backgroundNormal = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void LabelNineSliceButton::SetBackgroundHover(const std::shared_ptr<NineSliceSprite>& value)
    {
      if (value != m_backgroundHover)
      {
        m_backgroundHover = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void LabelNineSliceButton::SetBackgroundHoverOverlay(const std::shared_ptr<NineSliceSprite>& value)
    {
      if (value != m_backgroundHoverOverlay)
      {
        m_backgroundHoverOverlay = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void LabelNineSliceButton::SetContent(const StringViewLite& strView)
    {
      if (strView != m_content)
      {
        StringViewLiteUtil::Set(m_content, strView);
        PropertyUpdated(PropertyType::Content);
      }
    }

    void LabelNineSliceButton::SetContent(std::string&& value)
    {
      if (value != m_content)
      {
        m_content = std::move(value);
        PropertyUpdated(PropertyType::Content);
      }
    }


    void LabelNineSliceButton::SetFont(const std::shared_ptr<SpriteFont>& value)
    {
      if (!value)
      {
        throw std::invalid_argument("font can not be null");
      }

      if (value != m_font)
      {
        m_font = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void LabelNineSliceButton::SetBackgroundHoverOverlayColorUp(const Color& value)
    {
      if (value != m_backgroundColorHoverOverlayUp)
      {
        m_backgroundColorHoverOverlayUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::SetBackgroundHoverOverlayColorDown(const Color& value)
    {
      if (value != m_backgroundColorHoverOverlayDown)
      {
        m_backgroundColorHoverOverlayDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelNineSliceButton::SetBackgroundColorHoverUp(const Color& value)
    {
      if (value != m_backgroundColorHoverUp)
      {
        m_backgroundColorHoverUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::SetBackgroundColorUp(const Color& value)
    {
      if (value != m_backgroundColorUp)
      {
        m_backgroundColorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::SetBackgroundColorDown(const Color& value)
    {
      if (value != m_backgroundColorDown)
      {
        m_backgroundColorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::SetBackgroundColorDisabled(const Color& value)
    {
      if (value != m_backgroundColorDisabled)
      {
        m_backgroundColorDisabled = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelNineSliceButton::SetFontColorUp(const Color& value)
    {
      if (value != m_fontColorUp)
      {
        m_fontColorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelNineSliceButton::SetFontColorDown(const Color& value)
    {
      if (value != m_fontColorDown)
      {
        m_fontColorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::SetFontColorDisabled(const Color& value)
    {
      if (value != m_fontColorDisabled)
      {
        m_fontColorDisabled = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelNineSliceButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      const PxSize2D renderSizePx = RenderSizePx();


      auto& batch = *m_windowContext->Batch2D;


      if (m_backgroundNormal)
      {
        if (!IsDown() && m_isHovering && m_backgroundHover && IsEnabled())
        {
          ImageImpl::Draw(batch, m_backgroundHover.get(), context.TargetRect.TopLeft(), RenderSizePx(), m_backgroundCurrentColor.GetValue());
        }
        else
        {
          ImageImpl::Draw(batch, m_backgroundNormal.get(), context.TargetRect.TopLeft(), RenderSizePx(), m_backgroundCurrentColor.GetValue());
        }
      }

      const Color backgroundHoverOverlayColor = m_backgroundCurrentHoverOverlayColor.GetValue();
      if (m_backgroundHoverOverlay && backgroundHoverOverlayColor.PackedValue() > 0)
      {
        ImageImpl::Draw(batch, m_backgroundHoverOverlay.get(), context.TargetRect.TopLeft(), RenderSizePx(), backgroundHoverOverlayColor);
      }


      if (!m_content.empty())
      {
        const auto* const pFont = m_font.get();
        if (m_font != nullptr)
        {
          const PxThickness contentMarginPx = ImageImpl::GetContentMargin(m_backgroundHover.get(), m_backgroundNormal.get());
          const PxThickness paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_paddingDp);
          const int32_t widthPx = renderSizePx.Width() - contentMarginPx.SumX() - paddingPx.SumX() - m_labelSizePx.Width();
          const int32_t heightPx = renderSizePx.Height() - contentMarginPx.SumY() - paddingPx.SumY() - m_labelSizePx.Height();
          const int32_t offsetXPx = CalcAlignmentPx(m_contentAlignmentX, widthPx);
          const int32_t offsetYPx = CalcAlignmentPx(m_contentAlignmentY, heightPx);

          auto posPxf = context.TargetRect.TopLeft();
          posPxf.X += static_cast<float>(contentMarginPx.Left() + paddingPx.Left() + offsetXPx);
          posPxf.Y += static_cast<float>(contentMarginPx.Top() + paddingPx.Top() + offsetYPx);
          batch.ChangeTo(static_cast<BlendState>(pFont->GetInfo().MaterialInfo.NativeMaterialFlags));
          batch.DrawString(*pFont, m_content, TypeConverter::To<Vector2>(posPxf), m_fontCurrentColor.GetValue());
        }
      }
    }


    void LabelNineSliceButton::OnMouseOver(const RoutedEventArgs& /*args*/, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
    {
      m_isHovering = (theEvent->GetState() == EventTransactionState::Begin);
      theEvent->Handled();
    }

    PxSize2D LabelNineSliceButton::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return finalSizePx;
    }


    PxSize2D LabelNineSliceButton::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      FSL_PARAM_NOT_USED(availableSizePx);

      auto paddingPx = m_windowContext->UnitConverter.ToPxThickness(m_paddingDp);
      const auto* const pFont = m_font.get();
      if (pFont != nullptr)
      {
        const auto& fontInfo = pFont->GetInfo();
        const PxSize2D measuredPx = pFont->MeasureString(StringViewLiteUtil::AsStringViewLite(m_content));
        m_labelSizePx = PxSize2D(measuredPx.Width(), fontInfo.ScaledLineSpacingPx);
      }
      PxSize2D desiredSizePx = paddingPx.Sum() + m_labelSizePx;
      ImageImpl::ApplyMeasure(desiredSizePx, m_backgroundHover.get(), m_backgroundNormal.get(), m_backgroundHoverOverlay.get());
      return desiredSizePx;
    }

    void LabelNineSliceButton::UpdateAnimation(const TransitionTimeSpan& timeSpan)
    {
      ButtonBase::UpdateAnimation(timeSpan);
      m_backgroundCurrentColor.Update(timeSpan);
      m_backgroundCurrentHoverOverlayColor.Update(timeSpan);
      m_fontCurrentColor.Update(timeSpan);
    }

    bool LabelNineSliceButton::UpdateAnimationState(const bool forceCompleteAnimation)
    {
      const bool isEnabled = IsEnabled();
      const bool isDown = IsDown();

      // Determine background color

      const auto& backgroundColor = GetBackgroundColor(isEnabled, isDown, m_isHovering);
      m_backgroundCurrentColor.SetValue(backgroundColor);

      const auto& overlayColor =
        isEnabled && m_isHovering ? (!isDown ? m_backgroundColorHoverOverlayUp : m_backgroundColorHoverOverlayDown) : Color::Transparent();
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

    Color LabelNineSliceButton::GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const
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
