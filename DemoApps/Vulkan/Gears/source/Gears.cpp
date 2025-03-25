/****************************************************************************************************************************************************
 * Copyright 2025 NXP
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

#include "Gears.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace
  {
    // TODO:

    // - Other apps using arc camera: check if they handle: ConfigurationChanged

    namespace LocalConfig
    {
      constexpr uint32_t VertexBufferBindId = 0;
      constexpr float DefaultZoom = 18;
      constexpr float DefaultYRotation = MathHelper::TO_RADS * 20.0f;
    }

    glm::mat4 ToGlmMat4(const Matrix matrix)
    {
      const float* const pMatrix = matrix.DirectAccess();
      // While XNA style matrices are row major and glm is column major when we use the GLM constructor it will handle the conversion
      // So we do not need to manually transpose it.
      return {pMatrix[0], pMatrix[1], pMatrix[2],  pMatrix[3],  pMatrix[4],  pMatrix[5],  pMatrix[6],  pMatrix[7],
              pMatrix[8], pMatrix[9], pMatrix[10], pMatrix[11], pMatrix[12], pMatrix[13], pMatrix[14], pMatrix[15]};
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      std::array<VkDescriptorPoolSize, 1> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count * 3;    // *3 = One for each gear
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const GearsMeshInfo& mesh, const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!mesh.VertexAttributeDescription.empty());

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &mesh.VertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mesh.VertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = mesh.VertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
      pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      pipelineViewportStateCreateInfo.viewportCount = 1;
      pipelineViewportStateCreateInfo.pViewports = &viewport;
      pipelineViewportStateCreateInfo.scissorCount = 1;
      pipelineViewportStateCreateInfo.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
      pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      pipelineRasterizationStateCreateInfo.flags = 0;
      pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
      pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
      pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
      pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      pipelineMultisampleStateCreateInfo.flags = 0;
      pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

      VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
      pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
      pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

      VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
      pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      pipelineColorBlendStateCreateInfo.pNext = nullptr;
      pipelineColorBlendStateCreateInfo.attachmentCount = 1;
      pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
      depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
      depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfo.size());
      graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
      graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
      graphicsPipelineCreateInfo.pTessellationState = nullptr;
      graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
      graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
      graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
      graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }


    std::vector<std::unique_ptr<VulkanGear>> CreateGears(const Vulkan::VUDevice& device,
                                                         const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const VkQueue queue)
    {
      // Gear definitions
      std::array<float, 3> innerRadiuses = {1.0f, 0.5f, 1.3f};
      std::array<float, 3> outerRadiuses = {4.0f, 2.0f, 2.0f};
      std::array<float, 3> widths = {1.0f, 2.0f, 0.5f};
      std::array<int32_t, 3> toothCount = {20, 10, 10};
      std::array<float, 3> toothDepth = {0.7f, 0.7f, 0.7f};
      std::array<glm::vec3, 3> colors = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f)};
      std::array<glm::vec3, 3> positions = {glm::vec3(-3.0, 0.0, 0.0), glm::vec3(3.1, 0.0, 0.0), glm::vec3(-3.1, -6.2, 0.0)};
      std::array<float, 3> rotationSpeeds = {1.0f, -2.0f, -2.0f};
      std::array<float, 3> rotationOffsets = {0.0f, -9.0f, -30.0f};

      std::vector<std::unique_ptr<VulkanGear>> gears(positions.size());
      for (std::size_t i = 0; i < gears.size(); ++i)
      {
        GearInfo gearInfo;
        gearInfo.InnerRadius = innerRadiuses[i];
        gearInfo.OuterRadius = outerRadiuses[i];
        gearInfo.Width = widths[i];
        gearInfo.NumTeeth = toothCount[i];
        gearInfo.ToothDepth = toothDepth[i];
        gearInfo.Color = colors[i];
        gearInfo.Pos = positions[i];
        gearInfo.RotSpeed = rotationSpeeds[i];
        gearInfo.RotOffset = rotationOffsets[i];

        gears[i] = std::make_unique<VulkanGear>(device.GetPhysicalDevice(), device.Get(), bufferManager);
        gears[i]->Generate(gearInfo, queue);
      }
      return gears;
    }

    GearsMeshInfo CreateGearsInfo()
    {
      GearsMeshInfo info;

      // Binding and attribute descriptions are shared across all gears
      info.VertexInputBindingDescription.binding = LocalConfig::VertexBufferBindId;
      info.VertexInputBindingDescription.stride = sizeof(Vertex);
      info.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      // Location 0 : Position
      info.VertexAttributeDescription[0].location = 0;
      info.VertexAttributeDescription[0].binding = LocalConfig::VertexBufferBindId;
      info.VertexAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      info.VertexAttributeDescription[0].offset = 0;

      // Location 1 : Normal
      info.VertexAttributeDescription[1].location = 1;
      info.VertexAttributeDescription[1].binding = LocalConfig::VertexBufferBindId;
      info.VertexAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      info.VertexAttributeDescription[1].offset = sizeof(float) * 3;

      // Location 2 : Color
      info.VertexAttributeDescription[2].location = 2;
      info.VertexAttributeDescription[2].binding = LocalConfig::VertexBufferBindId;
      info.VertexAttributeDescription[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      info.VertexAttributeDescription[2].offset = sizeof(float) * 6;
      return info;
    }


    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }
  }


  Gears::Gears(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_camera(config.WindowMetrics.GetSizePx())
  {
    m_camera.SetRotation(Matrix::CreateRotationY(LocalConfig::DefaultYRotation));
    m_camera.SetZoom(LocalConfig::DefaultZoom);
    m_camera.SetInvertY(true);

    m_resources.BufferManager =
      std::make_shared<Vulkan::VMBufferManager>(m_device.GetPhysicalDevice(), m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    m_resources.Gears = CreateGears(m_device, m_resources.BufferManager, m_deviceQueue.Queue);
    m_resources.GearsInfo = CreateGearsInfo();

    const auto contentManager = GetContentManager();
    m_resources.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("shaders/gears.vert.spv"));
    m_resources.FragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("shaders/gears.frag.spv"));

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);
    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);


    for (auto& rGear : m_resources.Gears)
    {
      rGear->SetupDescriptorSet(maxFramesInFlight, m_resources.MainDescriptorPool.Get(), m_resources.MainDescriptorSetLayout.Get());
    }

    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
  }


  void Gears::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    base_type::ConfigurationChanged(windowMetrics);

    m_camera.SetScreenResolution(windowMetrics.GetSizePx());
  }


  void Gears::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
      {
        if (event.IsPressed())
        {
          m_camera.BeginDrag(event.GetPosition());
        }
        else if (m_camera.IsDragging())
        {
          m_camera.EndDrag(event.GetPosition());
        }
        event.Handled();
      }
      break;
    case VirtualMouseButton::Right:
      if (event.IsPressed())
      {
        m_camera.ResetRotation();
        m_camera.SetZoom(LocalConfig::DefaultZoom);
        event.Handled();
      }
      break;
    default:
      base_type::OnMouseButtonEvent(event);
      break;
    }
  }


  void Gears::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    base_type::OnMouseMoveEvent(event);

    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void Gears::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    base_type::OnMouseWheelEvent(event);
    m_camera.AddZoom(static_cast<float>(event.GetDelta()) * -0.001f);
  }


  void Gears::Update(const DemoTime& demoTime)
  {
    base_type::Update(demoTime);

    m_timer += demoTime.DeltaTime * 0.10f;
    if (m_timer >= 1.0f)
    {
      m_timer -= 1.0f;
    }
  }


  void Gears::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    {
      const float aspect = GetWindowAspectRatio();
      const glm::mat4 perspective = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);

      glm::mat4 matView(ToGlmMat4(m_camera.GetViewMatrix()));
      for (auto& rGear : m_resources.Gears)
      {
        rGear->UpdateUniformBuffer(currentFrameIndex, perspective, matView, m_timer * 360.0f);
      }
    }

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        // Draw the gears
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());
        for (const auto& gear : m_resources.Gears)
        {
          gear->Draw(currentFrameIndex, hCmdBuffer, m_resources.MainPipelineLayout.Get());
        }

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass Gears::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    m_dependentResources.Pipeline =
      CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShaderModule.Get(),
                     m_resources.FragShaderModule.Get(), m_resources.GearsInfo, m_dependentResources.MainRenderPass.Get(), 0);

    return m_dependentResources.MainRenderPass.Get();
  }


  void Gears::OnFreeResources()
  {
    m_dependentResources.Reset();
  }
}
