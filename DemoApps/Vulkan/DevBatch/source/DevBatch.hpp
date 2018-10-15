#ifndef VULKAN_DEVBATCH_DEVBATCH_HPP
#define VULKAN_DEVBATCH_DEVBATCH_HPP
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

#include <Shared/VulkanWindowExperimental/VulkanWindowDemoApp.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/Batch/Batch2D.hpp>
#include <FslUtil/Vulkan1_0/VUImage.hpp>
#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/Instance.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <vector>

namespace Fsl
{
  class DevBatch : public VulkanWindowDemoApp
  {
    struct FrameDrawRecord
    {
      RapidVulkan::Semaphore ImageAcquiredSemaphore;
      RapidVulkan::Semaphore RenderingCompleteSemaphore;
      RapidVulkan::Fence InFlightFence;
    };

    // m_surface
    RapidVulkan::CommandPool m_commandPool;
    RapidVulkan::Buffer m_vertexBuffer;
    RapidVulkan::Memory m_deviceMemoryVertexBuffer;
    RapidVulkan::ShaderModule m_vertexShaderModule;
    RapidVulkan::ShaderModule m_fragmentShaderModule;
    RapidVulkan::PipelineLayout m_pipelineLayout;

    Vulkan::VUSwapchainKHR m_swapchain;
    std::vector<RapidVulkan::ImageView> m_swapchainImageView;
    std::vector<RapidVulkan::Framebuffer> m_framebuffer;
    RapidVulkan::CommandBuffers m_cmdBuffers;
    RapidVulkan::RenderPass m_renderPass;
    RapidVulkan::GraphicsPipeline m_pipeline;

    std::vector<FrameDrawRecord> m_frames;
    std::size_t m_currentFrame = 0;

    std::shared_ptr<Vulkan::QuadBatch> m_test;
    std::shared_ptr<Vulkan::QuadBatch> m_batch2DQuad;
    std::shared_ptr<Vulkan::Batch2D> m_batch2D;

    Vulkan::VUTexture m_texture;
    Vulkan::VUTexture m_texture2;
    Vulkan::VUTexture m_texture3;
    Vulkan::VUTexture m_texture4;
    Vulkan::VUTexture m_texture4Pre;

    float m_currentAngle = 0.0f;
    float m_currentSpeed = 0.5f;
    Vector2 m_currentPos;
    float m_currentPos2 = 0.0f;
    float m_currentPos3 = 0.0f;
    Vector2 m_currentPos4;

  public:
    DevBatch(const DemoAppConfig& config);
    ~DevBatch() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void CreateFrameSyncObjects();
    void BuildVertexBuffer();
    void BuildShader();
    void BuildPipelineLayout();
    void BuildResources();

    void BuildRenderPass();
    void BuildPipeline();

    void BuildSwapchainImageView(const uint32_t bufferIndex);
    void BuildFramebuffer(const uint32_t bufferIndex);
    void BuildCmdBuffer(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t bufferIndex);

    void DrawQuads(const uint32_t bufferIndex, const VkCommandBuffer commandBuffer);
  };
}

#endif
