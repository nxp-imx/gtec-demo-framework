/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslSimpleUI/Base/Control/LabelButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    LabelButton::LabelButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_windowContext(context)
      , m_font(context->DefaultFont)
      , m_colorUp(Color::White())
      , m_colorDown(0xB0B0B0B0)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled));
    }


    void LabelButton::SetContent(const std::string& value)
    {
      if (value == m_content)
      {
        return;
      }

      m_content = value;
      PropertyUpdated(PropertyType::Content);
    }


    void LabelButton::SetFont(const std::shared_ptr<AtlasFont>& value)
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


    void LabelButton::SetColorUp(const Color& value)
    {
      if (value != m_colorUp)
      {
        m_colorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelButton::SetColorDown(const Color& value)
    {
      if (value != m_colorDown)
      {
        m_colorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      if (m_content.empty())
      {
        return;
      }

      const auto color = !IsDown() ? m_colorUp : m_colorDown;
      const auto batch = m_windowContext->Batch2D;
      const auto pFont = m_font.get();
      assert(pFont != nullptr);
      batch->DrawString(pFont->GetAtlasTexture(), pFont->GetAtlasBitmapFont(), m_content, context.TargetRect.TopLeft(), color);
    }


    Vector2 LabelButton::ArrangeOverride(const Vector2& finalSize)
    {
      return finalSize;
    }


    Vector2 LabelButton::MeasureOverride(const Vector2& availableSize)
    {
      auto fontInfo = m_font->GetAtlasBitmapFont();
      auto measured = fontInfo.MeasureString(m_content.c_str(), 0, m_content.size());
      return Vector2(measured.X, fontInfo.LineSpacing());
    }
  }
}
