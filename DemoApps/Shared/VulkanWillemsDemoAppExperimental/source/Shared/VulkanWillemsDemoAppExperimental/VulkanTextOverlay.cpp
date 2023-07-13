/*
 * Text overlay class for displaying debug information
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTextOverlay.hpp>
#include <array>
#include <cassert>
#include <cstring>
#include <utility>
#include "stb_font_consolas_24_latin1.inl"

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STB_FONT_NAME stb_font_consolas_24_latin1

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;

  namespace Willems
  {
    namespace
    {
      // Defines for the STB font used
      // STB font files can be found at http://nothings.org/stb/font/
      constexpr const auto STB_FONT_WIDTH = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
      constexpr const auto STB_FONT_HEIGHT = STB_FONT_consolas_24_latin1_BITMAP_HEIGHT;
      constexpr const auto STB_FIRST_CHAR = STB_FONT_consolas_24_latin1_FIRST_CHAR;
      constexpr const auto STB_NUM_CHARS = STB_FONT_consolas_24_latin1_NUM_CHARS;

      // Max. number of chars the text overlay buffer can hold
      constexpr const uint32_t MAX_CHAR_COUNT = 1024;

      std::array<stb_fontchar, STB_NUM_CHARS> g_stbFontData;
    }


    //! @brief Default constructor
    //! @param vulkanDevice Pointer to a valid VulkanDevice
    VulkanTextOverlay::VulkanTextOverlay(VulkanDevice* pVulkanDevice, const VkQueue queue, const std::vector<RapidVulkan::Framebuffer>* pFramebuffers,
                                         const PxExtent2D& framebufferExtentPx, const VkRenderPass renderPass,
                                         std::vector<VkPipelineShaderStageCreateInfo> shaderstages)
      : m_pVulkanDevice(pVulkanDevice)
      , m_queue(queue)
      , m_framebufferExtent(framebufferExtentPx)
      , m_renderPass(renderPass)
      , m_pFramebuffers(pFramebuffers)
      , m_shaderStages(std::move(shaderstages))
      , m_pMappedLocal(nullptr)
      , m_numLetters(0)
      , Visible(true)
      , Invalidated(false)
      , Dirty(true)
    {
      assert(m_pVulkanDevice != nullptr);
      assert(pFramebuffers != nullptr);

      PrepareResources();
      PreparePipeline();
    }


    VulkanTextOverlay::~VulkanTextOverlay()
    {
      if (m_vertexBuffer.IsValid())
      {
        m_vertexBuffer.Unmap();
      }
    }


    void VulkanTextOverlay::PrepareResources()
    {
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      static unsigned char font24pixels[STB_FONT_HEIGHT][STB_FONT_WIDTH];
      STB_FONT_NAME(g_stbFontData.data(), font24pixels, STB_FONT_HEIGHT);

      // Command buffer

      // Pool
      VkCommandPoolCreateInfo cmdPoolInfo{};
      cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      cmdPoolInfo.queueFamilyIndex = m_pVulkanDevice->GetQueueFamilyIndices().Graphics;
      cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      m_commandPool.Reset(m_pVulkanDevice->GetDevice(), cmdPoolInfo);

      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = UncheckedNumericCast<uint32_t>(m_pFramebuffers->size());

      // Vertex buffer
      m_vertexBuffer =
        m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      MAX_CHAR_COUNT * sizeof(glm::vec4));

      // Map persistent
      m_vertexBuffer.MapEx();

      // Font texture
      VkImageCreateInfo imageInfo{};
      imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageInfo.pNext = nullptr;
      imageInfo.imageType = VK_IMAGE_TYPE_2D;
      imageInfo.format = VK_FORMAT_R8_UNORM;
      imageInfo.extent.width = STB_FONT_WIDTH;
      imageInfo.extent.height = STB_FONT_HEIGHT;
      imageInfo.extent.depth = 1;
      imageInfo.mipLevels = 1;
      imageInfo.arrayLayers = 1;
      imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
      m_image.Reset(m_pVulkanDevice->GetDevice(), imageInfo);

      VkMemoryRequirements memReqs = m_image.GetImageMemoryRequirements();
      VkMemoryAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.pNext = nullptr;
      allocInfo.memoryTypeIndex = m_pVulkanDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      allocInfo.allocationSize = memReqs.size;
      m_imageMemory.Reset(m_pVulkanDevice->GetDevice(), allocInfo);
      RAPIDVULKAN_CHECK(vkBindImageMemory(m_pVulkanDevice->GetDevice(), m_image.Get(), m_imageMemory.Get(), 0));

      // Staging
      VulkanBuffer stagingBuffer = m_pVulkanDevice->CreateBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, allocInfo.allocationSize);

      {
        stagingBuffer.MapEx();
        // Only one channel, so data size = W * H (*R8)
        std::memcpy(stagingBuffer.GetMappedPointer(), &font24pixels[0][0], STB_FONT_WIDTH * STB_FONT_HEIGHT);
        stagingBuffer.Unmap();
      }

      const auto imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      // Copy to image
      {
        cmdBufAllocateInfo.commandBufferCount = 1;
        CommandBuffer copyCmd(m_pVulkanDevice->GetDevice(), cmdBufAllocateInfo);

        {
          VkCommandBufferBeginInfo cmdBufInfo{};
          cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
          cmdBufInfo.pNext = nullptr;
          copyCmd.Begin(cmdBufInfo);

          // Prepare for transfer
          CommandBufferUtil::SetImageLayout(copyCmd.Get(), m_image.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

          VkBufferImageCopy bufferCopyRegion{};
          bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          bufferCopyRegion.imageSubresource.mipLevel = 0;
          bufferCopyRegion.imageSubresource.layerCount = 1;
          bufferCopyRegion.imageExtent.width = STB_FONT_WIDTH;
          bufferCopyRegion.imageExtent.height = STB_FONT_HEIGHT;
          bufferCopyRegion.imageExtent.depth = 1;

          vkCmdCopyBufferToImage(copyCmd.Get(), stagingBuffer.GetBuffer(), m_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

          // Prepare for shader read
          CommandBufferUtil::SetImageLayout(copyCmd.Get(), m_image.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            imageLayout);

          copyCmd.End();
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = copyCmd.GetPointer();

        RAPIDVULKAN_CHECK(vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE));
        RAPIDVULKAN_CHECK(vkQueueWaitIdle(m_queue));

        stagingBuffer.Reset();
      }

      VkImageViewCreateInfo imageViewInfo{};
      imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewInfo.pNext = nullptr;
      imageViewInfo.image = m_image.Get();
      imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      imageViewInfo.format = imageInfo.format;
      imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
      imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      m_imageView.Reset(m_pVulkanDevice->GetDevice(), imageViewInfo);

      // Sampler
      VkSamplerCreateInfo samplerInfo{};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.pNext = nullptr;
      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.mipLodBias = 0.0f;
      samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerInfo.minLod = 0.0f;
      samplerInfo.maxLod = 1.0f;
      samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
      samplerInfo.maxAnisotropy = 1.0f;

      m_sampler.Reset(m_pVulkanDevice->GetDevice(), samplerInfo);

      // Descriptor
      // Font uses a separate descriptor pool
      std::array<VkDescriptorPoolSize, 1> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[0].descriptorCount = 1;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.pNext = nullptr;
      descriptorPoolInfo.maxSets = 1;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      m_descriptorPool.Reset(m_pVulkanDevice->GetDevice(), descriptorPoolInfo);

      // Descriptor set layout
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
      descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorSetLayoutInfo.pNext = nullptr;
      descriptorSetLayoutInfo.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

      m_descriptorSetLayout.Reset(m_pVulkanDevice->GetDevice(), descriptorSetLayoutInfo);

      // Pipeline layout
      VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.pNext = nullptr;
      pipelineLayoutInfo.setLayoutCount = 1;
      pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

      m_pipelineLayout.Reset(m_pVulkanDevice->GetDevice(), pipelineLayoutInfo);

      // Descriptor set
      VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
      descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      descriptorSetAllocInfo.pNext = nullptr;
      descriptorSetAllocInfo.descriptorPool = m_descriptorPool.Get();
      descriptorSetAllocInfo.descriptorSetCount = 1;
      descriptorSetAllocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

      RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_pVulkanDevice->GetDevice(), &descriptorSetAllocInfo, &m_descriptorSet));

      VkDescriptorImageInfo texDescriptor{};
      texDescriptor.sampler = m_sampler.Get();
      texDescriptor.imageView = m_imageView.Get();
      texDescriptor.imageLayout = imageLayout;

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = m_descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &texDescriptor;

      vkUpdateDescriptorSets(m_pVulkanDevice->GetDevice(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0,
                             nullptr);

      // Pipeline cache
      VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
      pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
      m_pipelineCache.Reset(m_pVulkanDevice->GetDevice(), pipelineCacheCreateInfo);
    }


    void VulkanTextOverlay::PreparePipeline()
    {
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
      inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssemblyState.flags = 0;
      inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      inputAssemblyState.primitiveRestartEnable = VK_FALSE;

      VkPipelineRasterizationStateCreateInfo rasterizationState{};
      rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizationState.flags = 0;
      rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
      rasterizationState.depthClampEnable = VK_FALSE;
      rasterizationState.lineWidth = 1.0f;

      // Enable blending
      VkPipelineColorBlendAttachmentState blendAttachmentState{};
      blendAttachmentState.blendEnable = VK_TRUE;
      blendAttachmentState.colorWriteMask = 0xf;
      blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

      VkPipelineColorBlendStateCreateInfo colorBlendState{};
      colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendState.pNext = nullptr;
      colorBlendState.attachmentCount = 1;
      colorBlendState.pAttachments = &blendAttachmentState;

      VkPipelineDepthStencilStateCreateInfo depthStencilState{};
      depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilState.depthTestEnable = VK_FALSE;
      depthStencilState.depthWriteEnable = VK_FALSE;
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

      std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

      VkPipelineDynamicStateCreateInfo dynamicState{};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.flags = 0;
      dynamicState.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicStateEnables.size());
      dynamicState.pDynamicStates = dynamicStateEnables.data();

      std::array<VkVertexInputBindingDescription, 2> vertexBindings{};
      vertexBindings[0].binding = 0;
      vertexBindings[0].stride = sizeof(glm::vec4);
      vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      vertexBindings[1].binding = 1;
      vertexBindings[1].stride = sizeof(glm::vec4);
      vertexBindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


      std::array<VkVertexInputAttributeDescription, 2> vertexAttribs{};
      // Position
      vertexAttribs[0].location = 0;
      vertexAttribs[0].binding = 0;
      vertexAttribs[0].format = VK_FORMAT_R32G32_SFLOAT;
      vertexAttribs[0].offset = 0;
      // UV
      vertexAttribs[1].location = 1;
      vertexAttribs[1].binding = 1;
      vertexAttribs[1].format = VK_FORMAT_R32G32_SFLOAT;
      vertexAttribs[1].offset = sizeof(glm::vec2);

      VkPipelineVertexInputStateCreateInfo inputState{};
      inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      inputState.pNext = nullptr;
      inputState.vertexBindingDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBindings.size());
      inputState.pVertexBindingDescriptions = vertexBindings.data();
      inputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexAttribs.size());
      inputState.pVertexAttributeDescriptions = vertexAttribs.data();

      VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
      pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineCreateInfo.pNext = nullptr;
      pipelineCreateInfo.flags = 0;
      pipelineCreateInfo.layout = m_pipelineLayout.Get();
      pipelineCreateInfo.renderPass = m_renderPass;
      pipelineCreateInfo.pVertexInputState = &inputState;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
      pipelineCreateInfo.pRasterizationState = &rasterizationState;
      pipelineCreateInfo.pColorBlendState = &colorBlendState;
      pipelineCreateInfo.pMultisampleState = &multisampleState;
      pipelineCreateInfo.pViewportState = &viewportState;
      pipelineCreateInfo.pDepthStencilState = &depthStencilState;
      pipelineCreateInfo.pDynamicState = &dynamicState;
      pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(m_shaderStages.size());
      pipelineCreateInfo.pStages = m_shaderStages.data();

      m_pipeline.Reset(m_pVulkanDevice->GetDevice(), m_pipelineCache.Get(), pipelineCreateInfo);
    }


    void VulkanTextOverlay::BeginTextUpdate()
    {
      m_pMappedLocal = static_cast<glm::vec4*>(m_vertexBuffer.GetMappedPointer());
      m_oldNumLetters = m_numLetters;
      m_numLetters = 0;
    }


    void VulkanTextOverlay::AddText(const std::string& text, const float x, const float y, const TextAlign align)
    {
      assert(m_pMappedLocal != nullptr);

      const auto fbW = static_cast<float>(m_framebufferExtent.Width.Value);
      const auto fbH = static_cast<float>(m_framebufferExtent.Height.Value);

      const float charW = 1.5f / fbW;
      const float charH = 1.5f / fbH;

      float posX = (x / fbW * 2.0f) - 1.0f;
      const float posY = (y / fbH * 2.0f) - 1.0f;

      // Calculate text width
      float textWidth = 0;
      for (auto letter : text)
      {
        stb_fontchar* charData = &g_stbFontData[static_cast<uint32_t>(letter) - STB_FIRST_CHAR];
        textWidth += charData->advance * charW;
      }

      switch (align)
      {
      case TextAlign::Right:
        posX -= textWidth;
        break;
      case TextAlign::Center:
        posX -= textWidth / 2.0f;
        break;
      case TextAlign::Left:
        break;
      }

      // Generate a uv mapped quad per char in the new text
      for (auto letter : text)
      {
        stb_fontchar* charData = &g_stbFontData[static_cast<uint32_t>(letter) - STB_FIRST_CHAR];

        m_pMappedLocal->x = (posX + static_cast<float>(charData->x0) * charW);
        m_pMappedLocal->y = (posY + static_cast<float>(charData->y0) * charH);
        m_pMappedLocal->z = charData->s0;
        m_pMappedLocal->w = charData->t0;
        ++m_pMappedLocal;

        m_pMappedLocal->x = (posX + static_cast<float>(charData->x1) * charW);
        m_pMappedLocal->y = (posY + static_cast<float>(charData->y0) * charH);
        m_pMappedLocal->z = charData->s1;
        m_pMappedLocal->w = charData->t0;
        ++m_pMappedLocal;

        m_pMappedLocal->x = (posX + static_cast<float>(charData->x0) * charW);
        m_pMappedLocal->y = (posY + static_cast<float>(charData->y1) * charH);
        m_pMappedLocal->z = charData->s0;
        m_pMappedLocal->w = charData->t1;
        ++m_pMappedLocal;

        m_pMappedLocal->x = (posX + static_cast<float>(charData->x1) * charW);
        m_pMappedLocal->y = (posY + static_cast<float>(charData->y1) * charH);
        m_pMappedLocal->z = charData->s1;
        m_pMappedLocal->w = charData->t1;
        ++m_pMappedLocal;

        posX += charData->advance * charW;

        ++m_numLetters;
      }
    }

    void VulkanTextOverlay::EndTextUpdate()
    {
      // Check for dirty and ensure we dont clear a dirty flag
      if (Visible && m_oldNumLetters != m_numLetters)
      {
        Dirty = true;
      }
    }


    void VulkanTextOverlay::AddToCommandBuffer(const VkCommandBuffer commandBuffer)
    {
      Dirty = false;
      VkViewport viewport{};
      viewport.width = static_cast<float>(m_framebufferExtent.Width.Value);
      viewport.height = static_cast<float>(m_framebufferExtent.Height.Value);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{};
      scissor.offset.x = 0;
      scissor.offset.y = 0;
      scissor.extent = TypeConverter::UncheckedTo<VkExtent2D>(m_framebufferExtent);

      // if (vkDebug::DebugMarker::active)
      //{
      //  vkDebug::DebugMarker::beginRegion(commandBuffer, "Text overlay", glm::vec4(1.0f, 0.94f, 0.3f, 1.0f));
      //}

      vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);

      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, m_vertexBuffer.GetBufferPointer(), &offsets);
      vkCmdBindVertexBuffers(commandBuffer, 1, 1, m_vertexBuffer.GetBufferPointer(), &offsets);
      for (uint32_t j = 0; j < m_numLetters; j++)
      {
        vkCmdDraw(commandBuffer, 4, 1, j * 4, 0);
      }

      // if (vkDebug::DebugMarker::active)
      //{
      //  vkDebug::DebugMarker::endRegion(commandBuffer);
      //}
    }
  }
}
