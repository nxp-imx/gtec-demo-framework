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

#include <FslSimpleUI/Control/CheckBox.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslSimpleUI/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/PropertyTypeFlags.hpp>
#include <FslSimpleUI/UIDrawContext.hpp>
#include <FslSimpleUI/WindowContext.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace UI
  {

    CheckBox::CheckBox(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_text()
      , m_font(context->DefaultFont)
      , m_texChecked()
      , m_texUnchecked()
      , m_isChecked(false)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput));
    }


    void CheckBox::SetFont(const std::shared_ptr<AtlasFont>& value)
    {
      if (!value)
        throw std::invalid_argument("font can not be null");

      if (value != m_font)
      {
        m_font = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void CheckBox::SetText(const std::string& value)
    {
      if (value != m_text)
      {
        m_text = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void CheckBox::SetCheckedTexture(const AtlasTexture2D& value)
    {
      if (value != m_texChecked)
      {
        m_texChecked = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void CheckBox::SetUncheckedTexture(const AtlasTexture2D& value)
    {
      if (value != m_texUnchecked)
      {
        m_texUnchecked = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void CheckBox::SetIsChecked(const bool value)
    {
      if (value != m_isChecked)
      {
        m_isChecked = value;
        PropertyUpdated(PropertyType::Content);
        SendEvent(GetEventPool()->AcquireWindowContentChangedEvent(0));
      }
    }

    void CheckBox::Toggle()
    {
      SetIsChecked(!m_isChecked);
    }

    void CheckBox::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);

      if (m_text.size() == 0)
        return;

      const auto batch = GetContext()->Batch2D;
      batch->ChangeTo(BlendState::AlphaBlend);

      Vector2 position = context.TargetRect.TopLeft();
      if (m_text.size() > 0)
      {
        auto fontInfo = m_font->GetAtlasBitmapFont();
        auto measured = fontInfo.MeasureString(m_text.c_str(), 0, m_text.size());
        measured.Y = fontInfo.LineSpacing();

        float centeredY = 0.0f;
        if (measured.Y < context.TargetRect.Height())
          centeredY = std::floor((context.TargetRect.Height() - measured.Y) * 0.5f);

        const auto pFont = m_font.get();
        assert(pFont != nullptr);
        batch->DrawString(pFont->GetAtlasTexture(), pFont->GetAtlasBitmapFont(), m_text, Vector2(position.X, position.Y + centeredY), Color::White());

        position.X += measured.X;
      }

      const AtlasTexture2D& rTex = m_isChecked ? m_texChecked : m_texUnchecked;
      if (rTex.IsValid())
      {
        float centeredY = 0.0f;
        if (rTex.GetSize().Y < context.TargetRect.Height())
          centeredY = std::floor((context.TargetRect.Height() - rTex.GetSize().Y) * 0.5f);

        batch->Draw(rTex, Vector2(position.X, position.Y + centeredY), Color::White());
      }
    }


    void CheckBox::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      if (!theEvent->IsHandled() && theEvent->IsBegin() && !theEvent->IsRepeat())
      {
        theEvent->Handled();
        SetIsChecked(!m_isChecked);
      }
    }


    Vector2 CheckBox::ArrangeOverride(const Vector2& finalSize)
    {
      return finalSize;
    }


    Vector2 CheckBox::MeasureOverride(const Vector2& availableSize)
    {
      Vector2 size;
      if (m_text.size() > 0)
      {
        auto fontInfo = m_font->GetAtlasBitmapFont();
        auto measured = fontInfo.MeasureString(m_text.c_str(), 0, m_text.size());
        size = Vector2(measured.X, fontInfo.LineSpacing());
      }

      const Point2 sizeTex1 = m_texChecked.IsValid() ? m_texChecked.GetSize() : Point2();
      const Point2 sizeTex2 = m_texUnchecked.IsValid() ? m_texChecked.GetSize() : Point2();
      const Point2 sizeTex(std::max(sizeTex1.X, sizeTex2.X), std::max(sizeTex1.Y, sizeTex2.Y));

      size.X += sizeTex.X;
      size.Y = std::max(size.Y, static_cast<float>(sizeTex.Y));

      return size;
    }
  }
}
