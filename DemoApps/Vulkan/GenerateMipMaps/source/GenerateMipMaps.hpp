#ifndef VULKAN_GENERATEMIPMAPS_GENERATEMIPMAPS_HPP
#define VULKAN_GENERATEMIPMAPS_GENERATEMIPMAPS_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/DescriptorSetLayout.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <array>
#include <utility>
#include <vector>

namespace Fsl
{
  class GenerateMipMaps final
    : public VulkanBasic::DemoAppVulkanBasic
    , public UI::EventListener
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
      uint32_t mipMapLod{};
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
      std::vector<RapidVulkan::Sampler> Samplers;
      VertexBufferInfo MainVertexBufferInfo;
      RapidVulkan::DescriptorSetLayout MainDescriptorSetLayout;
      RapidVulkan::DescriptorPool MainDescriptorPool;
      std::vector<FrameResources> MainFrameResources;
      RapidVulkan::PipelineLayout MainPipelineLayout;
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      RapidVulkan::GraphicsPipeline Pipeline;
    };


    struct VertexUBOData
    {
      Matrix MatModelView;
      Matrix MatProj;
    };

    struct UIRecord
    {
      std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> Slider;
      std::shared_ptr<UI::BackgroundLabelButton> BtnDefault;

      UIRecord() = default;
      UIRecord(std::shared_ptr<UI::SliderAndFmtValueLabel<uint32_t>> slider, std::shared_ptr<UI::BackgroundLabelButton> btnDefault)
        : Slider(std::move(slider))
        , BtnDefault(std::move(btnDefault))
      {
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    UIRecord m_ui;

    std::shared_ptr<Vulkan::VMBufferManager> m_bufferManager;

    Resources m_resources;
    DependentResources m_dependentResources;

    Matrix m_matModel;
    Matrix m_matProj;
    Matrix m_matTranslate;
    Vector3 m_angle;
    uint32_t m_defaultMipMapLevel{};

  public:
    explicit GenerateMipMaps(const DemoAppConfig& config);

  protected:
    void OnKeyEvent(const KeyEvent& event) final;
    // void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) final;
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) final;

    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() override;

  private:
    void SetDefaultSettings();
    UIRecord CreateUI(const uint32_t currentMipMapLevel, const uint32_t numMipMapLevels);

    void DrawToCommandBuffer(const FrameResources& frame, const VkCommandBuffer commandBuffer);
    void DrawCube(const FrameResources& frame, const VkCommandBuffer commandBuffer, const ProgramInfo& programInfo, const Matrix& matModel);
    static ProgramInfo CreateProgram(const Vulkan::VUDevice& device, const std::shared_ptr<IContentManager>& contentManager);
    static VertexBufferInfo CreateVertexBuffer(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager);

    static RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                        const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                        const VertexBufferInfo& vertexBufferInfo, const VkRenderPass renderPass);
  };
}

#endif
