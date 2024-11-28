/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include "RenderModel.hpp"
#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDemoApp/Base/Service/Texture/ITextureService.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float DefaultZoom = 1.1f;
      constexpr uint32_t VertexBufferBindId = 0;
    }

    using TestMesh = Graphics3D::GenericMesh<VertexPositionColorNormalTangentTexture, uint16_t>;
    using TestScene = Graphics3D::GenericScene<TestMesh>;

    Matrix ExtractRotationMatrix(const Matrix matrix)
    {
      const float* pM = matrix.DirectAccess();

      return {pM[MatrixFields::_M11],
              pM[MatrixFields::_M12],
              pM[MatrixFields::_M13],
              0,
              pM[MatrixFields::_M21],
              pM[MatrixFields::_M22],
              pM[MatrixFields::_M23],
              0,
              pM[MatrixFields::_M31],
              pM[MatrixFields::_M32],
              pM[MatrixFields::_M33],
              0,
              0,
              0,
              0,
              1};
    }

    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Texture& texture,
                                    const VkFilter filter, const VkSamplerAddressMode addressMode)
    {
      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = addressMode;
      samplerCreateInfo.addressModeV = addressMode;
      samplerCreateInfo.addressModeW = addressMode;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = static_cast<float>(texture.GetLevels());
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(texture, samplerCreateInfo);
    }


    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap,
                                    const VkFilter filter, const VkSamplerAddressMode addressMode, ITextureService* pTextureService)
    {
      if (pTextureService != nullptr)
      {
        std::optional<Texture> result = pTextureService->TryGenerateMipMaps(bitmap, TextureMipMapFilter::Box);
        if (result.has_value())
        {
          return CreateTexture(device, deviceQueue, *result, filter, addressMode);
        }
      }

      Vulkan::VulkanImageCreator imageCreator(device, deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = filter;
      samplerCreateInfo.minFilter = filter;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = addressMode;
      samplerCreateInfo.addressModeV = addressMode;
      samplerCreateInfo.addressModeW = addressMode;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
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


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 4> setLayoutBindings{};
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

      // Binding 3 : Fragment shader image sampler
      setLayoutBindings[3].binding = 3;
      setLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[3].descriptorCount = 1;
      setLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
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

    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUTexture& texture, const Vulkan::VUTexture& textureNormal,
                                        const Vulkan::VUTexture& textureSpecular)

    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 4> writeDescriptorSets{};
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      assert(texture.IsValid());
      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo1 = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo1;

      // Binding 2 : Fragment shader texture sampler
      const auto& rTexture2 = textureNormal.IsValid() ? textureNormal : texture;
      auto textureImageInfo2 = rTexture2.GetDescriptorImageInfo();
      writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[2].dstSet = descriptorSet;
      writeDescriptorSets[2].dstBinding = 2;
      writeDescriptorSets[2].descriptorCount = 1;
      writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[2].pImageInfo = &textureImageInfo2;

      // Binding 3 : Fragment shader texture sampler
      const auto& rTexture3 = textureSpecular.IsValid() ? textureSpecular : texture;
      auto textureImageInfo3 = rTexture3.GetDescriptorImageInfo();
      writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[3].dstSet = descriptorSet;
      writeDescriptorSets[3].dstBinding = 3;
      writeDescriptorSets[3].descriptorCount = 1;
      writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[3].pImageInfo = &textureImageInfo3;

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


    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const ReadOnlySpan<VkVertexInputAttributeDescription> meshVertexAttributeDescription,
                                                 const VkVertexInputBindingDescription& meshVertexInputBindingDescription,
                                                 const VkSampleCountFlagBits sampleCount, const VkRenderPass renderPass, const uint32_t subpass,
                                                 const bool cullEnabled, const bool wireframe)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!meshVertexAttributeDescription.empty());

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
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &meshVertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(meshVertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = meshVertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = !wireframe ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
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
      pipelineRasterizationStateCreateInfo.cullMode = (cullEnabled ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE);
      pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
      pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
      pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
      pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
      pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      pipelineMultisampleStateCreateInfo.rasterizationSamples = sampleCount;
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

      std::array<VkDynamicState, 2> dynamicState = {VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT};

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
      pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      pipelineDynamicStateCreateInfo.flags = 0;
      pipelineDynamicStateCreateInfo.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicState.size());
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState.data();

      VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
      graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
  }

  RenderModel::RenderModel(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                           const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, ITextureService* pTextureService,
                           IContentManager& contentManager, const IO::PathView srcModelPath, const IO::PathView shaderPath,
                           const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                           const float modelScale, const uint32_t maxFramesInFlight)
    : m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_lightDirection(0.0f, 0.0f, 1.0f)
    , m_uboData(Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.2f, 0.2f, 0.2f, 0.2f), Vector4(1, 1, 1, 1), 100.0f)
  {
    m_resources.ModelRecord = LoadModel(device, deviceQueue, bufferManager, pTextureService, contentManager, srcModelPath, shaderPath,
                                        diffuseTexturePath, specularTexturePath, normalTexturePath, modelScale);

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(device);
    m_resources.MainDescriptorPool = CreateDescriptorPool(device, maxFramesInFlight);
    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.UboBuffer = CreateUBO(device, sizeof(UBOData));
      rFrame.DescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      UpdateDescriptorSet(device.Get(), rFrame.DescriptorSet, rFrame.UboBuffer, m_resources.ModelRecord.Material.Texture.Diffuse,
                          m_resources.ModelRecord.Material.Texture.Normal, m_resources.ModelRecord.Material.Texture.Specular);
    }


    m_lightDirection.Normalize();
  }


  void RenderModel::BuildResources(const VkRenderPass renderPass, const uint32_t subpass, const VkSampleCountFlagBits sampleCount)
  {
    const VkExtent2D dummyExtent{1920, 1080};
    m_dependentResources.Pipeline = CreatePipeline(
      m_resources.MainPipelineLayout, dummyExtent, m_resources.ModelRecord.Material.Shader.VertShader.Get(),
      m_resources.ModelRecord.Material.Shader.FragShader.Get(), SpanUtil::AsReadOnlySpan(m_resources.ModelRecord.Model.VertexAttributeDescription),
      m_resources.ModelRecord.Model.VertexInputBindingDescription, sampleCount, renderPass, subpass, true, false);
  }


  void RenderModel::FreeResources()
  {
    m_dependentResources.Reset();
  }


  void RenderModel::Update(const DemoTime& demoTime, const bool isPaused)
  {
    if (!isPaused)
    {
      m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
      m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
      m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
      m_rotation.X = MathHelper::WrapAngle(m_rotation.X);
      m_rotation.Y = MathHelper::WrapAngle(m_rotation.Y);
      m_rotation.Z = MathHelper::WrapAngle(m_rotation.Z);
    }

    m_matrices.View = Matrix::CreateLookAt(Vector3(0, 0.2f, LocalConfig::DefaultZoom), Vector3(), Vector3::Up());
    m_matrices.World = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
  }


  void RenderModel::Draw(const VkCommandBuffer hCmdBuffer, const VulkanBasic::DrawContext& drawContext, const DemoTime& demoTime,
                         const PxRectangle windowRectanglePx)
  {
    const PxSize2DF windowSizePxf = TypeConverter::To<PxSize2DF>(windowRectanglePx.GetSize());
    const float aspectRatio = windowSizePxf.RawHeight() > 0 ? windowSizePxf.RawWidth() / windowSizePxf.RawHeight() : 1.0f;

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrixForDepthMinusOneToOne();

    m_matrices.Proj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 1.0f, 1000.0f) * vulkanClipMatrix;

    const auto cameraRotation = ExtractRotationMatrix(m_matrices.View);
    Vector4 cameraSpaceLightDirection = Vector4::Transform(m_lightDirection, cameraRotation);
    // Update UBO data
    m_uboData.LightDirection = Vector4(cameraSpaceLightDirection.X, cameraSpaceLightDirection.Y, cameraSpaceLightDirection.Z, 0.0f);
    m_uboData.LightDirection.Normalize();
    m_uboData.MatWorldView = m_matrices.World * m_matrices.View;
    m_uboData.NormalMatrix = Matrix::Transpose(Matrix::Invert(m_uboData.MatWorldView));
    m_uboData.MatWorldViewProjection = m_uboData.MatWorldView * m_matrices.Proj;

    DrawModel(hCmdBuffer, drawContext, m_resources.ModelRecord, windowRectanglePx);
  }


  void RenderModel::DrawModel(const VkCommandBuffer hCmdBuffer, const VulkanBasic::DrawContext& drawContext, const ModelRenderRecord& modelRecord,
                              const PxRectangle windowRectanglePx)
  {
    if (!modelRecord.Model.SubMeshes.empty())
    {
      auto& rFrame = m_resources.MainFrameResources[drawContext.CurrentFrameIndex];
      rFrame.UboBuffer.Upload(0, &m_uboData, sizeof(UBOData));

      // const auto& matShaderRecord = modelRecord.Material.Shader;


      vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &rFrame.DescriptorSet, 0,
                              nullptr);
      vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

      const VkViewport viewport{static_cast<float>(windowRectanglePx.RawLeft()),
                                static_cast<float>(windowRectanglePx.RawTop()),
                                static_cast<float>(windowRectanglePx.RawWidth()),
                                static_cast<float>(windowRectanglePx.RawHeight()),
                                0.0f,
                                1.0f};
      vkCmdSetViewport(hCmdBuffer, 0, 1, &viewport);

      const VkRect2D scissorRect = TypeConverter::UncheckedTo<VkRect2D>(windowRectanglePx);
      vkCmdSetScissor(hCmdBuffer, 0, 1, &scissorRect);

      for (const auto& subMesh : modelRecord.Model.SubMeshes)
      {
        if (subMesh.IndexBuffer.GetIndexCount() > 0)
        {
          VkDeviceSize offsets = 0;
          vkCmdBindVertexBuffers(hCmdBuffer, LocalConfig::VertexBufferBindId, 1, subMesh.VertexBuffer.GetBufferPointer(), &offsets);
          vkCmdBindIndexBuffer(hCmdBuffer, subMesh.IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
          vkCmdDrawIndexed(hCmdBuffer, subMesh.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
        }
      }
    }
  }


  RenderModel::ModelRenderRecord RenderModel::LoadModel(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                        const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                        ITextureService* pTextureService, IContentManager& contentManager,
                                                        const IO::PathView srcModelPath, const IO::PathView shaderPath,
                                                        const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath,
                                                        const IO::Path& normalTexturePath, const float modelScale)
  {
    // Load the model
    auto contentPath = contentManager.GetContentPath();
    auto modelRecord = LoadModel(device, bufferManager, contentPath, srcModelPath, modelScale);

    // Load the texture and create the material
    MaterialRecord modelMaterial = LoadMaterial(device, deviceQueue, pTextureService, contentManager, shaderPath, diffuseTexturePath,
                                                specularTexturePath, normalTexturePath, modelRecord.VertexDecl.AsSpan());

    return {std::move(modelRecord), std::move(modelMaterial)};
  }


  RenderModel::ModelRecord RenderModel::LoadModel(const Vulkan::VUDevice& device, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                  const IO::Path& contentPath, const IO::PathView srcModelPath, const float modelScale)
  {
    FSLLOG3_INFO("Loading model '{}'", srcModelPath);
    auto modelPath = IO::Path::Combine(contentPath, srcModelPath);
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, modelScale, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    // The most basic allocation of vertex and index buffers, not optimized in any way
    std::vector<SubMeshRecord> subMeshes(scene->Meshes.size());
    std::size_t totalVertexCount = 0;
    std::size_t totalIndexCount = 0;
    for (std::size_t i = 0; i < scene->Meshes.size(); ++i)
    {
      auto mesh = scene->Meshes[i];
      subMeshes[i] = SubMeshRecord(Vulkan::VMVertexBuffer(bufferManager, mesh->AsReadOnlyFlexVertexSpan(), Vulkan::VMBufferUsage::STATIC),
                                   Vulkan::VMIndexBuffer(bufferManager, mesh->AsReadOnlyIndexSpan(), Vulkan::VMBufferUsage::STATIC),
                                   mesh->GetVertexCount(), mesh->GetIndexCount());
      totalVertexCount += mesh->GetVertexCount();
      totalIndexCount += mesh->GetIndexCount();
    }

    if (subMeshes.empty())
    {
      throw NotSupportedException("There must be at least one mesh");
    }

    constexpr std::array<VertexElementUsage, 5> ShaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Color,
                                                                   VertexElementUsage::Normal, VertexElementUsage::Tangent,
                                                                   VertexElementUsage::TextureCoordinate};

    std::array<VkVertexInputAttributeDescription, 5> vertexAttributeDescription{};
    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(vertexAttributeDescription, ShaderBindOrder, subMeshes[0].VertexBuffer);
    VkVertexInputBindingDescription vertexInputBindingDescription{};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = subMeshes[0].VertexBuffer.GetElementStride();
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    FSLLOG3_INFO("Total vertex count: {}, Total index count: {}", totalVertexCount, totalIndexCount);
    return {totalVertexCount,           totalIndexCount,
            std::move(subMeshes),       scene->Meshes[0]->AsVertexDeclarationSpan(),
            vertexAttributeDescription, vertexInputBindingDescription};
  }


  RenderModel::MaterialRecord RenderModel::LoadMaterial(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                        ITextureService* pTextureService, IContentManager& contentManager,
                                                        const IO::PathView shaderPath, const IO::Path& diffuseTexturePath,
                                                        const IO::Path& specularTexturePath, const IO::Path& normalTexturePath,
                                                        const VertexDeclarationSpan vertexDeclarationSpan)
  {
    auto shader = LoadShader(device, contentManager, shaderPath, vertexDeclarationSpan);
    auto textures = LoadTextures(device, deviceQueue, pTextureService, contentManager, diffuseTexturePath, specularTexturePath, normalTexturePath);
    return {std::move(shader), std::move(textures)};
  }


  RenderModel::MaterialShaderRecord RenderModel::LoadShader(const Vulkan::VUDevice& device, IContentManager& contentManager,
                                                            const IO::PathView shaderPath, const VertexDeclarationSpan vertexDeclarationSpan)
  {
    constexpr IO::PathView StrShaderName("PerPixelDiffuseSpecNormal");
    const IO::Path strVertShaderFilename(StrShaderName + ".vert.spv");
    const IO::Path strFragShaderFilename(StrShaderName + ".frag.spv");
    const IO::Path strVertShaderPath = IO::Path::Combine(shaderPath, strVertShaderFilename);
    const IO::Path strFragShaderPath = IO::Path::Combine(shaderPath, strFragShaderFilename);

    RapidVulkan::ShaderModule vertShader(device.Get(), 0, contentManager.ReadBytes(strVertShaderPath));
    RapidVulkan::ShaderModule fragShader(device.Get(), 0, contentManager.ReadBytes(strFragShaderPath));

    return {std::move(vertShader), std::move(fragShader)};
  }


  RenderModel::MaterialTextureRecord RenderModel::LoadTextures(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                               ITextureService* pTextureService, IContentManager& contentManager,
                                                               const IO::Path& diffuseTexturePath, const IO::Path& specularTexturePath,
                                                               const IO::Path& normalTexturePath)
  {
    Bitmap bitmap;
    constexpr auto Origin = BitmapOrigin::LowerLeft;

    FSLLOG3_INFO("- Diffuse");
    contentManager.Read(bitmap, diffuseTexturePath, PixelFormat::R8G8B8A8_UNORM, Origin);
    Vulkan::VUTexture texDiffuse = CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, pTextureService);

    FSLLOG3_INFO("- Specular");
    contentManager.Read(bitmap, specularTexturePath, PixelFormat::R8G8B8A8_UNORM, Origin);
    Vulkan::VUTexture texSpecular = CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, pTextureService);

    FSLLOG3_INFO("- Normal");
    contentManager.Read(bitmap, normalTexturePath, PixelFormat::R8G8B8A8_UNORM, Origin);
    Vulkan::VUTexture texNormal = CreateTexture(device, deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, nullptr);

    return {std::move(texDiffuse), std::move(texSpecular), std::move(texNormal)};
  }
}
