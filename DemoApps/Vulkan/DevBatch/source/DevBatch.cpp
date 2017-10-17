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
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
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

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    const std::size_t NUMBER_DYNAMIC_STATES = 2;

    const std::size_t NUMBER_BUFFERS = 2;

    const std::size_t SHADER_STAGE_COUNT = 2;

    const auto VERTEX_SHADER_NAME = "vertex_only_ndc.vert.spv";
    const auto FRAGMENT_SHADER_NAME = "red.frag.spv";
  }


  DevBatch::DevBatch(const DemoAppConfig& config)
    : VulkanWindowDemoApp(config)
  {
    m_commandPool.Reset(m_device.Get(), 0, m_deviceQueue.QueueFamilyIndex);

    BuildVertexBuffer();
    BuildShader();
    BuildPipelineLayout();
    BuildResources();

    m_imageAcquiredSemaphore.Reset(m_device.Get(), 0);
    m_renderingCompleteSemaphore.Reset(m_device.Get(), 0);

    const auto vertexShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.vert.spv");
    const auto fragmentShaderBinary = GetContentManager()->ReadBytes("QuadBatch/QuadBatch.frag.spv");

    m_test.reset(new QuadBatch(m_device, m_swapchain.GetImageFormat(), m_deviceQueue.QueueFamilyIndex,
                               vertexShaderBinary, fragmentShaderBinary, 100));

    m_batch2DQuad.reset(new QuadBatch(m_device, m_swapchain.GetImageFormat(), m_deviceQueue.QueueFamilyIndex,
                                      vertexShaderBinary, fragmentShaderBinary, GenericBatch2D_DEFAULT_CAPACITY));
    m_batch2D.reset(new Batch2D(m_batch2DQuad, GetScreenResolution()));


    auto texture = GetContentManager()->ReadTexture("Texturing.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture2 = GetContentManager()->ReadTexture("Icons/boardgamewizard.png", PixelFormat::R8G8B8A8_UNORM);
    auto texture3 = GetContentManager()->ReadBitmap("Icons/hero_bard.png", PixelFormat::R8G8B8A8_UNORM);


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
      samplerCreateInfo.maxLod = (useStaging) ? (float)texture.GetLevels() : 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      samplerCreateInfo.maxAnisotropy = 1.0f;

      m_texture = imageCreator.CreateTexture(texture, samplerCreateInfo);
      m_texture2 = imageCreator.CreateTexture(texture2, samplerCreateInfo);
      m_texture3 = imageCreator.CreateTexture(texture3, samplerCreateInfo);
    }
  }


  DevBatch::~DevBatch()
  {
    try
    {
      m_device.DeviceWaitIdle();
    }
    catch (const std::exception& ex)
    {
      // We log and swallow it since destructors are not allowed to throw
      FSLLOG_ERROR("DeviceWaitIdle, threw exception: " << ex.what());
    }
  }


  void DevBatch::Update(const DemoTime& demoTime)
  {
  }


  void DevBatch::Draw(const DemoTime& demoTime)
  {
    VkResult result = VK_SUCCESS;
    ////

    uint32_t currentBuffer;
    if( result == VK_SUCCESS)
      result = vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), UINT64_MAX, m_imageAcquiredSemaphore.Get(), VK_NULL_HANDLE, &currentBuffer);
    if (!(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR))
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        // TODO: support 'soft restart'
        FSLLOG("Restaring app due to VK_ERROR_OUT_OF_DATE_KHR");
        GetDemoAppControl()->RequestAppRestart();
        return;
      }
      else
        throw std::runtime_error("Could not acquire next image.");
    }
    else
    {
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

        m_test->BeginFrame(m_framebuffer[currentBuffer].Get(), GetScreenExtent());
        m_test->Begin(GetScreenResolution(), BlendState::Opaque, true);
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
        m_test->EndFrame(m_deviceQueue.Queue, &signalSemaphores, &waitDstStageMask, 1);
      }

      VkSemaphore testSignalSemaphore = m_test->GetSignalSemaphore();

      m_batch2DQuad->BeginFrame(m_framebuffer[currentBuffer].Get(), GetScreenExtent());
      m_batch2D->Begin();
      m_batch2D->Draw(m_texture, Rect(0, 0, 200, 200), Color::White());
      m_batch2D->Draw(m_texture, Rect(600, 600, 500, 500), Color::Olive());
      m_batch2D->End();
      m_batch2DQuad->EndFrame(m_deviceQueue.Queue, &testSignalSemaphore, &waitDstStageMask, 1);

      VkSemaphore finalSignalSemaphore = m_batch2DQuad->GetSignalSemaphore();
      result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, 1, &finalSignalSemaphore, &currentBuffer, nullptr);
      if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
      {
        m_deviceQueue.WaitIdle();
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
        else
          throw std::runtime_error("Could not present queue");
      }
    }
  }


  void DevBatch::BuildVertexBuffer()
  {
    // Window clip origin is upper left.
    static const float vertices[3 * 4] = { -0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.0f, 1.0f };

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
    const auto memoryTypeIndex = MemoryTypeUtil::GetMemoryTypeIndex(VK_MAX_MEMORY_TYPES, physicalDeviceMemoryProperties.memoryTypes, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_deviceMemoryVertexBuffer.Reset(m_device.Get(), memoryRequirements.size, memoryTypeIndex);

    void* mappedData;

    // TODO: (Improvement) Use a scoped map memory command since it would be exception safe
    RAPIDVULKAN_CHECK(vkMapMemory(m_deviceMemoryVertexBuffer.GetDevice(), m_deviceMemoryVertexBuffer.Get(), 0, sizeof(vertices), 0, &mappedData));
    {
      std::memcpy(mappedData, vertices, sizeof(vertices));

      if (!(physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
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
    m_swapchain = SwapchainKHRUtil::CreateSwapchain(m_physicalDevice.Device, m_device.Get(), 0, m_surface, NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_TRUE, m_swapchain.Get());

    uint32_t swapchainImagesCount = m_swapchain.GetImageCount();
    if (swapchainImagesCount == 0)
      throw std::runtime_error("We need at least one image in the swapchain");

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
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference{};
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

    VkDynamicState dynamicState[NUMBER_DYNAMIC_STATES] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = NUMBER_DYNAMIC_STATES;
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
    VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    m_swapchainImageView[bufferIndex].Reset(m_device.Get(), 0, m_swapchain[bufferIndex], VK_IMAGE_VIEW_TYPE_2D, m_swapchain.GetImageFormat(), componentMapping, imageSubresourceRange);
  }


  void DevBatch::BuildFramebuffer(const uint32_t bufferIndex)
  {
    auto imageView = m_swapchainImageView[bufferIndex].Get();
    m_framebuffer[bufferIndex].Reset(m_device.Get(), 0, m_renderPass.Get(), 1, &imageView, m_swapchain.GetImageExtent().width, m_swapchain.GetImageExtent().height, 1);
  }


  void DevBatch::BuildCmdBuffer(const uint32_t bufferIndex)
  {
    m_cmdBuffer[bufferIndex].Reset(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    m_cmdBuffer[bufferIndex].Begin(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      m_swapchain.CmdPipelineBarrier(m_cmdBuffer[bufferIndex].Get(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, bufferIndex);

      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.0f;
      clearColorValue.float32[1] = 0.0f;
      clearColorValue.float32[2] = 1.0f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues[1] = { clearColorValue };

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_renderPass.Get();
      renderPassBeginInfo.framebuffer = m_framebuffer[bufferIndex].Get();
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = m_swapchain.GetImageExtent();
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = clearValues;

      m_cmdBuffer[bufferIndex].CmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        vkCmdBindPipeline(m_cmdBuffer[bufferIndex].Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_swapchain.GetImageExtent().width;
        viewport.height = (float)m_swapchain.GetImageExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(m_cmdBuffer[bufferIndex].Get(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = m_swapchain.GetImageExtent();

        vkCmdSetScissor(m_cmdBuffer[bufferIndex].Get(), 0, 1, &scissor);

        VkDeviceSize offsets[1] = { 0 };
        VkBuffer vertexBuffer = m_vertexBuffer.Get();
        vkCmdBindVertexBuffers(m_cmdBuffer[bufferIndex].Get(), 0, 1, &vertexBuffer, offsets);
        vkCmdDraw(m_cmdBuffer[bufferIndex].Get(), 3, 1, 0, 0);
      }
      m_cmdBuffer[bufferIndex].CmdEndRenderPass();

      m_swapchain.CmdPipelineBarrier(m_cmdBuffer[bufferIndex].Get(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, bufferIndex);
    }
    m_cmdBuffer[bufferIndex].End();
  }
}
