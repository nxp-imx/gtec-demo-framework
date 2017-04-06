/*
* Vulkan Example - Texture arrays and instanced rendering
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Texture Arrays' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "TexturingArrays.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslGraphicsVulkan1_0/Extend/Convert.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/ConvertUtil.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/MemoryTypeHelper.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <VulkanWillemsDemoAppExperimental/VulkanTool.hpp>
#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fsl
{
  using namespace Vulkan;
  using namespace Willems;

  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;
  }


  TexturingArrays::TexturingArrays(const DemoAppConfig& config)
    : VulkanWillemsDemoApp(config)
  {
    m_zoom = -15.0f;
    m_rotationSpeed = 0.25f;
    m_rotation = { -15.0f, 35.0f, 0.0f };

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Texture arrays";
  }


  TexturingArrays::~TexturingArrays()
  {

  }


  // This ensures the flow is the same as in the original sample
  void TexturingArrays::Prepare()
  {
    VulkanWillemsDemoApp::Prepare();
    SetupVertexDescriptions();
    LoadTextures();
    GenerateQuad();
    PrepareUniformBuffers();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    BuildCommandBuffers();
  }


  void TexturingArrays::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;


    VkClearValue clearValues[2];
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;

    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.width = static_cast<float>(screenExtent.width);
        viewport.height = static_cast<float>(screenExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = screenExtent;

        vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

        vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);

        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Quad.GetVertices().GetBufferPointer(), offsets);
        vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Quad.GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Solid.Get());

        vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Quad.GetIndexCount(), m_textureArray.GetLayers(), 0, 0, 0);

        vkCmdEndRenderPass(m_drawCmdBuffers[i]);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void TexturingArrays::OnViewChanged()
  {
    UpdateUniformBufferMatrices();
  }


  void TexturingArrays::Update(const DemoTime& demoTime)
  {
  }


  void TexturingArrays::Draw(const DemoTime& demoTime)
  {
    if (!TryPrepareFrame())
      return;

    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();
  }


  void TexturingArrays::SetupVertexDescriptions()
  {
    // Binding description
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = sizeof(Vertex);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(2);
    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = 0;

    // Location 1 : Texture coordinates
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = sizeof(float) * 3;

    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void TexturingArrays::LoadTextures()
  {
    if (m_deviceFeatures.textureCompressionBC)
    {
      //m_textureArray = m_textureLoader->LoadTextureArray("textures/texturearray_bc3.ktx", VK_FORMAT_BC3_UNORM_BLOCK);
      m_textureArray = LoadTextureArray("textures/texturearray_bc3.ktx", VK_FORMAT_BC3_UNORM_BLOCK);
    }
    else if (m_deviceFeatures.textureCompressionETC2)
    {
      m_textureArray = LoadTextureArray("textures/texturearray_etc2.ktx", VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK);
    }
    else
      throw NotSupportedException("No supported texture compression available");
  }


  VulkanTexture TexturingArrays::LoadTextureArray(const std::string& filename, const VkFormat format)
  {
    auto pixelFormat = ConvertUtil::Convert(format);
    auto textureArray = GetContentManager()->ReadTexture(filename, pixelFormat);
    auto texExtent = textureArray.GetExtent();
    texExtent.Depth = 1;

    //textureArray.width = tex2DArray.dimensions().x;
    //textureArray.height = tex2DArray.dimensions().y;
    //layerCount = tex2DArray.layers();

    // Create a host-visible staging buffer that contains the raw image data

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.size = textureArray.GetByteSize();
    // This buffer is used as a transfer source for the buffer copy
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Buffer stagingBuffer(m_device.Get(), bufferCreateInfo);

    // Get memory requirements for the staging buffer (alignment, memory type bits)
    VkMemoryRequirements memReqs = stagingBuffer.GetBufferMemoryRequirements();

    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.allocationSize = memReqs.size;
    // Get memory type index for a host visible buffer
    memAllocInfo.memoryTypeIndex = m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    Memory stagingMemory(m_device.Get(), memAllocInfo);

    FSLGRAPHICSVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), stagingBuffer.Get(), stagingMemory.Get(), 0));

    // Copy texture data into staging buffer
    void* pData;
    stagingMemory.MapMemory(0, memReqs.size, 0, &pData);
    {
      RawTexture rawTexture;
      Texture::ScopedDirectAccess directAccess(textureArray, rawTexture);
      std::memcpy(pData, rawTexture.GetContent(), rawTexture.GetContentByteSize());
    }
    stagingMemory.UnmapMemory();

    // Setup buffer copy regions for array layers
    std::vector<VkBufferImageCopy> bufferCopyRegions;

    // Check if all array layers have the same dimensions
    bool sameDims = true;
    // NOTE: the Texture class does not support multiple layers of different sizes
    //for (uint32_t layer = 0; layer < textureArray.GetLayers(); ++layer)
    //{
    //  if (tex2DArray[layer].dimensions().x != textureArray.width || tex2DArray[layer].dimensions().y != textureArray.height)
    //  {
    //    sameDims = false;
    //    break;
    //  }
    //}

    // If all layers of the texture array have the same dimensions, we only need to do one copy
    if (sameDims)
    {
      VkBufferImageCopy bufferCopyRegion = {};
      bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      bufferCopyRegion.imageSubresource.mipLevel = 0;
      bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
      bufferCopyRegion.imageSubresource.layerCount = textureArray.GetLayers();
      bufferCopyRegion.imageExtent = Convert(texExtent);
      bufferCopyRegion.bufferOffset = 0;

      bufferCopyRegions.push_back(bufferCopyRegion);
    }
    else
    {
      // If dimensions differ, copy layer by layer and pass offsets
      for (uint32_t layer = 0; layer < textureArray.GetLayers(); ++layer)
      {
        // FIX: texture does not currently support different layer sizes
        //const auto blobExtent = textureArray.GetExtent(0, layer);
        const auto blobExtent = textureArray.GetExtent(0);
        const auto blobRecord = textureArray.GetTextureBlob(0, 0, layer);
        VkBufferImageCopy bufferCopyRegion{};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = blobExtent.Width;
        bufferCopyRegion.imageExtent.height = blobExtent.Height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = blobRecord.Offset;

        bufferCopyRegions.push_back(bufferCopyRegion);
      }
    }

    // Create optimal tiled target image
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = textureArray.GetLayers();
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = Convert(texExtent);
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    Image texImage(m_device.Get(), imageCreateInfo);

    memReqs = texImage.GetImageMemoryRequirements();

    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Memory texMemory(m_device.Get(), memAllocInfo);
    FSLGRAPHICSVULKAN_CHECK(vkBindImageMemory(m_device.Get(), texImage.Get(), texMemory.Get(), 0));

    const VkImageLayout texImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    {
      CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

      // Image barrier for optimal image (target)
      // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
      VkImageSubresourceRange subresourceRange{};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.layerCount = textureArray.GetLayers();

      VulkanTool::SetImageLayout(copyCmd, texImage,
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

      // Copy the cube map faces from the staging buffer to the optimal tiled image
      vkCmdCopyBufferToImage(copyCmd.Get(), stagingBuffer.Get(), texImage.Get(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

      // Change texture image layout to shader read after all faces have been copied
      VulkanTool::SetImageLayout(copyCmd, texImage,
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        texImageLayout, subresourceRange);

      FlushCommandBuffer(copyCmd, m_deviceQueue.Queue, true);
    }

    // Create sampler
    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.pNext = nullptr;
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.addressModeV = sampler.addressModeU;
    sampler.addressModeW = sampler.addressModeU;
    sampler.mipLodBias = 0.0f;
    sampler.maxAnisotropy = 8;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = 0.0f;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    Sampler texSampler(m_device.Get(), sampler);

    // Create image view
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    view.format = format;
    view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    view.subresourceRange.layerCount = textureArray.GetLayers();
    view.image = texImage.Get();

    ImageView texImageView(m_device.Get(), view);

    VkDescriptorImageInfo texDescriptor{};
    texDescriptor.sampler = texSampler.Get();
    texDescriptor.imageView = texImageView.Get();
    texDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    // Transfer ownership to the texture object (move)
    return VulkanTexture(std::move(texSampler), std::move(texImage), texImageLayout, std::move(texMemory), std::move(texImageView),
                         texExtent, textureArray.GetLevels(), textureArray.GetLayers(), texDescriptor);
  }


  void TexturingArrays::GenerateQuad()
  {
    const float dim = 2.5f;
    std::vector<Vertex> vertexBuffer =
    {
      { { dim, dim, 0.0f }, { 1.0f, 1.0f } },
      { { -dim, dim, 0.0f }, { 0.0f, 1.0f } },
      { { -dim, -dim, 0.0f }, { 0.0f, 0.0f } },
      { { dim, -dim, 0.0f }, { 1.0f, 0.0f } }
    };

    MeshLoader::MeshBufferInfo meshVertexInfo;
    CreateBuffer(meshVertexInfo.Buffer, meshVertexInfo.Memory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 vertexBuffer.size() * sizeof(Vertex), vertexBuffer.data());

    // Setup indices
    const std::vector<uint32_t> indexBuffer = { 0, 1, 2, 2, 3, 0 };

    MeshLoader::MeshBufferInfo meshIndexInfo;
    CreateBuffer(meshIndexInfo.Buffer, meshIndexInfo.Memory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      indexBuffer.size() * sizeof(uint32_t), indexBuffer.data());

    std::vector<MeshLoader::MeshDescriptor> meshDescriptors;
    meshDescriptors.push_back(MeshLoader::MeshDescriptor(static_cast<uint32_t>(vertexBuffer.size()), 0, static_cast<uint32_t>(indexBuffer.size())));

    m_meshes.Quad.Reset(std::move(meshDescriptors), std::move(meshVertexInfo), std::move(meshIndexInfo),
                        static_cast<uint32_t>(indexBuffer.size()), glm::vec3(dim));
  }


  void TexturingArrays::PrepareUniformBuffers()
  {
    const auto layerCount = m_textureArray.GetLayers();
    m_uboVS.Instance.resize(layerCount);

    const uint32_t uboSize = sizeof(m_uboVS.Matrices) + (layerCount * sizeof(UboInstanceData));

    // Vertex shader uniform buffer block
    CreateBuffer(m_uniformData.VertexShader.Buffer, m_uniformData.VertexShader.Memory, m_uniformData.VertexShader.Descriptor,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      uboSize, &m_uboVS);

    // Array indices and model matrices are fixed
    const float offset = -1.5f;
    float center = (layerCount*offset) / 2;
    for (uint32_t i = 0; i < layerCount; ++i)
    {
      // Instance model matrix
      m_uboVS.Instance[i].Model = glm::translate(glm::mat4(), glm::vec3(0.0f, i * offset - center, 0.0f));
      m_uboVS.Instance[i].Model = glm::rotate(m_uboVS.Instance[i].Model, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      // Instance texture array index
      m_uboVS.Instance[i].ArrayIndex.x = static_cast<float>(i);
    }

    // Update instanced part of the uniform buffer
    void* pData;
    uint32_t dataOffset = sizeof(m_uboVS.Matrices);
    uint32_t dataSize = layerCount * sizeof(UboInstanceData);

    m_uniformData.VertexShader.Memory.MapMemory(dataOffset, dataSize, 0, &pData);
    {
      std::memcpy(pData, m_uboVS.Instance.data(), dataSize);
    }
    m_uniformData.VertexShader.Memory.UnmapMemory();

    UpdateUniformBufferMatrices();
  }


  void TexturingArrays::UpdateUniformBufferMatrices()
  {
    const auto screenExtent = GetScreenExtent();
    // Only updates the uniform buffer block part containing the global matrices

    // Projection
    const float aspect = static_cast<float>(screenExtent.Width) / static_cast<float>(screenExtent.Height);
    m_uboVS.Matrices.Projection = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);

    // View
    m_uboVS.Matrices.View = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, m_zoom));
    m_uboVS.Matrices.View = glm::rotate(m_uboVS.Matrices.View, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboVS.Matrices.View = glm::rotate(m_uboVS.Matrices.View, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboVS.Matrices.View = glm::rotate(m_uboVS.Matrices.View, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Only update the matrices part of the uniform buffer
    {
      void* pData;
      m_uniformData.VertexShader.Memory.MapMemory(0, sizeof(m_uboVS.Matrices), 0, &pData);
      std::memcpy(pData, &m_uboVS.Matrices, sizeof(m_uboVS.Matrices));
      m_uniformData.VertexShader.Memory.UnmapMemory();
    }
  }


  void TexturingArrays::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
    // Binding 0 : Vertex shader uniform buffer
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // Binding 1 : Fragment shader image sampler (texture array)
    setLayoutBindings[1].binding = 1;
    setLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[1].descriptorCount = 1;
    setLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    m_pipelineLayout.Reset(m_device.Get(), pipelineLayoutCreateInfo);
  }


  void TexturingArrays::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_FALSE;
    blendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
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

    // Instacing pipeline
    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0] = LoadShader("shaders/instancing.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/instancing.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.layout = m_pipelineLayout.Get();
    pipelineCreateInfo.renderPass = m_renderPass.Get();
    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    m_pipelines.Solid.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void TexturingArrays::SetupDescriptorPool()
  {
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void TexturingArrays::SetupDescriptorSet()
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    FSLGRAPHICSVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(2);
    // Binding 0 : Vertex shader uniform buffer
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.VertexShader.Descriptor;
    // Binding 1 : Fragment shader cubemap sampler
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = m_textureArray.GetImageDescriptorPointer();

    vkUpdateDescriptorSets(m_device.Get(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }
}
