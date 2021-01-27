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

#include <FslSimpleUI/Base/Control/LabelBase.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace UI
  {
    LabelBase::LabelBase(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
      , m_font(context->DefaultFont)
    {
      if (!m_font)
      {
        throw std::invalid_argument("context->DefaultFont is invalid");
      }
      Enable(WindowFlags(WindowFlags::DrawEnabled));
    }

    bool LabelBase::SetEnabled(const bool enabled)
    {
      const bool modified = enabled != m_enabled;
      if (modified)
      {
        m_enabled = enabled;
        PropertyUpdated(PropertyType::Other);
      }
      return modified;
    }


    void LabelBase::SetContentAlignmentX(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentX)
      {
        m_contentAlignmentX = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void LabelBase::SetContentAlignmentY(const ItemAlignment& value)
    {
      if (value != m_contentAlignmentY)
      {
        m_contentAlignmentY = value;
        PropertyUpdated(PropertyType::Alignment);
      }
    }


    void LabelBase::SetFont(const std::shared_ptr<SpriteFont>& value)
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

    void LabelBase::SetFontColor(const Color& color)
    {
      if (color != m_fontColor)
      {
        m_fontColor = color;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void LabelBase::SetFontDisabledColor(const Color& color)
    {
      if (color != m_fontDisabledColor)
      {
        m_fontDisabledColor = color;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void LabelBase::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);

      const auto content = DoGetContent();

      SpriteFont* const pFont = m_font.get();
      if (content.empty() || pFont == nullptr)
      {
        return;
      }

      const auto batch = m_windowContext->Batch2D;

      assert(pFont != nullptr);

      auto stringSizePx = pFont->MeasureString(content);
      auto dstPosPxf = context.TargetRect.TopLeft();

      switch (m_contentAlignmentX)
      {
      case ItemAlignment::Center:
      {
        auto renderSizePx = RenderSizePx();
        // We intentionally do the following calc in integers and convert it to a float at the end
        // NOLINTNEXTLINE(bugprone-integer-division)
        dstPosPxf.X += float((renderSizePx.Width() - stringSizePx.Width()) / 2);
        break;
      }
      case ItemAlignment::Far:
      {
        auto renderSizePx = RenderSizePx();
        dstPosPxf.X += float(renderSizePx.Width() - stringSizePx.Width());
        break;
      }
      case ItemAlignment::Near:
      case ItemAlignment::Stretch:
      default:
        break;
      }

      switch (m_contentAlignmentY)
      {
      case ItemAlignment::Center:
      {
        auto renderSizePx = RenderSizePx();
        // We intentionally do the following calc in integers and convert it to a float at the end
        // NOLINTNEXTLINE(bugprone-integer-division)
        dstPosPxf.Y += float((renderSizePx.Height() - stringSizePx.Height()) / 2);
        break;
      }
      case ItemAlignment::Far:
      {
        auto renderSizePx = RenderSizePx();
        dstPosPxf.Y += float(renderSizePx.Height() - stringSizePx.Height());
        break;
      }
      case ItemAlignment::Near:
      case ItemAlignment::Stretch:
      default:
        break;
      }

      const auto& color = m_enabled ? m_fontColor : m_fontDisabledColor;

      batch->ChangeTo(static_cast<BlendState>(pFont->GetInfo().MaterialInfo.NativeMaterialFlags));
      batch->DrawString(*pFont, content, TypeConverter::To<Vector2>(dstPosPxf), color);
    }


    PxSize2D LabelBase::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return finalSizePx;
    }


    PxSize2D LabelBase::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      FSL_PARAM_NOT_USED(availableSizePx);

      const auto* pFont = m_font.get();
      assert(pFont != nullptr);
      const auto content = DoGetContent();
      const auto& fontInfo = pFont->GetInfo();
      auto measured = pFont->MeasureString(content);
      return {measured.Width(), fontInfo.ScaledLineSpacingPx};
    }


    PxPoint2 LabelBase::DoMeasureRenderedString(const StringViewLite& value) const
    {
      const auto* pFont = m_font.get();
      assert(pFont != nullptr);
      const auto& fontInfo = pFont->GetInfo();
      auto measured = pFont->MeasureString(value);
      return {measured.Width(), fontInfo.ScaledLineSpacingPx};
    }
  }
}
