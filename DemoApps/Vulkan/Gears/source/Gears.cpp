/*
* Vulkan Example - Animated gears using multiple uniform buffers
*
* See readme.md for details
*
* Copyright (C) 2015 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Gears' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)



#include "Gears.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslGraphicsVulkan1_0/Extend/Convert.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/ConvertUtil.hpp>
#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fsl
{
  using namespace Vulkan;
  using namespace Willems;

  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
  }


  Gears::Gears(const DemoAppConfig& config)
    : VulkanWillemsDemoApp(config)
  {
    m_zoom = -16.0f;
    m_rotation = glm::vec3(-23.75, 41.25, 21.0);
    m_timerSpeed *= 0.25f;

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Gears";
  }


  Gears::~Gears()
  {

  }


  // This ensures the flow is the same as in the original sample
  void Gears::Prepare()
  {
    VulkanWillemsDemoApp::Prepare();
    PrepareVertices();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSets();
    UpdateUniformBuffers();
    BuildCommandBuffers();
  }


  void Gears::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkClearValue clearValues[2];
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.width);
    viewport.height = static_cast<float>(screenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = screenExtent;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
          vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);
          vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);
          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Solid.Get());

          for (auto& rGear : m_gears)
          {
            rGear->Draw(m_drawCmdBuffers[i], m_pipelineLayout.Get());
          }
        }
        vkCmdEndRenderPass(m_drawCmdBuffers[i]);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void Gears::OnViewChanged()
  {
    UpdateUniformBuffers();
  }


  void Gears::Update(const DemoTime& demoTime)
  {
  }


  void Gears::Draw(const DemoTime& demoTime)
  {
    vkDeviceWaitIdle(m_device.Get());
    {
      if (!TryPrepareFrame())
        return;

      // Command buffer to be submitted to the queue
      m_submitInfo.commandBufferCount = 1;
      m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

      // Submit to queue
      m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

      SubmitFrame();
    }
    vkDeviceWaitIdle(m_device.Get());
    if (!m_paused)
    {
      UpdateUniformBuffers();
    }
  }


  void Gears::PrepareVertices()
  {
    // Gear definitions
    std::vector<float> innerRadiuses = { 1.0f, 0.5f, 1.3f };
    std::vector<float> outerRadiuses = { 4.0f, 2.0f, 2.0f };
    std::vector<float> widths = { 1.0f, 2.0f, 0.5f };
    std::vector<int32_t> toothCount = { 20, 10, 10 };
    std::vector<float> toothDepth = { 0.7f, 0.7f, 0.7f };
    std::vector<glm::vec3> colors =
    {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.2f),
      glm::vec3(0.0f, 0.0f, 1.0f)
    };
    std::vector<glm::vec3> positions =
    {
      glm::vec3(-3.0, 0.0, 0.0),
      glm::vec3(3.1, 0.0, 0.0),
      glm::vec3(-3.1, -6.2, 0.0)
    };
    std::vector<float> rotationSpeeds = { 1.0f, -2.0f, -2.0f };
    std::vector<float> rotationOffsets = { 0.0f, -9.0f, -30.0f };

    m_gears.clear();
    m_gears.resize(positions.size());
    for (std::size_t i = 0; i < m_gears.size(); ++i)
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

      m_gears[i].reset(new VulkanGear(&m_vulkanDevice));
      m_gears[i]->Generate(gearInfo, m_deviceQueue.Queue);
    }

    // Binding and attribute descriptions are shared across all gears
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = sizeof(Vertex);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(3);

    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = 0;

    // Location 1 : Normal
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = sizeof(float) * 3;

    // Location 2 : Color
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = sizeof(float) * 6;


    // Lookup of initializer 'pipelineVertexInputStateCreateInfo'
    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void Gears::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(1);
    // Binding 0 : Vertex shader uniform buffer
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo{};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    m_pipelineLayout.Reset(m_device.Get(), pPipelineLayoutCreateInfo);
  }


  void Gears::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_FALSE;
    blendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.front.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkDynamicState> dynamicStateEnables =
    {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();

    // Solid rendering pipeline
    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0] = LoadShader("shaders/gears.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/gears.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.layout = m_pipelineLayout.Get();
    pipelineCreateInfo.renderPass = m_renderPass.Get();


    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    m_pipelines.Solid.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void Gears::SetupDescriptorPool()
  {
    // One UBO for each gear
    VkDescriptorPoolSize poolSize{};
    // Lookup of initializer 'descriptorPoolSize'
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 3;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 3; // Three descriptor sets (for each gear)
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &poolSize;

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void Gears::SetupDescriptorSets()
  {
    for (auto& rGear : m_gears)
    {
      rGear->SetupDescriptorSet(m_descriptorPool.Get(), m_descriptorSetLayout.Get());
    }
  }


  void Gears::UpdateUniformBuffers()
  {
    const auto screenExtent = GetScreenExtent();

    const float aspect = static_cast<float>(screenExtent.Width) / static_cast<float>(screenExtent.Height);
    const glm::mat4 perspective = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);

    for (auto& rGear : m_gears)
    {
      rGear->UpdateUniformBuffer(perspective, m_rotation, m_zoom, m_timer * 360.0f);
    }
  }
}
