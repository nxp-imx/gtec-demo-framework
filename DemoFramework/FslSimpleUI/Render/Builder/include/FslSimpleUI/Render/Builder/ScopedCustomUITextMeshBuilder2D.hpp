#ifndef FSLSIMPLEUI_RENDER_BUILDER_SCOPEDCUSTOMUITEXTMESHBUILDER2D_HPP
#define FSLSIMPLEUI_RENDER_BUILDER_SCOPEDCUSTOMUITEXTMESHBUILDER2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <FslSimpleUI/Render/Builder/UIRawMeshBuilder2D.hpp>
#include <FslSimpleUI/Render/Builder/UITextMeshBuilder.hpp>

namespace Fsl::UI
{
  class ScopedCustomUITextMeshBuilder2D
  {
    UIRawMeshBuilder2D& m_rBuilder;
    UITextMeshBuilder& m_rTextBuilder;
    const SpriteFont& m_spriteFont;
    bool m_clippingEnabled{false};
    PxAreaRectangleF m_clipRectanglePxf;

  public:
    ScopedCustomUITextMeshBuilder2D(UIRawMeshBuilder2D& rBuilder, UITextMeshBuilder& rTextBuilder, const SpriteFont& spriteFont)
      : m_rBuilder(rBuilder)
      , m_rTextBuilder(rTextBuilder)
      , m_spriteFont(spriteFont)
    {
    }

    ScopedCustomUITextMeshBuilder2D(UIRawMeshBuilder2D& rBuilder, UITextMeshBuilder& rTextBuilder, const SpriteFont& spriteFont,
                                    const PxAreaRectangleF clipRectanglePxf)
      : m_rBuilder(rBuilder)
      , m_rTextBuilder(rTextBuilder)
      , m_spriteFont(spriteFont)
      , m_clippingEnabled(true)
      , m_clipRectanglePxf(clipRectanglePxf)
    {
    }


    const SpriteFontInfo& GetFontInfo()
    {
      return m_spriteFont.GetInfo();
    }

    UIRenderColor GetColor() const
    {
      return m_rBuilder.GetColor();
    }

    void SetColor(const UIRenderColor color) const
    {
      m_rBuilder.SetColor(color);
    }

    void AddString(const PxVector2 dstPositionPxf, const StringViewLite strView)
    {
      if (!m_clippingEnabled)
      {
        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView);
      }
      else
      {
        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView, m_clipRectanglePxf);
      }
    }

    void AddString(const PxVector2 dstPositionPxf, const StringViewLite strView, const PxClipRectangle& clipRectPx)
    {
      if (!m_clippingEnabled)
      {
        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView, clipRectPx);
      }
      else
      {
        auto clipRectPxf = TypeConverter::To<PxAreaRectangleF>(clipRectPx);
        clipRectPxf = PxAreaRectangleF::Intersect(m_clipRectanglePxf, clipRectPxf);

        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView, clipRectPxf);
      }
    }

    void AddString(const PxVector2 dstPositionPxf, const StringViewLite strView, const PxAreaRectangleF& clipRectPxf)
    {
      if (!m_clippingEnabled)
      {
        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView, clipRectPxf);
      }
      else
      {
        auto finalClipRectPxf = PxAreaRectangleF::Intersect(m_clipRectanglePxf, clipRectPxf);
        m_rTextBuilder.AddString(m_rBuilder, m_spriteFont, dstPositionPxf, strView, finalClipRectPxf);
      }
    }
  };
}

#endif
