/**
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


// Based on a sample by Norbert Nopper from VKTS Examples (VKTS_Sample02)
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "VulkanTriangle.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfo.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <cstring>

namespace Fsl
{
  namespace
  {
    const std::size_t VKTS_NUMBER_BUFFERS = 2;

    const auto VKTS_VERTEX_SHADER_NAME = "vertex_only_ndc.vert.spv";
    const auto VKTS_FRAGMENT_SHADER_NAME = "red.frag.spv";
  }


  VulkanTriangle::VulkanTriangle(const DemoAppConfig& config)
    : VulkanWindowDemoApp(config)
  {
    FSLLOG3_INFO("VulkanTriangle app creating");

    m_commandPool.Reset(m_device.Get(), 0, m_deviceQueue.QueueFamilyIndex);

    BuildVertexBuffer();
    BuildShader();
    BuildPipelineLayout();
    BuildResources();

    m_imageAcquiredSemaphore.Reset(m_device.Get(), 0);
    m_renderingCompleteSemaphore.Reset(m_device.Get(), 0);

    FSLLOG3_INFO("VulkanTriangle app created");
  }


  VulkanTriangle::~VulkanTriangle()
  {
    try
    {
      // Wait for everything to be idle before we try to free it
      FSLLOG3_INFO("VulkanTriangle app destroying");
    }
    catch (const std::exception& ex)
    {
      // We log and swallow it since destructor's are not allowed to throw
      FSLLOG3_ERROR("Exception during destruction: {}", ex.what());
    }
  }


  void VulkanTriangle::Update(const DemoTime& /*demoTime*/)
  {
  }


  void VulkanTriangle::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    VkResult result = VK_SUCCESS;
    // FIX: hasCurrentExtentChanged missing, is this important?
    // if (surface->hasCurrentExtentChanged(physicalDevice->getPhysicalDevice()))
    //{
    //  const auto& currentExtent = surface->getCurrentExtent(physicalDevice->getPhysicalDevice(), VK_FALSE);
    //  if (currentExtent.width == 0 || currentExtent.height == 0)
    //  {
    //    return VK_TRUE;
    //  }
    //  result = VK_ERROR_OUT_OF_DATE_KHR;
    //}

    ////

    uint32_t currentBuffer = 0;
    if (result == VK_SUCCESS)
    {
      result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), UINT64_MAX, m_imageAcquiredSemaphore.Get(), VK_NULL_HANDLE, &currentBuffer);
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG3_INFO("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }

      throw std::runtime_error("Could not acquire next image.");
    }


    const VkSemaphore waitSemaphores = m_imageAcquiredSemaphore.Get();
    const VkSemaphore signalSemaphores = m_renderingCompleteSemaphore.Get();

    const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphores;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = m_cmdBuffer[currentBuffer].GetPointer();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphores;

    m_deviceQueue.Submit(1, &submitInfo, VK_NULL_HANDLE);

    result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, 1, &signalSemaphores, &currentBuffer, nullptr);
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
    {
      m_deviceQueue.WaitIdle();
    }
    else
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG3_INFO("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }

      throw std::runtime_error("Could not present queue");
    }
  }

  void VulkanTriangle::BuildVertexBuffer()
  {
    // Window clip origin is upper left.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    static const float vertices[3 * 4] = {-0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.0f, 1.0f};

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    m_vertexBuffer.Reset(m_device.Get(), bufferCreateInfo);

    VkMemoryRequirements memoryRequirements = m_vertexBuffer.GetBufferMemoryRequirements();

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;
    const auto memoryTypeIndex = Vulkan::MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits,
                                                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_deviceMemoryVertexBuffer.Reset(m_device.Get(), memoryRequirements.size, memoryTypeIndex);

    void* mappedData = nullptr;

    // TODO: (Improvement) Use a scoped map memory command since it would be exception safe
    RAPIDVULKAN_CHECK(vkMapMemory(m_deviceMemoryVertexBuffer.GetDevice(), m_deviceMemoryVertexBuffer.Get(), 0, sizeof(vertices), 0, &mappedData));
    {
      std::memcpy(mappedData, vertices, sizeof(vertices));

      if ((physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
      {
        VkMappedMemoryRange mappedMemoryRange{};
        mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedMemoryRange.memory = m_deviceMemoryVertexBuffer.Get();
        mappedMemoryRange.offset = 0;
        mappedMemoryRange.size = sizeof(vertices);

        RAPIDVULKAN_CHECK(vkFlushMappedMemoryRanges(m_deviceMemoryVertexBuffer.GetDevice(), 1, &mappedMemoryRange));
      }
    }
    vkUnmapMemory(m_deviceMemoryVertexBuffer.GetDevice(), m_deviceMemoryVertexBuffer.Get());

    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), m_vertexBuffer.Get(), m_deviceMemoryVertexBuffer.Get(), 0));
  }


  void VulkanTriangle::BuildShader()
  {
    const auto vertexShaderBinary = GetContentManager()->ReadBytes(VKTS_VERTEX_SHADER_NAME);
    const auto fragmentShaderBinary = GetContentManager()->ReadBytes(VKTS_FRAGMENT_SHADER_NAME);

    m_vertexShaderModule.Reset(m_device.Get(), 0, vertexShaderBinary.size(), reinterpret_cast<const uint32_t*>(vertexShaderBinary.data()));
    m_fragmentShaderModule.Reset(m_device.Get(), 0, fragmentShaderBinary.size(), reinterpret_cast<const uint32_t*>(fragmentShaderBinary.data()));
  }


  void VulkanTriangle::BuildPipelineLayout()
  {
    m_pipelineLayout.Reset(m_device.Get(), 0, 0, nullptr, 0, nullptr);
  }


  void VulkanTriangle::BuildResources()
  {
    auto fallbackExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());
    m_swapchain = Vulkan::SwapchainKHRUtil::CreateSwapchain(
      m_physicalDevice.Device, m_device.Get(), 0, m_surface, VKTS_NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
      0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_TRUE, m_swapchain.Get(), fallbackExtent, Vulkan::SurfaceFormatInfo());

    uint32_t swapchainImagesCount = m_swapchain.GetImageCount();
    if (swapchainImagesCount == 0)
    {
      throw std::runtime_error("We need at least one image in the swapchain");
    }

    m_swapchainImageView.clear();
    m_framebuffer.clear();
    m_cmdBuffer.clear();
    m_swapchainImageView.resize(swapchainImagesCount);
    m_framebuffer.resize(swapchainImagesCount);
    m_cmdBuffer.resize(swapchainImagesCount);

    BuildRenderPass();
    BuildPipeline();

    for (uint32_t i = 0; i < swapchainImagesCount; ++i)
    {
      BuildSwapchainImageView(i);
      BuildFramebuffer(i);
      BuildCmdBuffer(i);
    }
  }


  void VulkanTriangle::BuildRenderPass()
  {
    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.flags = 0;
    attachmentDescription.format = m_swapchain.GetImageFormat();
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;

    m_renderPass.Reset(m_device.Get(), 0, 1, &attachmentDescription, 1, &subpassDescription, 0, nullptr);
  }


  void VulkanTriangle::BuildPipeline()
  {
    std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};

    pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[0].flags = 0;
    pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipelineShaderStageCreateInfo[0].module = m_vertexShaderModule.Get();
    pipelineShaderStageCreateInfo[0].pName = "main";
    pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

    pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[1].flags = 0;
    pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineShaderStageCreateInfo[1].module = m_fragmentShaderModule.Get();
    pipelineShaderStageCreateInfo[1].pName = "main";
    pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

    VkVertexInputBindingDescription vertexInputBindingDescription{};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = 4 * sizeof(float);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertexInputAttributeDescription{};
    vertexInputAttributeDescription.location = 0;
    vertexInputAttributeDescription.binding = 0;
    vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributeDescription.offset = 0;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
    pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputCreateInfo.flags = 0;
    pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 1;
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = &vertexInputAttributeDescription;

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.GetImageExtent().width);
    viewport.height = static_cast<float>(m_swapchain.GetImageExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = m_swapchain.GetImageExtent();

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.flags = 0;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = &viewport;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
    pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.flags = 0;
    pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
    pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.flags = 0;
    pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
    pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
    pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
    pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
    pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
    pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.flags = 0;
    pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    pipelineColorBlendStateCreateInfo.attachmentCount = 1;
    pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
    pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicState.size());
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.flags = 0;
    graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfo.size());
    graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
    graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = m_pipelineLayout.Get();
    graphicsPipelineCreateInfo.renderPass = m_renderPass.Get();
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = 0;

    m_pipeline.Reset(m_device.Get(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
  }


  void VulkanTriangle::BuildSwapchainImageView(const uint32_t bufferIndex)
  {
    VkComponentMapping componentMapping = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    m_swapchainImageView[bufferIndex].Reset(m_device.Get(), 0, m_swapchain[bufferIndex], VK_IMAGE_VIEW_TYPE_2D, m_swapchain.GetImageFormat(),
                                            componentMapping, imageSubresourceRange);
  }


  void VulkanTriangle::BuildFramebuffer(const uint32_t bufferIndex)
  {
    VkImageView imageView = m_swapchainImageView[bufferIndex].Get();
    m_framebuffer[bufferIndex].Reset(m_device.Get(), 0, m_renderPass.Get(), 1, &imageView, m_swapchain.GetImageExtent().width,
                                     m_swapchain.GetImageExtent().height, 1);
  }


  void VulkanTriangle::BuildCmdBuffer(const uint32_t bufferIndex)
  {
    m_cmdBuffer[bufferIndex].Reset(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    m_cmdBuffer[bufferIndex].Begin(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      m_swapchain.CmdPipelineBarrier(m_cmdBuffer[bufferIndex].Get(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, bufferIndex);

      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.0f, 0.0f, 1.0f, 1.0f}};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_renderPass.Get();
      renderPassBeginInfo.framebuffer = m_framebuffer[bufferIndex].Get();
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = m_swapchain.GetImageExtent();
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      m_cmdBuffer[bufferIndex].CmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        vkCmdBindPipeline(m_cmdBuffer[bufferIndex].Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain.GetImageExtent().width);
        viewport.height = static_cast<float>(m_swapchain.GetImageExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(m_cmdBuffer[bufferIndex].Get(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = m_swapchain.GetImageExtent();

        vkCmdSetScissor(m_cmdBuffer[bufferIndex].Get(), 0, 1, &scissor);

        std::array<VkDeviceSize, 1> offsets = {0};
        VkBuffer vertexBuffer = m_vertexBuffer.Get();
        vkCmdBindVertexBuffers(m_cmdBuffer[bufferIndex].Get(), 0, static_cast<uint32_t>(offsets.size()), &vertexBuffer, offsets.data());
        vkCmdDraw(m_cmdBuffer[bufferIndex].Get(), 3, 1, 0, 0);
      }
      m_cmdBuffer[bufferIndex].CmdEndRenderPass();

      m_swapchain.CmdPipelineBarrier(m_cmdBuffer[bufferIndex].Get(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, bufferIndex);
    }
    m_cmdBuffer[bufferIndex].End();
  }
}
