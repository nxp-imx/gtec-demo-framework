#ifndef VULKAN_HDR02_FBBASICTONEMAPPING_HDR02_FBBASICTONEMAPPING_HPP
#define VULKAN_HDR02_FBBASICTONEMAPPING_HDR02_FBBASICTONEMAPPING_HPP
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

#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/FloatSliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <Shared/HDR/BasicScene/MenuUI.hpp>
#include <Shared/HDR/BasicScene/API/Vulkan/SimpleMesh.hpp>
#include <utility>
#include <vector>

namespace Fsl
{
  class HDR02_FBBasicToneMapping
    : public VulkanBasic::DemoAppVulkanBasic
    , public UI::EventListener
  {
    //! Resources that are duplicated per command buffer to ensure that it wont be 'in-use' while we update it
    struct FrameResources
    {
      Vulkan::VUBufferMemory VertUboBuffer;
      Vulkan::VUBufferMemory FragUboBuffer;
      VkDescriptorSet DescriptorSetRender;
      VkDescriptorSet DescriptorSetTonemap;
    };

    struct VertexUBOData
    {
      Matrix MatModel;
      Matrix MatView;
      Matrix MatProj;
    };

    // Beware we are using Vector4's for the array even though the shader requested vec3.
    // This is due to the Vulkan alignment requirements.
    // https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources
    struct FragmentUBOData
    {
      Vector4 LightPositions[4];
      Vector4 LightColors[4];
      float Exposure = 0.0f;
    };

    struct Resources
    {
      Vulkan::VUTexture TexSRGB;

      RapidVulkan::ShaderModule VertShaderModule;
      RapidVulkan::ShaderModule FragShaderModule;
      RapidVulkan::ShaderModule TonemapperVertShaderModule;
      RapidVulkan::ShaderModule TonemapperLDRFragShaderModule;
      RapidVulkan::ShaderModule TonemapperHDRFragShaderModule;

      SimpleMesh MeshTunnel;
      SimpleMesh MeshQuad;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      //! We only need one RenderAttachment as command buffers on the same queue are executed in order
      Vulkan::VUTexture RenderAttachment;

      RapidVulkan::GraphicsPipeline PipelineRender;
      RapidVulkan::GraphicsPipeline PipelineTonemapperHDR;
      RapidVulkan::GraphicsPipeline PipelineTonemapperLDR;
    };

    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;
    MenuUI m_menuUI;

    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled;
    Graphics3D::FirstPersonCamera m_camera;

    Resources m_resources;
    DependentResources m_dependentResources;

    VertexUBOData m_vertexUboData;
    FragmentUBOData m_fragmentUboData;

  public:
    HDR02_FBBasicToneMapping(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

    RapidVulkan::Framebuffer CreateFramebuffer(const VulkanBasic::FrameBufferCreateContext& frameBufferCreateContext) override;

  private:
    void DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer);

    void UpdateInput(const DemoTime& demoTime);
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);

    void DrawScene(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawTonemappedScene(const FrameResources& frame, const VkCommandBuffer commandBuffer);

    static FragmentUBOData PrepareLights();
  };
}

#endif
