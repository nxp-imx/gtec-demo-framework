#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSDEVICE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSDEVICE_HPP
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

#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeDevice.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/ExtendedCameraInfo.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsBufferFactory.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsMaterialFactory.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsTextureFactory.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/SaveGLState.hpp>
#include <memory>

namespace Fsl
{
  namespace GLES2
  {
    class NativeGraphicsDevice final : public Graphics3D::INativeDevice
    {
      struct CacheRecord
      {
        bool IsValid{false};
        SaveGLState SavedState;

        CacheRecord() = default;
        explicit CacheRecord(const bool /*dummy*/)
          : IsValid(true)
          , SavedState(true)
        {
        }
      };

      struct CommandsRecord
      {
        bool IsValid{false};
        BasicNativeBufferHandle IndexBufferHandle;
        BasicNativeBufferHandle VertexBufferHandle;
        BasicNativeMaterialHandle MaterialHandle;
        ExtendedCameraInfo CameraInfo;
        uint32_t CameraChangeId{0};
        bool VertexBufferModified{true};

        CommandsRecord(const CommandsRecord&) = delete;
        CommandsRecord& operator=(const CommandsRecord&) = delete;
        CommandsRecord& operator=(CommandsRecord&& other) noexcept = default;
        CommandsRecord(CommandsRecord&& other) noexcept = default;

        CommandsRecord() = default;
        explicit CommandsRecord(const bool /*dummy*/)
          : IsValid(true)
        {
        }

        bool SetCameraInfo(const ExtendedCameraInfo cameraInfo)
        {
          bool changed = CameraInfo != cameraInfo;
          if (changed)
          {
            CameraInfo = cameraInfo;
            ++CameraChangeId;
          }
          return changed;
        }
      };

      struct Frame
      {
        bool IsValid{false};
        CacheRecord Cache;
        CommandsRecord Commands;

        Frame() = default;
        explicit Frame(const bool isValid)
          : IsValid(isValid)
        {
        }
      };

      struct DependentResources
      {
        PxSize2D WindowSizePx;

        DependentResources() = default;
        explicit DependentResources(const PxSize2D windowSizePx)
          : WindowSizePx(windowSizePx)
        {
        }
      };

      NativeGraphicsBufferFactory m_bufferFactory;
      NativeGraphicsTextureFactory m_textureFactory;
      NativeGraphicsMaterialFactory m_materialFactory;

      bool m_isDisposed{false};
      DependentResources m_dependentResources;
      Frame m_frame;

    public:
      NativeGraphicsDevice();
      ~NativeGraphicsDevice() noexcept final;

      void Dispose() noexcept;

      GLTextureInfo TryGetTextureInfo(const BasicNativeTextureHandle hTexture) const;

      // Graphics3D::INativeBufferFactory
      Graphics3D::NativeBufferFactoryCaps GetBufferCaps() const final;

      BasicNativeBufferHandle CreateBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity,
                                           const bool isDynamic) final;
      bool DestroyBuffer(const BasicNativeBufferHandle hBuffer) final;
      void SetBufferData(const BasicNativeBufferHandle hBuffer, const uint32_t dstIndex, ReadOnlyFlexSpan bufferData) override;

      // Graphics3D::INativeMaterialFactory

      void CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles, ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan) final;
      bool DestroyMaterial(const BasicNativeMaterialHandle hMaterial) final;

      // Graphics3D::INativeTextureFactory
      Graphics3D::NativeTextureFactoryCaps GetTextureCaps() const final;

      BasicNativeTextureHandle CreateTexture(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags,
                                             const bool isDynamic) final;

      bool DestroyTexture(const BasicNativeTextureHandle hTexture) final;

      void SetTextureData(const BasicNativeTextureHandle hTexture, const RawTexture& texture, const Texture2DFilterHint filterHint,
                          const TextureFlags textureFlags) final;
      const IBasicNativeTexture* TryGetTexture(const BasicNativeTextureHandle hTexture) const override;

      // Graphics3D::INativeDevice
      void CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo) final;
      void DestroyDependentResources() final;

      void BeginFrame(const BasicNativeBeginFrameInfo& frameInfo) final;
      void EndFrame() final;

      void BeginCache() final;
      void EndCache() final;

      void BeginCmds() final;
      void EndCmds() final;

      void CmdSetCamera(const BasicCameraInfo& cameraInfo) final;

      void CmdBindMaterial(const BasicNativeMaterialHandle material, const BasicMaterialVariables& materialVariables,
                           const ReadOnlySpan<BasicNativeTextureHandle> textures) final;
      void CmdBindIndexBuffer(const BasicNativeBufferHandle indexBuffer) final;
      void CmdBindVertexBuffer(const BasicNativeBufferHandle vertexBuffer) final;

      void CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex) final;
      void CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex) final;

    private:
      // void DisableAttribArrays();
    };
  }
}

#endif
