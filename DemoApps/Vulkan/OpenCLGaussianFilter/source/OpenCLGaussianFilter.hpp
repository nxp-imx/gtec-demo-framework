#ifndef VULKAN_OPENCLGAUSSIANFILTER_OPENCLGAUSSIANFILTER_HPP
#define VULKAN_OPENCLGAUSSIANFILTER_OPENCLGAUSSIANFILTER_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>

namespace Fsl
{
  class OpenCLGaussianFilter : public VulkanBasic::DemoAppVulkanBasic
  {
  public:
    struct SimpleMesh
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      std::array<VkVertexInputAttributeDescription, 2> VertexAttributeDescription{};
      VkVertexInputBindingDescription VertexInputBindingDescription{};
    };

  private:
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory VertUboBuffer;
      VkDescriptorSet DescriptorSet;
    };

    struct VertexUBOData
    {
      Matrix MatModel;
      Matrix MatProj;
    };

    struct Resources
    {
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;

      Vulkan::VUTexture Texture;
      SimpleMesh Mesh;

      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        Pipeline.Reset();
        MainRenderPass.Reset();
      }
    };

    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    Resources m_resources;
    DependentResources m_dependentResources;

    VertexUBOData m_vertexUboData;

    Vector3 m_angle;
    Matrix m_matTranslate;
    // std::array<GLES3::GLVertexAttribLink, 2> m_attribLink;

  public:
    explicit OpenCLGaussianFilter(const DemoAppConfig& config);

  protected:
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

    void DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void PrepareMatrices(const Point2& currentSize);
  };
}

#endif
