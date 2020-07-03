/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslUtil/Vulkan1_0/Managed/Draw/VMLineDraw.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <array>
#include <cstdlib>
#include <limits>

namespace Fsl
{
  namespace Vulkan
  {
    constexpr uint32_t VERTICES_PER_LINE = 2u;
    constexpr uint32_t VERTEX_BUFFER_BIND_ID = 0;

    namespace
    {
      RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VUDevice& device)
      {
        std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
        // Binding 0 : Vertex shader uniform buffer
        setLayoutBindings[0].binding = 0;
        setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        setLayoutBindings[0].descriptorCount = 1;
        setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorLayout{};
        descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorLayout.pBindings = setLayoutBindings.data();

        return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
      }

      RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
      {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = count;

        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.maxSets = count;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();

        return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
      }

      Vulkan::VUBufferMemory CreateUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
      {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        Vulkan::VUBufferMemory buffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        // Keep the buffer mapped as we update it each frame
        buffer.Map();
        return buffer;
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

        VkDescriptorSet descriptorSet{};
        RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets",
                                __FILE__, __LINE__);

        return descriptorSet;
      }

      VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer)

      {
        assert(descriptorSet != VK_NULL_HANDLE);
        assert(vertUboBuffer.IsValid());

        std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
        // Binding 0 : Vertex shader uniform buffer
        auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].dstSet = descriptorSet;
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        return descriptorSet;
      }

      RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
      {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

        return RapidVulkan::PipelineLayout(descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo);
      }

      RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                   const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                   const std::array<VkVertexInputAttributeDescription, 2> lineVertexAttributeDescription,
                                                   const VkVertexInputBindingDescription lineVertexInputBindingDescription,
                                                   const VkRenderPass renderPass, const uint32_t subpass, const bool dynamicScissor)
      {
        assert(pipelineLayout.IsValid());
        assert(vertexShaderModule != VK_NULL_HANDLE);
        assert(fragmentShaderModule != VK_NULL_HANDLE);
        assert(renderPass != VK_NULL_HANDLE);
        assert(!lineVertexAttributeDescription.empty());

        std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{};
        pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
        pipelineShaderStageCreateInfo[0].pName = "main";
        pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

        pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
        pipelineShaderStageCreateInfo[1].pName = "main";
        pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

        VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};
        pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
        pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &lineVertexInputBindingDescription;
        pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(lineVertexAttributeDescription.size());
        pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = lineVertexAttributeDescription.data();

        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
        pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{{0, 0}, extent};

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
        pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        pipelineColorBlendStateCreateInfo.attachmentCount = 1;
        pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
        pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

        std::array<VkDynamicState, 1> dynamicState = {VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicScissor ? static_cast<uint32_t>(dynamicState.size()) : 0;
        pipelineDynamicStateCreateInfo.pDynamicStates = dynamicScissor ? dynamicState.data() : nullptr;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.front = {};
        depthStencilStateCreateInfo.back = {};
        depthStencilStateCreateInfo.minDepthBounds = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfo.size());
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

        return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
      }
    }

    VMLineDraw::VMLineDraw(const VUDevice& device, const std::shared_ptr<VMBufferManager>& bufferManager, const uint32_t maxFrames,
                           const std::size_t sizeOfVertexUBOData, const uint32_t initialLineCapacity)
    {
      Reset(device, bufferManager, maxFrames, sizeOfVertexUBOData, initialLineCapacity);
    }

    void VMLineDraw::Reset() noexcept
    {
      // Force free resources just in case
      FreeResources();
      m_resources = {};
    }

    void VMLineDraw::Reset(const VUDevice& device, const std::shared_ptr<VMBufferManager>& bufferManager, const uint32_t maxFrames,
                           const std::size_t sizeOfVertexUBOData, const uint32_t initialLineCapacity)
    {
      if (!device.IsValid())
      {
        throw std::invalid_argument("device must be valid");
      }
      if (!bufferManager)
      {
        throw std::invalid_argument("bufferManager can not be null");
      }
      if (IsValid())
      {
        Reset();
      }

      try
      {
        m_resources.IsValid = true;
        m_resources.BufferManager = bufferManager;
        m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(device);
        m_resources.MainDescriptorPool = CreateDescriptorPool(device, maxFrames);
        m_resources.MainFrameResources.resize(maxFrames);
        for (auto& rFrame : m_resources.MainFrameResources)
        {
          rFrame.VertUboBuffer = CreateUBO(device, sizeOfVertexUBOData);
          rFrame.DescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
          // Prepare a dynamic vertex buffer that can hold LINE_CAPACITY lines
          rFrame.LineVertBuffer.Reset(bufferManager, VERTICES_PER_LINE * initialLineCapacity, VertexPositionColor::GetVertexDeclaration());
          UpdateDescriptorSet(device.Get(), rFrame.DescriptorSet, rFrame.VertUboBuffer);
        }
        m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Exception: {}", ex.what());
        Reset();
        throw ex;
      }
    }

    void VMLineDraw::BuildResources(const VkExtent2D& extent, const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                    const VkRenderPass renderPass, const uint32_t subpass, const bool dynamicScissor)
    {
      if (!IsValid())
      {
        throw UsageErrorException("Can only build resources in a ValidObject");
      }
      if (vertexShaderModule == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("vertexShaderModule can not be invalid");
      }
      if (fragmentShaderModule == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("fragmentShaderModule can not be invalid");
      }
      if (renderPass == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("renderPass can not be invalid");
      }

      if (m_dependentResources.IsValid)
      {
        FSLLOG3_WARNING("Resources were already allocated, freeing them, this could indicate that you forgot to free the resources earlier")
        FreeResources();
      }
      m_dependentResources.IsValid = true;

      std::array<VkVertexInputAttributeDescription, 2> lineVertexAttributeDescription{};

      const std::array<VertexElementUsage, 2> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Color};
      Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(lineVertexAttributeDescription, shaderBindOrder,
                                                                      m_resources.MainFrameResources[0].LineVertBuffer);

      VkVertexInputBindingDescription lineVertexInputBindingDescription{};
      lineVertexInputBindingDescription.binding = 0;
      lineVertexInputBindingDescription.stride = m_resources.MainFrameResources[0].LineVertBuffer.GetElementStride();
      lineVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      m_dependentResources.PipelineRender =
        CreatePipeline(m_resources.MainPipelineLayout, extent, vertexShaderModule, fragmentShaderModule, lineVertexAttributeDescription,
                       lineVertexInputBindingDescription, renderPass, subpass, dynamicScissor);
    }

    void VMLineDraw::FreeResources() noexcept
    {
      if (!m_dependentResources.IsValid)
      {
        return;
      }
      m_dependentResources = {};
    }


    void VMLineDraw::UpdateVertexUBO(const void* pVertexUBOData, const std::size_t& sizeOfVertexUBOData, const uint32_t frameIndex)
    {
      if (frameIndex >= m_resources.MainFrameResources.size())
      {
        throw std::invalid_argument("frameIndex out of bounds");
      }

      auto& rFrame = m_resources.MainFrameResources[frameIndex];
      rFrame.VertUboBuffer.Upload(0u, pVertexUBOData, sizeOfVertexUBOData);
    }


    void VMLineDraw::Draw(const VkCommandBuffer commandBuffer, const VertexPositionColor* pVertices, const std::size_t vertexCount,
                          const uint32_t frameIndex)
    {
      if (!m_dependentResources.IsValid)
      {
        FSLLOG3_DEBUG_WARNING("Draw ignored, due to state");
        return;
      }

      if (commandBuffer == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("commandBuffer can not be null");
      }
      if (pVertices == nullptr)
      {
        throw std::invalid_argument("pVertices can not be null");
      }
      if ((vertexCount % VERTICES_PER_LINE) != 0u)
      {
        throw std::invalid_argument("vertexCount did not match a exact number of lines");
      }
      if (frameIndex >= m_resources.MainFrameResources.size())
      {
        throw std::invalid_argument("frameIndex out of bounds");
      }

      auto& rFrame = m_resources.MainFrameResources[frameIndex];

      if (vertexCount > rFrame.LineVertBuffer.GetVertexCount())
      {
        std::size_t newCapacity = vertexCount;
        // Warn the developer in debug mode
        FSLLOG3_DEBUG_WARNING("Performance-issue: Resizing LineVertexBuffer from: {} to {}", rFrame.LineVertBuffer.GetVertexCount(), newCapacity);
        // Dump the existing buffer and then create a new one
        rFrame.LineVertBuffer.Reset();
        rFrame.LineVertBuffer.Reset(m_resources.BufferManager, newCapacity, VertexPositionColor::GetVertexDeclaration());
      }


      {    // Update the vertex buffer data
        assert(vertexCount <= rFrame.LineVertBuffer.GetVertexCount());
        m_resources.MainFrameResources[frameIndex].LineVertBuffer.SetData(pVertices, vertexCount, sizeof(VertexPositionColor));
      }

      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &rFrame.DescriptorSet, 0,
                              nullptr);


      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PipelineRender.Get());

      std::array<VkDeviceSize, 1> offsets = {0};
      vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, static_cast<uint32_t>(offsets.size()), rFrame.LineVertBuffer.GetBufferPointer(),
                             offsets.data());

      assert(vertexCount <= std::numeric_limits<uint32_t>::max());
      vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexCount), 1, 0, 0);
    }
  }
}
