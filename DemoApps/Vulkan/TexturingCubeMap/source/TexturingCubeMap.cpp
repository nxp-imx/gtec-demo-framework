/*
 * Vulkan Example - Cube map texture loading and displaying
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called '(Texture mapping) Cube maps' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)


#include "TexturingCubeMap.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Memory.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace Fsl
{
  namespace
  {
    const uint32_t VERTEX_BUFFER_BIND_ID = 0;

    std::vector<Willems::MeshLoader::VertexLayout> g_vertexLayout = {Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_POSITION,
                                                                     Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_NORMAL,
                                                                     Willems::MeshLoader::VertexLayout::VERTEX_LAYOUT_UV};
  }


  TexturingCubeMap::TexturingCubeMap(const DemoAppConfig& config)
    : VulkanWillemsMeshDemoApp(config)
    , m_displaySkybox(true)
  {
    m_zoom = -4.0f;
    m_rotationSpeed = 0.25f;
    m_rotation = {-7.25f, -120.0f, 0.0f};

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Cube map";
  }


  TexturingCubeMap::~TexturingCubeMap()
  {
    SafeWaitForDeviceIdle();
  }


  // This ensures the flow is the same as in the original sample
  void TexturingCubeMap::Prepare()
  {
    VulkanWillemsMeshDemoApp::Prepare();

    LoadMeshes();
    SetupVertexDescriptions();
    PrepareUniformBuffers();

    // m_cubeMap = m_textureLoader->LoadCubemap("textures/cubemap_yokohama.ktx", VK_FORMAT_BC3_UNORM_BLOCK);
    if (m_deviceActiveFeatures.textureCompressionBC != VK_FALSE)
    {
      m_cubeMap = LoadCubemap("Textures/Cubemap/Yokohama3/Yokohama3_1024_bc3.ktx", VK_FORMAT_BC3_UNORM_BLOCK, false);
    }
    else if (m_deviceActiveFeatures.textureCompressionETC2 != VK_FALSE)
    {
      m_cubeMap = LoadCubemap("Textures/Cubemap/Yokohama3/Yokohama3_1024_etc2.ktx", VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, false);
    }
    else
    {
      throw NotSupportedException("No supported compression format");
    }


    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSets();
    BuildCommandBuffers();
  }


  void TexturingCubeMap::GetOverlayText(Willems::VulkanTextOverlay& rTextOverlay)
  {
    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed << m_uboVS.LodBias;
    rTextOverlay.AddText("Press \"s\" to toggle skybox", 5.0f, 85.0f, Willems::VulkanTextOverlay::TextAlign::Left);
    rTextOverlay.AddText("Press \"space\" to toggle object", 5.0f, 100.0f, Willems::VulkanTextOverlay::TextAlign::Left);
    rTextOverlay.AddText("LOD bias: " + ss.str() + " (numpad +/- to change)", 5.0f, 115.0f, Willems::VulkanTextOverlay::TextAlign::Left);
  }


  void TexturingCubeMap::BuildCommandBuffers()
  {
    const auto screenExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

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

        VkDeviceSize offsets = 0;

        // Skybox
        if (m_displaySkybox)
        {
          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSets.Skybox, 0,
                                  nullptr);
          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Skybox.GetVertices().GetBufferPointer(), &offsets);
          vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Skybox.GetIndices().Buffer.Get(), 0, VK_INDEX_TYPE_UINT32);
          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Skybox.Get());
          vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Skybox.GetIndexCount(), 1, 0, 0, 0);
        }

        // 3D object
        vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSets.Object, 0,
                                nullptr);
        vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_meshes.Objects[m_meshes.ObjectIndex].GetVertices().GetBufferPointer(),
                               &offsets);
        vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_meshes.Objects[m_meshes.ObjectIndex].GetIndices().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines.Reflect.Get());
        vkCmdDrawIndexed(m_drawCmdBuffers[i], m_meshes.Objects[m_meshes.ObjectIndex].GetIndexCount(), 1, 0, 0, 0);

        DrawUI(m_drawCmdBuffers[i]);

        vkCmdEndRenderPass(m_drawCmdBuffers[i]);
      }
      m_drawCmdBuffers.End(i);
    }
  }


  void TexturingCubeMap::OnViewChanged()
  {
    UpdateUniformBuffers();
  }


  void TexturingCubeMap::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }


    if (event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::UpArrow:
      case VirtualKey::Add:
      case VirtualKey::GamePadButtonRightShoulder:
        ChangeLodBias(0.1f);
        break;
      case VirtualKey::DownArrow:
      case VirtualKey::Subtract:
      case VirtualKey::GamePadButtonLeftShoulder:
        ChangeLodBias(-0.1f);
        break;
      case VirtualKey::S:
      case VirtualKey::GamePadButtonA:
        ToggleSkyBox();
        break;
      case VirtualKey::Space:
      case VirtualKey::GamePadButtonX:
        ToggleObject();
        break;
      default:
        break;
      }
    }

    VulkanWillemsMeshDemoApp::OnKeyEvent(event);
  }

  void TexturingCubeMap::Update(const DemoTime& /*demoTime*/)
  {
  }


  void TexturingCubeMap::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    if (!TryPrepareFrame())
    {
      return;
    }

    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();
  }


  void TexturingCubeMap::LoadMeshes()
  {
    // Skybox
    m_meshes.Skybox = LoadMesh("Models/Cube/cube.obj", g_vertexLayout, 0.05f);
    // Objects
    m_meshes.Objects.resize(3);
    m_meshes.Objects[0] = LoadMesh("Models/Sphere/sphere.obj", g_vertexLayout, 0.05f);
    m_meshes.Objects[1] = LoadMesh("Models/Teapot/teapot.dae", g_vertexLayout, 0.05f);
    m_meshes.Objects[2] = LoadMesh("Models/TorusKnot/torusknot.obj", g_vertexLayout, 0.05f);
  }


  void TexturingCubeMap::SetupVertexDescriptions()
  {
    // Binding description
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = Willems::MeshLoader::VertexSize(g_vertexLayout);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(3);
    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = 0;

    // Location 1 : Normal
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = sizeof(float) * 3;

    // Location 2 : Texture coordinates
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = sizeof(float) * 5;

    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = UncheckedNumericCast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }


  void TexturingCubeMap::PrepareUniformBuffers()
  {
    // 3D object
    CreateBuffer(m_uniformData.ObjectVS.Buffer, m_uniformData.ObjectVS.Memory, m_uniformData.ObjectVS.Descriptor, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboVS), nullptr);

    // Skybox
    CreateBuffer(m_uniformData.SkyboxVS.Buffer, m_uniformData.SkyboxVS.Memory, m_uniformData.SkyboxVS.Descriptor, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboVS), nullptr);

    UpdateUniformBuffers();
  }


  void TexturingCubeMap::UpdateUniformBuffers()
  {
    const auto screenExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());

    const float aspect = static_cast<float>(screenExtent.width) / static_cast<float>(screenExtent.height);

    // 3D object
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    m_uboVS.Projection = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, m_zoom));

    m_uboVS.Model = glm::mat4(1.0f);
    m_uboVS.Model = viewMatrix * glm::translate(m_uboVS.Model, m_cameraPos);
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    void* pData = nullptr;
    m_uniformData.ObjectVS.Memory.MapMemory(0, sizeof(m_uboVS), 0, &pData);
    {
      if (pData == nullptr)
      {
        throw std::runtime_error("failed to map memory");
      }

      std::memcpy(pData, &m_uboVS, sizeof(m_uboVS));
    }
    m_uniformData.ObjectVS.Memory.UnmapMemory();

    // Skybox
    viewMatrix = glm::mat4(1.0f);
    m_uboVS.Projection = glm::perspective(glm::radians(60.0f), aspect, 0.001f, 256.0f);

    m_uboVS.Model = glm::mat4(1.0f);
    m_uboVS.Model = viewMatrix * glm::translate(m_uboVS.Model, glm::vec3(0, 0, 0));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    m_uniformData.SkyboxVS.Memory.MapMemory(0, sizeof(m_uboVS), 0, &pData);
    {
      std::memcpy(pData, &m_uboVS, sizeof(m_uboVS));
    }
    m_uniformData.SkyboxVS.Memory.UnmapMemory();
  }


  Willems::VulkanTexture TexturingCubeMap::LoadCubemap(const IO::Path& filename, const VkFormat format, const bool /*forceLinearTiling*/)
  {
    Texture texCube = GetContentManager()->ReadTexture(filename);

    assert(texCube.GetFaces() == 6);

    PxExtent3D texExtent(texCube.GetExtent());
    texExtent.Depth = 1;

    // Create a host-visible staging buffer that contains the raw image data
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.size = texCube.GetByteSize();
    // This buffer is used as a transfer source for the buffer copy
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    RapidVulkan::Buffer stagingBuffer(m_device.Get(), bufferCreateInfo);

    // Get memory requirements for the staging buffer (alignment, memory type bits)
    VkMemoryRequirements memReqs = stagingBuffer.GetBufferMemoryRequirements();

    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.allocationSize = memReqs.size;
    // Get memory type index for a host visible buffer
    memAllocInfo.memoryTypeIndex =
      m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    RapidVulkan::Memory stagingMemory(m_device.Get(), memAllocInfo);

    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), stagingBuffer.Get(), stagingMemory.Get(), 0));

    // Copy texture data into staging buffer
    void* pData = nullptr;
    stagingMemory.MapMemory(0, memReqs.size, 0, &pData);
    {
      if (pData == nullptr)
      {
        throw std::runtime_error("Failed to map memory");
      }
      RawTexture rawTexture;
      Texture::ScopedDirectAccess directAccess(texCube, rawTexture);
      assert(rawTexture.GetByteSize() <= memReqs.size);
      std::memcpy(pData, rawTexture.GetContent(), rawTexture.GetByteSize());
    }
    stagingMemory.UnmapMemory();


    // Create optimal tiled target image
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = texCube.GetLevels();
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = TypeConverter::UncheckedTo<VkExtent3D>(texExtent);
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // Cube faces count as array layers in Vulkan
    imageCreateInfo.arrayLayers = texCube.GetFaces();
    // This flag is required for cube map images
    imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

    RapidVulkan::Image texImage(m_device.Get(), imageCreateInfo);

    memReqs = texImage.GetImageMemoryRequirements();

    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = m_vulkanDevice.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    RapidVulkan::Memory texMemory(m_device.Get(), memAllocInfo);
    RAPIDVULKAN_CHECK(vkBindImageMemory(m_device.Get(), texImage.Get(), texMemory.Get(), 0));

    RapidVulkan::CommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    // Setup buffer copy regions for each face including all of it's miplevels
    std::vector<VkBufferImageCopy> bufferCopyRegions;

    for (uint32_t face = 0; face < texCube.GetFaces(); ++face)
    {
      for (uint32_t level = 0; level < texCube.GetLevels(); ++level)
      {
        const auto textureBlob = texCube.GetTextureBlob(level, face);
        const auto extent = texCube.GetExtent(level);

        VkBufferImageCopy bufferCopyRegion{};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = level;
        bufferCopyRegion.imageSubresource.baseArrayLayer = face;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent = TypeConverter::UncheckedTo<VkExtent3D>(extent);
        bufferCopyRegion.bufferOffset = textureBlob.Offset;

        bufferCopyRegions.push_back(bufferCopyRegion);
      }
    }

    // Image barrier for optimal image (target)
    // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = texCube.GetLevels();
    subresourceRange.layerCount = texCube.GetFaces();

    Vulkan::CommandBufferUtil::SetImageLayout(copyCmd.Get(), texImage.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

    // Copy the cube map faces from the staging buffer to the optimal tiled image
    vkCmdCopyBufferToImage(copyCmd.Get(), stagingBuffer.Get(), texImage.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           UncheckedNumericCast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

    const auto texImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Change texture image layout to shader read after all faces have been copied
    Vulkan::CommandBufferUtil::SetImageLayout(copyCmd.Get(), texImage.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                              texImageLayout, subresourceRange);

    FlushCommandBuffer(copyCmd, m_deviceQueue.Queue, true);

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
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = static_cast<float>(texCube.GetLevels());
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler.maxAnisotropy = 1.0f;
    if (m_deviceActiveFeatures.samplerAnisotropy != VK_FALSE)
    {
      sampler.maxAnisotropy = m_vulkanDevice.GetProperties().limits.maxSamplerAnisotropy;
      sampler.anisotropyEnable = VK_TRUE;
    }
    RapidVulkan::Sampler texSampler(m_device.Get(), sampler);

    // Create image view
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;
    // Cube map view type
    view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    view.format = format;
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    // 6 array layers (faces)
    view.subresourceRange.layerCount = texCube.GetFaces();
    // Set number of mip levels
    view.subresourceRange.levelCount = texCube.GetLevels();
    view.image = texImage.Get();

    RapidVulkan::ImageView texImageView(m_device.Get(), view);

    VkDescriptorImageInfo descriptor{};
    descriptor.sampler = texSampler.Get();
    descriptor.imageView = texImageView.Get();
    descriptor.imageLayout = texImageLayout;

    return {std::move(texSampler),
            std::move(texImage),
            texImageLayout,
            std::move(texMemory),
            std::move(texImageView),
            texExtent,
            texCube.GetLevels(),
            1,
            descriptor};
  }


  void TexturingCubeMap::SetupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
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
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = UncheckedNumericCast<uint32_t>(setLayoutBindings.size());
    descriptorLayout.pBindings = setLayoutBindings.data();

    m_descriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    m_pipelineLayout.Reset(m_device.Get(), pipelineLayoutCreateInfo);
  }


  void TexturingCubeMap::PreparePipelines()
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
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
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
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;
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

    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = UncheckedNumericCast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();


    // Skybox pipeline (background cube)
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0] = LoadShader("shaders/skybox.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/skybox.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

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
    pipelineCreateInfo.stageCount = UncheckedNumericCast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    m_pipelines.Skybox.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);

    // Cube map reflect pipeline
    shaderStages[0] = LoadShader("shaders/reflect.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/reflect.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    // Enable depth test and write
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthTestEnable = VK_TRUE;
    // Flip cull mode
    rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;

    m_pipelines.Reflect.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void TexturingCubeMap::SetupDescriptorPool()
  {
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 2;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 2;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void TexturingCubeMap::SetupDescriptorSets()
  {
    // Image descriptor for the cube map texture
    VkDescriptorImageInfo cubeMapDescriptor{};
    cubeMapDescriptor.sampler = m_cubeMap.GetSampler();
    cubeMapDescriptor.imageView = m_cubeMap.GetImageView();
    cubeMapDescriptor.imageLayout = m_cubeMap.GetImageLayout();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    // 3D object descriptor set
    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSets.Object));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(2);
    // Binding 0 : Vertex shader uniform buffer
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSets.Object;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.ObjectVS.Descriptor;

    // Binding 1 : Fragment shader cubemap sampler
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSets.Object;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = &cubeMapDescriptor;

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

    // Sky box descriptor set
    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSets.Skybox));

    // Binding 0 : Vertex shader uniform buffer
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSets.Skybox;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformData.SkyboxVS.Descriptor;
    // Binding 1 : Fragment shader cubemap sampler
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSets.Skybox;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = &cubeMapDescriptor;

    vkUpdateDescriptorSets(m_device.Get(), UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }


  void TexturingCubeMap::ReBuildCommandBuffers()
  {
    if (!CheckCommandBuffers())
    {
      DestroyCommandBuffers();
      CreateCommandBuffers();
    }
    BuildCommandBuffers();
  }


  void TexturingCubeMap::ToggleSkyBox()
  {
    m_displaySkybox = !m_displaySkybox;
    ReBuildCommandBuffers();
  }


  void TexturingCubeMap::ToggleObject()
  {
    ++m_meshes.ObjectIndex;
    if (m_meshes.ObjectIndex >= UncheckedNumericCast<uint32_t>(m_meshes.Objects.size()))
    {
      m_meshes.ObjectIndex = 0;
    }
    ReBuildCommandBuffers();
  }


  void TexturingCubeMap::ChangeLodBias(const float delta)
  {
    m_uboVS.LodBias += delta;
    if (m_uboVS.LodBias < 0.0f)
    {
      m_uboVS.LodBias = 0.0f;
    }
    if (m_uboVS.LodBias > static_cast<float>(m_cubeMap.GetLevels()))
    {
      m_uboVS.LodBias = static_cast<float>(m_cubeMap.GetLevels());
    }
    UpdateUniformBuffers();
    UpdateTextOverlay();
  }
}
