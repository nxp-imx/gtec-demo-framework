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
    TextureAtlasBitmapFont ReadFont(const IContentManager& contentManager, const IO::Path& pathBitmapFont)
    {
      BitmapFont bitmapFont;
      contentManager.Read(bitmapFont, pathBitmapFont);
      return TextureAtlasBitmapFont(bitmapFont);
    }

    void GenerateVertices(Span<VertexPositionTexture> dstVertices, const PxPoint2& dstPositionPx, const ReadOnlySpan<FontGlyphPosition>& positions,
                          float zPos, const PxSize2D& fontTextureSize,
                          const std::function<NativeTextureArea(const PxRectangleU&, const PxSize2D&)>& fnCalcTextureArea)
    {
      assert(fnCalcTextureArea);
      uint32_t dstVertexIndex = 0;
      for (std::size_t i = 0; i < positions.size(); ++i)
      {
        const auto& glyph = positions[i];
        if (glyph.SrcRectPx.Width > 0)
        {
          const auto dstX0 = float(dstPositionPx.X + glyph.DstRectPx.Offset.X);
          const auto dstY0 = float(dstPositionPx.Y + glyph.DstRectPx.Offset.Y);
          const auto dstX1 = float(dstPositionPx.X + glyph.DstRectPx.Offset.X + glyph.DstRectPx.Extent.Width);
          const auto dstY1 = float(dstPositionPx.Y + glyph.DstRectPx.Offset.Y + glyph.DstRectPx.Extent.Height);

          const auto nativeTexArea = fnCalcTextureArea(glyph.SrcRectPx, fontTextureSize);

          dstVertices[dstVertexIndex + 0] = VertexPositionTexture(Vector3(dstX0, dstY0, zPos), Vector2(nativeTexArea.X0, nativeTexArea.Y0));
          dstVertices[dstVertexIndex + 1] = VertexPositionTexture(Vector3(dstX1, dstY0, zPos), Vector2(nativeTexArea.X1, nativeTexArea.Y0));
          dstVertices[dstVertexIndex + 2] = VertexPositionTexture(Vector3(dstX0, dstY1, zPos), Vector2(nativeTexArea.X0, nativeTexArea.Y1));
          dstVertices[dstVertexIndex + 3] = VertexPositionTexture(Vector3(dstX1, dstY1, zPos), Vector2(nativeTexArea.X1, nativeTexArea.Y1));

          dstVertexIndex += 4;
        }
      }
    }

    void GenerateIndices(Span<uint16_t> dstIndices, const ReadOnlySpan<FontGlyphPosition>& positions)
    {
      uint32_t dstVertexIndex = 0;
      uint32_t dstIBIndex = 0;
      for (std::size_t i = 0; i < positions.size(); ++i)
      {
        const auto& glyph = positions[i];
        if (glyph.SrcRectPx.Width > 0)
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
