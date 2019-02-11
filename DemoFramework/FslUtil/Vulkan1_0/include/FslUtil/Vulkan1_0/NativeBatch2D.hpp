#ifndef FSLUTIL_VULKAN1_0_NATIVEBATCH2D_HPP
#define FSLUTIL_VULKAN1_0_NATIVEBATCH2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Batch/Batch2D.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    class NativeBatch2D
      : public Batch2D
      , public INativeBatch2D
    {
    public:
      NativeBatch2D(const std::shared_ptr<QuadBatch>& quadRenderer, const Point2& screenResolution);

      void Begin() override
      {
        return Batch2D::Begin();
      }

      void Begin(const BlendState blendState) override
      {
        return Batch2D::Begin(blendState);
      }

      void Begin(const BlendState blendState, const bool restoreState) override
      {
        return Batch2D::Begin(blendState, restoreState);
      }

      void ChangeTo(const BlendState blendState) override
      {
        return Batch2D::ChangeTo(blendState);
      }

      void End() override
      {
        return Batch2D::End();
      }

      void SetScreenResolution(const Point2& resolution) override
      {
        Batch2D::SetScreenResolution(resolution);
      }

      // Pull in the draw methods from Batch2D
      using Batch2D::DebugDrawLine;
      using Batch2D::DebugDrawRectangle;
      using Batch2D::Draw;
      using Batch2D::DrawString;

      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin,
                const Vector2& scale) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin,
                const Vector2& scale) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin,
                const Vector2& scale) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color,
                const Vector2& origin, const Vector2& scale) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin,
                const Vector2& scale) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation,
                const Vector2& origin, const Vector2& scale) override;
      void Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation,
                const Vector2& origin, const Vector2& scale) override;

      void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Color& color) override;
      void Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Rectangle& srcRectangle,
                const Color& color) override;
      void Draw(const Texture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength, const Rectangle& srcRectangle,
                const Color& color) override;

      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPosition,
                      const Color& color) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition,
                      const Color& color) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const pStr, const int32_t startIndex,
                      const int32_t length, const Vector2& dstPosition, const Color& color) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex,
                      const int32_t length, const Vector2& dstPosition, const Color& color) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const psz, const Vector2& dstPosition,
                      const Color& color, const Vector2& origin, const Vector2& scale) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition,
                      const Color& color, const Vector2& origin, const Vector2& scale) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char* const pStr, const int32_t startIndex,
                      const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale) override;
      void DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex,
                      const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale) override;

      void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rectangle& dstRect, const Color& color) override;
      void DebugDrawRectangle(const Texture2D& srcFillTexture, const Rectangle& dstRect, const Color& color) override;
      void DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rect& dstRect, const Color& color) override;
      void DebugDrawRectangle(const Texture2D& srcFillTexture, const Rect& dstRect, const Color& color) override;
      void DebugDrawLine(const AtlasTexture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color) override;
      void DebugDrawLine(const Texture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color) override;
    };
  }
}

#endif
