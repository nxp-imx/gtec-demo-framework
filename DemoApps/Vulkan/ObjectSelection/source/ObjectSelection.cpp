/****************************************************************************************************************************************************
 * Copyright 2019, 2022-2023 NXP
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

#include "ObjectSelection.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/ViewportUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/Vertices/VertexPositionColorF.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/VUTextureUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/ObjectSelection/BoundingBoxUtil.hpp>
#include <Shared/ObjectSelection/OptionParser.hpp>
#include <vulkan/vulkan.h>
#include <cmath>
#include <random>
namespace Fsl
{
  namespace
  {
    const float DEFAULT_ZOOM = 10;

    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
    const uint32_t LINES_SUBPASS = 0u;

    constexpr uint32_t INITIAL_LINE_CAPACITY = 4096u;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }

    Vulkan::VUTexture CreateTexture(Vulkan::VulkanImageCreator& imageCreator, const Texture& texture)
    {
      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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


    Vulkan::VUSegmentedBufferMemory CreateSegmentedUBO(const Vulkan::VUDevice& device, const VkDeviceSize size, const uint32_t segmentCount)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = size;
      bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Vulkan::VUSegmentedBufferMemory buffer(device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                             segmentCount);
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

      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }


    VkDescriptorSet UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const Vulkan::VUBufferMemory& vertUboBuffer,
                                        const Vulkan::VUTexture& texture)
    {
      assert(descriptorSet != VK_NULL_HANDLE);
      assert(vertUboBuffer.IsValid());
      assert(texture.IsValid());

      std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
      // Binding 0 : uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }


    VkDescriptorSet UpdateObjectTransformDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet,
                                                       const Vulkan::VUSegmentedBufferMemory& vertUboBuffer)
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
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
      return descriptorSet;
    }

    RapidVulkan::DescriptorSetLayout CreatePlaneDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    RapidVulkan::DescriptorSetLayout CreateObjectTransformDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings{};
      // Binding 0 : Vertex shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }

    RapidVulkan::DescriptorSetLayout CreateObjectDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
      // Binding 0 : Fragment shader uniform buffer
      setLayoutBindings[0].binding = 0;
      setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      setLayoutBindings[0].descriptorCount = 1;
      setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      // Binding 1 : Fragment shader image sampler
      setLayoutBindings[1].binding = 1;
      setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      setLayoutBindings[1].descriptorCount = 1;
      setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return {device.Get(), descriptorLayout};
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t maxSets, const uint32_t framesInFlight)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 3> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = 2 * framesInFlight;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
      poolSizes[1].descriptorCount = 1 * framesInFlight;
      poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[2].descriptorCount = 2 * framesInFlight;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = maxSets * framesInFlight;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }


    RapidVulkan::PipelineLayout CreatePipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout)
    {
      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = 1;
      pipelineLayoutCreateInfo.pSetLayouts = descripterSetLayout.GetPointer();

      return {descripterSetLayout.GetDevice(), pipelineLayoutCreateInfo};
    }

    RapidVulkan::PipelineLayout CreateObjectPipelineLayout(const RapidVulkan::DescriptorSetLayout& descripterSetLayout1,
                                                           const RapidVulkan::DescriptorSetLayout& descripterSetLayout2)
    {
      std::array<VkDescriptorSetLayout, 2> layouts = {descripterSetLayout1.Get(), descripterSetLayout2.Get()};

      VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
      pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutCreateInfo.setLayoutCount = UncheckedNumericCast<uint32_t>(layouts.size());
      pipelineLayoutCreateInfo.pSetLayouts = layouts.data();

      return {descripterSetLayout1.GetDevice(), pipelineLayoutCreateInfo};
    }


    template <std::size_t TSize>
    RapidVulkan::GraphicsPipeline CreatePipeline(const RapidVulkan::PipelineLayout& pipelineLayout, const VkExtent2D& extent,
                                                 const VkShaderModule vertexShaderModule, const VkShaderModule fragmentShaderModule,
                                                 const VkVertexInputBindingDescription& vertexInputBindingDescription,
                                                 const std::array<VkVertexInputAttributeDescription, TSize>& vertexAttributeDescription,
                                                 const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!vertexAttributeDescription.empty());

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
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(vertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
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
      pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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


  ObjectSelection::ObjectSelection(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_menuUI(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_resources(3, VertexDeclaration(VertexPositionColorF::AsVertexDeclarationSpan()))
    , m_hasSelectedObject(false)
    , m_selectedIndex(0)
  {
    m_lightFragUboData.LightDirection = {0.0f, 1.0f, 1.0, 0.0f};
    m_lightFragUboData.LightColor = {0.8f, 0.8f, 0.8f, 0.0f};
    m_lightFragUboData.AmbientColor = {0.2f, 0.2f, 0.2f, 0.0f};

    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    m_camera.SetPosition(Vector3(0, 0, DEFAULT_ZOOM), Vector3(), Vector3::Up());

    m_lightFragUboData.LightDirection.Normalize();

    const auto optionParser = config.GetOptions<OptionParser>();

    auto contentManager = GetContentManager();

    m_resources.BufferManager =
      std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    CreateTextures(contentManager);

    m_resources.MeshPlane = PreparePlaneMesh(m_resources.BufferManager, m_resources.TextureChessboard);
    PrepareMeshes(m_resources.Meshes, m_resources.BufferManager, m_resources.Texture);

    m_resources.ProgDirectionalLight = PrepareDirectionalLightProgram(m_device.Get(), contentManager);
    m_resources.ProgramSolidColor = PrepareSolidColorProgram(m_device.Get(), contentManager);
    m_resources.ProgramTextured = PrepareTexturedProgram(m_device.Get(), contentManager);

    const auto objectCount = optionParser->GetObjectCount();
    GenerateObjects(m_resources.Objects, objectCount, m_menuUI.IsRandomSeedEnabled());
    m_objVertexUboData.resize(objectCount);

    m_resources.PlaneObject.Rotation = Vector3(-MathHelper::ToRadians(90), 0, 0);
    m_resources.PlaneObject.Position = Vector3(0, -8, 0);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;

    m_resources.PlaneDescriptorSetLayout = CreatePlaneDescriptorSetLayout(m_device);
    m_resources.ObjectTransformDescriptorSetLayout = CreateObjectTransformDescriptorSetLayout(m_device);
    m_resources.ObjectDescriptorSetLayout = CreateObjectDescriptorSetLayout(m_device);

    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, 3, maxFramesInFlight);

    m_resources.LineDraw.Reset(m_device, m_resources.BufferManager, maxFramesInFlight, sizeof(ProjectionUBOData), INITIAL_LINE_CAPACITY);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.PlaneVertUboBuffer = CreateUBO(m_device, sizeof(ProjectionUBOData));
      rFrame.PlaneDescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.PlaneDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), rFrame.PlaneDescriptorSet, rFrame.PlaneVertUboBuffer, m_resources.TextureChessboard);

      rFrame.ObjectTransformVertUboBuffer = CreateSegmentedUBO(m_device, sizeof(ObjUboData), objectCount);
      rFrame.ObjectTransformDescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.ObjectTransformDescriptorSetLayout);
      UpdateObjectTransformDescriptorSet(m_device.Get(), rFrame.ObjectTransformDescriptorSet, rFrame.ObjectTransformVertUboBuffer);

      rFrame.ObjectVertUboBuffer = CreateUBO(m_device, sizeof(ProjectionUBOData));
      rFrame.ObjectDescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.ObjectDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), rFrame.ObjectDescriptorSet, rFrame.ObjectVertUboBuffer, m_resources.Texture);
    }

    m_resources.PlanePipelineLayout = CreatePipelineLayout(m_resources.PlaneDescriptorSetLayout);
    m_resources.ObjectPipelineLayout =
      CreateObjectPipelineLayout(m_resources.ObjectTransformDescriptorSetLayout, m_resources.ObjectDescriptorSetLayout);
  }


  void ObjectSelection::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    if (event.IsPressed() && event.GetButton() == VirtualMouseButton::Left)
    {
      if (CheckCollision(event.GetPosition()))
      {
        event.Handled();
      }
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
      {
        m_rightMouseDown = event.IsPressed();
        if (m_demoAppControl->TryEnableMouseCaptureMode(m_rightMouseDown))
        {
          m_mouseCaptureEnabled = m_rightMouseDown;
        }
        else
        {
          m_mouseCaptureEnabled = false;
        }
        event.Handled();
        break;
      }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(Vector3(0, 0, DEFAULT_ZOOM), Vector3(), Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void ObjectSelection::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
  }


  void ObjectSelection::Update(const DemoTime& demoTime)
  {
    const PxSize2D windowSizePx = GetWindowSizePx();

    m_viewPort = Viewport(Rectangle(0, 0, windowSizePx.RawWidth(), windowSizePx.RawHeight()));

    m_matrixView = m_camera.GetViewMatrix();
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), m_viewPort.GetAspectRatio(), 0.1f, 500.0f);
    m_matrixViewProjection = m_matrixView * m_matrixProjection;

    for (auto& rEntry : m_resources.Objects)
    {
      rEntry.Rotation.X += rEntry.RotationSpeed.X * demoTime.DeltaTime;
      rEntry.Rotation.Y += rEntry.RotationSpeed.Y * demoTime.DeltaTime;
      rEntry.Rotation.Z += rEntry.RotationSpeed.Z * demoTime.DeltaTime;
      rEntry.WorldMatrix = Matrix::CreateScale(rEntry.Scale) * Matrix::CreateRotationX(rEntry.Rotation.X) *
                           Matrix::CreateRotationY(rEntry.Rotation.Y) * Matrix::CreateRotationZ(rEntry.Rotation.Z) *
                           Matrix::CreateTranslation(rEntry.Position);
      rEntry.WorldViewMatrix = rEntry.WorldMatrix * m_matrixView;
      rEntry.WorldViewProjectionMatrix = rEntry.WorldViewMatrix * m_matrixProjection;

      rEntry.MeshAABB = BoundingBoxUtil::CalculateAABB(rEntry.WorldMatrix, m_resources.Meshes[rEntry.MeshIndex].TheBoundingBox);
    }

    {
      m_resources.PlaneObject.WorldMatrix =
        Matrix::CreateRotationX(m_resources.PlaneObject.Rotation.X) * Matrix::CreateTranslation(m_resources.PlaneObject.Position);
      m_resources.PlaneObject.WorldViewMatrix = m_resources.PlaneObject.WorldMatrix * m_matrixView;
      m_resources.PlaneObject.WorldViewProjectionMatrix = m_resources.PlaneObject.WorldViewMatrix * m_matrixProjection;

      // m_resources.PlaneObject.NormalMatrix =
      // Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(m_resources.PlaneObject.WorldViewMatrix)));

      m_resources.PlaneObject.MeshAABB = BoundingBoxUtil::CalculateAABB(m_resources.PlaneObject.WorldMatrix, m_resources.MeshPlane.TheBoundingBox);
    }

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    m_linesVertexUboData.MatViewProjection = m_matrixViewProjection * vulkanClipMatrix;
    m_planeVertexUboData.MatViewProjection = m_resources.PlaneObject.WorldViewProjectionMatrix * vulkanClipMatrix;
    for (uint32_t i = 0; i < m_resources.Objects.size(); ++i)
    {
      m_objVertexUboData[i].WorldViewProjection = m_resources.Objects[i].WorldViewProjectionMatrix * vulkanClipMatrix;
      m_objVertexUboData[i].NormalMatrix =
        Matrix3Std140::Transpose(Matrix3Std140::Invert(MatrixConverter::ToMatrix3Std140(m_resources.Objects[i].WorldViewMatrix)));
    }

    auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
  }


  void ObjectSelection::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers,
                                   const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    // Upload the changes
    m_resources.LineDraw.UpdateVertexUBO(&m_linesVertexUboData, sizeof(ProjectionUBOData), currentFrameIndex);
    m_resources.MainFrameResources[currentFrameIndex].PlaneVertUboBuffer.Upload(0, &m_planeVertexUboData, sizeof(ProjectionUBOData));
    m_resources.MainFrameResources[currentFrameIndex].ObjectTransformVertUboBuffer.Upload(0, m_objVertexUboData);
    m_resources.MainFrameResources[currentFrameIndex].ObjectVertUboBuffer.Upload(0, &m_lightFragUboData, sizeof(LightUBOData));

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};
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

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        if (m_menuUI.IsPlaneEnabled())
        {
          DrawPlaneMesh(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);
        }
        DrawMeshes(m_resources.MainFrameResources[currentFrameIndex], hCmdBuffer);
        DrawDebugData(hCmdBuffer, currentFrameIndex);

        // Draw the UI overlay
        m_menuUI.Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass ObjectSelection::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_dependentResources.PlanePipeline =
      CreatePipeline(m_resources.PlanePipelineLayout, context.SwapchainImageExtent, m_resources.ProgramTextured.VertShaderModule.Get(),
                     m_resources.ProgramTextured.FragShaderModule.Get(), m_resources.MeshPlane.VertexInputBindingDescription,
                     m_resources.MeshPlane.VertexAttributeDescription, m_dependentResources.MainRenderPass.Get(), 0);

    // assert(m_resources.Meshes.size() == 3u);
    // assert(m_resources.Meshes[1].VertexInputBindingDescription == m_resources.Meshes[0].VertexInputBindingDescription);
    // assert(m_resources.Meshes[2].VertexInputBindingDescription == m_resources.Meshes[0].VertexInputBindingDescription);
    // assert(m_resources.Meshes[1].VertexAttributeDescription == m_resources.Meshes[0].VertexAttributeDescription);
    // assert(m_resources.Meshes[2].VertexAttributeDescription == m_resources.Meshes[0].VertexAttributeDescription);

    m_dependentResources.ObjectPipeline =
      CreatePipeline(m_resources.ObjectPipelineLayout, context.SwapchainImageExtent, m_resources.ProgDirectionalLight.VertShaderModule.Get(),
                     m_resources.ProgDirectionalLight.FragShaderModule.Get(), m_resources.Meshes[0].VertexInputBindingDescription,
                     m_resources.Meshes[0].VertexAttributeDescription, m_dependentResources.MainRenderPass.Get(), 0);

    m_resources.LineDraw.BuildResources(context.SwapchainImageExtent, m_resources.ProgramSolidColor.VertShaderModule.Get(),
                                        m_resources.ProgramSolidColor.FragShaderModule.Get(), m_dependentResources.MainRenderPass.Get(),
                                        LINES_SUBPASS);

    return m_dependentResources.MainRenderPass.Get();
  }


  void ObjectSelection::OnFreeResources()
  {
    m_resources.LineDraw.FreeResources();
    m_dependentResources.Reset();
  }


  void ObjectSelection::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      Vector3 sourcePos(static_cast<float>(mouseState.Position.X.Value), static_cast<float>(mouseState.Position.Y.Value), 0.0f);
      m_mousePositionNear = ViewportUtil::Unproject(m_viewPort, sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      sourcePos.Z = 1.0f;
      m_mousePositionFar = ViewportUtil::Unproject(m_viewPort, sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
      // FSLLOG3_INFO("2D: X: " << mouseState.Position.X << " Y: " << mouseState.Position.Y  << " 3D: X: " << m_mousePositionFar.X << " Y: " <<
      // m_mousePositionFar.Y << " Z: " << m_mousePositionFar.Z);


      if (!m_rightMouseDown)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X.Value, -mouseState.RawPosition.Y.Value);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 32.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }


  bool ObjectSelection::CheckCollision(const PxPoint2& screenSpacePosition)
  {
    Vector3 sourcePos(static_cast<float>(screenSpacePosition.X.Value), static_cast<float>(screenSpacePosition.Y.Value), 0.0f);

    // Unproject a point on the near and far plane
    const auto nearPoint = ViewportUtil::Unproject(m_viewPort, sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());
    sourcePos.Z = 1.0f;
    const auto farPoint = ViewportUtil::Unproject(m_viewPort, sourcePos, m_matrixProjection, m_matrixView, Matrix::GetIdentity());

    // Build a ray
    Vector3 direction = farPoint - nearPoint;
    direction.Normalize();
    Ray mouseRay(nearPoint, direction);

    // Check which AABB's the ray intersect and store the hits in m_pickScratchpad
    m_pickScratchpad.clear();
    for (std::size_t i = 0; i < m_resources.Objects.size(); ++i)
    {
      float distance = 0.0f;
      if (mouseRay.Intersects(m_resources.Objects[i].MeshAABB, distance))
      {
        m_pickScratchpad.emplace_back(i, distance);
      }
    }

    // Check if there is any stored hits
    m_hasSelectedObject = !m_pickScratchpad.empty();
    if (m_hasSelectedObject)
    {
      // Find the one with the shortest distance
      float distance = std::numeric_limits<float>::max();
      for (const auto& entry : m_pickScratchpad)
      {
        if (entry.Distance < distance)
        {
          distance = entry.Distance;
          m_selectedIndex = entry.ObjectIndex;
        }
      }
    }
    return m_hasSelectedObject;
  }


  void ObjectSelection::DrawMeshes(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    if (m_menuUI.IsObjectsEnabled())
    {
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.ObjectPipelineLayout.Get(), 1, 1,
                              &frame.ObjectDescriptorSet, 0, nullptr);

      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.ObjectPipeline.Get());

      const auto segmentStride = frame.ObjectTransformVertUboBuffer.GetSegmentStride();

      const bool forceBind = m_menuUI.IsForceBindEnabled();
      uint32_t oldMeshIndex = std::numeric_limits<uint32_t>::max();
      uint32_t transformOffset = 0;
      for (auto& rEntry : m_resources.Objects)
      {
        assert(transformOffset < frame.ObjectTransformVertUboBuffer.GetAllocationSize());

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.ObjectPipelineLayout.Get(), 0, 1,
                                &frame.ObjectTransformDescriptorSet, 1, &transformOffset);

        DrawMesh(m_resources.Meshes[rEntry.MeshIndex], (forceBind || oldMeshIndex != rEntry.MeshIndex), commandBuffer);
        oldMeshIndex = rEntry.MeshIndex;
        transformOffset += segmentStride;
      }
    }
  }


  void ObjectSelection::DrawPlaneMesh(const FrameResources& frame, const VkCommandBuffer commandBuffer)
  {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.PlanePipelineLayout.Get(), 0, 1, &frame.PlaneDescriptorSet, 0,
                            nullptr);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.PlanePipeline.Get());

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.MeshPlane.VertexBuffer.GetBufferPointer(), &offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_resources.MeshPlane.IndexBuffer.GetBuffer(), 0, m_resources.MeshPlane.IndexBuffer.GetIndexType());
    vkCmdDrawIndexed(commandBuffer, m_resources.MeshPlane.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
  }


  void ObjectSelection::DrawMesh(const Mesh& mesh, const bool bindMesh, const VkCommandBuffer commandBuffer)
  {
    // const auto indexBufferType = mesh.IndexBuffer.GetIndexType();
    if (mesh.IndexBuffer.GetIndexCount() <= 0)
    {
      return;
    }

    // if (bindMesh)
    //{
    //  // Bind and enable the vertex buffer
    //  glBindBuffer(mesh.VertexBuffer.GetTarget(), mesh.VertexBuffer.Get());
    //  glBindBuffer(mesh.IndexBuffer.GetTarget(), mesh.IndexBuffer.Get());

    //  // Since all our meshes use the same attrib pointers we dont have to enable/disable them all the time
    //  mesh.VertexBuffer.SetVertexAttribPointers(m_resources.AttribLink);
    //}
    // glDrawElements(GL_TRIANGLE_STRIP, mesh.IndexBuffer.GetCapacity(), indexBufferType, nullptr);

    if (bindMesh)
    {
      VkDeviceSize offsets = 0;
      vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, mesh.VertexBuffer.GetBufferPointer(), &offsets);
      vkCmdBindIndexBuffer(commandBuffer, mesh.IndexBuffer.GetBuffer(), 0, mesh.IndexBuffer.GetIndexType());
    }

    vkCmdDrawIndexed(commandBuffer, mesh.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
  }


  void ObjectSelection::DrawDebugData(const VkCommandBuffer commandBuffer, const uint32_t frameIndex)
  {
    m_resources.LineBuild.Clear();

    // Load the matrices
    if (m_menuUI.IsDrawOrientedBoundingBoxEnabled())
    {
      for (auto& rEntry : m_resources.Objects)
      {
        m_resources.LineBuild.Add(m_resources.Meshes[rEntry.MeshIndex].TheBoundingBox, Color::Red(), rEntry.WorldMatrix);
      }
    }

    if (m_menuUI.IsDrawAxisAlignedBoundingBoxEnabled())
    {
      for (std::size_t i = 0; i < m_resources.Objects.size(); ++i)
      {
        if (i != m_selectedIndex || !m_hasSelectedObject)
        {
          m_resources.LineBuild.Add(m_resources.Objects[i].MeshAABB, Color::Blue());
        }
      }
    }
    if (m_hasSelectedObject)
    {
      m_resources.LineBuild.Add(m_resources.Objects[m_selectedIndex].MeshAABB, Color::White());
    }

    // Draw bounding boxes

    {
      // DrawLine(m_mousePositionNear, m_mousePositionFar, Color::Pink());

      if (m_menuUI.IsDrawNearPlaneMouseEnabled())
      {
        const float magic = 0.001f;
        m_resources.LineBuild.Add(m_mousePositionNear + Vector3(-magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(magic, magic, 0.0f)),
                                  Color::Pink());
        m_resources.LineBuild.Add(m_mousePositionNear + Vector3(magic, -magic, 0.0f), m_mousePositionNear + Vector3(Vector3(-magic, magic, 0.0f)),
                                  Color::Pink());
      }
      if (m_menuUI.IsDrawFarPlaneMouseEnabled())
      {
        const float magic = 5.0f;
        m_resources.LineBuild.Add(m_mousePositionFar + Vector3(-magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(magic, magic, 1.0f)),
                                  Color::Cyan());
        m_resources.LineBuild.Add(m_mousePositionFar + Vector3(magic, -magic, 1.0f), m_mousePositionFar + Vector3(Vector3(-magic, magic, 1.0f)),
                                  Color::Cyan());
      }
    }

    if (!m_resources.LineBuild.IsEmpty())
    {
      m_resources.LineDraw.Draw(commandBuffer, m_resources.LineBuild.GetVertexSpan(), frameIndex);
    }
  }

  void ObjectSelection::CreateTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    Vulkan::VulkanImageCreator imageCreator(m_device, m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    auto texture =
      contentManager->ReadTexture("SeamlessFur.png", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::Undefined, PixelChannelOrder::Undefined, true);
    m_resources.Texture = CreateTexture(imageCreator, texture);

    texture =
      contentManager->ReadTexture("MarbleChessboard.jpg", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::Undefined, PixelChannelOrder::Undefined, true);
    m_resources.TextureChessboard = CreateTexture(imageCreator, texture);
  }


  ObjectSelection::Mesh ObjectSelection::PreparePlaneMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                          const Vulkan::VUTexture& texture)
  {
    const auto textureExtent = texture.GetExtent();
    const auto tex1Size = PxSize2D::Create(UncheckedNumericCast<int32_t>(textureExtent.width), UncheckedNumericCast<int32_t>(textureExtent.height));
    TextureRectangle texRect(PxRectangle(PxValue(0), PxValue(0), tex1Size.Width(), tex1Size.Height()), tex1Size);
    const NativeTextureArea texRepeatArea(Vulkan::VUTextureUtil::CalcTextureArea(texRect, 15 / 5, 15 / 5));
    const auto mesh =
      Procedural::SegmentedQuadGenerator::GenerateStrip(Vector3(0, 0, 0), 1000 / 5.0f, 1000 / 5.0f, 1, 1, texRepeatArea, WindingOrder::CCW);
    return CreatePlaneMesh(bufferManager, mesh);
  }


  void ObjectSelection::PrepareMeshes(std::vector<Mesh>& rMeshes, const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                      const Vulkan::VUTexture& texture)
  {
    const auto textureExtent = texture.GetExtent();
    const auto tex1Size = PxSize2D::Create(UncheckedNumericCast<int32_t>(textureExtent.width), UncheckedNumericCast<int32_t>(textureExtent.height));
    TextureRectangle texRect(PxRectangle(PxValue(0), PxValue(0), tex1Size.Width(), tex1Size.Height()), tex1Size);

    const NativeTextureArea texArea(Vulkan::VUTextureUtil::CalcTextureArea(texRect, 1, 1));

    auto mesh = Procedural::TorusGenerator::GenerateStrip(16, 16, 2, 0.5f, texArea, WindingOrder::CCW);
    rMeshes[0] = CreateMesh(bufferManager, mesh);

    mesh = Procedural::TorusGenerator::GenerateStrip(3, 3, 2, 0.5f, texArea, WindingOrder::CCW);
    rMeshes[1] = CreateMesh(bufferManager, mesh);

    std::array<NativeTextureArea, 6> texAreas = {texArea, texArea, texArea, texArea, texArea, texArea};
    mesh = Procedural::BoxGenerator::GenerateStrip(Vector3(), 2.0f, 2.0f, 2.0f, texAreas.data(), UncheckedNumericCast<int32_t>(texAreas.size()),
                                                   WindingOrder::CCW);
    rMeshes[2] = CreateMesh(bufferManager, mesh);
  }


  ObjectSelection::Mesh ObjectSelection::CreatePlaneMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager,
                                                         const Procedural::BasicMesh& mesh)
  {
    Mesh newMesh;

    newMesh.IndexBuffer.Reset(bufferManager, mesh.GetIndexArray(), Vulkan::VMBufferUsage::STATIC);
    newMesh.VertexBuffer.Reset(bufferManager, mesh.AsReadOnlyFlexVertexSpan(), Vulkan::VMBufferUsage::STATIC);

    std::array<VertexElementUsage, 3> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate,
                                                         VertexElementUsage::Normal};

    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(newMesh.VertexAttributeDescription, shaderBindOrder, newMesh.VertexBuffer);
    newMesh.VertexInputBindingDescription.binding = 0;
    newMesh.VertexInputBindingDescription.stride = newMesh.VertexBuffer.GetElementStride();
    newMesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    newMesh.TheBoundingBox = BoundingBoxUtil::CalculateBoundingBox(mesh.GetVertexArray());
    return newMesh;
  }

  ObjectSelection::Mesh ObjectSelection::CreateMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, const Procedural::BasicMesh& mesh)
  {
    Mesh newMesh;

    newMesh.IndexBuffer.Reset(bufferManager, mesh.GetIndexArray(), Vulkan::VMBufferUsage::STATIC);
    newMesh.VertexBuffer.Reset(bufferManager, mesh.AsReadOnlyFlexVertexSpan(), Vulkan::VMBufferUsage::STATIC);

    std::array<VertexElementUsage, 3> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Normal,
                                                         VertexElementUsage::TextureCoordinate};

    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(newMesh.VertexAttributeDescription, shaderBindOrder, newMesh.VertexBuffer);
    newMesh.VertexInputBindingDescription.binding = 0;
    newMesh.VertexInputBindingDescription.stride = newMesh.VertexBuffer.GetElementStride();
    newMesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    newMesh.TheBoundingBox = BoundingBoxUtil::CalculateBoundingBox(mesh.GetVertexArray());
    return newMesh;
  }


  ObjectSelection::ProgramDirectionalLight ObjectSelection::PrepareDirectionalLightProgram(const VkDevice device,
                                                                                           const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramDirectionalLight newProgram;
    newProgram.VertShaderModule.Reset(device, 0, contentManager->ReadBytes("BasicShaderDLight.vert.spv"));
    newProgram.FragShaderModule.Reset(device, 0, contentManager->ReadBytes("BasicShaderDLightTextured.frag.spv"));
    return newProgram;
  }


  ObjectSelection::ProgramColor ObjectSelection::PrepareSolidColorProgram(const VkDevice device,
                                                                          const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramColor newProgram;
    newProgram.VertShaderModule.Reset(device, 0, contentManager->ReadBytes("VertexColored.vert.spv"));
    newProgram.FragShaderModule.Reset(device, 0, contentManager->ReadBytes("VertexColored.frag.spv"));
    return newProgram;
  }


  ObjectSelection::ProgramColor ObjectSelection::PrepareTexturedProgram(const VkDevice device, const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramColor newProgram;
    newProgram.VertShaderModule.Reset(device, 0, contentManager->ReadBytes("Textured.vert.spv"));
    newProgram.FragShaderModule.Reset(device, 0, contentManager->ReadBytes("Textured.frag.spv"));
    return newProgram;
  }


  void ObjectSelection::GenerateObjects(std::vector<MeshObject>& rObjects, const uint32_t objectCount, const bool useRandomSeed)
  {
    // Distribute the objects at random positions, scale and rotation
    rObjects.resize(objectCount);

    std::mt19937 random = useRandomSeed ? std::mt19937(std::random_device{}()) : std::mt19937(42);

    std::uniform_real_distribution<float> randomPositionX(-8, 8);
    std::uniform_real_distribution<float> randomPositionY(-5, 5);
    std::uniform_real_distribution<float> randomPositionZ(-50, -1);
    std::uniform_real_distribution<float> randomRotation(-1.0f, 1.0f);
    std::uniform_real_distribution<float> randomScale(0.15f, 0.4f);
    std::uniform_real_distribution<float> randomAngle(MathHelper::RADS0, MathHelper::RADS360);
    std::uniform_int_distribution<uint32_t> randomMesh(0, 2);


    for (auto& rEntry : rObjects)
    {
      rEntry.MeshIndex = randomMesh(random);
      rEntry.Position = Vector3(randomPositionX(random), randomPositionY(random), randomPositionZ(random));
      rEntry.RotationSpeed = Vector3(randomRotation(random), randomRotation(random), randomRotation(random));
      rEntry.Rotation = Vector3(randomAngle(random), randomAngle(random), randomAngle(random));
      rEntry.Scale = randomScale(random) * 2.0f;
    }

    // Sort the objects according to mesh index to make it simpler to render
    std::sort(rObjects.begin(), rObjects.end(), [](const MeshObject& lhs, const MeshObject& rhs) -> bool { return lhs.MeshIndex < rhs.MeshIndex; });
  }
}
