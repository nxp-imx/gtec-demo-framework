/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include "ModelLoaderBasics.hpp"
#include <FslAssimp/SceneImporter.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MatrixConverter.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh.hpp>
#include <FslGraphics3D/BasicScene/GenericScene.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  using namespace Graphics3D;

  using TestMesh = GenericMesh<VertexPositionColorNormalTexture, uint16_t>;
  using TestScene = GenericScene<TestMesh>;
  namespace
  {
    const float DEFAULT_ZOOM = 10;
    const float DEFAULT_MODEL_SCALE = 5;

    const uint32_t VERTEX_BUFFER_BIND_ID = 0;

    VulkanBasic::DemoAppVulkanSetup CreateSetup()
    {
      using namespace VulkanBasic;

      DemoAppVulkanSetup setup;
      setup.DepthBuffer = DepthBufferMode::Enabled;
      return setup;
    }


    Vulkan::VUTexture CreateTexture(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue, const Bitmap& bitmap,
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
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

      return imageCreator.CreateTexture(bitmap, samplerCreateInfo);
    }

    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const Vulkan::VUDevice& device)
    {
      std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings{};
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

      VkDescriptorSetLayoutCreateInfo descriptorLayout{};
      descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
      descriptorLayout.pBindings = setLayoutBindings.data();

      return RapidVulkan::DescriptorSetLayout(device.Get(), descriptorLayout);
    }

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count;

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

      VkDescriptorSet descriptorSet;
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
      // Binding 0 : Vertex shader uniform buffer
      auto vertUboBufferInfo = vertUboBuffer.GetDescriptorBufferInfo();
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      writeDescriptorSets[0].pBufferInfo = &vertUboBufferInfo;

      // Binding 1 : Fragment shader texture sampler
      auto textureImageInfo = texture.GetDescriptorImageInfo();
      writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[1].dstSet = descriptorSet;
      writeDescriptorSets[1].dstBinding = 1;
      writeDescriptorSets[1].descriptorCount = 1;
      writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[1].pImageInfo = &textureImageInfo;

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
                                                 const ModelMesh& mesh, const VkRenderPass renderPass, const uint32_t subpass)
    {
      assert(pipelineLayout.IsValid());
      assert(vertexShaderModule != VK_NULL_HANDLE);
      assert(fragmentShaderModule != VK_NULL_HANDLE);
      assert(renderPass != VK_NULL_HANDLE);
      assert(!mesh.VertexAttributeDescription.empty());

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
      pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &mesh.VertexInputBindingDescription;
      pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mesh.VertexAttributeDescription.size());
      pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = mesh.VertexAttributeDescription.data();

      VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
      pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
      graphicsPipelineCreateInfo.pDynamicState = nullptr;
      graphicsPipelineCreateInfo.layout = pipelineLayout.Get();
      graphicsPipelineCreateInfo.renderPass = renderPass;
      graphicsPipelineCreateInfo.subpass = subpass;
      graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
      graphicsPipelineCreateInfo.basePipelineIndex = 0;

      return RapidVulkan::GraphicsPipeline(pipelineLayout.GetDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    }
  }


  ModelLoaderBasics::ModelLoaderBasics(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config, CreateSetup())
    , m_rotationSpeed(0.0f, -0.6f, 0.0f)
    , m_uboData(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4(0.8f, 0.8f, 0.8f, 1.0f), Vector4(0.2f, 0.2f, 0.2f, 1.0f))
  {
    m_uboData.LightDirection.Normalize();
    auto contentManager = GetContentManager();
    auto contentPath = contentManager->GetContentPath();

    // Load the texture
    FSLLOG3_INFO("Loading texture");
    {
      Bitmap bitmap;
      contentManager->Read(bitmap, "Models/Knight2/armor_default_color.jpg", PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::LowerLeft);
      FSLLOG3_INFO("TODO: Add mip map support");
      // GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_resources.Texture = CreateTexture(m_device, m_deviceQueue, bitmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    FSLLOG3_INFO("Loading scene");
    // Load the scene
    // Default is Fast
    // aiProcessPreset_TargetRealtime_Fast
    // aiProcessPreset_TargetRealtime_Quality
    // aiProcessPreset_TargetRealtime_MaxQuality
    auto modelPath = IO::Path::Combine(contentPath, "Models/Knight2/armor.obj");
    SceneImporter sceneImporter;
    const std::shared_ptr<TestScene> scene = sceneImporter.Load<TestScene>(modelPath, DEFAULT_MODEL_SCALE, true);

    if (scene->GetMeshCount() <= 0)
    {
      throw NotSupportedException("Scene did not contain any meshes");
    }

    auto rootNode = scene->GetRootNode();
    if (!rootNode)
    {
      throw NotSupportedException("Scene did not contain a root node");
    }

    FSLLOG3_INFO("Preparing");
    // Create index and vertex buffers for all the meshes.
    {
      std::size_t vertexCount = 0;
      std::size_t indexCount = 0;
      for (const auto& mesh : scene->Meshes)
      {
        vertexCount += mesh->GetVertexCount();
        indexCount += mesh->GetIndexCount();
      }
      FSLLOG3_INFO("Total vertex count: {}, Total index count: {}, SubMesh count: {}", vertexCount, indexCount, scene->GetMeshCount());

      // copy the vertices and indices to one large buffer
      std::vector<VertexPositionColorNormalTexture> vertices(vertexCount);
      std::vector<uint16_t> indices(indexCount);
      {
        m_resources.Mesh.SubMeshes.resize(scene->GetMeshCount());
        uint32_t vertexOffset = 0u;
        uint32_t indexOffset = 0u;
        const auto meshCount = scene->GetMeshCount();
        for (int32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
          const auto& mesh = scene->Meshes[meshIndex];
          const auto meshVertexCount = mesh->GetVertexCount();
          const auto meshIndexCount = mesh->GetIndexCount();
          m_resources.Mesh.SubMeshes[meshIndex] = SubMeshInfo(vertexOffset, indexOffset, meshVertexCount, meshIndexCount);
          const auto& srcVertices = mesh->GetVertexArray();
          auto startVertexOffset = vertexOffset;
          for (std::size_t i = 0; i < meshVertexCount; ++i)
          {
            vertices[vertexOffset] = srcVertices[i];
            ++vertexOffset;
          }
          const auto& srcIndices = mesh->GetIndexArray();
          for (std::size_t i = 0; i < meshIndexCount; ++i)
          {
            indices[indexOffset] = startVertexOffset + srcIndices[i];
            ++indexOffset;
          }
        }
      }

      m_resources.BufferManager =
        std::make_shared<Vulkan::VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

      std::array<VertexElementUsage, 4> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::Color, VertexElementUsage::Normal,
                                                           VertexElementUsage::TextureCoordinate};

      m_resources.Mesh.VertexBuffer.Reset(m_resources.BufferManager, vertices, Vulkan::VMBufferUsage::STATIC);
      m_resources.Mesh.IndexBuffer.Reset(m_resources.BufferManager, indices, Vulkan::VMBufferUsage::STATIC);

      Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(m_resources.Mesh.VertexAttributeDescription, shaderBindOrder,
                                                                      m_resources.Mesh.VertexBuffer);
      m_resources.Mesh.VertexInputBindingDescription.binding = 0;
      m_resources.Mesh.VertexInputBindingDescription.stride = m_resources.Mesh.VertexBuffer.GetElementStride();
      m_resources.Mesh.VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    {    // Prepare shaders
      m_resources.VertShader.Reset(m_device.Get(), 0, contentManager->ReadBytes("BasicShaderDLight.vert.spv"));
      m_resources.FragShader.Reset(m_device.Get(), 0, contentManager->ReadBytes("BasicShaderDLightTextured.frag.spv"));
    }

    m_resources.MainDescriptorSetLayout = CreateDescriptorSetLayout(m_device);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.MainDescriptorPool = CreateDescriptorPool(m_device, maxFramesInFlight);

    m_resources.MainFrameResources.resize(maxFramesInFlight);
    for (auto& rFrame : m_resources.MainFrameResources)
    {
      rFrame.UboBuffer = CreateUBO(m_device, sizeof(UBOData));
      rFrame.DescriptorSet = CreateDescriptorSet(m_resources.MainDescriptorPool, m_resources.MainDescriptorSetLayout);
      UpdateDescriptorSet(m_device.Get(), rFrame.DescriptorSet, rFrame.UboBuffer, m_resources.Texture);
    }

    m_resources.MainPipelineLayout = CreatePipelineLayout(m_resources.MainDescriptorSetLayout);

    FSLLOG3_INFO("Ready");
  }


  void ModelLoaderBasics::Update(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    m_rotation.X += m_rotationSpeed.X * demoTime.DeltaTime;
    m_rotation.Y += m_rotationSpeed.Y * demoTime.DeltaTime;
    m_rotation.Z += m_rotationSpeed.Z * demoTime.DeltaTime;
    m_matrixWorld = Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    m_matrixView = Matrix::CreateTranslation(0, 0, -DEFAULT_ZOOM);

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = Vulkan::MatrixUtil::GetClipMatrix();

    // The ordering in the monogame based Matrix library is the reverse of glm (so perspective * clip instead of clip * perspective)
    m_matrixProjection =
      Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / static_cast<float>(screenResolution.Y), 1, 1000.0f) *
      vulkanClipMatrix;

    // Update Vertex UBO
    auto matWorldView = m_matrixWorld * m_matrixView;
    m_uboData.MatWorldViewProjection = matWorldView * m_matrixProjection;
    // m_uboData.NormalMatrix = Matrix3::Transpose(Matrix3::Invert(MatrixConverter::ToMatrix3(matWorldView)));
    m_uboData.NormalMatrix = Matrix::Transpose(Matrix::Invert(matWorldView));
  }

  void ModelLoaderBasics::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t frameIndex = drawContext.CurrentFrameIndex;
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    // Upload the changes
    m_resources.MainFrameResources[frameIndex].UboBuffer.Upload(0, &m_uboData, sizeof(UBOData));

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {0.5f, 0.5f, 0.5f, 1.0f};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        DrawMeshes(m_resources.MainFrameResources[frameIndex], hCmdBuffer);
        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass ModelLoaderBasics::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_dependentResources.Pipeline = CreatePipeline(m_resources.MainPipelineLayout, context.SwapchainImageExtent, m_resources.VertShader.Get(),
                                                   m_resources.FragShader.Get(), m_resources.Mesh, m_dependentResources.MainRenderPass.Get(), 0);

    return m_dependentResources.MainRenderPass.Get();
  }


  void ModelLoaderBasics::OnFreeResources()
  {
    m_dependentResources = {};
  }


  void ModelLoaderBasics::DrawMeshes(const FrameResources& frame, const VkCommandBuffer hCmdBuffer)
  {
    vkCmdBindDescriptorSets(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_resources.MainPipelineLayout.Get(), 0, 1, &frame.DescriptorSet, 0,
                            nullptr);

    vkCmdBindPipeline(hCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_dependentResources.Pipeline.Get());

    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(hCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, m_resources.Mesh.VertexBuffer.GetBufferPointer(), offsets);
    vkCmdBindIndexBuffer(hCmdBuffer, m_resources.Mesh.IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(hCmdBuffer, m_resources.Mesh.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
  }
}
