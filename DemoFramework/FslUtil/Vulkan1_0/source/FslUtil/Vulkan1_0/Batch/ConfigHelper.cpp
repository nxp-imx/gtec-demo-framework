/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/Batch/ConfigHelper.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>

namespace Fsl::Vulkan::ConfigHelper
{
  void ExtractVertexAttributes(Span<VkVertexInputAttributeDescription> dstAttributes, const VertexDeclarationSpan& vertexDecl)
  {
    if (vertexDecl.size() > dstAttributes.size())
    {
      throw NotSupportedException("vertexDecl contains too many entries");
    }

    for (std::size_t i = 0; i < vertexDecl.size(); ++i)
    {
      auto entry = vertexDecl.At(i);
      dstAttributes[i].location = UncheckedNumericCast<uint32_t>(i);
      dstAttributes[i].binding = 0;
      dstAttributes[i].format = VulkanConvert::ToVkFormat(entry.Format);
      dstAttributes[i].offset = entry.Offset;
    }
  }


  RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutUniform,
                                                   const VkDescriptorSetLayout& descriptorSetLayoutTexture)
  {
    const std::array<VkDescriptorSetLayout, 2> layouts = {descriptorSetLayoutUniform, descriptorSetLayoutTexture};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = UncheckedNumericCast<uint32_t>(layouts.size());
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    return {device, pipelineLayoutInfo};
  }


  RapidVulkan::PipelineLayout CreatePipelineLayoutWithPushConstant(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutUniform,
                                                                   const VkDescriptorSetLayout& descriptorSetLayoutTexture,
                                                                   const uint32_t cbPushConstants)
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = cbPushConstants;

    const std::array<VkDescriptorSetLayout, 2> layouts = {descriptorSetLayoutUniform, descriptorSetLayoutTexture};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = UncheckedNumericCast<uint32_t>(layouts.size());
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    return {device, pipelineLayoutInfo};
  }

  VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState(const BlendState blendState)
  {
    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    switch (blendState)
    {
    case BlendState::Additive:
      blendAttachmentState.blendEnable = VK_TRUE;
      blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendState::AlphaBlend:
      blendAttachmentState.blendEnable = VK_TRUE;
      blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendState::NonPremultiplied:
    case BlendState::Sdf:
      blendAttachmentState.blendEnable = VK_TRUE;
      blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendState::Opaque:
      blendAttachmentState.blendEnable = VK_FALSE;
      blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
      blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
      break;
    default:
      throw NotSupportedException("Unsupported blend state");
    }
    return blendAttachmentState;
  }

  RapidVulkan::GraphicsPipeline CreateGraphicsPipeline(const VkDevice device, const VkShaderModule vertexShader, const VkShaderModule fragmentShader,
                                                       const VkPipelineLayout pipelineLayout, const VkPipelineCache pipelineCache,
                                                       const VkRenderPass renderPass, const uint32_t subpass, const PxExtent2D& screenExtentPx,
                                                       const BlendState blendState, const BasicCullMode cullMode, const BasicFrontFace frontFace,
                                                       const VertexDeclarationSpan& vertexDeclaration)
  {
    assert(device != VK_NULL_HANDLE);
    assert(vertexShader != VK_NULL_HANDLE);
    assert(fragmentShader != VK_NULL_HANDLE);
    assert(pipelineLayout != VK_NULL_HANDLE);
    // assert(pipelineCache != VK_NULL_HANDLE);
    assert(renderPass != VK_NULL_HANDLE);
    std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
    pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[0].flags = 0;
    pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipelineShaderStageCreateInfo[0].module = vertexShader;
    pipelineShaderStageCreateInfo[0].pName = "main";
    pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

    pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[1].flags = 0;
    pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineShaderStageCreateInfo[1].module = fragmentShader;
    pipelineShaderStageCreateInfo[1].pName = "main";
    pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

    VkVertexInputBindingDescription vertexBindings{};
    vertexBindings.binding = 0;
    vertexBindings.stride = vertexDeclaration.VertexStride();
    vertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 16> nativeVertexAttributes{};
    auto nativeVertexAttributeSpan = SpanUtil::AsSpan(nativeVertexAttributes).subspan(0, vertexDeclaration.size());
    ExtractVertexAttributes(nativeVertexAttributeSpan, vertexDeclaration);

    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &vertexBindings;
    vertexInputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(nativeVertexAttributeSpan.size());
    vertexInputState.pVertexAttributeDescriptions = nativeVertexAttributeSpan.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtentPx.Width);
    viewport.height = static_cast<float>(screenExtentPx.Height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{};
    scissor.extent = TypeConverter::UncheckedTo<VkExtent2D>(screenExtentPx);

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = TypeConverter::ChangeTo<VkCullModeFlags>(cullMode);
    rasterizationState.frontFace = TypeConverter::ChangeTo<VkFrontFace>(frontFace);
    rasterizationState.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.front.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    auto blendAttachmentState = CreatePipelineColorBlendAttachmentState(blendState);

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(pipelineShaderStageCreateInfo.size());
    pipelineCreateInfo.pStages = pipelineShaderStageCreateInfo.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = subpass;

    return {device, pipelineCache, pipelineCreateInfo};
  }
}
