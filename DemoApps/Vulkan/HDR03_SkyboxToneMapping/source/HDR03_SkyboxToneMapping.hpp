#ifndef VULKAN_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
#define VULKAN_HDR03_SKYBOXTONEMAPPING_HDR03_SKYBOXTONEMAPPING_HPP
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
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/HDR/SkyboxTonemapping/MenuUI.hpp>
#include <Shared/HDR/SkyboxTonemapping/OptionParser.hpp>
#include <Shared/CubeMapping/API/Vulkan/SkyboxMesh.hpp>
#include <Shared/HDR/SkyboxTonemapping/RenderRecord.hpp>
#include <memory>
#include <vector>
#include <utility>

namespace Fsl
{
  class HDR03_SkyboxToneMapping : public VulkanBasic::DemoAppVulkanBasic
  {
  public:
    struct SimpleMesh
    {
      Vulkan::VMVertexBuffer VertexBuffer;
      std::array<VkVertexInputAttributeDescription, 2> VertexAttributeDescription;
      VkVertexInputBindingDescription VertexInputBindingDescription{};
    };

  private:
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory SceneVertUboBuffer;
      Vulkan::VUBufferMemory TonemapVertUboBuffer;
      VkDescriptorSet SceneDescriptorSet;
      VkDescriptorSet TonemapDescriptorSet;
    };

    struct VertexUBOData
    {
      // Matrix MatModel;
      Matrix MatView;
      Matrix MatProj;
    };

    struct TonemapperUBOData
    {
      float Exposure = 0.0f;
    };


    struct Scene
    {
      Vulkan::VUTexture CubemapTexture;
      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
      SkyboxMesh Mesh;
      RapidVulkan::DescriptorSetLayout SceneDescriptorSetLayout;
      RapidVulkan::PipelineLayout ScenePipelineLayout;
    };

    struct Resources
    {
      RapidVulkan::ShaderModule TonemapperVertShaderModule;
      std::vector<RapidVulkan::ShaderModule> ProgramTonemap;

      Scene MainScene;

      SimpleMesh MeshQuad;
      RapidVulkan::DescriptorSetLayout TonemapDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;

      std::vector<FrameResources> MainFrameResources;

      RapidVulkan::PipelineLayout TonemapPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      //! We only need one RenderAttachment as command buffers on the same queue are executed in order
      Vulkan::VUTexture RenderAttachment;

      RapidVulkan::GraphicsPipeline ScenePipeline;
      std::array<RapidVulkan::GraphicsPipeline, Tonemapper::COUNT> PipelineTonemapper;
    };

    MenuUI m_menuUI;
    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;
    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    bool m_rightMouseDown = false;
    Graphics3D::FirstPersonCamera m_camera;

    Resources m_resources;
    DependentResources m_dependentResources;

    VertexUBOData m_vertexUboData;
    TonemapperUBOData m_tonemapUboData;

    Vector2 m_rotationSpeed;

  public:
    HDR03_SkyboxToneMapping(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void Resized(const Point2& size) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

    RapidVulkan::Framebuffer CreateFramebuffer(const VulkanBasic::FrameBufferCreateContext& frameBufferCreateContext) override;

  private:
    void DrawIt(const DemoTime& demoTime, const FrameResources& frame, const VkCommandBuffer commandBuffer);

    void UpdateInput(const DemoTime& demoTime);
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    void DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer, const Scene& scene);
    void DrawTonemappedScene(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void PrepareScene(const std::shared_ptr<IContentManager>& contentManager, Scene& rScene);

    std::vector<RapidVulkan::ShaderModule> CreateTonemappers(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager);
    SimpleMesh CreateQuadVertexArray(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                     const std::array<VertexElementUsage, 2>& quadShaderBindOrder);
  };
}

#endif
