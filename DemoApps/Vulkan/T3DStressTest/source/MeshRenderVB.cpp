/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include "MeshRenderVB.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    constexpr auto BasicLightCount = 4;
    constexpr auto MaxLightCount = 10;

    constexpr auto VertexBufferBindId = 0;
    // const auto INDEX_BUFFER_BIND_ID = 0;

    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 3> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      // Binding 2 : Fragment shader image sampler
      setLayoutBindings[2].binding = 2;
      setLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[2].descriptorCount = 1;
      setLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    Vulkan::VUBufferMemory CreateVertexShaderUBO(const Vulkan::VUDevice& device, const VkDeviceSize size)
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
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout, const Vulkan::VUBufferMemory& uboBuffer,
                                        const Vulkan::VUTexture& texture0, const Vulkan::VUTexture& texture1)
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

      std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};

      // Binding 0 : Vertex shader uniform buffer
      auto uboBufferInfo = uboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &uboBufferInfo;
      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo0 = texture0.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].pNext = nullptr;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo0;
      // Binding 2 : Fragment shader texture sampler
      auto textureImageInfo1 = texture1.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].pNext = nullptr;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo1;

      vkUpdateDescriptorSets(descriptorPool.GetDevice(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0,
                             nullptr);

      return descriptorSet;
    }

    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPushConstantRange pushConstantRange{};
      pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      pushConstantRange.offset = 0;
      pushConstantRange.size = sizeof(float);

      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();
      pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
      pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }

    VertexBufferInfo<3> BuildVB(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const Procedural::BasicMesh& mesh)
    {
      VertexBufferInfo<3> info;
      info.VertexBuffer.Reset(bufferManager, mesh.AsReadOnlyFlexVertexSpan(), Vulkan::VMBufferUsage::STATIC);

      // Generate attribute description by matching shader layout with the vertex declarations
      constexpr std::array<VertexElementUsage, 3> ShaderAttribOrder = {VertexElementUsage::Position, VertexElementUsage::Normal,
                                                                       VertexElementUsage::TextureCoordinate};
      static_assert(ShaderAttribOrder.size() == info.AttributeDescription.size(), "We expect the sizes to match");

      for (std::size_t i = 0; i < info.AttributeDescription.size(); ++i)
      {
        const auto& vertexDeclElement = info.VertexBuffer.GetVertexElement(ShaderAttribOrder[i], 0);

        info.AttributeDescription[i].location = UncheckedNumericCast<uint32_t>(i);
        info.AttributeDescription[i].binding = 0;
        info.AttributeDescription[i].format = vertexDeclElement.NativeFormat;
        info.AttributeDescription[i].offset = vertexDeclElement.Offset;
      }

      info.BindingDescription.stride = info.VertexBuffer.GetElementStride();
      info.BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return info;
    }


    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const VertexBufferInfo<3>& vertexBufferInfo, const VkRenderPass renderPass,
                                                 const VkPrimitiveTopology topology, const bool opaque, const bool enableDepthTest)
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
      pipelineInputAssemblyStateCreateInfo.topology = topology;
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
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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
      if (opaque)
      {
        pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
      }
      else
      {
        pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
      }

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

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

      VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
      depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencilStateCreateInfo.depthTestEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
      depthStencilStateCreateInfo.depthWriteEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
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
      graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = 0;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return {pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo};
    }

    inline VkPrimitiveTopology GetTopology(const Fsl::PrimitiveType primitiveType)
    {
      switch (primitiveType)
      {
      case PrimitiveType::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
      case PrimitiveType::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
      case PrimitiveType::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      case PrimitiveType::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      default:
        throw NotSupportedException("GetTopology");
      }
    }
  }

  MeshRenderVB::MeshRenderVB(const IContentManager& contentManager, const Vulkan::VUDevice& device,
                             const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                             const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh, const Vulkan::VUTexture& texture0,
                             const Vulkan::VUTexture& texture1, const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount,
                             const bool enableDepthTest)
    : MeshFurRender(contentManager, device, shaderPath, useHighPrecision, lightCount)
    , m_resources(maxFramesInFlight)
    , m_topology(GetTopology(mesh.GetPrimitiveType()))
    , m_lightCount(lightCount)
    , m_enableDepthTest(enableDepthTest)
  {
    if (lightCount > MaxLightCount)
    {
      throw std::invalid_argument("lightCount");
    }
    m_uboData.LightCount = static_cast<float>(lightCount);

    DoConstruct(contentManager, device, bufferManager, descriptorPool, mesh, texture0, texture1);
  }

  MeshRenderVB::MeshRenderVB(const IContentManager& contentManager, const Vulkan::VUDevice& device,
                             const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                             const uint32_t maxFramesInFlight, const Procedural::BasicMesh& mesh, const Vulkan::VUTexture& texture0,
                             const Vulkan::VUTexture& texture1, const IO::Path& vertShaderPath, const IO::Path& fragShaderPath, const int lightCount,
                             const bool enableDepthTest)
    : MeshFurRender(contentManager, device, vertShaderPath, fragShaderPath, lightCount)
    , m_resources(maxFramesInFlight)
    , m_topology(GetTopology(mesh.GetPrimitiveType()))
    , m_lightCount(lightCount)
    , m_enableDepthTest(enableDepthTest)
  {
    if (lightCount > MaxLightCount)
    {
      throw std::invalid_argument("lightCount");
    }
    m_uboData.LightCount = static_cast<float>(lightCount);

    DoConstruct(contentManager, device, bufferManager, descriptorPool, mesh, texture0, texture1);
  }

  void MeshRenderVB::OnBuildResources(const VulkanBasic::BuildResourcesContext& context, const VkRenderPass renderPass)
  {
    FSLLOG3_VERBOSE("MeshRenderVB::OnBuildResources");
    FSLLOG3_VERBOSE("- OpaquePipeline");
    m_dependentResources.OpaquePipeline = CreatePipeline(m_resources.PipelineLayout, context.SwapchainImageExtent, m_vertShader.Get(),
                                                         m_fragShader.Get(), m_resources.VB, renderPass, m_topology, true, m_enableDepthTest);
    FSLLOG3_VERBOSE("- Pipeline");
    m_dependentResources.Pipeline = CreatePipeline(m_resources.PipelineLayout, context.SwapchainImageExtent, m_vertShader.Get(), m_fragShader.Get(),
                                                   m_resources.VB, renderPass, m_topology, false, m_enableDepthTest);
  }

  void MeshRenderVB::OnFreeResources()
  {
    m_dependentResources.Reset();
  }


  void MeshRenderVB::Bind(const VkCommandBuffer hCmdBuffer, const uint32_t frameIndex)
  {
    auto& rFrame = m_resources.FrameResources[frameIndex];
    rFrame.UboBuffer.Upload(0, &m_uboData, sizeof(VertexUBOData));

    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.PipelineLayout.Get(), 0, 1, &rFrame.DescriptorSet, 0, nullptr);
    const VkPipeline hPipeline = m_drawOpaque ? m_dependentResources.OpaquePipeline.Get() : m_dependentResources.Pipeline.Get();
    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, hPipeline);
  }

  void MeshRenderVB::Draw(const VkCommandBuffer hCmdBuffer)
  {
    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(hCmdBuffer, VertexBufferBindId, 1, m_resources.VB.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdBindIndexBuffer(hCmdBuffer, m_resources.IB.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(hCmdBuffer, m_resources.IB.GetIndexCount(), 1, 0, 0, 0);
  }

  void MeshRenderVB::Unbind()
  {
  }

  void MeshRenderVB::SetWorld(const Matrix& matrix)
  {
    m_uboData.World = matrix;
  }

  void MeshRenderVB::SetView(const Matrix& matrix)
  {
    m_uboData.View = matrix;
  }

  void MeshRenderVB::SetProjection(const Matrix& matrix)
  {
    m_uboData.Projection = matrix;
  }

  void MeshRenderVB::SetDisplacement(const Vector3& displacement)
  {
    m_uboData.Displacement = Vector4(displacement.X, displacement.Y, displacement.Z, 1.0f);
  }

  int MeshRenderVB::GetLightCount() const
  {
    return m_lightCount;
  }

  void MeshRenderVB::SetLightDirection(const int index, const Vector3& lightDirection)
  {
    if (index < 0 || index > m_lightCount)
    {
      throw std::invalid_argument("index");
    }

    if (m_lightCount > BasicLightCount)
    {
      m_uboData.Light.L10.LightDirection[index] = PODVector4(lightDirection.X, lightDirection.Y, lightDirection.Z, 0.0f);
      return;
    }

    switch (index)
    {
    case 0:
      m_uboData.Light.L4.LightDirection1 = PODVector4(lightDirection.X, lightDirection.Y, lightDirection.Z, 0.0f);
      break;
    case 1:
      m_uboData.Light.L4.LightDirection2 = PODVector4(lightDirection.X, lightDirection.Y, lightDirection.Z, 0.0f);
      break;
    case 2:
      m_uboData.Light.L4.LightDirection3 = PODVector4(lightDirection.X, lightDirection.Y, lightDirection.Z, 0.0f);
      break;
    case 3:
      m_uboData.Light.L4.LightDirection4 = PODVector4(lightDirection.X, lightDirection.Y, lightDirection.Z, 0.0f);
      break;
    default:
      throw std::invalid_argument("index");
    }
  }

  void MeshRenderVB::SetLightColor(const int index, const Vector3& lightColor)
  {
    if (index < 0 || index > m_lightCount)
    {
      throw std::invalid_argument("index");
    }

    if (m_lightCount > BasicLightCount)
    {
      m_uboData.Light.L10.LightColor[index] = PODVector4(lightColor.X, lightColor.Y, lightColor.Z, 1.0f);
      return;
    }

    switch (index)
    {
    case 0:
      m_uboData.Light.L4.LightColor1 = PODVector4(lightColor.X, lightColor.Y, lightColor.Z, 1.0f);
      break;
    case 1:
      m_uboData.Light.L4.LightColor2 = PODVector4(lightColor.X, lightColor.Y, lightColor.Z, 1.0f);
      break;
    case 2:
      m_uboData.Light.L4.LightColor3 = PODVector4(lightColor.X, lightColor.Y, lightColor.Z, 1.0f);
      break;
    case 3:
      m_uboData.Light.L4.LightColor4 = PODVector4(lightColor.X, lightColor.Y, lightColor.Z, 1.0f);
      break;
    default:
      throw std::invalid_argument("index");
    }
  }

  void MeshRenderVB::SetLightAmbientColor(const Vector3& ambientColor)
  {
    m_uboData.AmbientColor = Vector4(ambientColor.X, ambientColor.Y, ambientColor.Z, 1.0f);
  }

  void MeshRenderVB::SetMaxHairLength(const float maxHairLength)
  {
    m_uboData.MaxHairLength = maxHairLength;
  }

  void MeshRenderVB::SetCurrentLayer(const VkCommandBuffer hCmdBuffer, const float currentLayer)
  {
    // Submit via push constant (rather than a UBO)
    vkCmdPushConstants(hCmdBuffer, m_resources.PipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float),
                       &currentLayer);
  }

  void MeshRenderVB::SetDrawOpaque(const bool enabled)
  {
    m_drawOpaque = enabled;
  }

  void MeshRenderVB::DoConstruct(const IContentManager& /*contentManager*/, const Vulkan::VUDevice& device,
                                 const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const RapidVulkan::DescriptorPool& descriptorPool,
                                 const Procedural::BasicMesh& mesh, const Vulkan::VUTexture& texture0, const Vulkan::VUTexture& texture1)
  {
    m_resources.VB = BuildVB(bufferManager, mesh);
    m_resources.IB.Reset(bufferManager, mesh.GetIndexArray(), Vulkan::VMBufferUsage::STATIC);

    m_resources.DescriptorSetLayout = CreateDescriptorSetLayout(device);

    for (auto& rFrame : m_resources.FrameResources)
    {
      rFrame.UboBuffer = CreateVertexShaderUBO(device, sizeof(VertexUBOData));
      rFrame.DescriptorSet = CreateDescriptorSet(descriptorPool, m_resources.DescriptorSetLayout, rFrame.UboBuffer, texture0, texture1);
    }

    m_resources.PipelineLayout = CreatePipelineLayout(m_resources.DescriptorSetLayout);
  }

}
