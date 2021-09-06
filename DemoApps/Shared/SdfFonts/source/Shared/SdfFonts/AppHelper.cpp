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

#include <Shared/SdfFonts/AppHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <cassert>

namespace Fsl
{
  namespace AppHelper
  {
    TextureAtlasSpriteFont ReadFont(const SpriteNativeAreaCalc& spriteNativeAreaCalc, const PxExtent2D textureExtentPx,
                                    const IContentManager& contentManager, const IO::Path& pathBitmapFont, const uint32_t densityDpi)
    {
      BitmapFont bitmapFont;
      contentManager.Read(bitmapFont, pathBitmapFont);
      return TextureAtlasSpriteFont(spriteNativeAreaCalc, textureExtentPx, bitmapFont, densityDpi);
    }

    void GenerateVertices(Span<VertexPositionTexture> dstVertices, const PxPoint2& dstPositionPx,
                          const ReadOnlySpan<SpriteFontGlyphPosition>& positions, float zPos, const PxSize2D& /*fontTextureSize*/)
    {
      uint32_t dstVertexIndex = 0;
      for (std::size_t i = 0; i < positions.size(); ++i)
      {
        const auto& glyph = positions[i];
        if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
        {
          const auto dstX0 = float(dstPositionPx.X) + glyph.DstRectPxf.Left();
          const auto dstY0 = float(dstPositionPx.Y) + glyph.DstRectPxf.Top();
          const auto dstX1 = float(dstPositionPx.X) + glyph.DstRectPxf.Right();
          const auto dstY1 = float(dstPositionPx.Y) + glyph.DstRectPxf.Bottom();

          dstVertices[dstVertexIndex + 0] = VertexPositionTexture(Vector3(dstX0, dstY0, zPos), Vector2(glyph.TextureArea.X0, glyph.TextureArea.Y0));
          dstVertices[dstVertexIndex + 1] = VertexPositionTexture(Vector3(dstX1, dstY0, zPos), Vector2(glyph.TextureArea.X1, glyph.TextureArea.Y0));
          dstVertices[dstVertexIndex + 2] = VertexPositionTexture(Vector3(dstX0, dstY1, zPos), Vector2(glyph.TextureArea.X0, glyph.TextureArea.Y1));
          dstVertices[dstVertexIndex + 3] = VertexPositionTexture(Vector3(dstX1, dstY1, zPos), Vector2(glyph.TextureArea.X1, glyph.TextureArea.Y1));

          dstVertexIndex += 4;
        }
      }
    }

    void GenerateIndices(Span<uint16_t> dstIndices, const ReadOnlySpan<SpriteFontGlyphPosition>& positions)
    {
      uint32_t dstVertexIndex = 0;
      uint32_t dstIBIndex = 0;
      for (std::size_t i = 0; i < positions.size(); ++i)
      {
        const auto& glyph = positions[i];
        if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
        {
          // A B
          // |\|
          // C D
          dstIndices[dstIBIndex + 0] = dstVertexIndex;
          dstIndices[dstIBIndex + 1] = dstVertexIndex + 2;
          dstIndices[dstIBIndex + 2] = dstVertexIndex + 3;
          dstIndices[dstIBIndex + 3] = dstVertexIndex + 3;
          dstIndices[dstIBIndex + 4] = dstVertexIndex + 1;
          dstIndices[dstIBIndex + 5] = dstVertexIndex;

          dstVertexIndex += 4;
          dstIBIndex += 6;
        }
      }
    }
  }
}
