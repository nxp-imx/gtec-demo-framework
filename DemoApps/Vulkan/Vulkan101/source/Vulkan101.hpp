#ifndef VULKAN_VULKAN101_VULKAN101_HPP
#define VULKAN_VULKAN101_VULKAN101_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/ShaderModule.hpp>

namespace Fsl
{
  class Vulkan101 : public VulkanBasic::DemoAppVulkanBasic
  {
    struct Resources
    {
      Vulkan::VUBufferMemory m_vertexBuffer;
      RapidVulkan::ShaderModule m_vertexShaderModule;
      RapidVulkan::ShaderModule m_fragmentShaderModule;
      RapidVulkan::PipelineLayout m_pipelineLayout;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass m_renderPass;
      RapidVulkan::GraphicsPipeline m_pipeline;

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        m_pipeline.Reset();
        m_renderPass.Reset();
      }
    };

    Resources m_resources;
    DependentResources m_dependentResources;

  public:
    explicit Vulkan101(const DemoAppConfig& config);
    ~Vulkan101() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() noexcept override;

  private:
    void CreateShaders();

    void BuildCmdBuffer(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t currentSwapBufferIndex, const VkExtent2D& swapchainImageExtent,
                        const VkFramebuffer framebuffer);
  };
}

#endif
