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

#include "Vulkan101.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>
#include <array>
//#include <vulkan/vulkan.h>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    const auto VERTEX_SHADER_NAME = "vertex_only_ndc.vert.spv";
    const auto FRAGMENT_SHADER_NAME = "red.frag.spv";

    Vulkan::VUBufferMemory CreateVertexBuffer(const VUDevice& device)
    {
      // Window clip origin is upper left.
      static constexpr const std::array<float, 3 * 4> vertices = {-0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.0f, 1.0f};

      const VkDeviceSize cbVertices = vertices.size() * sizeof(float);
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.flags = 0;
      bufferCreateInfo.size = cbVertices;
      bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      bufferCreateInfo.queueFamilyIndexCount = 0;
      bufferCreateInfo.pQueueFamilyIndices = nullptr;

      Vulkan::VUBufferMemory vertexBuffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      vertexBuffer.Upload(0, vertices.data(), cbVertices);
      return vertexBuffer;
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device)
    {
      return RapidVulkan::PipelineLayout(device, 0, 0, nullptr, 0, nullptr);
    }


    RapidVulkan::RenderPass CreateRenderPass(const VkDevice device, const VkFormat swapchainImageFormat)
    {
      VkAttachmentDescription attachmentDescription{};
      attachmentDescription.flags = 0;
      attachmentDescription.format = swapchainImageFormat;
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

      return RapidVulkan::RenderPass(device, 0, 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency);
    }

    RapidVulkan::GraphicsPipeline CreatePipeline(const VkDevice device, const VkShaderModule vertShaderModule, const VkShaderModule fragShaderModule,
                                                 const VkExtent2D& swapchainImageExtent, const VkPipelineLayout pipelineLayout,
                                                 const VkRenderPass renderPass)
    {
      assert(device != VK_NULL_HANDLE);
      assert(vertShaderModule != VK_NULL_HANDLE);
      assert(fragShaderModule != VK_NULL_HANDLE);
      assert(pipelineLayout != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);

      std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};

      pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      pipelineShaderStageCreateInfo[0].module = vertShaderModule;
      pipelineShaderStageCreateInfo[0].pName = "main";
      pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

      pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      pipelineShaderStageCreateInfo[1].module = fragShaderModule;
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
      viewport.width = static_cast<float>(swapchainImageExtent.width);
      viewport.height = static_cast<float>(swapchainImageExtent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor = {{0, 0}, swapchainImageExtent};

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
      pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      pipelineViewportStateCreateInfo.viewportCount = 1;
      pipelineViewportStateCreateInfo.pViewports = &viewport;
      pipelineViewportStateCreateInfo.scissorCount = 1;
      pipelineViewportStateCreateInfo.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
      pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
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
      graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
      graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout;
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = 0;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return RapidVulkan::GraphicsPipeline(device, VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }
  }


  Vulkan101::Vulkan101(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
  {
    m_resources.m_vertexBuffer = CreateVertexBuffer(m_device);
    CreateShaders();
    m_resources.m_pipelineLayout = CreatePipelineLayout(m_device.Get());
  }


  Vulkan101::~Vulkan101() = default;

  void Vulkan101::Update(const DemoTime& /*demoTime*/)
  {
  }


  void Vulkan101::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    // GetVulkanDrawContext()
    // - CmdBufferIndex
    // - Framebuffer

    // Rebuild the primary cmd buffer
    BuildCmdBuffer(rCmdBuffers, drawContext.CurrentSwapBufferIndex, drawContext.SwapchainImageExtent, drawContext.Framebuffer);
  }


  VkRenderPass Vulkan101::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.m_renderPass = CreateRenderPass(m_device.Get(), context.SwapchainImageFormat);
    m_dependentResources.m_pipeline =
      CreatePipeline(m_device.Get(), m_resources.m_vertexShaderModule.Get(), m_resources.m_fragmentShaderModule.Get(), context.SwapchainImageExtent,
                     m_resources.m_pipelineLayout.Get(), m_dependentResources.m_renderPass.Get());
    return m_dependentResources.m_renderPass.Get();
  }


  void Vulkan101::OnFreeResources() noexcept
  {
    m_dependentResources = {};
  }


  void Vulkan101::CreateShaders()
  {
    const auto contentManager = GetContentManager();
    m_resources.m_vertexShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(VERTEX_SHADER_NAME));
    m_resources.m_fragmentShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes(FRAGMENT_SHADER_NAME));
  }


  void Vulkan101::BuildCmdBuffer(RapidVulkan::CommandBuffers& rCmdBuffers, const uint32_t currentSwapBufferIndex,
                                 const VkExtent2D& swapchainImageExtent, const VkFramebuffer framebuffer)
  {
    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.0f;
      clearColorValue.float32[1] = 0.0f;
      clearColorValue.float32[2] = 1.0f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.m_renderPass.Get();
      renderPassBeginInfo.framebuffer = framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = swapchainImageExtent;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = &clearValues;

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.m_pipeline.Get());

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(hCmdBuffer, 0, 1, m_resources.m_vertexBuffer.GetBufferPointer(), &offsets);
        vkCmdDraw(hCmdBuffer, 3, 1, 0, 0);

        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }
}
