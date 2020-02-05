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

#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslBase/Bits/AlignmentUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics/Vertices/VertexElementFormat.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VUBufferMemoryUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Debug/Strings/VkResult.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 2;
      const uint32_t QUAD_MIN_BATCH_SIZE = 2048;
      const int32_t INTERNAL_QUAD_VERTEX_COUNT = 6;


      VUBufferMemory CreateBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDeviceSize bufferByteSize,
                                  const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags)
      {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = bufferByteSize;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        return VUBufferMemory(physicalDevice, device, bufferCreateInfo, memoryPropertyFlags);
      }


      VUBufferMemory CreateUniformBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice& device)
      {
        const auto bufferByteSize = AlignmentUtil::GetByteSize(sizeof(Matrix), 16);
        return CreateBuffer(physicalDevice, device, bufferByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      }


      DescriptorSetLayout CreateDescriptorSetLayout(const VkDevice device, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags)
      {
        // Descriptor set layout
        VkDescriptorSetLayoutBinding setLayoutBindings{};
        setLayoutBindings.binding = 0;
        setLayoutBindings.descriptorType = descriptorType;
        setLayoutBindings.descriptorCount = 1;
        setLayoutBindings.stageFlags = stageFlags;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.pNext = nullptr;
        descriptorSetLayoutInfo.bindingCount = 1;
        descriptorSetLayoutInfo.pBindings = &setLayoutBindings;

        return DescriptorSetLayout(device, descriptorSetLayoutInfo);
      }


      DescriptorPool CreateDescriptorPool(const VkDevice device, const VkDescriptorType type)
      {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = type;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.maxSets = 1;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;

        return DescriptorPool(device, descriptorPoolCreateInfo);
      }


      VkDescriptorSet CreateDescriptorSet(const VkDevice device, const DescriptorPool& descriptorPool, const DescriptorSetLayout& descriptorSetLayout)
      {
        VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = descriptorPool.Get();
        descriptorSetAllocInfo.descriptorSetCount = 1;
        descriptorSetAllocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

        VkDescriptorSet descriptorSet{};
        RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocInfo, &descriptorSet));
        return descriptorSet;
      }


      PipelineLayout CreatePipelineLayout(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutUniform,
                                          const VkDescriptorSetLayout& descriptorSetLayoutTexture)
      {
        VkDescriptorSetLayout layouts[] = {descriptorSetLayoutUniform, descriptorSetLayoutTexture};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 2;
        pipelineLayoutInfo.pSetLayouts = layouts;
        return PipelineLayout(device, pipelineLayoutInfo);
      }


      PipelineCache CreatePipelineCache(const VkDevice device)
      {
        // Pipeline cache
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        return PipelineCache(device, pipelineCacheCreateInfo);
      }


      std::array<VkVertexInputAttributeDescription, 3> GetVertexAttributes(const VertexDeclaration& vertexDecl)
      {
        assert(vertexDecl.Count() == 3);

        std::array<VkVertexInputAttributeDescription, 3> vertexAttributes{};

        for (std::size_t i = 0; i < vertexAttributes.size(); ++i)
        {
          auto entry = vertexDecl.At(i);
          vertexAttributes[i].location = static_cast<uint32_t>(i);
          vertexAttributes[i].binding = 0;
          vertexAttributes[i].format = ConvertUtil::Convert(entry.Format);
          vertexAttributes[i].offset = entry.Offset;
        }
        return vertexAttributes;
      }

      VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState(const BlendState blendState)
      {
        VkPipelineColorBlendAttachmentState blendAttachmentState{};
        blendAttachmentState.colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

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

      GraphicsPipeline CreateGraphicsPipeline(const VkDevice device, const ShaderModule& vertexShader, const ShaderModule& fragmentShader,
                                              const PipelineLayout& pipelineLayout, const PipelineCache& pipelineCache, const VkRenderPass renderPass,
                                              const uint32_t subpass, const Extent2D& screenExtent, const BlendState blendState)
      {
        assert(device != VK_NULL_HANDLE);
        assert(vertexShader.IsValid());
        assert(fragmentShader.IsValid());
        assert(pipelineLayout.IsValid());
        assert(pipelineCache.IsValid());
        assert(renderPass != VK_NULL_HANDLE);
        VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2]{};
        pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfo[0].flags = 0;
        pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        pipelineShaderStageCreateInfo[0].module = vertexShader.Get();
        pipelineShaderStageCreateInfo[0].pName = "main";
        pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

        pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineShaderStageCreateInfo[1].flags = 0;
        pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pipelineShaderStageCreateInfo[1].module = fragmentShader.Get();
        pipelineShaderStageCreateInfo[1].pName = "main";
        pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

        auto vertexDecl = VertexPositionColorTexture::GetVertexDeclaration();

        VkVertexInputBindingDescription vertexBindings{};
        vertexBindings.binding = 0;
        vertexBindings.stride = vertexDecl.VertexStride();
        vertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        const auto vertexAttributes = GetVertexAttributes(vertexDecl);

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = &vertexBindings;
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
        vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkViewport viewport{};
        viewport.width = static_cast<float>(screenExtent.Width);
        viewport.height = static_cast<float>(screenExtent.Height);
        VkRect2D scissor{};
        scissor.extent = ConvertUtil::Convert(screenExtent);

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
        pipelineCreateInfo.pVertexInputState = &vertexInputState;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.layout = pipelineLayout.Get();
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = subpass;

        return GraphicsPipeline(device, pipelineCache.Get(), pipelineCreateInfo);
      }


      void UpdateDescriptorSetUniform(const VkDevice device, const VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo& bufferInfo)
      {
        VkWriteDescriptorSet writeDescriptorSet{};
        // Binding 0: Vertex shader uniform buffer
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
      }

      void UpdateDescriptorSetTexture(const VkDevice device, const VkDescriptorSet descriptorSet, const VkDescriptorImageInfo& imageInfo)
      {
        VkWriteDescriptorSet writeDescriptorSet{};
        // Binding 0: Fragment shader texture sampler
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
      }
    }


    QuadBatch::QuadBatch(const std::vector<uint8_t>& vertexShaderBinary, const std::vector<uint8_t>& fragmentShaderBinary,
                         const uint32_t quadCapacityHint, const bool logEnabled)
      : m_vertexShaderBinary(vertexShaderBinary)
      , m_fragmentShaderBinary(fragmentShaderBinary)
      , m_quadCapacity(std::max(quadCapacityHint, QUAD_MIN_BATCH_SIZE))
      , m_vertexCapacity(INTERNAL_QUAD_VERTEX_COUNT * m_quadCapacity)
      , m_logEnabled(logEnabled)
    {
    }


    QuadBatch::~QuadBatch()
    {
      DestroyDeviceResources();
    }


    void QuadBatch::BeginFrame(const VkCommandBuffer commandBuffer, const uint32_t commandBufferIndex)
    {
      // First we check that 'activeFrame' state is correct (to ensure that the 'Usage' is correct)
      if (m_activeFrame.CurrentState != FrameState::NotReady)
      {
        throw UsageErrorException("Can not call BeginFrame while inside a BeginFrame/EndFrame block");
      }

      // Then we check the 'resource' state to determine how to proceed
      if (!m_dependentResource.IsValid)
      {
        // We just log a warning about the QuadBatch operating in 'null' mode.
        // This means that all draw operations do nothing except log warnings if called.
        FSLLOG3_WARNING_IF(m_logEnabled && commandBuffer == VK_NULL_HANDLE, "Resources not ready, void mode enabled.");
        FSLLOG3_WARNING_IF(m_logEnabled && commandBuffer != VK_NULL_HANDLE,
                           "Resources not ready, void mode enabled. Supplied command buffer in null mode ought to be VK_NULL_HANDLE");
        m_activeFrame.CurrentState = FrameState::DrawVoidFrame;
        return;
      }

      // Finally we validate the input parameters
      if (commandBuffer == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("commandBuffer can not be invalid");
      }
      if (commandBufferIndex > m_dependentResource.CommandBufferCount)
      {
        throw std::invalid_argument("commandBufferIndex should be less than commandBufferCount");
      }


      assert(m_deviceResource.IsValid);

      // Prepare the device render resources for the given command index for a new frame
      assert(m_deviceResource.Render[commandBufferIndex].IsValid());
      m_deviceResource.Render[commandBufferIndex].Clear();

      m_activeFrame.CurrentState = FrameState::DrawFrame;
      m_activeFrame.CommandBuffer = commandBuffer;
      m_activeFrame.CommandBufferIndex = commandBufferIndex;
    }


    void QuadBatch::Begin(const Point2& screenResolution, const BlendState blendState, const bool restoreState)
    {
      // First we ensure that we that the 'activeFrame' state is correct
      if (m_activeFrame.CurrentState == FrameState::NotReady)
      {
        throw UsageErrorException("Call BeginFrame before Begin");
      }
      if (m_activeFrame.Block.IsValid)
      {
        throw UsageErrorException("Calling Begin inside a Begin 'scope' is not allowed");
      }

      // Then we check the input parameters
      if ((static_cast<uint32_t>(screenResolution.X) != m_deviceResource.CachedScreenExtent.Width ||
           static_cast<uint32_t>(screenResolution.Y) != m_deviceResource.CachedScreenExtent.Height) &&
          m_activeFrame.CurrentState != FrameState::DrawVoidFrame)
      {
        throw NotSupportedException("Dynamic changes of the screen resolution is not supported");
      }

      // Finally we proceed with the actual 'Begin' operation
      assert(m_activeFrame.CommandBuffer != VK_NULL_HANDLE || m_activeFrame.CurrentState == FrameState::DrawVoidFrame);
      assert(!m_activeFrame.Block.IsValid);
      VkPipeline selectedPipeline = VK_NULL_HANDLE;
      switch (blendState)
      {
      case BlendState::Additive:
        selectedPipeline = m_dependentResource.PipelineAdditive.Get();
        break;
      case BlendState::AlphaBlend:
        selectedPipeline = m_dependentResource.PipelineAlphaBlend.Get();
        break;
      case BlendState::NonPremultiplied:
        selectedPipeline = m_dependentResource.PipelineNonPremultiplied.Get();
        break;
      case BlendState::Opaque:
        selectedPipeline = m_dependentResource.PipelineOpaque.Get();
        break;
      default:
        throw NotSupportedException("Unsupported blend state");
      }
      assert(selectedPipeline != VK_NULL_HANDLE || m_activeFrame.CurrentState == FrameState::DrawVoidFrame);
      m_activeFrame.Block.IsValid = true;
      m_activeFrame.Block.ActivePipeline = selectedPipeline;
    }

    // Basic quad vertex format
    // 0-1
    // | |
    // 2-3
    void QuadBatch::DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const VUTextureInfo& textureInfo)
    {
      if (!m_activeFrame.Block.IsValid)
      {
        throw UsageErrorException("Call Begin before DrawQuads");
      }
      assert(m_activeFrame.CurrentState != FrameState::NotReady);
      if (m_activeFrame.CurrentState == FrameState::DrawVoidFrame)
      {
        // In void draw mode we just ignore draw calls
        return;
      }

      auto& rRender = m_deviceResource.Render[m_activeFrame.CommandBufferIndex];

      const VertexPositionColorTexture* pSrcVertices = pVertices;
      const VertexPositionColorTexture* const pSrcVerticesEnd = pVertices + (length * 4);

      uint32_t remainingQuads = length;
      while (pSrcVertices < pSrcVerticesEnd)
      {
        auto current = rRender.VertexBuffers.NextFree(remainingQuads * INTERNAL_QUAD_VERTEX_COUNT);

        auto pDst = current.pMapped;
        auto pDstEnd = current.pMapped + current.VertexCapacity;

        static_assert(INTERNAL_QUAD_VERTEX_COUNT == 6, "the code below expects the internal quad vertex count to be six");
        while (pDst < pDstEnd)
        {
          // Expand the quad to two counter clockwise triangles
          pDst[0] = pSrcVertices[2];
          pDst[1] = pSrcVertices[3];
          pDst[2] = pSrcVertices[0];

          pDst[3] = pSrcVertices[0];
          pDst[4] = pSrcVertices[3];
          pDst[5] = pSrcVertices[1];

          pSrcVertices += 4;
          pDst += INTERNAL_QUAD_VERTEX_COUNT;
          --remainingQuads;
        }

        assert(m_activeFrame.CommandBuffer != VK_NULL_HANDLE);
        assert(m_activeFrame.Block.ActivePipeline != VK_NULL_HANDLE);

        auto currentPipeline = m_activeFrame.Block.ActivePipeline;
        if (currentPipeline != m_activeFrame.Block.CachedPipeline)
        {
          m_activeFrame.Block.CachedPipeline = currentPipeline;
          vkCmdBindPipeline(m_activeFrame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline);
        }

        if (textureInfo != m_activeFrame.Block.CachedTextureInfo)
        {
          m_activeFrame.Block.CachedTextureInfo = textureInfo;
          auto textureDescriptorSet = rRender.TextureDescriptorSets.NextFree();

          assert(textureDescriptorSet != VK_NULL_HANDLE);

          UpdateDescriptorSetTexture(m_deviceResource.UniformBuffer.GetDevice(), textureDescriptorSet, textureInfo.ImageInfo);

          VkDescriptorSet descriptorSets[2] = {m_deviceResource.DescriptorSetUniform, textureDescriptorSet};
          vkCmdBindDescriptorSets(m_activeFrame.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_deviceResource.PipelineLayout.Get(), 0, 2,
                                  descriptorSets, 0, nullptr);
        }

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(m_activeFrame.CommandBuffer, 0, 1, &current.VertexBuffer, &offsets);
        vkCmdDraw(m_activeFrame.CommandBuffer, current.VertexCapacity, 1, current.UsedStartIndex, 0);
      }
    }


    void QuadBatch::End()
    {
      if (!m_activeFrame.Block.IsValid)
      {
        throw UsageErrorException("Call Begin before End");
      }
      assert(m_activeFrame.CurrentState != FrameState::NotReady);

      m_activeFrame.Block = {};
      assert(m_activeFrame.Block.CheckIsEmpty());
    }


    void QuadBatch::EndFrame()
    {
      if (m_activeFrame.Block.IsValid)
      {
        throw UsageErrorException("Can not call EndFrame inside a Begin/End block");
      }
      if (m_activeFrame.CurrentState == FrameState::NotReady)
      {
        throw UsageErrorException("Can not call EndFrame without a BeginFrame");
      }

      // Sanity check that the m_activeFrame.Block has been returned to its default state
      assert(m_activeFrame.Block.CheckIsEmpty());
      m_activeFrame = {};

      // Sanity check that the m_activeFrame is in its default state
      assert(m_activeFrame.CheckIsEmpty());
    }


    void QuadBatch::CreateDeviceResources(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device)
    {
      if (!physicalDevice.IsValid())
      {
        throw std::invalid_argument("physicalDevice must be valid");
      }
      if (device == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("device must be valid");
      }

      try
      {
        // We set IsValid here to ensure that the DestroyDeviceResources tries to free everything if its called
        // Just don't call any other method that works on m_deviceResources before its been fully filled
        m_deviceResource.IsValid = true;
        m_deviceResource.PhysicalDevice = physicalDevice;
        m_deviceResource.UniformBuffer = CreateUniformBuffer(physicalDevice, device);
        m_deviceResource.DescriptorSetLayoutUniform =
          CreateDescriptorSetLayout(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        m_deviceResource.DescriptorPool = CreateDescriptorPool(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        m_deviceResource.DescriptorSetUniform =
          CreateDescriptorSet(device, m_deviceResource.DescriptorPool, m_deviceResource.DescriptorSetLayoutUniform);
        m_deviceResource.DescriptorSetTexture =
          CreateDescriptorSetLayout(device, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
        m_deviceResource.VertexShader.Reset(device, 0, m_vertexShaderBinary);
        m_deviceResource.FragmentShader.Reset(device, 0, m_fragmentShaderBinary);
        m_deviceResource.PipelineLayout =
          CreatePipelineLayout(device, m_deviceResource.DescriptorSetLayoutUniform.Get(), m_deviceResource.DescriptorSetTexture.Get());
        m_deviceResource.PipelineCache = CreatePipelineCache(device);

        m_deviceResource.Render.clear();
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR_IF(m_logEnabled, "Exception occurred during QuadBatch device resource creation: {}", ex.what());
        DestroyDeviceResources();
        throw;
      }
    }


    void QuadBatch::DestroyDeviceResources() noexcept
    {
      if (!m_deviceResource.IsValid)
      {
        return;
      }

      // Dependent resources, we also rely on DestroyDependent resources to fix any broke state and do the necessary waits
      DestroyDependentResources();

      assert(!m_activeFrame.Block.IsValid);
      assert(m_activeFrame.CurrentState == FrameState::NotReady);

      m_deviceResource = {};
    }


    void QuadBatch::CreateDependentResources(const uint32_t commandBufferCount, const VkRenderPass renderPass, const uint32_t subpass,
                                             const Extent2D& screenExtent)
    {
      if (m_activeFrame.CurrentState != FrameState::NotReady)
      {
        throw UsageErrorException("Can not RecreateResources during BeginFrame/EndFrame");
      }
      if (!m_deviceResource.IsValid)
      {
        throw UsageErrorException("Can't create dependent resources before the normal resources");
      }
      if (renderPass == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("renderPass can not be null");
      }
      if (commandBufferCount <= 0)
      {
        throw std::invalid_argument("commandBufferCount should be >= 1");
      }


      if (screenExtent != m_deviceResource.CachedScreenExtent)
      {
        m_deviceResource.CachedScreenExtent = screenExtent;

        // Setup the shader
        const auto screenWidth = static_cast<float>(screenExtent.Width);
        const auto screenHeight = static_cast<float>(screenExtent.Height);
        const float screenOffsetX = std::floor(screenWidth / 2.0f);
        const float screenOffsetY = std::floor(screenHeight / 2.0f);

        const Matrix matViewProj =
          Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, -2.0f) * Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
        VUBufferMemoryUtil::Upload(m_deviceResource.UniformBuffer, 0, matViewProj);
        UpdateDescriptorSetUniform(m_deviceResource.UniformBuffer.GetDevice(), m_deviceResource.DescriptorSetUniform,
                                   m_deviceResource.UniformBuffer.GetDescriptorBufferInfo());
      }

      EnforceCommandBufferCount(commandBufferCount);

      try
      {
        const auto device = m_deviceResource.UniformBuffer.GetDevice();

        // We set IsValid here to ensure that the DestroyDependentResources tries to free everything if its called.
        // Just don't call any other method that works on m_dependentResource before its been fully filled
        m_dependentResource.IsValid = true;
        m_dependentResource.CommandBufferCount = commandBufferCount;
        m_dependentResource.PipelineAdditive =
          CreateGraphicsPipeline(device, m_deviceResource.VertexShader, m_deviceResource.FragmentShader, m_deviceResource.PipelineLayout,
                                 m_deviceResource.PipelineCache, renderPass, subpass, screenExtent, BlendState::Additive);

        m_dependentResource.PipelineAlphaBlend =
          CreateGraphicsPipeline(device, m_deviceResource.VertexShader, m_deviceResource.FragmentShader, m_deviceResource.PipelineLayout,
                                 m_deviceResource.PipelineCache, renderPass, subpass, screenExtent, BlendState::AlphaBlend);

        m_dependentResource.PipelineNonPremultiplied =
          CreateGraphicsPipeline(device, m_deviceResource.VertexShader, m_deviceResource.FragmentShader, m_deviceResource.PipelineLayout,
                                 m_deviceResource.PipelineCache, renderPass, subpass, screenExtent, BlendState::NonPremultiplied);

        m_dependentResource.PipelineOpaque =
          CreateGraphicsPipeline(device, m_deviceResource.VertexShader, m_deviceResource.FragmentShader, m_deviceResource.PipelineLayout,
                                 m_deviceResource.PipelineCache, renderPass, subpass, screenExtent, BlendState::Opaque);
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR_IF(m_logEnabled, "Exception occurred during QuadBatch dependent resource creation: {}", ex.what());
        DestroyDependentResources();
        throw;
      }
    }

    void QuadBatch::DestroyDependentResources() noexcept
    {
      if (m_activeFrame.Block.IsValid)
      {
        FSLLOG3_ERROR_IF(m_logEnabled, "DestroyDependentResources called inside a Begin/End block, force ending it");
        End();
      }
      if (m_activeFrame.CurrentState != FrameState::NotReady)
      {
        FSLLOG3_ERROR_IF(m_logEnabled, "DestroyDependentResources called inside a BeginFrame/EndFrame block, force ending it");
        EndFrame();
      }

      if (!m_deviceResource.IsValid)
      {
        assert(!m_dependentResource.IsValid);
        return;
      }

      auto waitResult = vkDeviceWaitIdle(m_deviceResource.UniformBuffer.GetDevice());
      if (waitResult != VK_SUCCESS)
      {
        FSLLOG3_ERROR_IF(m_logEnabled, "vkDeviceWaitIdle failed with: {}({}) at {} line: {}", RapidVulkan::Debug::ToString(waitResult), waitResult,
                         __FILE__, __LINE__);
      }

      if (!m_dependentResource.IsValid)
      {
        return;
      }
      assert(m_deviceResource.IsValid);

      // Release all dependent resources
      m_dependentResource = {};

      // Sanity check that the m_activeFrame is in its default state
      assert(m_activeFrame.CheckIsEmpty());
    }

    void QuadBatch::EnforceCommandBufferCount(const uint32_t commandBufferCount)
    {
      assert(m_deviceResource.IsValid);
      if (commandBufferCount == m_deviceResource.Render.size())
      {
        return;
      }
      m_deviceResource.Render.resize(commandBufferCount);
      auto device = m_deviceResource.UniformBuffer.GetDevice();
      assert(device != VK_NULL_HANDLE);
      for (auto& rEntry : m_deviceResource.Render)
      {
        if (!rEntry.IsValid())
        {
          rEntry.Reset(m_deviceResource.PhysicalDevice, device, m_deviceResource.DescriptorSetTexture.Get(), INTERNAL_QUAD_VERTEX_COUNT);
        }
      }
    }
  }
}
