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

#include "DisplacementMapping.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iomanip>

namespace Fsl
{
  using namespace Vulkan;
  using namespace Vulkan::ConvertUtil;
  using namespace Willems;

  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;

    // Vertex layout for this example
    const std::vector<MeshLoader::VertexLayout> g_vertexLayout = {
      MeshLoader::VertexLayout::VERTEX_LAYOUT_POSITION, MeshLoader::VertexLayout::VERTEX_LAYOUT_NORMAL, MeshLoader::VertexLayout::VERTEX_LAYOUT_UV};
  }


  DisplacementMapping::DisplacementMapping(const DemoAppConfig& config)
    : VulkanWillemsMeshDemoApp(config)
    , m_descriptorSet(VK_NULL_HANDLE)
    , m_displacement(true)
    , m_splitScreen(true)
  {
    m_zoom = -1.25f;
    m_rotation = glm::vec3(-20.0f, 45.0f, 0.0f);
    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Tessellation shader displacement mapping";

    // Support for tessellation shaders is optional, so check first
    if (m_deviceActiveFeatures.tessellationShader == VK_FALSE)
    {
      throw NotSupportedException("Selected GPU does not support tessellation shaders!");
    }
    if (m_deviceActiveFeatures.fillModeNonSolid == VK_FALSE)
    {
      FSLLOG("Disabling split screen");
      m_splitScreen = false;
    }
  }


  DisplacementMapping::~DisplacementMapping()
  {
    SafeWaitForDeviceIdle();
  }


  // This ensures the flow is the same as in the original sample
  void DisplacementMapping::Prepare()
  {
    VulkanWillemsMeshDemoApp::Prepare();
    LoadMeshes();
    LoadTextures();
    SetupVertexDescriptions();
    PrepareUniformBuffers();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    BuildCommandBuffers();
  }


  void DisplacementMapping::GetOverlayText(VulkanTextOverlay& rTextOverlay)
  {
    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed << m_uboTE.TessStrength;
    rTextOverlay.AddText("Tessellation height: " + ss.str() + " (numpad +/-)", 5.0f, 85.0f, VulkanTextOverlay::TextAlign::Left);
    rTextOverlay.AddText("Press \"d\" to toggle displacement", 5.0f, 100.0f, VulkanTextOverlay::TextAlign::Left);
    rTextOverlay.AddText("Press \"s\" to toggle splitscreen", 5.0f, 115.0f, VulkanTextOverlay::TextAlign::Left);
  }


  void DisplacementMapping::OnViewChanged()
  {
    VulkanWillemsMeshDemoApp::OnViewChanged();

    UpdateUniformBuffers();
  }


  void DisplacementMapping::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkClearValue clearValues[2];
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.width);
    viewport.height = static_cast<float>(screenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = m_splitScreen ? screenExtent.width / 2 : screenExtent.width;
    scissor.extent.height = screenExtent.height;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        m_drawCmdBuffers.CmdBeginRenderPass(i, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
          vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);
          scissor.offset.x = 0;
          vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);
          vkCmdSetLineWidth(m_drawCmdBuffers[i], 1.0f);

          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);

          VkDeviceSize offsets[1] = {0};
          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Object.GetVertices().GetBufferPointer(), offsets);
          vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Object.GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

          if (m_splitScreen && m_pipelines.Wireframe.IsValid())
          {
            vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Wireframe.Get());
            vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Object.GetIndexCount(), 1, 0, 0, 0);
            scissor.offset.x = screenExtent.width / 2;
            vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);
          }

          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Solid.Get());
          vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Object.GetIndexCount(), 1, 0, 0, 0);

          DrawUI(m_drawCmdBuffers[i]);
        }
        m_drawCmdBuffers.CmdEndRenderPass(i);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void DisplacementMapping::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }


    if (event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::UpArrow:
      case VirtualKey::Add:
      case VirtualKey::GamePadButtonRightShoulder:
        ChangeTessellationStrength(0.025f);
        break;
      case VirtualKey::DownArrow:
      case VirtualKey::Subtract:
      case VirtualKey::GamePadButtonLeftShoulder:
        ChangeTessellationStrength(-0.025f);
        break;
      case VirtualKey::D:
      case VirtualKey::GamePadButtonA:
        ToggleDisplacement();
        break;
      case VirtualKey::S:
      case VirtualKey::GamePadButtonX:
        ToggleSplitScreen();
        break;
      default:
        break;
      }
    }

    VulkanWillemsMeshDemoApp::OnKeyEvent(event);
  }


  void DisplacementMapping::Update(const DemoTime& demoTime)
  {
  }


  void DisplacementMapping::Draw(const DemoTime& demoTime)
  {
    if (!TryPrepareFrame())
    {
      return;
    }

    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();
  }


  void DisplacementMapping::LoadMeshes()
  {
    m_meshes.Object = LoadMesh("Models/Plane/plane.obj", g_vertexLayout, 0.25f);
  }


  void DisplacementMapping::LoadTextures()
  {
    if (m_deviceActiveFeatures.textureCompressionBC != VK_FALSE)
    {
      FSLLOG("Using BC compression");
      m_textures.ColorHeightMap = m_textureLoader->LoadTexture("Textures/Pattern36/pattern_36_bc3.ktx");
    }
    else if (m_deviceActiveFeatures.textureCompressionETC2 != VK_FALSE)
    {
      FSLLOG("Using ETC2 compression");
      m_textures.ColorHeightMap = m_textureLoader->LoadTexture("Textures/Pattern36/pattern_36_etc2.ktx");
    }
    else
    {
      throw NotSupportedException("No supported compression format found");
    }
  }


  void DisplacementMapping::SetupVertexDescriptions()
  {
    // Binding description
    m_vertices.BindingDescriptions.clear();
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = MeshLoader::VertexSize(g_vertexLayout);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.clear();
    m_vertices.AttributeDescriptions.resize(3);

    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = 0;
    // Location 1 : Normals
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = sizeof(float) * 3;
    // Location 2 : Texture coordinates
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = sizeof(float) * 6;

    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  // Prepare and initialize uniform buffer containing shader uniforms
  void DisplacementMapping::PrepareUniformBuffers()
  {
    // Tessellation evaluation shader uniform buffer
    CreateBuffer(m_uniformDataTE.Buffer, m_uniformDataTE.Memory, m_uniformDataTE.Descriptor, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboTE), nullptr);

    // Tessellation control shader uniform buffer
    CreateBuffer(m_uniformDataTC.Buffer, m_uniformDataTC.Memory, m_uniformDataTC.Descriptor, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboTC), nullptr);

    UpdateUniformBuffers();
  }

  void DisplacementMapping::UpdateUniformBuffers()
  {
    const auto screenExtent = GetScreenExtent();

    const float aspect = static_cast<float>(screenExtent.Width) / static_cast<float>(screenExtent.Height);
    // Tessellation eval
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    m_uboTE.Projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 256.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, m_zoom));

    // float offset = 0.5f;
    // int uboIndex = 1;
    m_uboTE.Model = glm::mat4(1.0f);
    m_uboTE.Model = viewMatrix * glm::translate(m_uboTE.Model, glm::vec3(0, 0, 0));
    m_uboTE.Model = glm::rotate(m_uboTE.Model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboTE.Model = glm::rotate(m_uboTE.Model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboTE.Model = glm::rotate(m_uboTE.Model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    m_uboTE.LightPos.y = -0.5f - m_uboTE.TessStrength;
    {
      m_uniformDataTE.Map(0, sizeof(m_uboTE));
      std::memcpy(m_uniformDataTE.pMapped, &m_uboTE, sizeof(m_uboTE));
      m_uniformDataTE.Unmap();
    }

    // Tessellation control
    float savedLevel = m_uboTC.TessLevel;
    if (!m_displacement)
    {
      m_uboTC.TessLevel = 1.0f;
    }

    {
      m_uniformDataTC.Map(0, sizeof(m_uboTC));
      std::memcpy(m_uniformDataTC.pMapped, &m_uboTC, sizeof(m_uboTC));
      m_uniformDataTC.Unmap();
    }

    if (!m_displacement)
    {
      m_uboTC.TessLevel = savedLevel;
    }
  }


  void DisplacementMapping::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(3);
    // Binding 0 : Tessellation control shader ubo
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    // Binding 1 : Tessellation evaluation shader ubo
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    // Binding 2 : Combined color (rgb) and height (alpha) map
    setLayoutBindings[2].binding = 2;
    setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[2].descriptorCount = 1;
    setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    m_pipelineLayout.Reset(m_device.Get(), pipelineLayoutCreateInfo);
  }


  void DisplacementMapping::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

    std::vector<VkDynamicState> dynamicStateEnables1 = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState1{};
    dynamicState1.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState1.flags = 0;
    dynamicState1.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables1.size());
    dynamicState1.pDynamicStates = dynamicStateEnables1.data();

    VkPipelineTessellationStateCreateInfo tessellationState{};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.patchControlPoints = 3;

    // Tessellation pipeline
    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 4> shaderStages{};
    shaderStages[0] = LoadShader("shaders/base.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/base.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderStages[2] = LoadShader("shaders/displacement.tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    shaderStages[3] = LoadShader("shaders/displacement.tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

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
    pipelineCreateInfo.pDynamicState = &dynamicState1;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.renderPass = m_renderPass.Get();

    FSLLOG("Creating solid pipe");
    // Solid pipeline
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    m_pipelines.Solid.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);

    // Wireframe pipeline
    if (m_deviceActiveFeatures.fillModeNonSolid != VK_FALSE)
    {
      std::vector<VkDynamicState> dynamicStateEnables2 = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH};

      VkPipelineDynamicStateCreateInfo dynamicState2{};
      dynamicState2.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState2.flags = 0;
      dynamicState2.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables2.size());
      dynamicState2.pDynamicStates = dynamicStateEnables2.data();

      pipelineCreateInfo.pDynamicState = &dynamicState2;

      FSLLOG("Creating wireless pipe");
      rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
      rasterizationState.cullMode = VK_CULL_MODE_NONE;
      m_pipelines.Wireframe.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
    }
  }


  void DisplacementMapping::SetupDescriptorPool()
  {
    // Example uses two ubos and two image samplers
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 2;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void DisplacementMapping::SetupDescriptorSet()
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));

    // Color and height map image descriptor
    VkDescriptorImageInfo texDescriptor{};
    texDescriptor.sampler = m_textures.ColorHeightMap.GetSampler();
    texDescriptor.imageView = m_textures.ColorHeightMap.GetImageView();
    texDescriptor.imageLayout = m_textures.ColorHeightMap.GetImageLayout();

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(3);
    // Binding 0 : Tessellation control shader ubo
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformDataTC.Descriptor;
    // Binding 1 : Tessellation evaluation shader ubo
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[1].pBufferInfo = &m_uniformDataTE.Descriptor;
    // Binding 2 : Color and displacement map (alpha channel)
    writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[2].pNext = nullptr;
    writeDescriptorSets[2].dstSet = m_descriptorSet;
    writeDescriptorSets[2].dstBinding = 2;
    writeDescriptorSets[2].descriptorCount = 1;
    writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[2].pImageInfo = &texDescriptor;

    vkUpdateDescriptorSets(m_device.Get(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }


  void DisplacementMapping::ReBuildCommandBuffers()
  {
    if (!CheckCommandBuffers())
    {
      DestroyCommandBuffers();
      CreateCommandBuffers();
    }
    BuildCommandBuffers();
  }


  void DisplacementMapping::ChangeTessellationLevel(const float delta)
  {
    m_uboTC.TessLevel += delta;
    m_uboTC.TessLevel = std::max(1.0f, std::min(m_uboTC.TessLevel, 32.0f));
    UpdateUniformBuffers();
    UpdateTextOverlay();
  }


  void DisplacementMapping::ChangeTessellationStrength(const float delta)
  {
    m_uboTE.TessStrength += delta;
    m_uboTE.TessStrength = std::max(0.0f, std::min(m_uboTE.TessStrength, 1.0f));
    UpdateUniformBuffers();
    UpdateTextOverlay();
  }


  void DisplacementMapping::ToggleSplitScreen()
  {
    m_splitScreen = !m_splitScreen;
    ReBuildCommandBuffers();
    UpdateUniformBuffers();
  }


  void DisplacementMapping::ToggleDisplacement()
  {
    m_displacement = !m_displacement;
    UpdateUniformBuffers();
  }
}
