/*
* Text overlay class for displaying debug information
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTextOverlay.hpp>
#include <RapidVulkan/Check.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <cassert>
#include <cstring>
#include "stb_font_consolas_24_latin1.inl"

#define STB_FONT_NAME stb_font_consolas_24_latin1

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;
  using namespace Vulkan::ConvertUtil;

  namespace Willems
  {
    namespace
    {
      // Defines for the STB font used
      // STB font files can be found at http://nothings.org/stb/font/
      const auto STB_FONT_WIDTH = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
      const auto STB_FONT_HEIGHT = STB_FONT_consolas_24_latin1_BITMAP_HEIGHT;
      const auto STB_FIRST_CHAR = STB_FONT_consolas_24_latin1_FIRST_CHAR;
      const auto STB_NUM_CHARS = STB_FONT_consolas_24_latin1_NUM_CHARS;

      // Max. number of chars the text overlay buffer can hold
      const uint32_t MAX_CHAR_COUNT = 1024;

      stb_fontchar g_stbFontData[STB_NUM_CHARS];
    }


    //! @brief Default constructor
    //! @param vulkanDevice Pointer to a valid VulkanDevice
    VulkanTextOverlay::VulkanTextOverlay(VulkanDevice* pVulkanDevice, const VkQueue queue, const std::vector<RapidVulkan::Framebuffer>* pFramebuffers,
      const VkFormat colorformat, const VkFormat depthformat, const Extent2D& framebufferExtent,
      const std::vector<VkPipelineShaderStageCreateInfo>& shaderstages)
      : m_pVulkanDevice(pVulkanDevice)
      , m_queue(queue)
      , m_colorFormat(colorformat)
      , m_depthFormat(depthformat)
      , m_framebufferExtent(framebufferExtent)
      , m_pFramebuffers(pFramebuffers)
      , m_shaderStages(shaderstages)
      , m_pMappedLocal(nullptr)
      , m_numLetters(0)
      , Visible(true)
      , Invalidated(false)
      , CmdBuffers()
    {
      assert(m_pVulkanDevice != nullptr);
      assert(pFramebuffers != nullptr);

      PrepareResources();
      PrepareRenderPass();
      PreparePipeline();
    }


    VulkanTextOverlay::~VulkanTextOverlay()
    {
      if (m_vertexBuffer.IsValid())
        m_vertexBuffer.Unmap();
    }


    void VulkanTextOverlay::PrepareResources()
    {
      static unsigned char font24pixels[STB_FONT_HEIGHT][STB_FONT_WIDTH];
      STB_FONT_NAME(g_stbFontData, font24pixels, STB_FONT_HEIGHT);

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
      cmdBufAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_pFramebuffers->size());

      CmdBuffers.Reset(m_pVulkanDevice->GetDevice(), cmdBufAllocateInfo);

      // Vertex buffer
      m_vertexBuffer = m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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
      VulkanBuffer stagingBuffer = m_pVulkanDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        allocInfo.allocationSize);

      {
        stagingBuffer.MapEx();
        // Only one channel, so data size = W * H (*R8)
        std::memcpy(stagingBuffer.GetMappedPointer(), &font24pixels[0][0], STB_FONT_WIDTH * STB_FONT_HEIGHT);
        stagingBuffer.Unmap();
      }

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
          CommandBufferUtil::SetImageLayout(copyCmd.Get(), m_image.Get(), VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

          VkBufferImageCopy bufferCopyRegion{};
          bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          bufferCopyRegion.imageSubresource.mipLevel = 0;
          bufferCopyRegion.imageSubresource.layerCount = 1;
          bufferCopyRegion.imageExtent.width = STB_FONT_WIDTH;
          bufferCopyRegion.imageExtent.height = STB_FONT_HEIGHT;
          bufferCopyRegion.imageExtent.depth = 1;

          vkCmdCopyBufferToImage(copyCmd.Get(), stagingBuffer.GetBuffer(), m_image.Get(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

          // Prepare for shader read
          CommandBufferUtil::SetImageLayout(copyCmd.Get(), m_image.Get(), VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
      imageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
      imageViewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
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
      descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
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
      descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
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
      texDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = m_descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &texDescriptor;

      vkUpdateDescriptorSets(m_pVulkanDevice->GetDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

      // Pipeline cache
      VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
      pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
      m_pipelineCache.Reset(m_pVulkanDevice->GetDevice(), pipelineCacheCreateInfo);

      // Command buffer execution fence
      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      m_fence.Reset(m_pVulkanDevice->GetDevice(), fenceCreateInfo);
    }



    void VulkanTextOverlay::PrepareRenderPass()
    {
      VkAttachmentDescription attachments[2]{};

      // Color attachment
      attachments[0].format = m_colorFormat;
      attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
      // Don't clear the framebuffer (like the renderpass from the example does)
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      // Depth attachment
      attachments[1].format = m_depthFormat;
      attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkAttachmentReference colorReference{};
      colorReference.attachment = 0;
      colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkAttachmentReference depthReference{};
      depthReference.attachment = 1;
      depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkSubpassDependency subpassDependencies[2]{};

      // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commmands executed outside of the actual renderpass)
      subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependencies[0].dstSubpass = 0;
      subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // Transition from initial to final
      subpassDependencies[1].srcSubpass = 0;
      subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      VkSubpassDescription subpassDescription{};
      subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription.flags = 0;
      subpassDescription.inputAttachmentCount = 0;
      subpassDescription.pInputAttachments = nullptr;
      subpassDescription.colorAttachmentCount = 1;
      subpassDescription.pColorAttachments = &colorReference;
      subpassDescription.pResolveAttachments = nullptr;
      subpassDescription.pDepthStencilAttachment = &depthReference;
      subpassDescription.preserveAttachmentCount = 0;
      subpassDescription.pPreserveAttachments = nullptr;

      VkRenderPassCreateInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.pNext = nullptr;
      renderPassInfo.attachmentCount = 2;
      renderPassInfo.pAttachments = attachments;
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subpassDescription;
      renderPassInfo.dependencyCount = 2;
      renderPassInfo.pDependencies = subpassDependencies;

      m_renderPass.Reset(m_pVulkanDevice->GetDevice(), renderPassInfo);
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
      inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
      inputState.pVertexBindingDescriptions = vertexBindings.data();
      inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribs.size());
      inputState.pVertexAttributeDescriptions = vertexAttribs.data();

      VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
      pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineCreateInfo.pNext = nullptr;
      pipelineCreateInfo.flags = 0;
      pipelineCreateInfo.layout = m_pipelineLayout.Get();
      pipelineCreateInfo.renderPass = m_renderPass.Get();
      pipelineCreateInfo.pVertexInputState = &inputState;
      pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
      pipelineCreateInfo.pRasterizationState = &rasterizationState;
      pipelineCreateInfo.pColorBlendState = &colorBlendState;
      pipelineCreateInfo.pMultisampleState = &multisampleState;
      pipelineCreateInfo.pViewportState = &viewportState;
      pipelineCreateInfo.pDepthStencilState = &depthStencilState;
      pipelineCreateInfo.pDynamicState = &dynamicState;
      pipelineCreateInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
      pipelineCreateInfo.pStages = m_shaderStages.data();

      m_pipeline.Reset(m_pVulkanDevice->GetDevice(), m_pipelineCache.Get(), pipelineCreateInfo);
    }


    void VulkanTextOverlay::BeginTextUpdate()
    {
      m_pMappedLocal = static_cast<glm::vec4*>(m_vertexBuffer.GetMappedPointer());
      m_numLetters = 0;
    }


    void VulkanTextOverlay::AddText(const std::string& text, const float x, const float y, const TextAlign align)
    {
      assert(m_pMappedLocal != nullptr);

      const float fbW = static_cast<float>(m_framebufferExtent.Width);
      const float fbH = static_cast<float>(m_framebufferExtent.Height);

      const float charW = 1.5f / fbW;
      const float charH = 1.5f / fbH;

      float posX = (x / fbW * 2.0f) - 1.0f;
      const float posY = (y / fbH * 2.0f) - 1.0f;

      // Calculate text width
      float textWidth = 0;
      for (auto letter : text)
      {
        stb_fontchar *charData = &g_stbFontData[(uint32_t)letter - STB_FIRST_CHAR];
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
        stb_fontchar *charData = &g_stbFontData[(uint32_t)letter - STB_FIRST_CHAR];

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
      UpdateCommandBuffers();
    }


    void VulkanTextOverlay::UpdateCommandBuffers()
    {
      VkCommandBufferBeginInfo cmdBufInfo{};
      cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      cmdBufInfo.pNext = nullptr;

      VkClearValue clearValues[1];
      clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.pNext = nullptr;
      renderPassBeginInfo.renderPass = m_renderPass.Get();
      renderPassBeginInfo.renderArea.extent = Convert(m_framebufferExtent);
      //renderPassBeginInfo.clearValueCount = 1;
      //renderPassBeginInfo.pClearValues = clearValues;

      VkViewport viewport{};
      viewport.width = static_cast<float>(m_framebufferExtent.Width);
      viewport.height = static_cast<float>(m_framebufferExtent.Height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{};
      scissor.offset.x = 0;
      scissor.offset.y = 0;
      scissor.extent = Convert(m_framebufferExtent);

      for (std::size_t i = 0; i < CmdBuffers.Size(); ++i)
      {
        renderPassBeginInfo.framebuffer = (*m_pFramebuffers)[i].Get();

        {
          CmdBuffers.Begin(i, cmdBufInfo);
          //if (vkDebug::DebugMarker::active)
          //{
          //  vkDebug::DebugMarker::beginRegion(cmdBuffers[i], "Text overlay", glm::vec4(1.0f, 0.94f, 0.3f, 1.0f));
          //}

          vkCmdBeginRenderPass(CmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

          vkCmdSetViewport(CmdBuffers[i], 0, 1, &viewport);

          vkCmdSetScissor(CmdBuffers[i], 0, 1, &scissor);

          vkCmdBindPipeline(CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());
          vkCmdBindDescriptorSets(CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);

          VkDeviceSize offsets = 0;
          vkCmdBindVertexBuffers(CmdBuffers[i], 0, 1, m_vertexBuffer.GetBufferPointer(), &offsets);
          vkCmdBindVertexBuffers(CmdBuffers[i], 1, 1, m_vertexBuffer.GetBufferPointer(), &offsets);
          for (uint32_t j = 0; j < m_numLetters; j++)
          {
            vkCmdDraw(CmdBuffers[i], 4, 1, j * 4, 0);
          }

          vkCmdEndRenderPass(CmdBuffers[i]);

          //if (vkDebug::DebugMarker::active)
          //{
          //  vkDebug::DebugMarker::endRegion(cmdBuffers[i]);
          //}

          CmdBuffers.End(i);
        }
      }
    }


    void VulkanTextOverlay::Submit(const VkQueue queue, const uint32_t bufferindex, VkSubmitInfo submitInfo)
    {
      if (!Visible)
      {
        return;
      }

      submitInfo.pCommandBuffers = CmdBuffers.GetPointer(bufferindex);
      submitInfo.commandBufferCount = 1;

      RAPIDVULKAN_CHECK(vkQueueSubmit(m_queue, 1, &submitInfo, m_fence.Get()));

      m_fence.WaitForFence(UINT64_MAX);
      m_fence.ResetFence();
    }

    void VulkanTextOverlay::ReallocateCommandBuffers()
    {
      CmdBuffers.Reset();

      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = m_commandPool.Get();
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_pFramebuffers->size());

      CmdBuffers.Reset(m_pVulkanDevice->GetDevice(), cmdBufAllocateInfo);
    }

  }
}
