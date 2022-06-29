/*
 * Vulkan Example - Attraction based compute shader particle system
 *
 * Updated compute shader by Lukas Bergdoll (https://github.com/Voultapher)
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'ComputeParticles' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "ComputeParticles.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/Config.hpp>
#include <array>
#include <cstddef>
#include <cstring>
#include <random>

namespace Fsl
{
  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
  }


  ComputeParticles::ComputeParticles(const DemoAppConfig& config)
    : VulkanWillemsDemoApp(config)
    , m_optionParser(config.GetOptions<OptionParserEx>())
    , m_compute(m_optionParser->GetParticleCount())
  {
    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Compute shader particle system";
  }


  ComputeParticles::~ComputeParticles()
  {
    SafeWaitForDeviceIdle();

    // Controlled shutdown since things are created and destroyed in a different order than the 'declaration order'

    m_compute.Fence.Reset();
    m_compute.CommandBuffer.Reset();
    m_compute.CommandPool.Reset();
    m_compute.Pipeline.Reset();
    // m_compute.DescriptorSet.Reset(); (pool managed)
    m_compute.DescriptorSetLayout.Reset();
    // m_graphics.DescriptorSet.Reset();
    m_descriptorPool.Reset();
    m_graphics.Pipeline.Reset();
    m_graphics.PipelineLayout.Reset();
    m_graphics.DescriptorSetLayout.Reset();
    m_compute.UniformBuffer.Unmap();
    m_compute.UniformBuffer.Reset();
    // m_compute.StorageBuffer.Unmap();
    m_compute.StorageBuffer.Reset();
  }


  // This ensures the flow is the same as in the original sample
  void ComputeParticles::Prepare()
  {
    VulkanWillemsDemoApp::Prepare();
    LoadTextures();
    PrepareStorageBuffers();
    PrepareUniformBuffers();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    PrepareCompute();
    BuildCommandBuffers();
  }


  void ComputeParticles::GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay)
  {
    rTextOverlay.AddText("Particles: " + ToString(m_optionParser->GetParticleCount()), 5.0f, 85.0f, Willems::VulkanTextOverlay::TextAlign::Left);
  }


  void ComputeParticles::OnKeyEvent(const KeyEvent& event)
  {
    Willems::VulkanWillemsDemoApp::OnKeyEvent(event);

    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::A:
    case VirtualKey::GamePadButtonA:
      if (event.IsPressed())
      {
        m_animate = !m_animate;
      }
      break;
    default:
      break;
    }
  }


  void ComputeParticles::Update(const DemoTime& /*demoTime*/)
  {
  }


  void ComputeParticles::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    {    // Do the draw
      // Submit graphics commands

      if (!TryPrepareFrame())
      {
        return;
      }

      // Command buffer to be submitted to the queue
      m_submitInfo.commandBufferCount = 1;
      m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

      // Submit to queue
      m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

      SubmitFrame();

      // Submit compute commands
      m_device.WaitForFences(1, m_compute.Fence.GetPointer(), VK_TRUE, UINT64_MAX);
      m_device.ResetFences(1, m_compute.Fence.GetPointer());

      VkSubmitInfo computeSubmitInfo{};
      computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      computeSubmitInfo.pNext = nullptr;
      computeSubmitInfo.commandBufferCount = 1;
      computeSubmitInfo.pCommandBuffers = m_compute.CommandBuffer.GetPointer();

      RAPIDVULKAN_CHECK(vkQueueSubmit(m_compute.Queue, 1, &computeSubmitInfo, m_compute.Fence.Get()));
    }

    if (m_animate)
    {
      if (m_animStart > 0.0f)
      {
        m_animStart -= m_frameTimer * 5.0f;
      }
      else if (m_animStart <= 0.0f)
      {
        m_timer += m_frameTimer * 0.04f;
        if (m_timer > 1.f)
        {
          m_timer = 0.f;
        }
      }
    }

    UpdateUniformBuffers();
  }


  void ComputeParticles::BuildCommandBuffers()
  {
    // Destroy command buffers if already present
    // FIX:
    // if (!CheckCommandBuffers())
    //{
    //  DestroyCommandBuffers();
    //  CreateCommandBuffers();
    //}
    auto screenExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());


    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;

    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.width);
    viewport.height = static_cast<float>(screenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = screenExtent;

    VkDeviceSize offsets = 0;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        // Draw the particle system using the update vertex buffer
        vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
          vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);
          vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics.Pipeline.Get());
          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics.PipelineLayout.Get(), 0, 1,
                                  &m_graphics.DescriptorSet, 0, nullptr);

          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_compute.StorageBuffer.GetBufferPointer(), &offsets);
          vkCmdDraw(m_drawCmdBuffers[i], m_optionParser->GetParticleCount(), 1, 0, 0);

          DrawUI(m_drawCmdBuffers[i]);
        }
        vkCmdEndRenderPass(m_drawCmdBuffers[i]);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void ComputeParticles::LoadTextures()
  {
    assert(m_textureLoader);

    m_textures.Particle = m_textureLoader->LoadTexture("particle01_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, false);
    m_textures.Gradient = m_textureLoader->LoadTexture("particle_gradient_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, false);
  }


  // Setup and fill the compute shader storage buffers containing the particles
  void ComputeParticles::PrepareStorageBuffers()
  {
    std::mt19937 rGenerator;
    std::uniform_real_distribution<float> rDistribution(-1.0f, 1.0f);

    // Initial particle positions
    std::vector<Particle> particleBuffer(m_optionParser->GetParticleCount());
    for (auto& particle : particleBuffer)
    {
      particle.Pos = glm::vec2(rDistribution(rGenerator), rDistribution(rGenerator));
      particle.Vel = glm::vec2(0.0f);
      particle.GradientPos.x = particle.Pos.x / 2.0f;
    }

    VkDeviceSize storageBufferSize = particleBuffer.size() * sizeof(Particle);

    // Staging
    // SSBO won't be changed on the host after upload so copy to device local memory

    Willems::VulkanBuffer stagingBuffer =
      m_vulkanDevice.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  storageBufferSize, particleBuffer.data());

    // The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
    m_compute.StorageBuffer =
      m_vulkanDevice.CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, storageBufferSize);

    // Copy to staging buffer
    RapidVulkan::CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    VkBufferCopy copyRegion{};
    copyRegion.size = storageBufferSize;
    vkCmdCopyBuffer(copyCmd.Get(), stagingBuffer.GetBuffer(), m_compute.StorageBuffer.GetBuffer(), 1, &copyRegion);
    FlushCommandBuffer(copyCmd, m_deviceQueue.Queue, true);

    // Binding description
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = sizeof(Particle);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(2);
    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = offsetof(Particle, Pos);

    // Location 1 : Gradient position
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = offsetof(Particle, GradientPos);

    // Assign to vertex buffer
    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;

    m_vertices.InputState.vertexBindingDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void ComputeParticles::PrepareUniformBuffers()
  {
    // Compute shader uniform buffer block
    m_compute.UniformBuffer = m_vulkanDevice.CreateBuffer(
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_compute.Ubo));

    // Map for host access
    m_compute.UniformBuffer.MapEx();

    UpdateUniformBuffers();
  }


  void ComputeParticles::UpdateUniformBuffers()
  {
    m_compute.Ubo.DeltaT = m_frameTimer * 2.5f;
    if (m_animate)
    {
      m_compute.Ubo.DestX = static_cast<float>(sin(glm::radians(m_timer * 360.0f)) * 0.75f);
      m_compute.Ubo.DestY = 0.0f;
    }
    else
    {
      const auto screenExtent = GetScreenExtent();
      if (screenExtent.Width > 0 && screenExtent.Height > 0)
      {
        const float normalizedMx = (m_mousePos.x - (static_cast<float>(screenExtent.Width) / 2.0f)) / (static_cast<float>(screenExtent.Width) / 2.0f);
        const float normalizedMy =
          (m_mousePos.y - (static_cast<float>(screenExtent.Height) / 2.0f)) / (static_cast<float>(screenExtent.Height) / 2.0f);
        m_compute.Ubo.DestX = normalizedMx;
        m_compute.Ubo.DestY = normalizedMy;
      }
    }

    assert(m_compute.UniformBuffer.GetMappedPointer() != nullptr);
    std::memcpy(m_compute.UniformBuffer.GetMappedPointer(), &m_compute.Ubo, sizeof(m_compute.Ubo));
  }


  void ComputeParticles::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
    // Binding 0 : Particle color map
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 1 : Particle gradient ramp
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_graphics.DescriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_graphics.DescriptorSetLayout.GetPointer();

    m_graphics.PipelineLayout.Reset(m_device.Get(), pipelineLayoutCreateInfo);
  }


  void ComputeParticles::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
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
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
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

    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();

    // Rendering pipeline, load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0] = LoadShader("particle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("particle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.layout = m_graphics.PipelineLayout.Get();
    pipelineCreateInfo.renderPass = m_renderPass.Get();
    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.renderPass = m_renderPass.Get();

    // Additive blending
    blendAttachmentState.colorWriteMask = 0xF;
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;

    m_graphics.Pipeline.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void ComputeParticles::SetupDescriptorPool()
  {
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = 1;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 2;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void ComputeParticles::SetupDescriptorSet()
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_graphics.DescriptorSetLayout.GetPointer();

    vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_graphics.DescriptorSet);

    std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};

    // Binding 0 : Particle color map
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_graphics.DescriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[0].pImageInfo = m_textures.Particle.GetImageDescriptorPointer();

    // Binding 1 : Particle gradient ramp
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_graphics.DescriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = m_textures.Gradient.GetImageDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }


  void ComputeParticles::PrepareCompute()
  {
    // Create a compute capable device queue
    // The VulkanDevice::createLogicalDevice functions finds a compute capable queue and prefers queue families that only support compute
    // Depending on the implementation this may result in different queue family indices for graphics and computes,
    // requiring proper synchronization (see the memory barriers in buildComputeCommandBuffer)
    vkGetDeviceQueue(m_device.Get(), m_vulkanDevice.GetQueueFamilyIndices().Compute, 0, &m_compute.Queue);

    // Create compute pipeline
    // Compute pipelines are created separate from graphics pipelines even if they use the same queue (family index)

    std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
    // Binding 0 : Particle position storage buffer
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    // Binding 1 : Uniform buffer
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_compute.DescriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo{};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = m_compute.DescriptorSetLayout.GetPointer();

    m_compute.PipelineLayout.Reset(m_device.Get(), pPipelineLayoutCreateInfo);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_compute.DescriptorSetLayout.GetPointer();

    vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_compute.DescriptorSet);

    std::array<VkWriteDescriptorSet, 2> computeWriteDescriptorSets{};
    // Binding 0 : Particle position storage buffer
    computeWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    computeWriteDescriptorSets[0].pNext = nullptr;
    computeWriteDescriptorSets[0].dstSet = m_compute.DescriptorSet;
    computeWriteDescriptorSets[0].dstBinding = 0;
    computeWriteDescriptorSets[0].descriptorCount = 1;
    computeWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeWriteDescriptorSets[0].pBufferInfo = m_compute.StorageBuffer.GetDescriptorPointer();
    // Binding 1 : Uniform buffer
    computeWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    computeWriteDescriptorSets[1].pNext = nullptr;
    computeWriteDescriptorSets[1].dstSet = m_compute.DescriptorSet;
    computeWriteDescriptorSets[1].dstBinding = 1;
    computeWriteDescriptorSets[1].descriptorCount = 1;
    computeWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    computeWriteDescriptorSets[1].pBufferInfo = m_compute.UniformBuffer.GetDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(computeWriteDescriptorSets.size()), computeWriteDescriptorSets.data(), 0,
                           nullptr);

    // Create pipeline
    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.layout = m_compute.PipelineLayout.Get();
    computePipelineCreateInfo.stage = LoadShader("particle.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT);

    m_compute.Pipeline.Reset(m_device.Get(), m_pipelineCache.Get(), computePipelineCreateInfo);

    // Separate command pool as queue family for compute may be different than graphics
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = m_vulkanDevice.GetQueueFamilyIndices().Compute;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    m_compute.CommandPool.Reset(m_device.Get(), cmdPoolInfo);

    // Create a command buffer for compute operations
    VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = m_compute.CommandPool.Get();
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    m_compute.CommandBuffer.Reset(m_device.Get(), cmdBufAllocateInfo);

    // Fence for compute CB sync
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    m_compute.Fence.Reset(m_device.Get(), fenceCreateInfo);

    // Build a single command buffer containing the compute dispatch commands
    BuildComputeCommandBuffer();
  }


  void ComputeParticles::BuildComputeCommandBuffer()
  {
    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    m_compute.CommandBuffer.Begin(cmdBufInfo);
    {
      // Compute particle movement

      // Add memory barrier to ensure that the (graphics) vertex shader has fetched attributes before compute starts to write to the buffer
      VkBufferMemoryBarrier bufferBarrier{};
      bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
      bufferBarrier.pNext = nullptr;
      bufferBarrier.buffer = m_compute.StorageBuffer.GetBuffer();
      bufferBarrier.size = m_compute.StorageBuffer.GetDesciptor().range;
      bufferBarrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;    // Vertex shader invocations have finished reading from the buffer
      bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;             // Compute shader wants to write to the buffer
      // Compute and graphics queue may have different queue families (see VulkanDevice::createLogicalDevice)
      // For the barrier to work across different queues, we need to set their family indices
      bufferBarrier.srcQueueFamilyIndex =
        m_vulkanDevice.GetQueueFamilyIndices().Graphics;    // Required as compute and graphics queue may have different families
      bufferBarrier.dstQueueFamilyIndex =
        m_vulkanDevice.GetQueueFamilyIndices().Compute;    // Required as compute and graphics queue may have different families

      vkCmdPipelineBarrier(m_compute.CommandBuffer.Get(), VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                           Willems::Config::FLAGS_NONE, 0, nullptr, 1, &bufferBarrier, 0, nullptr);

      vkCmdBindPipeline(m_compute.CommandBuffer.Get(), VK_PIPELINE_BIND_POINT_COMPUTE, m_compute.Pipeline.Get());
      vkCmdBindDescriptorSets(m_compute.CommandBuffer.Get(), VK_PIPELINE_BIND_POINT_COMPUTE, m_compute.PipelineLayout.Get(), 0, 1,
                              &m_compute.DescriptorSet, 0, nullptr);

      // Dispatch the compute job
      vkCmdDispatch(m_compute.CommandBuffer.Get(), m_optionParser->GetParticleCount() / 16, 1, 1);

      // Add memory barrier to ensure that compute shader has finished writing to the buffer
      // Without this the (rendering) vertex shader may display incomplete results (partial data from last frame)
      bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;             // Compute shader has finished writes to the buffer
      bufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;    // Vertex shader invocations want to read from the buffer
      bufferBarrier.buffer = m_compute.StorageBuffer.GetBuffer();
      bufferBarrier.size = m_compute.StorageBuffer.GetDesciptor().range;
      // Compute and graphics queue may have different queue families (see VulkanDevice::createLogicalDevice)
      // For the barrier to work across different queues, we need to set their family indices
      bufferBarrier.srcQueueFamilyIndex =
        m_vulkanDevice.GetQueueFamilyIndices().Compute;    // Required as compute and graphics queue may have different families
      bufferBarrier.dstQueueFamilyIndex =
        m_vulkanDevice.GetQueueFamilyIndices().Graphics;    // Required as compute and graphics queue may have different families

      vkCmdPipelineBarrier(m_compute.CommandBuffer.Get(), VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
                           Willems::Config::FLAGS_NONE, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
    }
    m_compute.CommandBuffer.End();
  }
}
