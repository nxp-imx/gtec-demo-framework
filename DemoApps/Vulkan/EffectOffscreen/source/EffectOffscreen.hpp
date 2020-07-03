#ifndef VULKAN_EFFECTOFFSCREEN_EFFECTOFFSCREEN_HPP
#define VULKAN_EFFECTOFFSCREEN_EFFECTOFFSCREEN_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <array>

namespace Fsl
{
  class EffectOffscreen : public VulkanBasic::DemoAppVulkanBasic
  {
    struct ProgramInfo
    {
      RapidVulkan::ShaderModule VertexShaderModule;
      RapidVulkan::ShaderModule FragmentShaderModule;
    };

    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory UboBuffer;
      VkDescriptorSet DescriptorSetScene;
      Vulkan::VUBufferMemory EffectUboBuffer;
      VkDescriptorSet DescriptorSetEffect;
    };

    struct VertexBufferInfo
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      VkVertexInputBindingDescription BindingDescription{};
      std::array<VkVertexInputAttributeDescription, 2> AttributeDescription{};
    };

    struct EffectVertexBufferInfo
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      VkVertexInputBindingDescription BindingDescription{};
      std::array<VkVertexInputAttributeDescription, 3> AttributeDescription{};
    };

    struct Resources
    {
      ProgramInfo MainProgramInfo;
      RapidVulkan::ShaderModule EffectVertexShader;
      RapidVulkan::ShaderModule EffectFragmentShaderTop;
      RapidVulkan::ShaderModule EffectFragmentShaderBottom;

      Vulkan::VUTexture Texture;
      Vulkan::VUTexture EffectTexture;
      VertexBufferInfo CubeVertexBufferInfo;
      EffectVertexBufferInfo DoubleQuadVertexBufferInfo;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorSetLayout EffectDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;
      RapidVulkan::PipelineLayout EffectBottomPipelineLayout;
    };

    struct OffscreenResources
    {
      VkExtent2D Extent{};
      RapidVulkan::RenderPass RP;
      RapidVulkan::Framebuffer FB;
      Vulkan::VUTexture Color;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline ScenePipeline;
      RapidVulkan::GraphicsPipeline PipelineEffectBottom;
      OffscreenResources Offscreen;
    };


    struct VertexUBOData
    {
      Matrix MatModelView;
      Matrix MatProj;
    };

    struct EffectFragUBOData
    {
      float Time{0};
    };

    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    Resources m_resources;
    DependentResources m_dependentResources;

    Matrix m_matModel;
    Matrix m_matProj;
    Matrix m_matTranslate;
    Vector3 m_angle;

    EffectFragUBOData m_effectUboData;

  public:
    explicit EffectOffscreen(const DemoAppConfig& config);

  protected:
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    RapidVulkan::Framebuffer CreateOffscreenFramebuffer(const VkImageView colorImageView, const VkImageView depthImageView,
                                                        const VkRenderPass renderPass, const VkExtent2D& extent);
    void DrawOffscreenRenderpass(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext);
    void DrawSceneToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer, const VkExtent2D& extent);
    void DrawCube(const FrameResources& frame, const VkCommandBuffer commandBuffer, const ProgramInfo& programInfo, const Matrix& matModel);


    // void DrawCube(const ProgramInfo& programInfo, const Matrix& matModel);
    static VertexBufferInfo CreateCubeVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager);
    static EffectVertexBufferInfo CreateDoubleQuadVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager);

    static RapidVulkan::GraphicsPipeline CreateScenePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                             const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                             const VertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass,
                                                             const uint32_t subpass);
    static RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                        const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                        const EffectVertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass,
                                                        const uint32_t subpass);
  };
}

#endif
