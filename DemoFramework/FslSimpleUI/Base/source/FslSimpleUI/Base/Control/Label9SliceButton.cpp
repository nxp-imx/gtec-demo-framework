/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/Base/Control/Label9SliceButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "Impl/Draw9SliceImpl.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      inline float CalcAlignment(const ItemAlignment alignment, const float delta)
      {
        switch (alignment)
        {
        case ItemAlignment::Center:
          return delta / 2.0f;
        case ItemAlignment::Far:
          return delta;
        case ItemAlignment::Near:
        default:
          return 0;
        }
      }
    }

    Label9SliceButton::Label9SliceButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_font(context->DefaultFont)
      , m_colorUp(Color::White())
      , m_colorDown(0xB0B0B0B0)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled));
    }


    void Label9SliceButton::SetContentAlignmentX(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentX)
      {
        m_contentAlignmentX = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void Label9SliceButton::SetContentAlignmentY(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentY)
      {
        m_contentAlignmentY = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void Label9SliceButton::SetPadding(const ThicknessF& value)
    {
      if (value != m_padding)
      {
        m_padding = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void Label9SliceButton::SetBackground(const AtlasTexture2D& value)
    {
      if (value != m_background)
      {
        m_background = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void Label9SliceButton::SetNineSlice(const NineSlice& value)
    {
      if (value != m_nineSlice)
      {
        m_nineSlice = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Label9SliceButton::SetMinimumBackgroundMargin(const ThicknessF& value)
    {
      if (value != m_minimumBackgroundMargin)
      {
        m_minimumBackgroundMargin = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Label9SliceButton::SetContent(const std::string& value)
    {
      if (value == m_content)
      {
        return;
      }

      m_content = value;
      PropertyUpdated(PropertyType::Content);
    }


    void Label9SliceButton::SetFont(const std::shared_ptr<AtlasFont>& value)
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


    void Label9SliceButton::SetColorUp(const Color& value)
    {
      if (value != m_colorUp)
      {
        m_colorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Label9SliceButton::SetColorDown(const Color& value)
    {
      if (value != m_colorDown)
      {
        m_colorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Label9SliceButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      const auto color = !IsDown() ? m_colorUp : m_colorDown;

      if (m_background.IsValid())
      {
        Draw9SliceImpl::WinDraw(m_windowContext->Batch2D, context.TargetRect, m_background, m_nineSlice, m_minimumBackgroundMargin, color);
      }

      if (!m_content.empty())
      {
        const auto width = context.TargetRect.Width() - m_padding.SumX() - m_labelSize.X;
        const auto height = context.TargetRect.Height() - m_padding.SumY() - m_labelSize.Y;
        const auto offsetX = CalcAlignment(m_contentAlignmentX, width);
        const auto offsetY = CalcAlignment(m_contentAlignmentY, height);

        const auto batch = m_windowContext->Batch2D;
        const auto pFont = m_font.get();
        assert(pFont != nullptr);
        auto pos = context.TargetRect.TopLeft();
        pos.X += m_padding.Left() + offsetX;
        pos.Y += m_padding.Top() + offsetY;
        batch->DrawString(pFont->GetAtlasTexture(), pFont->GetAtlasBitmapFont(), m_content, pos, color);
      }
    }


    Vector2 Label9SliceButton::ArrangeOverride(const Vector2& finalSize)
    {
      return finalSize;
    }


    Vector2 Label9SliceButton::MeasureOverride(const Vector2& availableSize)
    {
      FSL_PARAM_NOT_USED(availableSize);

      const auto fontInfo = m_font->GetAtlasBitmapFont();
      const auto measured = fontInfo.MeasureString(m_content.c_str(), 0, m_content.size());
      m_labelSize = Vector2(measured.X, fontInfo.LineSpacing());

      // Measure our content
      return Vector2(m_padding.SumX() + m_labelSize.X, m_padding.SumY() + m_labelSize.Y);
    }
  }
}
