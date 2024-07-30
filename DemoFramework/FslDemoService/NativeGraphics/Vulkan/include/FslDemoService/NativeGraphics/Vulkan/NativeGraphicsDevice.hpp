#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSDEVICE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSDEVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsBufferFactory.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsMaterialFactory.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsTextureFactory.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeDevice.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/VUTextureInfo.hpp>
#include <vulkan/vulkan.h>
#include <memory>

namespace Fsl::Vulkan
{
  class VMBufferManager;
  class VulkanImageCreator;

  class NativeGraphicsDevice final : public Graphics3D::INativeDevice
  {
    struct ExtendedCameraInfo
    {
      BasicCameraInfo Info;
      Matrix ModelViewProjection;
      bool IsValid{false};
      constexpr ExtendedCameraInfo() noexcept = default;
      explicit constexpr ExtendedCameraInfo(const BasicCameraInfo& info) noexcept
        : Info(info.Model, info.View, info.Projection * Vulkan::MatrixUtil::GetClipMatrix())
        , ModelViewProjection(Info.Model * Info.View * Info.Projection)
        , IsValid(true)
      {
      }
    };

    struct CommandSection
    {
      bool IsActive{false};
      BasicNativeBufferHandle BoundVertexBufferHandle;
      BasicNativeBufferHandle BoundIndexBufferHandle;
      BasicNativeMaterialHandle BoundMaterialHandle;
      ExtendedCameraInfo CameraInfo;
      VkPipelineLayout BoundPipelineLayout{VK_NULL_HANDLE};
      VkDescriptorSet BoundDescriptorSet{VK_NULL_HANDLE};
      uint32_t CameraChangeId{0};

      CommandSection() = default;
      explicit CommandSection(const bool isActive)
        : IsActive(isActive)
      {
      }
    };

    struct Frame
    {
      VkCommandBuffer CommandBuffer{VK_NULL_HANDLE};
      CommandSection Commands;

      Frame() = default;
      explicit Frame(const VkCommandBuffer commandBuffer)
        : CommandBuffer(commandBuffer)
      {
      }

      bool IsValid() const
      {
        return CommandBuffer != VK_NULL_HANDLE;
      }
    };

    NativeGraphicsBufferFactory m_bufferFactory;
    NativeGraphicsTextureFactory m_textureFactory;
    NativeGraphicsMaterialFactory m_materialFactory;
    bool m_isDisposed{false};
    Frame m_frame;

  public:
    NativeGraphicsDevice(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex,
                         const std::shared_ptr<VMBufferManager>& bufferManager, const std::shared_ptr<VulkanImageCreator>& imageCreator,
                         const uint32_t maxFramesInFlight);
    ~NativeGraphicsDevice() noexcept final;

    void Dispose() noexcept;


    VUTextureInfo TryGetTextureInfo(const BasicNativeTextureHandle hTexture) const noexcept;

    // Graphics3D::INativeBufferFactory
    Graphics3D::NativeBufferFactoryCaps GetBufferCaps() const noexcept final;

    BasicNativeBufferHandle CreateBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity,
                                         const bool isDynamic) final;
    bool DestroyBuffer(const BasicNativeBufferHandle hBuffer) noexcept final;
    void SetBufferData(const BasicNativeBufferHandle hBuffer, const uint32_t dstIndex, ReadOnlyFlexSpan bufferData) final;


    // Graphics3D::INativeShaderFactory

    ReadOnlySpan<BasicNativeShaderCreateInfo> GetPredefinedShaders() const final;
    BasicNativeShaderHandle CreateShader(const BasicNativeShaderCreateInfo& createInfo) final;
    bool DestroyShader(const BasicNativeShaderHandle hShader) noexcept final;

    // Graphics3D::INativeMaterialFactory

    void CreateMaterials(Span<BasicNativeMaterialHandle> dstMaterialHandles, ReadOnlySpan<BasicNativeMaterialCreateInfo> createInfoSpan) final;
    bool DestroyMaterial(const BasicNativeMaterialHandle hMaterial) noexcept final;

    // Graphics3D::INativeTextureFactory
    Graphics3D::NativeTextureFactoryCaps GetTextureCaps() const noexcept final;

    BasicNativeTextureHandle CreateTexture(const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags,
                                           const bool isDynamic) final;

    bool DestroyTexture(const BasicNativeTextureHandle hTexture) noexcept final;

    void SetTextureData(const BasicNativeTextureHandle hTexture, const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint,
                        const TextureFlags textureFlags) final;
    const IBasicNativeTexture* TryGetTexture(const BasicNativeTextureHandle hTexture) const noexcept final;

    // Graphics3D::INativeDevice
    void CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo) final;
    void DestroyDependentResources() final;

    void BeginFrame(const BasicNativeBeginFrameInfo& frameInfo) final;
    void EndFrame() noexcept final;

    void BeginCache() final {};
    void EndCache() noexcept final {};

    void BeginCmds() final;
    void EndCmds() noexcept final;

    void CmdSetCamera(const BasicCameraInfo& cameraInfo) final;
    void CmdBindIndexBuffer(const BasicNativeBufferHandle indexBuffer) final;
    void CmdBindMaterial(const BasicNativeMaterialHandle material, const BasicMaterialVariables& materialVariables,
                         const ReadOnlySpan<BasicNativeTextureHandle> textures) final;
    void CmdBindVertexBuffer(const BasicNativeBufferHandle vertexBuffer) final;

    void CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex) noexcept final;
    void CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex) noexcept final;
  };
}

#endif
