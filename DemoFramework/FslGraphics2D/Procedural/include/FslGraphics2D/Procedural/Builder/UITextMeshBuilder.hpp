#ifndef FSLGRAPHICS2D_PROCEDURAL_BUILDER_UITEXTMESHBUILDER_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BUILDER_UITEXTMESHBUILDER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontGlyphPosition.hpp>
#include <vector>

namespace Fsl
{
  struct PxClipRectangle;
  struct PxAreaRectangleF;
  class SpriteFont;
  class UIRawMeshBuilder2D;

  class UITextMeshBuilder
  {
    std::vector<SpriteFontGlyphPosition> m_glyphScratchpad;

  public:
    explicit UITextMeshBuilder(const uint32_t initialCapacity);
    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView);
    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView,
                   const PxClipRectangle& clipRectPx);
    void AddString(UIRawMeshBuilder2D& rBuilder, const SpriteFont& spriteFont, const PxVector2 dstPositionPxf, const StringViewLite strView,
                   const PxAreaRectangleF& clipRectPxf);

    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs);
    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs,
                   const PxClipRectangle& clipRectPx);
    void AddString(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const ReadOnlySpan<SpriteFontGlyphPosition> glyphs,
                   const PxAreaRectangleF& clipRectPxf);
  };
}


#endif
