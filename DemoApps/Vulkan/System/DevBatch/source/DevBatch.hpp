#ifndef VULKAN_SYSTEM_DEVBATCH_DEVBATCH_HPP
#define VULKAN_SYSTEM_DEVBATCH_DEVBATCH_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslUtil/Vulkan1_0/Batch/Batch2D.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUImageMemoryView.hpp>
#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/Instance.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <vector>

namespace Fsl
{
  class DevBatch final : public VulkanBasic::DemoAppVulkanBasic
  {
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    struct AppTextures
    {
      Vulkan::VUTexture Texture1;
      Vulkan::VUTexture Texture2;
      Vulkan::VUTexture Texture3;
      Vulkan::VUTexture Texture4;
      Vulkan::VUTexture Texture4Pre;
      Vulkan::VUTexture NormalFontTexture;
      Vulkan::VUTexture SdfFontTexture;
    };

    struct Resources
    {
      AppTextures Textures;
      Vulkan::VMVertexBuffer VertexBuffer;
      TextureAtlasSpriteFont NormalFontOld;
      TextureAtlasSpriteFont NormalFontNew;
      TextureAtlasSpriteFont SdfFontOld;
      TextureAtlasSpriteFont SdfFontNew;
      RapidVulkan::ShaderModule VertexShaderModule;
      RapidVulkan::ShaderModule FragmentShaderModule;
      RapidVulkan::PipelineLayout MainPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    std::shared_ptr<Vulkan::QuadBatch> m_test;
    std::shared_ptr<Vulkan::QuadBatch> m_batch2DQuad;
    std::shared_ptr<Vulkan::Batch2D> m_batch2D;

    float m_currentAngle = 0.0f;
    float m_currentSpeed = 0.5f;
    Vector2 m_currentPos;
    float m_currentPos2 = 0.0f;
    float m_currentPos3 = 0.0f;
    Vector2 m_currentPos4;
    bool m_newFontFormat{false};
    bool m_kerningEnabled{false};

  public:
    explicit DevBatch(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) final;

    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

  private:
    static AppTextures CreateTextures(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                      const std::shared_ptr<IContentManager>& contentManger);

    void CreateShaders();

    void DrawQuads(const uint32_t swapBufferIndex, const VkCommandBuffer commandBuffer);
  };
}

#endif
