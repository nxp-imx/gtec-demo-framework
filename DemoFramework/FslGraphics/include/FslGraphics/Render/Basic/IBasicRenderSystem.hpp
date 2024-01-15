#ifndef FSLGRAPHICS_RENDER_BASIC_IBASICRENDERSYSTEM_HPP
#define FSLGRAPHICS_RENDER_BASIC_IBASICRENDERSYSTEM_HPP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Render/Basic/BasicBufferUsage.hpp>
#include <FslGraphics/Render/Basic/BasicMaterial.hpp>
#include <FslGraphics/Render/Basic/BasicShader.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/TextureFlags.hpp>
// #include <FslGraphics/Render/Basic/BasicMaterialCreateInfo.hpp>
// #include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
// #include <FslGraphics/Render/Basic/IBasicStaticBuffer.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpan.hpp>
#include <memory>

namespace Fsl
{
  struct BasicCameraInfo;
  struct BasicNativeTextureHandle;
  struct BasicMaterialCreateInfo;
  struct BasicShaderCreateInfo;
  class Bitmap;
  class IBasicNativeTexture;
  class IBasicStaticBuffer;
  class IBasicDynamicBuffer;
  class IDynamicNativeTexture2D;
  class INativeTexture2D;
  class RawBitmap;
  class RawTexture;
  class Texture;

  class IBasicRenderSystem
  {
  public:
    virtual ~IBasicRenderSystem() noexcept = default;

    // Texture creation

    virtual std::shared_ptr<INativeTexture2D> CreateTexture2D(const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                                                              const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                              const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<INativeTexture2D> CreateTexture2D(const Texture& texture, const Texture2DFilterHint filterHint,
                                                              const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const Texture& texture, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                              const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;
    virtual std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags = TextureFlags::NotDefined) = 0;

    virtual PxExtent2D GetTextureExtentPx(const std::shared_ptr<INativeTexture2D>& texture) const noexcept = 0;

    virtual const IBasicNativeTexture* TryGetNativeTexture(const BasicNativeTextureHandle& hTexture) const noexcept = 0;

    // Generic buffer creation (index buffer)
    virtual std::shared_ptr<IBasicStaticBuffer> CreateBuffer(const ReadOnlySpan<uint16_t> indexSpan, const BasicBufferUsage usage) = 0;
    virtual std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const ReadOnlySpan<uint16_t> indexSpan) = 0;
    virtual std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const ReadOnlySpan<uint16_t> indexSpan, const uint32_t capacity) = 0;
    virtual std::shared_ptr<IBasicStaticBuffer> CreateStaticBuffer(const ReadOnlySpan<uint16_t> indexSpan) = 0;

    // Generic buffer creation (vertex buffer)
    virtual std::shared_ptr<IBasicStaticBuffer> CreateBuffer(const ReadOnlyFlexVertexSpan& vertexSpan, const BasicBufferUsage usage) = 0;
    virtual std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const ReadOnlyFlexVertexSpan& vertexSpan) = 0;
    virtual std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const ReadOnlyFlexVertexSpan& vertexSpan, const uint32_t capacity) = 0;
    virtual std::shared_ptr<IBasicDynamicBuffer> CreateDynamicBuffer(const VertexDeclarationSpan vertexDeclaration, const uint32_t capacity) = 0;
    virtual std::shared_ptr<IBasicStaticBuffer> CreateStaticBuffer(const ReadOnlyFlexVertexSpan& vertexSpan) = 0;

    // Shaders
    virtual BasicShader CreateShader(const BasicShaderCreateInfo& createInfo) = 0;
    // virtual BasicShader CreateShader(const BasicCustomShaderCreateInfo& createInfo) = 0;

    // Materials
    virtual BasicMaterial CreateMaterial(const BasicMaterialCreateInfo& createInfo, const std::shared_ptr<INativeTexture2D>& texture,
                                         const bool isDynamic = false) = 0;

    //! @brief Provides a simple way to create a new instance of material with a updated texture
    virtual BasicMaterial CloneMaterial(const BasicMaterial& sourceMaterial, const std::shared_ptr<INativeTexture2D>& texture,
                                        const bool isDynamic = false) = 0;

    //! @brief Provides a simple way to create a new instance of material with different material info
    virtual BasicMaterial CloneMaterial(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo, const bool isDynamic = false) = 0;

    //! @brief Get basic information about the material
    virtual BasicMaterialInfo GetMaterialInfo(const BasicMaterial& sourceMaterial) const = 0;
    virtual void SetMaterialInfo(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo) = 0;

    virtual std::shared_ptr<INativeTexture2D> GetMaterialTexture(const BasicMaterial& hMaterial) const = 0;
    virtual std::shared_ptr<INativeTexture2D> TryGetMaterialTexture(const BasicMaterial& hMaterial) const = 0;

    //! @brief Request that the backend starts the stat caching (if needed) at this point in time
    //!        If BeginCache is called then EndCache must also be called!
    //! @note  This is a optional request, if not called manually the first BeginCmds will execute this BeginCache instead.
    virtual void BeginCache() = 0;
    //! @brief Restore the cached state
    virtual void EndCache() = 0;

    //! @brief Begin a command sequence
    virtual void BeginCmds() = 0;
    //! @brief End a command sequence
    virtual void EndCmds() = 0;

    virtual void CmdSetCamera(const BasicCameraInfo& cameraInfo) = 0;
    virtual void CmdBindMaterial(const BasicMaterial& material) = 0;
    virtual void CmdBindIndexBuffer(const std::shared_ptr<IBasicStaticBuffer>& indexBuffer) = 0;
    virtual void CmdBindVertexBuffer(const std::shared_ptr<IBasicStaticBuffer>& vertexBuffer) = 0;
    virtual void CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex) = 0;
    virtual void CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex) = 0;
  };
}

#endif
