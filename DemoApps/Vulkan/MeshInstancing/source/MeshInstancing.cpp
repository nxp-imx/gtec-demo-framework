/*
* Vulkan Example - Instanced mesh rendering, uses a separate vertex buffer for instanced data
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Mesh instancing' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "MeshInstancing.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslGraphicsVulkan1_0/Extend/Convert.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/ConvertUtil.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/MemoryTypeHelper.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <array>
#include <cstring>
#include <cmath>
#include <ctime>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fsl
{
  using namespace Vulkan;
  using namespace Willems;

  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
    const uint32_t INSTANCE_BUFFER_BIND_ID = 1;

#ifdef __ANDROID__
    const uint32_t INSTANCE_COUNT = 1024;
#else
    const uint32_t INSTANCE_COUNT = 2048;
#endif


    // Vertex layout for this example
    const std::vector<MeshLoader::VertexLayout> g_vertexLayout =
    {
      MeshLoader::VertexLayout::VERTEX_LAYOUT_POSITION,
      MeshLoader::VertexLayout::VERTEX_LAYOUT_NORMAL,
      MeshLoader::VertexLayout::VERTEX_LAYOUT_UV,
      MeshLoader::VertexLayout::VERTEX_LAYOUT_COLOR
    };
  }


  MeshInstancing::MeshInstancing(const DemoAppConfig& config)
    : VulkanWillemsMeshDemoApp(config)
  {
    m_zoom = -12.0f;
    m_rotationSpeed = 0.25f;

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Instanced mesh rendering";
  }


  MeshInstancing::~MeshInstancing()
  {

  }


  // This ensures the flow is the same as in the original sample
  void MeshInstancing::Prepare()
  {
    VulkanWillemsMeshDemoApp::Prepare();
    LoadTextures();
    LoadMeshes();
    PrepareInstanceData();
    SetupVertexDescriptions();
    PrepareUniformBuffers();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    BuildCommandBuffers();
  }


  void MeshInstancing::GetOverlayText(VulkanTextOverlay& rTextOverlay)
  {
    rTextOverlay.AddText("Rendering " + ToString(INSTANCE_COUNT) + " instances", 5.0f, 85.0f, VulkanTextOverlay::TextAlign::Left);
  }


  void MeshInstancing::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    VkClearValue clearValues[2];
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.width = static_cast<float>(screenExtent.width);
        viewport.height = static_cast<float>(screenExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = screenExtent;

        vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

        vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);
        vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Solid.Get());

        VkDeviceSize offsets[1] = { 0 };
        // Binding point 0 : Mesh vertex buffer
        vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Example.GetVertices().GetBufferPointer(), offsets);
        // Binding point 1 : Instance data buffer
        vkCmdBindVertexBuffers(m_drawCmdBuffers[i], INSTANCE_BUFFER_BIND_ID, 1, m_instanceBuffer.Buffer.GetPointer(), offsets);

        vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Example.GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // Render instances
        vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Example.GetIndexCount(), INSTANCE_COUNT, 0, 0, 0);

        vkCmdEndRenderPass(m_drawCmdBuffers[i]);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void MeshInstancing::OnViewChanged()
  {
    VulkanWillemsMeshDemoApp::OnViewChanged();
    UpdateUniformBuffer(true);
  }


  void MeshInstancing::Update(const DemoTime& demoTime)
  {
  }


  void MeshInstancing::Draw(const DemoTime& demoTime)
  {
    if (!TryPrepareFrame())
      return;

    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();

    // TODO:
    //if (!paused)
    UpdateUniformBuffer(false);
  }


  void MeshInstancing::LoadTextures()
  {
    if (m_deviceFeatures.textureCompressionBC)
    {
      FSLLOG("Using BC texture compression");
      m_textures.ColorMap = m_textureLoader->LoadTextureArray("textures/texturearray_rocks_bc3.ktx");
    }
    else if (m_deviceFeatures.textureCompressionETC2)
    {
      FSLLOG("Using ETC2 texture compression");
      m_textures.ColorMap = m_textureLoader->LoadTextureArray("textures/texturearray_rocks_etc2.ktx");
    }
    else
      throw NotSupportedException("No supported texture compression found");
  }


  void MeshInstancing::LoadMeshes()
  {
    m_meshes.Example = LoadMesh("models/rock01.dae", g_vertexLayout, 0.1f);

  }


  void MeshInstancing::PrepareInstanceData()
  {
    std::vector<InstanceData> instanceData(INSTANCE_COUNT);
    std::mt19937 rndGenerator(static_cast<std::mt19937::result_type>(time(nullptr)));
    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);
    std::uniform_int_distribution<uint32_t> uniformTexDist(0, m_textures.ColorMap.GetLayers()-1);

    for (std::size_t i = 0; i < INSTANCE_COUNT; ++i)
    {
      instanceData[i].rot = glm::vec3(MathHelper::PI * uniformDist(rndGenerator), MathHelper::PI * uniformDist(rndGenerator), MathHelper::PI * uniformDist(rndGenerator));
      const float theta = static_cast<float>(2.0 * MathHelper::PI * uniformDist(rndGenerator));
      const float phi = static_cast<float>(std::acos(1.0 - 2.0 * uniformDist(rndGenerator)));
      glm::vec3 pos;
      instanceData[i].pos = glm::vec3(std::sin(phi) * std::cos(theta), std::sin(theta) * uniformDist(rndGenerator) / 1500.0f, std::cos(phi)) * 7.5f;
      instanceData[i].scale = static_cast<float>(1.0 + uniformDist(rndGenerator) * 2.0);
      instanceData[i].texIndex = uniformTexDist(rndGenerator);
    }

    m_instanceBuffer.Size = static_cast<uint32_t>(instanceData.size() * sizeof(InstanceData));

    // Staging
    // Instanced data is static, copy to device local memory
    // This results in better performance
    struct
    {
      Vulkan::Buffer Buffer;
      Vulkan::Memory Memory;
    } stagingBuffer;

    CreateBuffer(stagingBuffer.Buffer, stagingBuffer.Memory,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      m_instanceBuffer.Size, instanceData.data());

    CreateBuffer(m_instanceBuffer.Buffer, m_instanceBuffer.Memory,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      m_instanceBuffer.Size, nullptr);

    { // Copy to staging buffer
      CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

      VkBufferCopy copyRegion{};
      copyRegion.size = m_instanceBuffer.Size;
      vkCmdCopyBuffer(copyCmd.Get(), stagingBuffer.Buffer.Get(), m_instanceBuffer.Buffer.Get(), 1, &copyRegion);

      FlushCommandBuffer(copyCmd, m_deviceQueue.Queue, true);
    }

    m_instanceBuffer.Descriptor.range = m_instanceBuffer.Size;
    m_instanceBuffer.Descriptor.buffer = m_instanceBuffer.Buffer.Get();
    m_instanceBuffer.Descriptor.offset = 0;
  }


  void MeshInstancing::SetupVertexDescriptions()
  {
    // Binding description
    m_vertices.BindingDescriptions.resize(2);

    // Mesh vertex buffer (description) at binding point 0
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = MeshLoader::VertexSize(g_vertexLayout);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Input rate for the data passed to shader// Step for each vertex rendered;

    m_vertices.BindingDescriptions[1].binding = INSTANCE_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[1].stride = sizeof(InstanceData);
    m_vertices.BindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE; // Input rate for the data passed to shader// Step for each instance rendered;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.clear();
    m_vertices.AttributeDescriptions.resize(8);

    // Per-Vertex attributes
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
    // Location 2 : Texture coordinates
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = sizeof(float) * 6;
    // Location 3 : Color
    m_vertices.AttributeDescriptions[3].location = 3;
    m_vertices.AttributeDescriptions[3].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[3].offset = sizeof(float) * 8;
    // Instanced attributes
    // Location 4 : Position
    m_vertices.AttributeDescriptions[4].location = 5; // TODO: 5?
    m_vertices.AttributeDescriptions[4].binding = INSTANCE_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[4].offset = sizeof(float) * 3;
    // Location 5 : Rotation
    m_vertices.AttributeDescriptions[5].location = 4; // TODO: 4
    m_vertices.AttributeDescriptions[5].binding = INSTANCE_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[5].offset = 0;
    // Location 6 : Scale
    m_vertices.AttributeDescriptions[6].location = 6;
    m_vertices.AttributeDescriptions[6].binding = INSTANCE_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[6].format = VK_FORMAT_R32_SFLOAT;
    m_vertices.AttributeDescriptions[6].offset = sizeof(float) * 6;
    // Location 7 : Texture array layer index
    m_vertices.AttributeDescriptions[7].location = 7;
    m_vertices.AttributeDescriptions[7].binding = INSTANCE_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[7].format = VK_FORMAT_R32_SINT;
    m_vertices.AttributeDescriptions[7].offset = sizeof(float) * 7;


    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void MeshInstancing::PrepareUniformBuffers()
  {
    CreateBuffer(m_uniformData.VsScene.Buffer, m_uniformData.VsScene.Memory, m_uniformData.VsScene.Descriptor,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      sizeof(m_uboVS), nullptr);

    // Map for host access
    m_uniformData.VsScene.Map(0, sizeof(m_uboVS));
    UpdateUniformBuffer(true);
    //m_uniformData.VsScene.Unmap();
  }


  void MeshInstancing::UpdateUniformBuffer(const bool viewChanged)
  {
    if (viewChanged)
    {
      const auto screenExtent = GetScreenExtent();

      const float aspect = static_cast<float>(screenExtent.Width) / static_cast<float>(screenExtent.Height);
      m_uboVS.Projection = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);
      m_uboVS.View = glm::translate(glm::mat4(), m_cameraPos + glm::vec3(0.0f, 0.0f, m_zoom));
      m_uboVS.View = glm::rotate(m_uboVS.View, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
      m_uboVS.View = glm::rotate(m_uboVS.View, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
      m_uboVS.View = glm::rotate(m_uboVS.View, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // FIX:
    //if (!m_paused)
    {
      m_uboVS.Time += m_frameTimer * 0.05f;
    }

    std::memcpy(m_uniformData.VsScene.pMapped, &m_uboVS, sizeof(m_uboVS));
  }


  void MeshInstancing::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
    // Binding 0 : Vertex shader uniform buffer
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // Binding 1 : Fragment shader combined sampler
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

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


  void MeshInstancing::PreparePipelines()
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


    // Instancing pipeline
    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0] = LoadShader("shaders/instancing.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/instancing.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

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


  void MeshInstancing::SetupDescriptorPool()
  {
    // Example uses one ubo
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
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


  void MeshInstancing::SetupDescriptorSet()
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    FSLGRAPHICSVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(2);
    // Binding 0 : Vertex shader uniform buffer
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.VsScene.Descriptor;
    // Binding 1 : Color map
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = m_textures.ColorMap.GetImageDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }
}
