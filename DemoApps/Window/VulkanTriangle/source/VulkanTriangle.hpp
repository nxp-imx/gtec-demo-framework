#ifndef WINDOW_VULKANTRIANGLE_VULKANTRIANGLE_HPP
#define WINDOW_VULKANTRIANGLE_VULKANTRIANGLE_HPP
/**
* The MIT License (MIT)
*
* Copyright (c) since 2014 Norbert Nopper
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/


// Based on a sample by Norbert Nopper from VKTS Examples (VKTS_Sample02)
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <VulkanWindowExperimental/VulkanWindowDemoApp.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/CommandBuffer.hpp>
#include <FslGraphicsVulkan1_0/CommandPool.hpp>
#include <FslGraphicsVulkan1_0/Device.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/Framebuffer.hpp>
#include <FslGraphicsVulkan1_0/Instance.hpp>
#include <FslGraphicsVulkan1_0/ImageView.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
#include <FslGraphicsVulkan1_0/RenderPass.hpp>
#include <FslGraphicsVulkan1_0/Semaphore.hpp>
#include <FslGraphicsVulkan1_0/ShaderModule.hpp>
#include <FslGraphicsVulkan1_0/Extend/SwapchainKHREx.hpp>
#include <VulkanExperimental/DeviceQueue.hpp>
#include <vector>

namespace Fsl
{
  class VulkanTriangle : public VulkanWindowDemoApp
  {
    // m_surface
    Vulkan::CommandPool m_commandPool;
    Vulkan::Buffer m_vertexBuffer;
    Vulkan::Memory m_deviceMemoryVertexBuffer;
    Vulkan::ShaderModule m_vertexShaderModule;
    Vulkan::ShaderModule m_fragmentShaderModule;
    Vulkan::PipelineLayout m_pipelineLayout;

    Vulkan::SwapchainKHREx m_swapchain;
    std::vector<Vulkan::ImageView> m_swapchainImageView;
    std::vector<Vulkan::Framebuffer> m_framebuffer;
    std::vector<Vulkan::CommandBuffer> m_cmdBuffer;
    Vulkan::RenderPass m_renderPass;
    Vulkan::GraphicsPipeline m_pipeline;

    Vulkan::Semaphore m_imageAcquiredSemaphore;
    Vulkan::Semaphore m_renderingCompleteSemaphore;

  public:
    VulkanTriangle(const DemoAppConfig& config);
    ~VulkanTriangle();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void BuildVertexBuffer();
    void BuildShader();
    void BuildPipelineLayout();
    void BuildResources();

    void BuildRenderPass();
    void BuildPipeline();

    void BuildSwapchainImageView(const uint32_t usedBuffer);
    void BuildFramebuffer(const uint32_t usedBuffer);
    void BuildCmdBuffer(const uint32_t usedBuffer);
  };
}

#endif
