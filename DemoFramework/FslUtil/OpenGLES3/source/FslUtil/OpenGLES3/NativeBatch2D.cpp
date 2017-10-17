/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/OpenGLES3/Common.hpp>

#include <FslUtil/OpenGLES3/NativeBatch2D.hpp>
#include <FslUtil/OpenGLES3/NativeTexture2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>

namespace Fsl
{
  namespace GLES3
  {

    NativeBatch2D::NativeBatch2D(const Point2& screenResolution)
      : GLBatch2D(screenResolution)
    {

    }


    NativeBatch2D::NativeBatch2D(const std::shared_ptr<GLBatch2DQuadRenderer>& quadRenderer, const Point2& screenResolution)
      : GLBatch2D(quadRenderer, screenResolution)
    {
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstRectangle, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstRectangle, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstRectangle, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Rectangle& dstRectangle, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstRectangle, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstRectangle, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Rect& dstRectangle, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstRectangle, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, color, origin, scale);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");


      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, color, origin, scale);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, color, rotation, origin, scale);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");


      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, color, rotation, origin, scale);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, srcRectangle, color, origin, scale);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");


      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, srcRectangle, color, origin, scale);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), dstPosition, srcRectangle, color, rotation, origin, scale);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2& dstPosition, const Rectangle& srcRectangle, const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");


      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), dstPosition, srcRectangle, color, rotation, origin, scale);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2*const pDstPositions, const int32_t dstPositionsLength, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), pDstPositions, dstPositionsLength, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2*const pDstPositions, const int32_t dstPositionsLength, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), pDstPositions, dstPositionsLength, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2*const pDstPositions, const int32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetAtlasSize()), srcTexture.GetInfo()), pDstPositions, dstPositionsLength, srcRectangle, color);
    }


    void NativeBatch2D::Draw(const Texture2D& srcTexture, const Vector2*const pDstPositions, const int32_t dstPositionsLength, const Rectangle& srcRectangle, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::Draw(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), pDstPositions, dstPositionsLength, srcRectangle, color);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, psz, dstPosition, color);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, str, dstPosition, color);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, pStr, startIndex, length, dstPosition, color);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, str, startIndex, length, dstPosition, color);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char*const psz, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, psz, dstPosition, color, origin, scale);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, str, dstPosition, color, origin, scale);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const char*const pStr, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, pStr, startIndex, length, dstPosition, color, origin, scale);
    }


    void NativeBatch2D::DrawString(const Texture2D& srcTexture, const TextureAtlasBitmapFont& font, const std::string& str, const int32_t startIndex, const int32_t length, const Vector2& dstPosition, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DrawString(GLBatch2D::texture_type(nativeTex->Get(), srcTexture.GetSize()), font, str, startIndex, length, dstPosition, color, origin, scale);
    }


    void NativeBatch2D::DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rectangle& dstRect, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawRectangle(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetAtlasSize()), srcFillTexture.GetInfo()), dstRect, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const Texture2D& srcFillTexture, const Rectangle& dstRect, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawRectangle(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetSize()), dstRect, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const Rect& dstRect, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawRectangle(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetAtlasSize()), srcFillTexture.GetInfo()), dstRect, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const Texture2D& srcFillTexture, const Rect& dstRect, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawRectangle(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetSize()), dstRect, color);
    }


    void NativeBatch2D::DebugDrawLine(const AtlasTexture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawLine(GLBatch2D::atlas_texture_type(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetAtlasSize()), srcFillTexture.GetInfo()), dstFrom, dstTo, color);
    }


    void NativeBatch2D::DebugDrawLine(const Texture2D& srcFillTexture, const Vector2& dstFrom, const Vector2& dstTo, const Color& color)
    {
      const std::shared_ptr<NativeTexture2D> nativeTex = std::dynamic_pointer_cast<NativeTexture2D>(srcFillTexture.TryGetNative());
      if (!nativeTex)
        throw std::invalid_argument("srcTexture can not be a invalid texture");

      GLBatch2D::DebugDrawLine(GLBatch2D::texture_type(nativeTex->Get(), srcFillTexture.GetSize()), dstFrom, dstTo, color);
    }

  }
}
