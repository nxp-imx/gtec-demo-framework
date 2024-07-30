#ifndef FSLSIMPLEUI_RENDER_BUILDER_UITEXTMESHBUILDER_HPP
#define FSLSIMPLEUI_RENDER_BUILDER_UITEXTMESHBUILDER_HPP
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
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Render/Batch2DUtil.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontGlyphPosition.hpp>
#include <FslSimpleUI/Render/Builder/UIRawMeshBuilder2D.hpp>
#include <FslSimpleUI/Render/Builder/UITextMeshBuilder.hpp>
#include <algorithm>
#include <vector>

namespace Fsl::UI
{
  class UIRawMeshBuilder2D;

  class UITextMeshBuilder
  {
    std::vector<SpriteFontGlyphPosition> m_glyphScratchpad;

  public:
    explicit UITextMeshBuilder(const uint32_t initialCapacity)
      : m_glyphScratchpad(initialCapacity)
    {
    }

    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView)
    {
      if (strView.empty())
      {
        return;
      }

      const auto maxChars = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const StringViewLite strRenderView = strView.substr(0, maxChars);

      // Ensure capacity for our glyphs
      if (m_glyphScratchpad.size() < strRenderView.size())
      {
        m_glyphScratchpad.resize(strRenderView.size());
      }

      const SpriteFontInfo& info = spriteFont.GetInfo();
      const TextureAtlasSpriteFont& atlasBitmapFont = spriteFont.GetTextureAtlasSpriteFont();

      // Extract the render rules
      auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
      if (atlasBitmapFont.ExtractRenderRules(glyphScratchpadSpan, strRenderView, info.FontConfig))
      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(strRenderView.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphScratchpadSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphScratchpadSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            rBuilder.AddRect(dstPositionPxf.X + srcGlyph.DstRectPxf.Left(), dstPositionPxf.Y + srcGlyph.DstRectPxf.Top(),
                             dstPositionPxf.X + srcGlyph.DstRectPxf.Right(), dstPositionPxf.Y + srcGlyph.DstRectPxf.Bottom(), srcGlyph.TextureArea);
          }
        }
      }
    }


    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView,
                   const PxClipRectangle& clipRectPx)
    {
      if (strView.empty() || clipRectPx.Left() == clipRectPx.Right() || clipRectPx.Top() == clipRectPx.Bottom())
      {
        return;
      }

      const auto maxChars = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const StringViewLite strRenderView = strView.substr(0, maxChars);

      // Ensure capacity for our glyphs
      if (m_glyphScratchpad.size() < strRenderView.size())
      {
        m_glyphScratchpad.resize(strRenderView.size());
      }

      const SpriteFontInfo& info = spriteFont.GetInfo();
      const TextureAtlasSpriteFont& atlasBitmapFont = spriteFont.GetTextureAtlasSpriteFont();

      // Extract the render rules
      auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
      if (atlasBitmapFont.ExtractRenderRules(glyphScratchpadSpan, strRenderView, info.FontConfig))
      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(strRenderView.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphScratchpadSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphScratchpadSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            PxAreaRectangleF dstRectanglePxf(PxAreaRectangleF::AddLocation(dstPositionPxf, srcGlyph.DstRectPxf));
            NativeTextureArea textureArea(srcGlyph.TextureArea);
            if (Batch2DUtil::Clip(dstRectanglePxf, textureArea, clipRectPx))
            {
              rBuilder.AddRect(dstRectanglePxf, textureArea);
            }
          }
        }
      }
    }

    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView,
                   const PxAreaRectangleF& clipRectPxf)
    {
      if (strView.empty() || clipRectPxf.Left() == clipRectPxf.Right() || clipRectPxf.Top() == clipRectPxf.Bottom())
      {
        return;
      }

      const auto maxChars = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const StringViewLite strRenderView = strView.substr(0, maxChars);

      // Ensure capacity for our glyphs
      if (m_glyphScratchpad.size() < strRenderView.size())
      {
        m_glyphScratchpad.resize(strRenderView.size());
      }

      const SpriteFontInfo& info = spriteFont.GetInfo();
      const TextureAtlasSpriteFont& atlasBitmapFont = spriteFont.GetTextureAtlasSpriteFont();

      // Extract the render rules
      auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
      if (atlasBitmapFont.ExtractRenderRules(glyphScratchpadSpan, strRenderView, info.FontConfig))
      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(strRenderView.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphScratchpadSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphScratchpadSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            PxAreaRectangleF dstRectanglePxf(PxAreaRectangleF::AddLocation(dstPositionPxf, srcGlyph.DstRectPxf));
            NativeTextureArea textureArea(srcGlyph.TextureArea);
            if (Batch2DUtil::Clip(dstRectanglePxf, textureArea, clipRectPxf))
            {
              rBuilder.AddRect(dstRectanglePxf, textureArea);
            }
          }
        }
      }
    }

    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs)
    {
      if (glyphs.empty())
      {
        return;
      }

      const auto maxCharsInBuffer = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const ReadOnlySpan<SpriteFontGlyphPosition> glyphSpan = glyphs.clamped_first(maxCharsInBuffer);

      // Extract the render rules
      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(glyphSpan.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            rBuilder.AddRect(dstPositionPxf.X + srcGlyph.DstRectPxf.Left(), dstPositionPxf.Y + srcGlyph.DstRectPxf.Top(),
                             dstPositionPxf.X + srcGlyph.DstRectPxf.Right(), dstPositionPxf.Y + srcGlyph.DstRectPxf.Bottom(), srcGlyph.TextureArea);
          }
        }
      }
    }

    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs,
                   const PxClipRectangle& clipRectPx)
    {
      if (glyphs.empty() || clipRectPx.Left() == clipRectPx.Right() || clipRectPx.Top() == clipRectPx.Bottom())
      {
        return;
      }

      const auto maxCharsInBuffer = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const ReadOnlySpan<SpriteFontGlyphPosition> glyphSpan = glyphs.clamped_first(maxCharsInBuffer);

      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(glyphSpan.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            PxAreaRectangleF dstRectanglePxf(PxAreaRectangleF::AddLocation(dstPositionPxf, srcGlyph.DstRectPxf));
            NativeTextureArea textureArea(srcGlyph.TextureArea);
            if (Batch2DUtil::Clip(dstRectanglePxf, textureArea, clipRectPx))
            {
              rBuilder.AddRect(dstRectanglePxf, textureArea);
            }
          }
        }
      }
    }

    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs,
                   const PxAreaRectangleF& clipRectPxf)
    {
      if (glyphs.empty() || clipRectPxf.Left() == clipRectPxf.Right() || clipRectPxf.Top() == clipRectPxf.Bottom())
      {
        return;
      }

      const auto maxCharsInBuffer = std::min(rBuilder.GetRemainingVertexCapacity() / 4, rBuilder.GetRemainingIndexCapacity() / 6);
      const ReadOnlySpan<SpriteFontGlyphPosition> glyphSpan = glyphs.clamped_first(maxCharsInBuffer);

      // Extract the render rules
      {
        const uint32_t glyphCount = std::min(UncheckedNumericCast<uint32_t>(glyphSpan.size()), rBuilder.GetRemainingVertexCapacity() / 4);
        assert((rBuilder.GetRemainingVertexCapacity() / 4) <= (rBuilder.GetRemainingIndexCapacity() / 6));

        assert(glyphCount <= glyphSpan.size());
        for (uint32_t i = 0; i < glyphCount; ++i)
        {
          const SpriteFontGlyphPosition& srcGlyph = glyphSpan[i];
          if (srcGlyph.TextureArea.X1 > srcGlyph.TextureArea.X0)
          {
            PxAreaRectangleF dstRectanglePxf(PxAreaRectangleF::AddLocation(dstPositionPxf, srcGlyph.DstRectPxf));
            NativeTextureArea textureArea(srcGlyph.TextureArea);
            if (Batch2DUtil::Clip(dstRectanglePxf, textureArea, clipRectPxf))
            {
              rBuilder.AddRect(dstRectanglePxf, textureArea);
            }
          }
        }
      }
    }
  };
}


#endif
