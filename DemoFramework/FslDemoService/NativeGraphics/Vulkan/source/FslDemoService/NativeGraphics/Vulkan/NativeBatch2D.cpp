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

#include <FslDemoService/NativeGraphics/Vulkan/NativeBatch2D.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDevice.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsTexture.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Sprite/Material/ISpriteMaterial.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      inline VUTextureInfo TryExtract(const NativeGraphicsDevice* const pNativeDevice, const INativeTexture2D& srcTexture)
      {
        BasicNativeTextureHandle hNativeTexture = srcTexture.TryGetNativeHandle();
        if (!hNativeTexture.IsValid())
        {
          FSLLOG3_DEBUG_WARNING("There are currently no actual texture associated, call ignored");
          return {};
        }
        if (pNativeDevice == nullptr)
        {
          FSLLOG3_DEBUG_WARNING("NativeDevice is invalid, call ignored");
          return {};
        }
        VUTextureInfo textureInfo = pNativeDevice->TryGetTextureInfo(hNativeTexture);
        FSLLOG3_DEBUG_WARNING_IF(!textureInfo.IsValid(), "The handle did not point to a valid texture");
        return textureInfo;
      }

      inline VUTextureInfo TryExtract(const NativeGraphicsDevice* const pNativeDevice, const IBasicRenderSystem* const pBasicRenderSystem,
                                      const BasicSpriteMaterial& material)
      {
        if (pBasicRenderSystem == nullptr)
        {
          FSLLOG3_DEBUG_WARNING("pBasicRenderSystem is invalid, call ignored");
          return {};
        }
        std::shared_ptr<INativeTexture2D> nativeTexture = pBasicRenderSystem->TryGetMaterialTexture(material.Material);
        if (!nativeTexture)
        {
          FSLLOG3_DEBUG_WARNING("material texture not of the expected type, call ignored");
          return {};
        }
        return TryExtract(pNativeDevice, *nativeTexture);
      }

      inline VUTextureInfo TryExtract(const NativeGraphicsDevice* const pNativeDevice, const IBasicRenderSystem* const pBasicRenderSystem,
                                      const SpriteFont& font)
      {
        assert(font.GetMaterialCount() == 1u);
        const auto* pBasicSpriteMaterial = dynamic_cast<const BasicSpriteMaterial*>(font.GetMaterialInfo(0u).Material.get());
        if (pBasicSpriteMaterial == nullptr)
        {
          FSLLOG3_DEBUG_WARNING("material texture not of the expected type, call ignored");
          return {};
        }
        return TryExtract(pNativeDevice, pBasicRenderSystem, *pBasicSpriteMaterial);
      }


      inline VUTextureInfo TryExtract(const NativeGraphicsDevice* const pNativeDevice, const BaseTexture2D& srcTexture)
      {
        const INativeTexture2D* const pNativeTexture2D = srcTexture.TryGetNativePointer();
        if (pNativeTexture2D == nullptr)
        {
          FSLLOG3_DEBUG_WARNING("Trying to render a invalid texture, call ignored");
          return {};
        }
        return TryExtract(pNativeDevice, *pNativeTexture2D);
      }


      inline VUTextureInfo TryExtract(const NativeGraphicsDevice* const pNativeDevice, const AtlasTexture2D& srcTexture)
      {
        const INativeTexture2D* const pNativeTexture2D = srcTexture.TryGetNativePointer();
        if (pNativeTexture2D == nullptr)
        {
          FSLLOG3_DEBUG_WARNING("Trying to render a invalid texture, call ignored");
          return {};
        }
        return TryExtract(pNativeDevice, *pNativeTexture2D);
      }
    }

    NativeBatch2D::NativeBatch2D(std::weak_ptr<IBasicRenderSystem> basicRenderSystem, std::weak_ptr<NativeGraphicsDevice> nativeGraphicsDevice,
                                 const std::shared_ptr<QuadBatch>& quadRenderer, const PxExtent2D& currentExtent)
      : Batch2D(quadRenderer, currentExtent)
      , m_renderSystem(std::move(basicRenderSystem))
      , m_nativeGraphicsDevice(std::move(nativeGraphicsDevice))
    {
    }

    NativeBatch2D::~NativeBatch2D() = default;

    // ---------- 0

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Vector4& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Vector4& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color);
    }


    // ---------- 0 with clip

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Color& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color, clipRectPx);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeTextureArea& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Vector4& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color, clipRectPx);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Color& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color, clipRectPx);
    }

    void NativeBatch2D::Draw(const INativeTexture2D& srcTexture, const NativeQuadTextureCoords& srcArea, const PxAreaRectangleF& dstRectanglePxf,
                             const Vector4& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(textureInfo, srcArea, dstRectanglePxf, color, clipRectPx);
    }

    // ---------- 1

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePx, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePx, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePxf, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePxf, color);
    }


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePxf, color, effect);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color, const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePxf, color, effect);
    }

    // ---------- 2

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color);
    }

    // ---------- 2 with clip

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color, clipRectPx);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color,
                             const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color, clipRectPx);
    }

    // ---------- 2A

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color, effect);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color,
                             const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color, effect);
    }

    // ---------- 3

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU32& srcRectanglePx,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePx, srcRectanglePx, color);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxRectangle& dstRectanglePx, const PxRectangleU32& srcRectanglePx,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePx, srcRectanglePx, color);
    }

    // ---------- 4

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePxf, srcRectanglePx, color);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePxf, srcRectanglePx, color);
    }

    // ---------- 4 with clip


    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePxf, srcRectanglePx, color, clipRectPx);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePxf, srcRectanglePx, color, clipRectPx);
    }


    // ---------- 4A

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstRectanglePxf, srcRectanglePx, color, effect);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const PxAreaRectangleF& dstRectanglePxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const BatchEffect effect)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstRectanglePxf, srcRectanglePx, color, effect);
    }


    // ---------- 5

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                             const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, color, origin, scale);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                             const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, color, origin, scale);
    }

    // ---------- 6

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation,
                             const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, color, rotation, origin, scale);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const Color& color, const float rotation,
                             const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, color, rotation, origin, scale);
    }

    // ---------- 7

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color, origin, scale);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color,
                             const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color, origin, scale);
    }

    // ---------- 7 with clip

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color, origin, scale, clipRectPx);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color,
                             const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color, origin, scale,
                    clipRectPx);
    }

    // ---------- 8

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx,
                             const Color& color, const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), dstPositionPxf, srcRectanglePx, color, rotation, origin, scale);
    }

    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2& dstPositionPxf, const PxRectangleU32& srcRectanglePx, const Color& color,
                             const float rotation, const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), dstPositionPxf, srcRectanglePx, color, rotation, origin,
                    scale);
    }

    // ---------- 9

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), pDstPositions, dstPositionsLength, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                             const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), pDstPositions, dstPositionsLength, color);
    }

    // ---------- 10

    void NativeBatch2D::Draw(const AtlasTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                             const PxRectangleU32& srcRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::atlas_texture_type(textureInfo, srcTexture.GetInfo()), pDstPositions, dstPositionsLength, srcRectanglePx, color);
    }


    void NativeBatch2D::Draw(const BaseTexture2D& srcTexture, const Vector2* const pDstPositions, const int32_t dstPositionsLength,
                             const PxRectangleU32& srcRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::Draw(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), pDstPositions, dstPositionsLength, srcRectanglePx, color);
    }

    // ---------- 11

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const StringViewLite& strView,
                                   const Vector2& dstPositionPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, strView, dstPositionPxf, color);
    }

    // ---------- 12

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const StringViewLite& strView,
                                   const Vector2& dstPositionPxf, const Color& color, const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, strView, dstPositionPxf, color, origin, scale);
    }

    // ---------- 13

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                                   const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, fontConfig, strView, dstPositionPxf, color);
    }

    // ---------- 13 with clip

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                                   const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                                   const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, fontConfig, strView, dstPositionPxf, color,
                          clipRectPx);
    }

    // ---------- 14

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                                   const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                   const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, fontConfig, strView, dstPositionPxf, color,
                          origin, scale);
    }

    // ---------- 14 with clip

    void NativeBatch2D::DrawString(const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                                   const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color, const Vector2& origin,
                                   const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcTexture);
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, srcTexture.GetSize()), font, fontConfig, strView, dstPositionPxf, color,
                          origin, scale, clipRectPx);
    }

    // ---------- 15

    void NativeBatch2D::DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), m_current.RenderSystem.get(), font);
      const auto& fontInfo = font.GetInfo();
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, fontInfo.MaterialInfo.ExtentPx), font.GetTextureAtlasSpriteFont(),
                          fontInfo.FontConfig, strView, dstPositionPxf, color);
    }

    // ---------- 15 with clip

    void NativeBatch2D::DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                                   const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), m_current.RenderSystem.get(), font);
      const auto& fontInfo = font.GetInfo();
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, fontInfo.MaterialInfo.ExtentPx), font.GetTextureAtlasSpriteFont(),
                          fontInfo.FontConfig, strView, dstPositionPxf, color, clipRectPx);
    }

    // ---------- 16

    void NativeBatch2D::DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                                   const Vector2& origin, const Vector2& scale)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), m_current.RenderSystem.get(), font);
      const auto& fontInfo = font.GetInfo();
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, fontInfo.MaterialInfo.ExtentPx), font.GetTextureAtlasSpriteFont(),
                          fontInfo.FontConfig, strView, dstPositionPxf, color, origin, scale);
    }

    // ---------- 16 with clip

    void NativeBatch2D::DrawString(const SpriteFont& font, const StringViewLite& strView, const Vector2& dstPositionPxf, const Color& color,
                                   const Vector2& origin, const Vector2& scale, const PxClipRectangle& clipRectPx)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), m_current.RenderSystem.get(), font);
      const auto& fontInfo = font.GetInfo();
      Batch2D::DrawString(Batch2D::texture_type(textureInfo.ImageInfo, fontInfo.MaterialInfo.ExtentPx), font.GetTextureAtlasSpriteFont(),
                          fontInfo.FontConfig, strView, dstPositionPxf, color, origin, scale, clipRectPx);
    }
    // ----------

    void NativeBatch2D::DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const PxRectangle& dstRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawRectangle(Batch2D::atlas_texture_type(textureInfo, srcFillTexture.GetInfo()), dstRectanglePx, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const BaseTexture2D& srcFillTexture, const PxRectangle& dstRectanglePx, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawRectangle(textureInfo, dstRectanglePx, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const AtlasTexture2D& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawRectangle(Batch2D::atlas_texture_type(textureInfo, srcFillTexture.GetInfo()), dstRectanglePxf, color);
    }


    void NativeBatch2D::DebugDrawRectangle(const BaseTexture2D& srcFillTexture, const PxAreaRectangleF& dstRectanglePxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawRectangle(textureInfo, dstRectanglePxf, color);
    }

    // ----------

    void NativeBatch2D::DebugDrawLine(const AtlasTexture2D& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawLine(Batch2D::atlas_texture_type(textureInfo, srcFillTexture.GetInfo()), dstFromPxf, dstToPxf, color);
    }


    void NativeBatch2D::DebugDrawLine(const BaseTexture2D& srcFillTexture, const Vector2& dstFromPxf, const Vector2& dstToPxf, const Color& color)
    {
      const VUTextureInfo textureInfo = TryExtract(m_current.NativeDevice.get(), srcFillTexture);
      Batch2D::DebugDrawLine(textureInfo, dstFromPxf, dstToPxf, color);
    }
  }
}
