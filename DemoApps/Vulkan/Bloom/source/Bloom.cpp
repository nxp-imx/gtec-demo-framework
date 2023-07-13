/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include "Bloom.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include "RenderScene.hpp"
#include "WhiteRectScene.hpp"

namespace Fsl
{
  namespace
  {
    const constexpr float DEFAULT_ZOOM = 10;
    const constexpr float DEFAULT_X_ROTATION = MathHelper::TO_RADS * 20.0f;

    constexpr uint32_t SIZE_MOD = 2;
    constexpr uint32_t SIZE_16 = 16 * SIZE_MOD;
    constexpr uint32_t SIZE_32 = 32 * SIZE_MOD;
    constexpr uint32_t SIZE_64 = 64 * SIZE_MOD;
    constexpr uint32_t SIZE_128 = 128 * SIZE_MOD;
    constexpr uint32_t SIZE_256 = 256 * SIZE_MOD;

    const constexpr uint32_t VERTEX_BUFFER_BIND_ID = 0;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      // Ensure that we can reuse the depth buffer for offscreen rendering even when the 'screen resolution' would be lower
      setup.DepthBufferMinimumExtent = PxExtent2D::Create(SIZE_256, SIZE_256);
      return setup;
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count * 3;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }

    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      // Binding 0 : Fragment shader image sampler
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }

    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUTexture& texture)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};

      // Binding 0 : Fragment shader texture sampler
      auto textureImageInfo0 = texture.GetDescriptorImageInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &textureImageInfo0;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }


    RapidVulkan::DescriptorSetLayout CreateBloomDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 5> setLayoutBindings{};
      // Binding 0-4 : Fragment shader image sampler
      for (uint32_t i = 0; i < setLayoutBindings.size(); ++i)
      {
        setLayoutBindings[i].binding = i;
        setLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        setLayoutBindings[i].descriptorCount = 1;
        setLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      }

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    VkDescriptorSet UpdateBloomDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUTexture& texture256,
                                             const Vulkan::VUTexture& texture128, const Vulkan::VUTexture& texture64,
                                             const Vulkan::VUTexture& texture32, const Vulkan::VUTexture& texture16)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(texture256.IsValid());
      assert(texture128.IsValid());
      assert(texture64.IsValid());
      assert(texture32.IsValid());
      assert(texture16.IsValid());

      std::array<VkDescriptorImageInfo, 5> imageInfo = {texture256.GetDescriptorImageInfo(), texture128.GetDescriptorImageInfo(),
                                                        texture64.GetDescriptorImageInfo(), texture32.GetDescriptorImageInfo(),
                                                        texture16.GetDescriptorImageInfo()};
      std::array<VkWriteDescriptorSet, 5> writeDescriptorSets{};

      // Bindings : Fragment shader texture sampler
      for (uint32_t i = 0; i < writeDescriptorSets.size(); ++i)
      {
        writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[i].dstSet = descriptorSet;
        writeDescriptorSets[i].dstBinding = i;
        writeDescriptorSets[i].descriptorCount = 1;
        writeDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[i].pImageInfo = &imageInfo[i];
      }

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }


    RapidVulkan::PipelineLayout CreatePipelineLayoutWithPushConstants(const RapidVulkan::DescriptorSetLayout& descripterSetLayout,
                                                                      const uint32_t numFloats)
    {
      VkPushConstantRange pushConstantRange{};
      pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      pushConstantRange.offset = 0;
      pushConstantRange.size = sizeof(float) * numFloats;

      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();
      pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
      pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }

    RapidVulkan::RenderPass CreateRenderPass(const VkDevice device, const VkFormat renderFormat,
                                             const VkFormat depthImageFormat = VK_FORMAT_UNDEFINED)
    {
      assert(device != VK_NULL_HANDLE);

      const bool enableDepth = (depthImageFormat != VK_FORMAT_UNDEFINED);

      VkAttachmentReference colorAttachmentReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
      VkAttachmentReference depthAttachmentReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

      std::array<VkSubpassDescription, 1> subpassDescription{};
      // Rendering to a offscreen buffer
      subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescription[0].colorAttachmentCount = 1;
      subpassDescription[0].pColorAttachments = &colorAttachmentReference;
      subpassDescription[0].pDepthStencilAttachment = enableDepth ? &depthAttachmentReference : nullptr;

      std::array<VkSubpassDependency, 2> subpassDependency{};
      // The offscreen buffer
      subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[0].dstSubpass = 0;
      subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      // output
      subpassDependency[1].srcSubpass = 0;
      subpassDependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
      subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      subpassDependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      std::array<VkAttachmentDescription, 2> attachments{};
      // color
      attachments[0].format = renderFormat;
      attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      // Depth
      attachments[1].format = depthImageFormat;
      attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
      attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      const uint32_t attachmentCount = enableDepth ? 2 : 1;

      return {device,
              0,
              attachmentCount,
              attachments.data(),
              UncheckedNumericCast<uint32_t>(subpassDescription.size()),
              subpassDescription.data(),
              UncheckedNumericCast<uint32_t>(subpassDependency.size()),
              subpassDependency.data()};
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const QuadMesh& vertexBufferInfo, const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(vertexBufferInfo.VertexBuffer.IsValid());
      assert(renderPass != VK_NULL_HANDLE);

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};

      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].flags = 0;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].flags = 0;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.flags = 0;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexBufferInfo.BindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBufferInfo.AttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexBufferInfo.AttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.flags = 0;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

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

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.front = {};
      depthStencilStateCreateInfo.back = {};
      depthStencilStateCreateInfo.minDepthBounds = 0.0f;
      depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

      std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT};
      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      pipelineDynamicStateCreateInfo.flags = 0;
      pipelineDynamicStateCreateInfo.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicState.size());
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.flags = 0;
      graphicsPipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
      graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
      graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
      graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
      graphicsPipelineCreateInfo.pTessellationState = nullptr;
      graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
      graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
      graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
      graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }

    RapidVulkan::GraphicsPipeline CreateBloomPipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                      const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                      const QuadMesh& vertexBufferInfo, const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(vertexBufferInfo.VertexBuffer.IsValid());
      assert(renderPass != VK_NULL_HANDLE);

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};

      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].flags = 0;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].flags = 0;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
      pipelineShaderStageCreateInfo[1].pName = "main";
      pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
      pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      pipelineVertexInputCreateInfo.flags = 0;
      pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexBufferInfo.BindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexBufferInfo.AttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexBufferInfo.AttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.flags = 0;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(extent.width);
      viewport.height = static_cast<float>(extent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{{0, 0}, extent};

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
      pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
      pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
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

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
      depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      depthStencilStateCreateInfo.front = {};
      depthStencilStateCreateInfo.back = {};
      depthStencilStateCreateInfo.minDepthBounds = 0.0f;
      depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      graphicsPipelineCreateInfo.flags = 0;
      graphicsPipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
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

  }

  // Bloom as described here
  // The idea is not to create the most accurate bloom, but something that is fairly fast.
  // http://prideout.net/archive/bloom/
  // http://kalogirou.net/2006/05/20/how-to-do-good-bloom-for-hdr-rendering/
  Bloom::Bloom(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_menuUI(config)
    , m_batch(std::dynamic_pointer_cast<Vulkan::NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_camera(config.WindowMetrics.GetSizePx())
    , m_rotationSpeed(0, -0.6f, 0)
    , m_renderUI(true)
    , m_gaussianBlurKernelWeight(m_menuUI.GetKernelWeightMod())
    , m_activeBlueShaderType(BlurShaderType::Gaussian5X5)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetRotation(Matrix::CreateRotationX(DEFAULT_X_ROTATION));
    m_camera.SetZoom(DEFAULT_ZOOM);
    m_storedStartRotation = m_rotation;

    m_resources.BufferManager =
      std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    m_resources.VBFullScreen = VBHelper::BuildVB(m_resources.BufferManager, BoxF(-1, -1, 1, 1), BoxF(0.0f, 0.0f, 1.0f, 1.0f));

    const auto contentManager = GetContentManager();

    m_resources.ShaderPassVert.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Pass.vert.spv"));
    m_resources.ShaderBrightPassFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/BrightPass.frag.spv"));
    m_resources.ShaderCopyFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/CopyPass.frag.spv"));
    m_resources.ShaderBloomPassFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/BloomPass.frag.spv"));

    // Custom pass
    m_resources.ShaderBlurHGaussian5X5FixedFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian5HFixedPass.frag.spv"));
    m_resources.ShaderBlurVGaussian5X5FixedFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian5VFixedPass.frag.spv"));
    m_resources.ShaderBlurHGaussian9X9FixedFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian9HFixedPass.frag.spv"));
    m_resources.ShaderBlurVGaussian9X9FixedFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian9VFixedPass.frag.spv"));
    m_resources.ShaderBlurHCustomFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/BlurHPass.frag.spv"));
    m_resources.ShaderBlurVCustomFrag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/BlurVPass.frag.spv"));
    m_resources.ShaderBlurHGaussian5X5Frag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian5HPass.frag.spv"));
    m_resources.ShaderBlurVGaussian5X5Frag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian5VPass.frag.spv"));
    m_resources.ShaderBlurHGaussian9X9Frag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian9HPass.frag.spv"));
    m_resources.ShaderBlurVGaussian9X9Frag.Reset(m_device.Get(), 0, contentManager->ReadBytes("Shaders/Gaussian9VPass.frag.spv"));

    // Prepare the blur shader
    SetBlurShader(BlurShaderType::Gaussian5X5);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;

    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, (maxFramesInFlight * 2) + 11 + 2);

    FSLLOG3_INFO("Preparing scene0");
    m_sceneRender = std::make_shared<RenderScene>(config, m_menuUI.GetSceneId(), m_device, m_deviceQueue, m_resources.BufferManager,
                                                  m_resources.MainDescriptorPool, maxFramesInFlight);
    FSLLOG3_INFO("Preparing scene1");
    m_sceneWhiteRect = std::make_shared<WhiteRectScene>(config, m_menuUI.GetSceneId(), m_device, m_deviceQueue, m_resources.BufferManager,
                                                        m_resources.MainDescriptorPool, maxFramesInFlight);

    if (m_menuUI.GetSceneId() == 2)
    {
      m_scene = m_sceneWhiteRect;
    }
    else
    {
      m_scene = m_sceneRender;
    }

    m_resources.OffscreenDescriptorSetLayout = CreateDescriptorSetLayout(m_device);

    m_resources.OffscreenDescriptorSetFB256 = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB256A = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB256B = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB128A = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB128B = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB64A = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB64B = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB32A = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB32B = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB16A = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);
    m_resources.OffscreenDescriptorSetFB16B = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.OffscreenDescriptorSetLayout);

    m_resources.OffscreenPipelineLayout = CreatePipelineLayout(m_resources.OffscreenDescriptorSetLayout);
    m_resources.BlurPipelineLayout = CreatePipelineLayoutWithPushConstants(m_resources.OffscreenDescriptorSetLayout, 1u);
    m_resources.BlurPipelineLayout5 = CreatePipelineLayoutWithPushConstants(m_resources.OffscreenDescriptorSetLayout, 3u);
    m_resources.BlurPipelineLayout9 = CreatePipelineLayoutWithPushConstants(m_resources.OffscreenDescriptorSetLayout, 5u);

    m_resources.BloomDescriptorSetLayout = CreateBloomDescriptorSetLayout(m_device);
    m_resources.BloomDescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.BloomDescriptorSetLayout);
    m_resources.BloomPipelineLayout = CreatePipelineLayoutWithPushConstants(m_resources.BloomDescriptorSetLayout, 1u);

    FSLLOG3_INFO("Ready");
  }

  Bloom::~Bloom() = default;

  void Bloom::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::U:
      m_renderUI = !m_renderUI;
      event.Handled();
      break;
    case VirtualKey::Code1:
      m_scene = m_sceneRender;
      event.Handled();
      break;
    case VirtualKey::Code2:
      m_scene = m_sceneWhiteRect;
      event.Handled();
      break;
    case VirtualKey::Code5:
      SetBlurShader(BlurShaderType::Gaussian5X5);
      event.Handled();
      break;
    case VirtualKey::Code6:
      SetBlurShader(BlurShaderType::Gaussian9X9);
      event.Handled();
      break;
    case VirtualKey::Code7:
      SetBlurShader(BlurShaderType::Custom);
      event.Handled();
      break;
    default:
      break;
    }
  }


  void Bloom::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

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
        m_camera.SetRotation(Matrix::CreateRotationX(DEFAULT_X_ROTATION));
        m_camera.SetZoom(DEFAULT_ZOOM);
        m_rotation = m_storedStartRotation;
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void Bloom::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }


    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void Bloom::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    m_camera.AddZoom(static_cast<float>(event.GetDelta()) * -0.001f);
  }

  void Bloom::Update(const DemoTime& demoTime)
  {
    if (m_menuUI.IsRotateEnabled())
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    }

    if (m_scene)
    {
      m_scene->Update(demoTime, m_camera.GetViewMatrix(), m_camera.GetRotationMatrix(), m_rotation, GetWindowSizePx());
    }

    if (m_menuUI.GetKernelWeightMod() != m_gaussianBlurKernelWeight)
    {
      m_gaussianBlurKernelWeight = m_menuUI.GetKernelWeightMod();
      SetBlurShader(m_activeBlueShaderType);
    }
  }


  void Bloom::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t frameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[frameIndex];
    if (m_scene)
    {
      m_scene->PreDraw(frameIndex, hCmdBuffer);
    }

    rCmdBuffers.Begin(frameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      DrawOffscreenRenderpasses(rCmdBuffers, drawContext);

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

      rCmdBuffers.CmdBeginRenderPass(frameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawFinalComposite(frameIndex, hCmdBuffer);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, frameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(frameIndex);
    }
    rCmdBuffers.End(frameIndex);
  }


  VkRenderPass Bloom::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    const auto offscreenRFormat = context.SwapchainImageFormat;
    m_dependentResources.OffscreenRP = CreateRenderPass(m_device.Get(), offscreenRFormat, context.DepthBufferImageFormat);
    m_dependentResources.OffscreenRPNoDepth = CreateRenderPass(m_device.Get(), offscreenRFormat);
    const VkRenderPass offscreenRP = m_dependentResources.OffscreenRP.Get();
    const VkRenderPass offscreenRPNoDepth = m_dependentResources.OffscreenRPNoDepth.Get();
    const VkImageView depthImageView = context.DepthImageView;

    m_dependentResources.OffscreenFB256.Reset(m_device, VkExtent2D{SIZE_256, SIZE_256}, offscreenRFormat, offscreenRP, depthImageView, "FB256");
    m_dependentResources.OffscreenFB256A.Reset(m_device, VkExtent2D{SIZE_256, SIZE_256}, offscreenRFormat, offscreenRPNoDepth, "FB256A");
    m_dependentResources.OffscreenFB256B.Reset(m_device, VkExtent2D{SIZE_256, SIZE_256}, offscreenRFormat, offscreenRPNoDepth, "FB256B");
    m_dependentResources.OffscreenFB128A.Reset(m_device, VkExtent2D{SIZE_128, SIZE_128}, offscreenRFormat, offscreenRPNoDepth, "FB128A");
    m_dependentResources.OffscreenFB128B.Reset(m_device, VkExtent2D{SIZE_128, SIZE_128}, offscreenRFormat, offscreenRPNoDepth, "FB128B");
    m_dependentResources.OffscreenFB64A.Reset(m_device, VkExtent2D{SIZE_64, SIZE_64}, offscreenRFormat, offscreenRPNoDepth, "FB64A");
    m_dependentResources.OffscreenFB64B.Reset(m_device, VkExtent2D{SIZE_64, SIZE_64}, offscreenRFormat, offscreenRPNoDepth, "FB64B");
    m_dependentResources.OffscreenFB32A.Reset(m_device, VkExtent2D{SIZE_32, SIZE_32}, offscreenRFormat, offscreenRPNoDepth, "FB32A");
    m_dependentResources.OffscreenFB32B.Reset(m_device, VkExtent2D{SIZE_32, SIZE_32}, offscreenRFormat, offscreenRPNoDepth, "FB32B");
    m_dependentResources.OffscreenFB16A.Reset(m_device, VkExtent2D{SIZE_16, SIZE_16}, offscreenRFormat, offscreenRPNoDepth, "FB16A");
    m_dependentResources.OffscreenFB16B.Reset(m_device, VkExtent2D{SIZE_16, SIZE_16}, offscreenRFormat, offscreenRPNoDepth, "FB16B");

    m_dependentResources.PipelineBrightPass =
      CreatePipeline(m_resources.OffscreenPipelineLayout, context.SwapchainImageExtent, m_resources.ShaderPassVert.Get(),
                     m_resources.ShaderBrightPassFrag.Get(), m_resources.VBFullScreen, offscreenRPNoDepth, 0);
    m_dependentResources.PipelineCopy =
      CreatePipeline(m_resources.OffscreenPipelineLayout, context.SwapchainImageExtent, m_resources.ShaderPassVert.Get(),
                     m_resources.ShaderCopyFrag.Get(), m_resources.VBFullScreen, offscreenRPNoDepth, 0);

    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB256, m_dependentResources.OffscreenFB256.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB256A, m_dependentResources.OffscreenFB256A.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB256B, m_dependentResources.OffscreenFB256B.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB128A, m_dependentResources.OffscreenFB128A.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB128B, m_dependentResources.OffscreenFB128B.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB64A, m_dependentResources.OffscreenFB64A.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB64B, m_dependentResources.OffscreenFB64B.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB32A, m_dependentResources.OffscreenFB32A.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB32B, m_dependentResources.OffscreenFB32B.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB16A, m_dependentResources.OffscreenFB16A.Texture());
    UpdateDescriptorSet(m_device.Get(), m_resources.OffscreenDescriptorSetFB16B, m_dependentResources.OffscreenFB16B.Texture());

    {
      const auto& blurPipelineLayout = m_resources.BlurPipelineLayout;
      const auto blurExtent = context.SwapchainImageExtent;
      const VkShaderModule hVertShader = m_resources.ShaderPassVert.Get();
      const VkRenderPass hRP = offscreenRPNoDepth;
      const auto& vb = m_resources.VBFullScreen;
      m_dependentResources.PipelineBlurCustomH =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurHCustomFrag.Get(), vb, hRP, 0);
      m_dependentResources.PipelineBlurCustomV =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurVCustomFrag.Get(), vb, hRP, 0);

      m_dependentResources.PipelineBlurFixed5H =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurHGaussian5X5FixedFrag.Get(), vb, hRP, 0);
      m_dependentResources.PipelineBlurFixed5V =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurVGaussian5X5FixedFrag.Get(), vb, hRP, 0);

      m_dependentResources.PipelineBlurFixed9H =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurHGaussian9X9FixedFrag.Get(), vb, hRP, 0);
      m_dependentResources.PipelineBlurFixed9V =
        CreatePipeline(blurPipelineLayout, blurExtent, hVertShader, m_resources.ShaderBlurVGaussian9X9FixedFrag.Get(), vb, hRP, 0);

      const auto& blurPipelineLayout5 = m_resources.BlurPipelineLayout5;
      m_dependentResources.PipelineBlur5H =
        CreatePipeline(blurPipelineLayout5, blurExtent, hVertShader, m_resources.ShaderBlurHGaussian5X5Frag.Get(), vb, hRP, 0);
      m_dependentResources.PipelineBlur5V =
        CreatePipeline(blurPipelineLayout5, blurExtent, hVertShader, m_resources.ShaderBlurVGaussian5X5Frag.Get(), vb, hRP, 0);

      const auto& blurPipelineLayout9 = m_resources.BlurPipelineLayout9;
      m_dependentResources.PipelineBlur9H =
        CreatePipeline(blurPipelineLayout9, blurExtent, hVertShader, m_resources.ShaderBlurHGaussian9X9Frag.Get(), vb, hRP, 0);
      m_dependentResources.PipelineBlur9V =
        CreatePipeline(blurPipelineLayout9, blurExtent, hVertShader, m_resources.ShaderBlurVGaussian9X9Frag.Get(), vb, hRP, 0);
    }


    m_dependentResources.PipelineBloom =
      CreateBloomPipeline(m_resources.BloomPipelineLayout, context.SwapchainImageExtent, m_resources.ShaderPassVert.Get(),
                          m_resources.ShaderBloomPassFrag.Get(), m_resources.VBFullScreen, m_dependentResources.MainRenderPass.Get(), 0);

    UpdateBloomDescriptorSet(m_device.Get(), m_resources.BloomDescriptorSet, m_dependentResources.OffscreenFB256A.Texture(),
                             m_dependentResources.OffscreenFB128A.Texture(), m_dependentResources.OffscreenFB64A.Texture(),
                             m_dependentResources.OffscreenFB32A.Texture(), m_dependentResources.OffscreenFB16A.Texture());


    if (m_sceneRender)
    {
      m_sceneRender->OnBuildResources(context, offscreenRP);
    }
    if (m_sceneWhiteRect)
    {
      m_sceneWhiteRect->OnBuildResources(context, offscreenRP);
    }
    return m_dependentResources.MainRenderPass.Get();
  }


  void Bloom::OnFreeResources()
  {
    if (m_sceneWhiteRect)
    {
      m_sceneWhiteRect->OnFreeResources();
    }
    if (m_sceneRender)
    {
      m_sceneRender->OnFreeResources();
    }
    m_dependentResources = {};
  }

  void Bloom::DrawOffscreenRenderpasses(RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const auto frameIndex = drawContext.CurrentFrameIndex;
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[frameIndex];

    // 1. Render the scene to a low res frame buffer
    {
      const VkExtent2D fbExtent{SIZE_256, SIZE_256};
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.OffscreenRP.Get();
      renderPassBeginInfo.framebuffer = m_dependentResources.OffscreenFB256.GetFramebuffer();
      renderPassBeginInfo.renderArea.offset = {0, 0};
      renderPassBeginInfo.renderArea.extent = fbExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(frameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(fbExtent.width);
        viewport.height = static_cast<float>(fbExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = fbExtent;
        vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

        if (m_scene)
        {
          m_scene->Draw(frameIndex, hCmdBuffer);
        }
      }
      rCmdBuffers.CmdEndRenderPass(frameIndex);
    }

    // 2. Apply bright pass
    const VkPipelineLayout hPipelineLayout = m_resources.OffscreenPipelineLayout.Get();
    const VkRenderPass renderPass = m_dependentResources.OffscreenRPNoDepth.Get();
    if (m_menuUI.IsBrightPassEnabled())
    {
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB256A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB256,
                  m_dependentResources.PipelineBrightPass);
    }
    else
    {
      // To make the code simpler when the brightpass is disabled we just do a copy instead (but this copy really is unnecessary and wasting GPU
      // time)
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB256A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB256,
                  m_dependentResources.PipelineCopy);
    }

    // 3. copy to the smaller blur render targets
    if (m_menuUI.IsScaleInputSequentiallyEnabled())
    {
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB128A, hPipelineLayout,
                  m_resources.OffscreenDescriptorSetFB256A, m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB64A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB128A,
                  m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB32A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB64A,
                  m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB16A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB32A,
                  m_dependentResources.PipelineCopy);
    }
    else
    {
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB128A, hPipelineLayout,
                  m_resources.OffscreenDescriptorSetFB256A, m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB64A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB256A,
                  m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB32A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB256A,
                  m_dependentResources.PipelineCopy);
      PostProcess(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB16A, hPipelineLayout, m_resources.OffscreenDescriptorSetFB256A,
                  m_dependentResources.PipelineCopy);
    }

    if (m_menuUI.IsBlurEnabled())
    {
      // 4. Blur the content using two passes for 256, 128, 64, 32 and 16
      PostProcessBlurH(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB256B, m_resources.OffscreenDescriptorSetFB256A);
      PostProcessBlurV(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB256A, m_resources.OffscreenDescriptorSetFB256B);

      PostProcessBlurH(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB128B, m_resources.OffscreenDescriptorSetFB256A);
      PostProcessBlurV(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB128A, m_resources.OffscreenDescriptorSetFB128B);

      PostProcessBlurH(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB64B, m_resources.OffscreenDescriptorSetFB64A);
      PostProcessBlurV(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB64A, m_resources.OffscreenDescriptorSetFB64B);

      PostProcessBlurH(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB32B, m_resources.OffscreenDescriptorSetFB32A);
      PostProcessBlurV(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB32A, m_resources.OffscreenDescriptorSetFB32B);

      PostProcessBlurH(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB16B, m_resources.OffscreenDescriptorSetFB16A);
      PostProcessBlurV(rCmdBuffers, frameIndex, renderPass, m_dependentResources.OffscreenFB16A, m_resources.OffscreenDescriptorSetFB16B);
    }
  }

  void Bloom::DrawFinalComposite(const uint32_t frameIndex, const VkCommandBuffer hCmdBuffer)
  {
    const auto screenExtent = GetScreenExtent();

    // Composite everything
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(screenExtent.Width.Value);
    viewport.height = static_cast<float>(screenExtent.Height.Value);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VkExtent2D{screenExtent.Width.Value, screenExtent.Height.Value};
    vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

    if (m_menuUI.IsFinalSceneEnabled())
    {
      m_scene->Draw(frameIndex, hCmdBuffer);
    }

    // Draw bloom with a fullscreen additive pass
    if (m_menuUI.IsFinalBloomEnabled())
    {
      const VkPipelineLayout hBloomPipelineLayout = m_resources.BloomPipelineLayout.Get();
      const auto currentLayer = m_menuUI.GetBlendLevel();
      vkCmdPushConstants(hCmdBuffer, hBloomPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &currentLayer);

      vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, hBloomPipelineLayout, 0, 1, &m_resources.BloomDescriptorSet, 0, nullptr);
      vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineBloom.Get());

      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.VBFullScreen.VertexBuffer.GetBufferPointer(), &offsets);
      vkCmdDraw(hCmdBuffer, m_resources.VBFullScreen.VertexBuffer.GetVertexCount(), 1, 0, 0);
    }

    // Draw some debug overlays
    if (m_menuUI.IsShowBuffersEnabled())
    {
      uint32_t dstX = 0;
      m_batch->Begin(BlendState::Opaque);
      m_batch->Draw(m_dependentResources.OffscreenFB256, Vector2(dstX, 0u), Color::White());
      dstX += m_dependentResources.OffscreenFB256.GetExtent2D().width;
      m_batch->Draw(m_dependentResources.OffscreenFB256A, Vector2(dstX, 0u), Color::White());
      dstX += m_dependentResources.OffscreenFB256A.GetExtent2D().width;
      m_batch->Draw(m_dependentResources.OffscreenFB128A, Vector2(dstX, 0u), Color::White());
      dstX += m_dependentResources.OffscreenFB128A.GetExtent2D().width;
      m_batch->Draw(m_dependentResources.OffscreenFB64A, Vector2(dstX, 0u), Color::White());
      dstX += m_dependentResources.OffscreenFB64A.GetExtent2D().width;
      m_batch->Draw(m_dependentResources.OffscreenFB32A, Vector2(dstX, 0u), Color::White());
      dstX += m_dependentResources.OffscreenFB32A.GetExtent2D().width;
      m_batch->Draw(m_dependentResources.OffscreenFB16A, Vector2(dstX, 0u), Color::White());
      // dstX += m_dependentResources.OffscreenFB16A.GetExtent2D().width;
      m_batch->End();
    }

    if (m_renderUI)
    {
      m_menuUI.Draw();
    }
  }

  VkPipelineLayout Bloom::PrepareBlurPipeline(const BlurShaderType shaderType, const VkCommandBuffer hCmdBuffer, const uint32_t texelSize) const
  {
    const float texSize = 1.0f / static_cast<float>(texelSize);

    if (m_menuUI.GetKernelWeightMod() != m_menuUI.GetKernelWeightRange())
    {
      switch (shaderType)
      {
      case BlurShaderType::Gaussian5X5:
        {
          const VkPipelineLayout hPipelineLayout = m_resources.BlurPipelineLayout5.Get();
          std::array<float, 3> pushConstants = {m_gaussian5.Weight0, m_gaussian5.Weight1, m_gaussian5.Offset0 * texSize};
          vkCmdPushConstants(hCmdBuffer, hPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstants), pushConstants.data());
          return hPipelineLayout;
        }
      case BlurShaderType::Gaussian9X9:
        {
          const VkPipelineLayout hPipelineLayout = m_resources.BlurPipelineLayout9.Get();
          std::array<float, 5> pushConstants = {m_gaussian9.Weight0, m_gaussian9.Weight1, m_gaussian9.Weight2, m_gaussian9.Offset0 * texSize,
                                                m_gaussian9.Offset1 * texSize};
          vkCmdPushConstants(hCmdBuffer, hPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstants), pushConstants.data());
          return hPipelineLayout;
        }
      default:
        break;
      }
    }

    {
      const VkPipelineLayout hPipelineLayout = m_resources.BlurPipelineLayout.Get();
      vkCmdPushConstants(hCmdBuffer, hPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &texSize);
      return hPipelineLayout;
    }
  }

  const RapidVulkan::GraphicsPipeline& Bloom::GetBlurHPipeline(const BlurShaderType shaderType) const
  {
    const bool fullPower = m_menuUI.GetKernelWeightMod() == m_menuUI.GetKernelWeightRange();

    switch (shaderType)
    {
    case BlurShaderType::Gaussian5X5:
      return fullPower ? m_dependentResources.PipelineBlurFixed5H : m_dependentResources.PipelineBlur5H;
    case BlurShaderType::Gaussian9X9:
      return fullPower ? m_dependentResources.PipelineBlurFixed9H : m_dependentResources.PipelineBlur9H;
    default:
    case BlurShaderType::Custom:
      return m_dependentResources.PipelineBlurCustomH;
    }
  }

  const RapidVulkan::GraphicsPipeline& Bloom::GetBlurVPipeline(const BlurShaderType shaderType) const
  {
    const bool fullPower = m_menuUI.GetKernelWeightMod() == m_menuUI.GetKernelWeightRange();

    switch (shaderType)
    {
    case BlurShaderType::Gaussian5X5:
      return fullPower ? m_dependentResources.PipelineBlurFixed5V : m_dependentResources.PipelineBlur5V;
    case BlurShaderType::Gaussian9X9:
      return fullPower ? m_dependentResources.PipelineBlurFixed9V : m_dependentResources.PipelineBlur9V;
    default:
    case BlurShaderType::Custom:
      return m_dependentResources.PipelineBlurCustomV;
    }
  }


  void Bloom::PostProcessBlurH(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                               const Vulkan::VUFramebuffer& dst, const VkDescriptorSet& srcDescriptorSet)
  {
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[frameIndex];
    // works since the dst and src size should be the same
    const VkPipelineLayout hPipelineLayout = PrepareBlurPipeline(m_activeBlueShaderType, hCmdBuffer, dst.GetExtent2D().width);
    const auto& pipeline = GetBlurHPipeline(m_activeBlueShaderType);

    PostProcess(rCmdBuffers, frameIndex, renderPass, dst, hPipelineLayout, srcDescriptorSet, pipeline);
  }


  void Bloom::PostProcessBlurV(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                               const Vulkan::VUFramebuffer& dst, const VkDescriptorSet& srcDescriptorSet)
  {
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[frameIndex];
    // works since the dst and src size should be the same
    const VkPipelineLayout hPipelineLayout = PrepareBlurPipeline(m_activeBlueShaderType, hCmdBuffer, dst.GetExtent2D().height);
    const auto& pipeline = GetBlurVPipeline(m_activeBlueShaderType);

    PostProcess(rCmdBuffers, frameIndex, renderPass, dst, hPipelineLayout, srcDescriptorSet, pipeline);
  }


  void Bloom::PostProcess(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t frameIndex, const VkRenderPass renderPass,
                          const Vulkan::VUFramebuffer& dst, const VkPipelineLayout hPipelineLayout, const VkDescriptorSet& srcDescriptorSet,
                          const RapidVulkan::GraphicsPipeline& pipeline)
  {
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[frameIndex];

    // Composite everything
    const auto dstExtent = dst.GetExtent2D();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = dst.GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = dstExtent;
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    rCmdBuffers.CmdBeginRenderPass(frameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(dstExtent.width);
      viewport.height = static_cast<float>(dstExtent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);
      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = dstExtent;
      vkCmdSetScissor(hCmdBuffer, 0, 1, &scissor);

      PostProcess(hCmdBuffer, hPipelineLayout, srcDescriptorSet, pipeline);
    }
    rCmdBuffers.CmdEndRenderPass(frameIndex);
  }


  void Bloom::PostProcess(const VkCommandBuffer hCmdBuffer, const VkPipelineLayout hPipelineLayout, const VkDescriptorSet& srcDescriptorSet,
                          const RapidVulkan::GraphicsPipeline& pipeline)
  {
    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, hPipelineLayout, 0, 1, &srcDescriptorSet, 0, nullptr);
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Get());

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.VBFullScreen.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdDraw(hCmdBuffer, m_resources.VBFullScreen.VertexBuffer.GetVertexCount(), 1, 0, 0);
  }


  void Bloom::SetBlurShader(const BlurShaderType shaderType)
  {
    m_activeBlueShaderType = shaderType;

    const float gaussianBlurKernelWeightMod = m_menuUI.GetKernelWeightMod() / (m_menuUI.GetKernelWeightRange());
    switch (shaderType)
    {
    case BlurShaderType::Gaussian5X5:
      m_gaussian5 = GaussianShaderBuilder::Build5x5(gaussianBlurKernelWeightMod);
      break;
    case BlurShaderType::Gaussian9X9:
      m_gaussian9 = GaussianShaderBuilder::Build9x9(gaussianBlurKernelWeightMod);
      break;
    default:
      break;
    }
  }
}
