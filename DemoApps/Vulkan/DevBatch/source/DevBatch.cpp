/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include "DevBatch.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <Shared/VulkanWindowExperimental/VulkanWindowSystem.hpp>
#include <Shared/VulkanWindowExperimental/VulkanWindowSystemHelper.hpp>
#include <Shared/VulkanWindowExperimental/OptionParser.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <cstring>
#include <limits>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    const auto DEFAULT_TIMEOUT = std::numeric_limits<uint64_t>::max();

    // The maximum number of frames currently being rendered
    const std::size_t MAX_FRAMES_IN_FLIGHT = 2;

    // The number of dynamic state entries in the pipeline
    const std::size_t PIPELINE_DYNAMIC_STATE_COUNT = 2;

    // The desired minimum image count for the swap buffers
    const std::size_t MIN_SWAP_BUFFER_COUNT = 2;

    const std::size_t SHADER_STAGE_COUNT = 2;

    const auto VERTEX_SHADER_NAME = "vertex_only_ndc.vert.spv";
    const auto FRAGMENT_SHADER_NAME = "red.frag.spv";

    const float FORCED_TEX_WIDTH = 200.0f;
    const float FORCED_TEX_HEIGHT = 100.0f;
  }


  DevBatch::DevBatch(const DemoAppConfig& config)
    : VulkanWindowDemoApp(config)
  {
    m_commandPool.Reset(m_device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_deviceQueue.QueueFamilyIndex);

    BuildVertexBuffer();
    BuildShader();
    BuildPipelineLayout();
    BuildResources();

    CreateFrameSyncObjects();

    const auto vertexShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.vert.spv");
    const auto fragmentShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.frag.spv");

    m_test.reset(new QuadBatch(m_device, m_renderPass.Get(), m_swapchain.GetImageCount(), vertexShaderBinary, fragmentShaderBinary, 100));

    m_batch2DQuad.reset(new QuadBatch(m_device, m_renderPass.Get(), m_swapchain.GetImageCount(), vertexShaderBinary, fragmentShaderBinary,
                                      GenericBatch2D_DEFAULT_CAPACITY));
    m_batch2D.reset(new Batch2D(m_batch2DQuad, GetScreenResolution()));


    auto texture = GetContentManager()->ReadTexture("Texturing.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture2 = GetContentManager()->ReadTexture("Icons/boardgamewizard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture3 = GetContentManager()->ReadBitmap("Icons/hero_bard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4 = GetContentManager()->ReadBitmap("Knight/Idle1.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture4Pre = GetContentManager()->ReadBitmap("Knight/Idle1_pre.png", PixelFormat::R8G8B8A8_UNORM);


    {
      VulkanImageCreator imageCreator(m_device, m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

      bool useStaging = true;
      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      // Set max level-of- detail to mip level count of the texture
      samplerCreateInfo.maxLod = (useStaging) ? static_cast<float>(texture.GetLevels()) : 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      samplerCreateInfo.maxAnisotropy = 1.0f;

      m_texture = imageCreator.CreateTexture(texture, samplerCreateInfo);
      m_texture2 = imageCreator.CreateTexture(texture2, samplerCreateInfo);
      m_texture3 = imageCreator.CreateTexture(texture3, samplerCreateInfo);
      m_texture4 = imageCreator.CreateTexture(texture4, samplerCreateInfo);
      m_texture4Pre = imageCreator.CreateTexture(texture4Pre, samplerCreateInfo);
    }
  }


  DevBatch::~DevBatch()
  {
    SafeWaitForDeviceIdle();
  }


  void DevBatch::Update(const DemoTime& demoTime)
  {
    auto res = GetScreenResolution();

    m_currentPos2 = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * FORCED_TEX_WIDTH);
    m_currentPos3 = ((-std::cos(m_currentAngle) + 1.0f) / 2.0f) * (4 * FORCED_TEX_WIDTH);

    m_currentPos.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (res.X - FORCED_TEX_WIDTH);
    m_currentPos.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (res.Y - FORCED_TEX_HEIGHT);

    m_currentPos4.X = ((std::cos(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - FORCED_TEX_WIDTH);
    m_currentPos4.Y = ((std::sin(m_currentAngle) + 1.0f) / 2.0f) * (400.0f - FORCED_TEX_HEIGHT);

    m_currentAngle += m_currentSpeed * demoTime.DeltaTime;
    // Clamp the angle so we dont run into precession issues over time
    if (m_currentAngle >= MathHelper::RADS360)
    {
      m_currentAngle -= MathHelper::RADS360;
    }
  }


  void DevBatch::Draw(const DemoTime& demoTime)
  {
    const auto waitSemaphore = m_frames[m_currentFrame].ImageAcquiredSemaphore.Get();
    const auto signalSemaphore = m_frames[m_currentFrame].RenderingCompleteSemaphore.Get();
    const auto inFlightFence = m_frames[m_currentFrame].InFlightFence.Get();

    uint32_t currentBufferIndex;
    auto result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), DEFAULT_TIMEOUT, waitSemaphore, VK_NULL_HANDLE, &currentBufferIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }

      throw std::runtime_error("Could not acquire next image.");
    }

    // time to synchronize by waiting for the fence before we modify command queues etc.
    vkWaitForFences(m_device.Get(), 1, &inFlightFence, VK_TRUE, DEFAULT_TIMEOUT);
    vkResetFences(m_device.Get(), 1, &inFlightFence);


    // Rebuild the primary cmd buffer
    BuildCmdBuffer(m_cmdBuffers, currentBufferIndex);


    const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = m_cmdBuffers.GetPointer(currentBufferIndex);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    m_deviceQueue.Submit(1, &submitInfo, inFlightFence);

    result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, 1, &signalSemaphore, &currentBufferIndex, nullptr);
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
    {
      // This stalls the GPU, so its not optimal
      // m_deviceQueue.WaitIdle();
    }
    else
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }

      throw std::runtime_error("Could not present queue");
    }

    // Move to next frame
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }


  void DevBatch::CreateFrameSyncObjects()
  {
    m_frames.resize(MAX_FRAMES_IN_FLIGHT);

    for (auto& rFrame : m_frames)
    {
      rFrame.ImageAcquiredSemaphore.Reset(m_device.Get(), 0);
      rFrame.RenderingCompleteSemaphore.Reset(m_device.Get(), 0);
      rFrame.InFlightFence.Reset(m_device.Get(), VK_FENCE_CREATE_SIGNALED_BIT);
    }
  }


  void DevBatch::BuildVertexBuffer()
  {
    // Window clip origin is upper left.
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

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.GetPhysicalDeviceMemoryProperties();
    const auto memoryTypeIndex = MemoryTypeUtil::GetMemoryTypeIndex(VK_MAX_MEMORY_TYPES, physicalDeviceMemoryProperties.memoryTypes,
                                                                    memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_deviceMemoryVertexBuffer.Reset(m_device.Get(), memoryRequirements.size, memoryTypeIndex);

    void* mappedData;

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


  void DevBatch::BuildShader()
  {
    const auto contentManager = GetContentManager();
    const auto vertexShaderBinary = contentManager->ReadBytes(VERTEX_SHADER_NAME);
    const auto fragmentShaderBinary = contentManager->ReadBytes(FRAGMENT_SHADER_NAME);

    m_vertexShaderModule.Reset(m_device.Get(), 0, vertexShaderBinary.size(), reinterpret_cast<const uint32_t*>(vertexShaderBinary.data()));
    m_fragmentShaderModule.Reset(m_device.Get(), 0, fragmentShaderBinary.size(), reinterpret_cast<const uint32_t*>(fragmentShaderBinary.data()));
  }


  void DevBatch::BuildPipelineLayout()
  {
    m_pipelineLayout.Reset(m_device.Get(), 0, 0, nullptr, 0, nullptr);
  }


  void DevBatch::BuildResources()
  {
    m_swapchain = SwapchainKHRUtil::CreateSwapchain(m_physicalDevice.Device, m_device.Get(), 0, m_surface, MIN_SWAP_BUFFER_COUNT, 1,
                                                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr,
                                                    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_TRUE, m_swapchain.Get());

    uint32_t swapchainImagesCount = m_swapchain.GetImageCount();
    if (swapchainImagesCount == 0)
    {
      throw std::runtime_error("We need at least one image in the swapchain");
    }

    m_swapchainImageView.clear();
    m_framebuffer.clear();
    m_swapchainImageView.resize(swapchainImagesCount);
    m_framebuffer.resize(swapchainImagesCount);

    m_cmdBuffers.Reset(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapchainImagesCount);

    BuildRenderPass();
    BuildPipeline();

    for (uint32_t i = 0; i < swapchainImagesCount; ++i)
    {
      BuildSwapchainImageView(i);
      BuildFramebuffer(i);
    }
  }


  void DevBatch::BuildRenderPass()
  {
    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.flags = 0;
    attachmentDescription.format = m_swapchain.GetImageFormat();
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

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

    m_renderPass.Reset(m_device.Get(), 0, 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency);
  }


  void DevBatch::BuildPipeline()
  {
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[SHADER_STAGE_COUNT]{};
    static_assert(SHADER_STAGE_COUNT == 2, "We expect 2 stages");

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

    VkDynamicState dynamicState[PIPELINE_DYNAMIC_STATE_COUNT] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = PIPELINE_DYNAMIC_STATE_COUNT;
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.flags = 0;
    graphicsPipelineCreateInfo.stageCount = SHADER_STAGE_COUNT;
    graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
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


  void DevBatch::BuildSwapchainImageView(const uint32_t bufferIndex)
  {
    VkComponentMapping componentMapping = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    m_swapchainImageView[bufferIndex].Reset(m_device.Get(), 0, m_swapchain[bufferIndex], VK_IMAGE_VIEW_TYPE_2D, m_swapchain.GetImageFormat(),
                                            componentMapping, imageSubresourceRange);
  }


  void DevBatch::BuildFramebuffer(const uint32_t bufferIndex)
  {
    auto imageView = m_swapchainImageView[bufferIndex].Get();
    m_framebuffer[bufferIndex].Reset(m_device.Get(), 0, m_renderPass.Get(), 1, &imageView, m_swapchain.GetImageExtent().width,
                                     m_swapchain.GetImageExtent().height, 1);
  }


  void DevBatch::BuildCmdBuffer(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t bufferIndex)
  {
    auto hCmdBuffer = rCmdBuffers.Get(bufferIndex);
    rCmdBuffers.Begin(bufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.0f;
      clearColorValue.float32[1] = 0.0f;
      clearColorValue.float32[2] = 1.0f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues[1] = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_renderPass.Get();
      renderPassBeginInfo.framebuffer = m_framebuffer[bufferIndex].Get();
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = m_swapchain.GetImageExtent();
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = clearValues;

      rCmdBuffers.CmdBeginRenderPass(bufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain.GetImageExtent().width);
        viewport.height = static_cast<float>(m_swapchain.GetImageExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = m_swapchain.GetImageExtent();

        vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

        VkDeviceSize offsets[1] = {0};
        VkBuffer vertexBuffer = m_vertexBuffer.Get();
        vkCmdBindVertexBuffers(hCmdBuffer, 0, 1, &vertexBuffer, offsets);
        vkCmdDraw(hCmdBuffer, 3, 1, 0, 0);

        // Draw the quads
        DrawQuads(bufferIndex, hCmdBuffer);
      }
      rCmdBuffers.CmdEndRenderPass(bufferIndex);

      m_swapchain.CmdPipelineBarrier(hCmdBuffer, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, bufferIndex);
    }
    rCmdBuffers.End(bufferIndex);
  }

  void DevBatch::DrawQuads(const uint32_t bufferIndex, const VkCommandBuffer commandBuffer)
  {
    const auto screenRes = GetScreenResolution();
    {
      auto col = Color::White().ToVector4();

      VertexPositionColorTexture testQuad[8]{};
      testQuad[0].Color = col;
      testQuad[1].Color = col;
      testQuad[2].Color = col;
      testQuad[3].Color = col;
      testQuad[4].Color = col;
      testQuad[5].Color = col;
      testQuad[6].Color = col;
      testQuad[7].Color = col;

      const float v0 = 0.0f;
      const float v1 = 1.0f;

      testQuad[0].TextureCoordinate = Vector2(0.0f, v0);
      testQuad[1].TextureCoordinate = Vector2(1.0f, v0);
      testQuad[2].TextureCoordinate = Vector2(0.0f, v1);
      testQuad[3].TextureCoordinate = Vector2(1.0f, v1);
      testQuad[4].TextureCoordinate = Vector2(0.0f, v0);
      testQuad[5].TextureCoordinate = Vector2(1.0f, v0);
      testQuad[6].TextureCoordinate = Vector2(0.0f, v1);
      testQuad[7].TextureCoordinate = Vector2(1.0f, v1);

      float z = 0;
      float x = 200;
      float y = 200;

      testQuad[0].Position = Vector3(0, 0, z);
      testQuad[1].Position = Vector3(100, 0, z);
      testQuad[2].Position = Vector3(0, 100, z);
      testQuad[3].Position = Vector3(100, 100, z);

      m_test->BeginFrame(bufferIndex, commandBuffer, GetScreenExtent());
      m_test->Begin(screenRes, BlendState::Opaque, true);
      m_test->DrawQuads(testQuad, 1, m_texture);

      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      m_test->DrawQuads(testQuad, 1, m_texture);

      x = 100;
      y = 100;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      m_test->DrawQuads(testQuad, 1, m_texture2);

      x = 0;
      y = 200;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      m_test->DrawQuads(testQuad, 1, m_texture3);

      x = 400;
      y = 400;
      testQuad[0].Position = Vector3(x + 0, y + 0, z);
      testQuad[1].Position = Vector3(x + 100, y + 0, z);
      testQuad[2].Position = Vector3(x + 0, y + 100, z);
      testQuad[3].Position = Vector3(x + 100, y + 100, z);
      x = 200;
      y = 400;
      testQuad[4].Position = Vector3(x + 0, y + 0, z);
      testQuad[5].Position = Vector3(x + 100, y + 0, z);
      testQuad[6].Position = Vector3(x + 0, y + 100, z);
      testQuad[7].Position = Vector3(x + 100, y + 100, z);
      m_test->DrawQuads(testQuad, 2, m_texture3);

      m_test->End();
      m_test->EndFrame();
    }

    const auto colMovingPre = Color(0.7f, 0.7f, 0.7f, 0.7f);

    const auto col1 = Color::Red();
    const auto col2 = Color::Green();
    const auto col3 = Color::Blue();
    const auto col4 = Color::Olive();
    const auto col5 = Color::White();

    m_batch2DQuad->BeginFrame(bufferIndex, commandBuffer, GetScreenExtent());

    m_batch2D->Begin(BlendState::NonPremultiplied);
    // Bottom left
    m_batch2D->Draw(m_texture4, Rect(0 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col1);
    m_batch2D->Draw(m_texture4, Rect(1 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col2);
    m_batch2D->Draw(m_texture4, Rect(2 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col3);
    m_batch2D->Draw(m_texture4, Rect(3 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col4);
    m_batch2D->Draw(m_texture4, Rect(4 * FORCED_TEX_WIDTH, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col5);
    m_batch2D->Draw(m_texture4Pre, Rect(m_currentPos2, screenRes.Y - FORCED_TEX_HEIGHT, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    m_batch2D->End();


    m_batch2D->Begin(BlendState::AlphaBlend);
    // Bottom left
    m_batch2D->Draw(m_texture4Pre, Rect(0 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col1);
    m_batch2D->Draw(m_texture4Pre, Rect(1 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col2);
    m_batch2D->Draw(m_texture4Pre, Rect(2 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col3);
    m_batch2D->Draw(m_texture4Pre, Rect(3 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col4);
    m_batch2D->Draw(m_texture4Pre, Rect(4 * FORCED_TEX_WIDTH, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), col5);

    m_batch2D->Draw(m_texture4Pre, Rect(m_currentPos.X, m_currentPos.Y, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    m_batch2D->Draw(m_texture4Pre, Rect(m_currentPos2, screenRes.Y - (2 * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    m_batch2D->Draw(m_texture4Pre, Rect(m_currentPos4.X, m_currentPos4.Y, FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), Color::White());
    m_batch2D->Draw(m_texture4Pre, Rect(screenRes.X - 200.0f, 50.0f, 200.0f, 200.0f), Color::White());
    // m_batch2D->Draw(m_texture4, m_currentPos, Color(1.0f, 1.0f, 1.0f, 0.70f));
    m_batch2D->End();

    m_batch2D->Begin(BlendState::Additive);
    m_batch2D->Draw(m_texture4Pre, Rect(m_currentPos3, screenRes.Y - (1.5f * FORCED_TEX_HEIGHT), FORCED_TEX_WIDTH, FORCED_TEX_HEIGHT), colMovingPre);
    m_batch2D->End();

    m_batch2DQuad->EndFrame();
  }
}
