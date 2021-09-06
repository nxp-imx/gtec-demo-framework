#ifndef VULKAN_BLOOM_BLOOM_HPP
#define VULKAN_BLOOM_BLOOM_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeBatch2D.hpp>
#include <FslGraphics3D/Camera/ArcballCamera.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <FslUtil/Vulkan1_0/VUFramebuffer.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <Shared/Bloom/GaussianShaderBuilder.hpp>
#include <Shared/Bloom/MenuUI.hpp>
#include <deque>
#include <vector>
#include "VBHelper.hpp"

namespace Fsl
{
  class IVulkanScene;

  class Bloom : public VulkanBasic::DemoAppVulkanBasic
  {
    enum class BlurShaderType
    {
      Gaussian5X5,
      Gaussian9X9,
      Custom
    };

    struct Resources
    {
      std::shared_ptr<Vulkan::VMBufferManager> BufferManager;
      RapidVulkan::ShaderModule ShaderPassVert;
      RapidVulkan::ShaderModule ShaderBrightPassFrag;
      RapidVulkan::ShaderModule ShaderCopyFrag;
      RapidVulkan::ShaderModule ShaderBloomPassFrag;
      RapidVulkan::ShaderModule ShaderBlurHGaussian5X5FixedFrag;
      RapidVulkan::ShaderModule ShaderBlurVGaussian5X5FixedFrag;
      RapidVulkan::ShaderModule ShaderBlurHGaussian9X9FixedFrag;
      RapidVulkan::ShaderModule ShaderBlurVGaussian9X9FixedFrag;
      RapidVulkan::ShaderModule ShaderBlurHCustomFrag;
      RapidVulkan::ShaderModule ShaderBlurVCustomFrag;
      RapidVulkan::ShaderModule ShaderBlurHGaussian5X5Frag;
      RapidVulkan::ShaderModule ShaderBlurVGaussian5X5Frag;
      RapidVulkan::ShaderModule ShaderBlurHGaussian9X9Frag;
      RapidVulkan::ShaderModule ShaderBlurVGaussian9X9Frag;
      QuadMesh VBFullScreen;

      RapidVulkan::DescriptorPool MainDescriptorPool;

      RapidVulkan::DescriptorSetLayout OffscreenDescriptorSetLayout;
      VkDescriptorSet OffscreenDescriptorSetFB256{};
      VkDescriptorSet OffscreenDescriptorSetFB256A{};
      VkDescriptorSet OffscreenDescriptorSetFB256B{};
      VkDescriptorSet OffscreenDescriptorSetFB128A{};
      VkDescriptorSet OffscreenDescriptorSetFB128B{};
      VkDescriptorSet OffscreenDescriptorSetFB64A{};
      VkDescriptorSet OffscreenDescriptorSetFB64B{};
      VkDescriptorSet OffscreenDescriptorSetFB32A{};
      VkDescriptorSet OffscreenDescriptorSetFB32B{};
      VkDescriptorSet OffscreenDescriptorSetFB16A{};
      VkDescriptorSet OffscreenDescriptorSetFB16B{};
      RapidVulkan::PipelineLayout OffscreenPipelineLayout;

      RapidVulkan::PipelineLayout BlurPipelineLayout;
      RapidVulkan::PipelineLayout BlurPipelineLayout5;
      RapidVulkan::PipelineLayout BlurPipelineLayout9;

      RapidVulkan::DescriptorSetLayout BloomDescriptorSetLayout;
      VkDescriptorSet BloomDescriptorSet{};
      RapidVulkan::PipelineLayout BloomPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::RenderPass OffscreenRP;
      RapidVulkan::RenderPass OffscreenRPNoDepth;

      Vulkan::VUFramebuffer OffscreenFB256;
      Vulkan::VUFramebuffer OffscreenFB256A;
      Vulkan::VUFramebuffer OffscreenFB256B;
      Vulkan::VUFramebuffer OffscreenFB128A;
      Vulkan::VUFramebuffer OffscreenFB128B;
      Vulkan::VUFramebuffer OffscreenFB64A;
      Vulkan::VUFramebuffer OffscreenFB64B;
      Vulkan::VUFramebuffer OffscreenFB32A;
      Vulkan::VUFramebuffer OffscreenFB32B;
      Vulkan::VUFramebuffer OffscreenFB16A;
      Vulkan::VUFramebuffer OffscreenFB16B;

      RapidVulkan::GraphicsPipeline PipelineBrightPass;
      RapidVulkan::GraphicsPipeline PipelineCopy;
      RapidVulkan::GraphicsPipeline PipelineBlurCustomH;
      RapidVulkan::GraphicsPipeline PipelineBlurCustomV;
      RapidVulkan::GraphicsPipeline PipelineBlurFixed5H;
      RapidVulkan::GraphicsPipeline PipelineBlurFixed5V;
      RapidVulkan::GraphicsPipeline PipelineBlurFixed9H;
      RapidVulkan::GraphicsPipeline PipelineBlurFixed9V;
      RapidVulkan::GraphicsPipeline PipelineBlur5H;
      RapidVulkan::GraphicsPipeline PipelineBlur5V;
      RapidVulkan::GraphicsPipeline PipelineBlur9H;
      RapidVulkan::GraphicsPipeline PipelineBlur9V;

      RapidVulkan::GraphicsPipeline PipelineBloom;
    };

    Resources m_resources;
    DependentResources m_dependentResources;

    MenuUI m_menuUI;

    std::shared_ptr<Vulkan::NativeBatch2D> m_batch;
    Graphics3D::ArcballCamera m_camera;
    Vector3 m_rotation;
    Vector3 m_rotationSpeed;
    Vector3 m_storedStartRotation;
    std::shared_ptr<IVulkanScene> m_sceneRender;
    std::shared_ptr<IVulkanScene> m_sceneWhiteRect;
    std::shared_ptr<IVulkanScene> m_scene;

    bool m_renderUI;
    float m_gaussianBlurKernelWeight;
    BlurShaderType m_activeBlueShaderType;
    Gaussian5 m_gaussian5;
    Gaussian9 m_gaussian9;

  public:
    explicit Bloom(const DemoAppConfig& config);
    ~Bloom() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    void DrawOffscreenRenderpasses(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext);
    void DrawFinalComposite(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer);
    VkPipelineLayout PrepareBlurPipeline(const BlurShaderType shaderType, const VkCommandBuffer hCmdBuffer, const uint32_t texelSize) const;
    const RapidVulkan::GraphicsPipeline& GetBlurHPipeline(const BlurShaderType shaderType) const;
    const RapidVulkan::GraphicsPipeline& GetBlurVPipeline(const BlurShaderType shaderType) const;
    void PostProcessBlurH(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                          const Vulkan::VUFramebuffer& dst, const VkDescriptorSet& srcDescriptorSet);
    void PostProcessBlurV(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                          const Vulkan::VUFramebuffer& dst, const VkDescriptorSet& srcDescriptorSet);
    void PostProcess(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                     const Vulkan::VUFramebuffer& dst, const VkPipelineLayout hPipelineLayout, const VkDescriptorSet& srcDescriptorSet,
                     const RapidVulkan::GraphicsPipeline& pipeline);
    void PostProcess(const VkCommandBuffer hCmdBuffer, const VkPipelineLayout hPipelineLayout, const VkDescriptorSet& srcDescriptorSet,
                     const RapidVulkan::GraphicsPipeline& pipeline);

    void SetBlurShader(const BlurShaderType shaderType);
  };
}

#endif
