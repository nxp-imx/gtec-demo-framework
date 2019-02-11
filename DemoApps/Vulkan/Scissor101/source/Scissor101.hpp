#ifndef VULKAN_SCISSOR101_SCISSOR101_HPP
#define VULKAN_SCISSOR101_SCISSOR101_HPP
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
#include <Shared/Scissor101/OptionParser.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <array>

namespace Fsl
{
  class Scissor101 : public VulkanBasic::DemoAppVulkanBasic
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
      VkDescriptorSet DescriptorSet;
    };

    struct VertexBufferInfo
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      VkVertexInputBindingDescription BindingDescription{};
      std::array<VkVertexInputAttributeDescription, 2> AttributeDescription{};
    };

    struct Resources
    {
      ProgramInfo MainProgramInfo;
      Vulkan::VUTexture Texture;
      VertexBufferInfo MainVertexBufferInfo;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass TheRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;
    };


    struct VertexUBOData
    {
      Matrix MatModelView;
      Matrix MatProj;
    };


    std::shared_ptr<OptionParser> m_options;
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    Resources m_resources;
    DependentResources m_dependentResources;

    Matrix m_matModel;
    Matrix m_matProj;
    Matrix m_matTranslate;
    Vector3 m_angle;
    Vector2 m_speed1A;
    Vector2 m_speed1B;
    Vector2 m_speed2A;
    Vector2 m_speed2B;
    Vector2 m_angle1A;
    Vector2 m_angle1B;
    Vector2 m_angle2A;
    Vector2 m_angle2B;
    Point2 m_clip1;
    Point2 m_clip2;
    bool m_clipX;
    bool m_clipY;

  public:
    Scissor101(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    void DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawCube(const FrameResources& frame, const VkCommandBuffer commandBuffer, const ProgramInfo& programInfo, const Matrix& matModel);


    // void DrawCube(const ProgramInfo& programInfo, const Matrix& matModel);
    static ProgramInfo CreateProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager);
    static VertexBufferInfo CreateVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager);

    static RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                        const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                        const VertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass);
  };
}

#endif
