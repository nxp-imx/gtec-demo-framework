/*
 * Vulkan Example - Texture loading (and display) example (including mip maps)
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'Texture' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)


#include "Texturing.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;
  using namespace Vulkan::ConvertUtil;
  using namespace Willems;

  namespace
  {
    const auto VERTEX_BUFFER_BIND_ID = 0;
    const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;


    struct Vertex
    {
      float pos[3];
      float uv[2];
      float normal[3];
    };

    //! @brief Create a buffer on the device
    //! @param usageFlags Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer)
    //! @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
    //! @param buffer Pointer to a vk::Vulkan buffer object
    //! @param size Size of the buffer in byes
    //! @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
    //! @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
    VkResult DoCreateBuffer(BufferData& rBuffer, const VUPhysicalDeviceRecord& physicalDevice, VkDevice device, VkBufferUsageFlags usageFlags,
                            VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, void* data = nullptr)
    {
      using namespace MemoryTypeUtil;
      const VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.MemoryProperties;

      // Create the buffer handle
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.pNext = nullptr;
      bufferCreateInfo.usage = usageFlags;
      bufferCreateInfo.size = size;
      bufferCreateInfo.flags = 0;

      rBuffer.Buffer.Reset(device, bufferCreateInfo);

      VkMemoryRequirements memReqs = rBuffer.Buffer.GetBufferMemoryRequirements();

      // Create the memory backing up the buffer handle
      VkMemoryAllocateInfo memAlloc{};
      memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAlloc.allocationSize = memReqs.size;
      // Find a memory type index that fits the properties of the buffer
      memAlloc.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, memoryPropertyFlags);
      rBuffer.Memory.Reset(device, memAlloc);

      rBuffer.Alignment = memReqs.alignment;
      rBuffer.Size = memAlloc.allocationSize;
      rBuffer.usageFlags = usageFlags;
      rBuffer.memoryPropertyFlags = memoryPropertyFlags;

      // If a pointer to the buffer data has been passed, map the buffer and copy over the data
      if (data != nullptr)
      {
        RAPIDVULKAN_CHECK(vkMapMemory(device, rBuffer.Memory.Get(), 0, VK_WHOLE_SIZE, 0, &rBuffer.pMapped));
        std::memcpy(rBuffer.pMapped, data, size);
        vkUnmapMemory(device, rBuffer.Memory.Get());
      }

      // Initialize a default descriptor that covers the whole buffer size
      rBuffer.Descriptor.offset = 0;
      rBuffer.Descriptor.buffer = rBuffer.Buffer.Get();
      rBuffer.Descriptor.range = VK_WHOLE_SIZE;

      // Attach the memory to the buffer object
      return vkBindBufferMemory(device, rBuffer.Buffer.Get(), rBuffer.Memory.Get(), 0);
    }
  }

  Texturing::Texturing(const DemoAppConfig& config)
    : VulkanWillemsDemoApp(config)
    , m_indexCount(0)
    , m_descriptorSet(VK_NULL_HANDLE)
  {
    FSLLOG("Texturing app creating");
    m_zoom = -2.5f;
    m_rotation = glm::vec3(0.0f, 15.0f, 0.0f);

    m_enableTextOverlay = true;
    m_title = "Vulkan Example - Texturing";

    FSLLOG("Texturing app created");
  }


  Texturing::~Texturing()
  {
    SafeWaitForDeviceIdle();
  }


  // This ensures the flow is the same as in the original sample
  void Texturing::Prepare()
  {
    VulkanWillemsDemoApp::Prepare();

    GenerateQuad();
    SetupVertexDescriptions();
    PrepareUniformBuffers();

    bool forceLinearTiling = false;
    if (m_deviceFeatures.textureCompressionBC != VK_FALSE)
    {
      LoadTexture("Textures/Pattern02/pattern_02_bc2.ktx", forceLinearTiling);
      // LoadTexture("Textures/Checkerboard/checkerboard_nomips_rgba.ktx", VK_FORMAT_BC2_UNORM_BLOCK, false);
    }
    else if (m_deviceFeatures.textureCompressionETC2 != VK_FALSE)
    {
      LoadTexture("Textures/Pattern02/pattern_02_etc2.ktx", forceLinearTiling);
    }
    else
    {
      throw NotSupportedException("No supported compression format available");
    }

    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    BuildCommandBuffers();
  }


  void Texturing::GetOverlayText(VulkanTextOverlay& rTextOverlay)
  {
    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed << m_uboVS.LodBias;
    rTextOverlay.AddText("LOD bias: " + ss.str() + " (numpad +/- to change)", 5.0f, 85.0f, VulkanTextOverlay::TextAlign::Left);
  }


  void Texturing::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkClearValue clearValues[2];
    clearValues[0].color = m_defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    VkViewport viewport{};
    viewport.width = static_cast<float>(screenExtent.width);
    viewport.height = static_cast<float>(screenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = screenExtent;

    const auto count = static_cast<uint32_t>(m_drawCmdBuffers.Size());
    for (uint32_t i = 0; i < count; ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      m_drawCmdBuffers.Begin(i, cmdBufInfo);
      {
        m_drawCmdBuffers.CmdBeginRenderPass(i, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
          vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);
          vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

          vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);
          vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.Get());

          VkDeviceSize offsets[1] = {0};
          vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_vertexBuffer.Buffer.GetPointer(), offsets);
          vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_indexBuffer.Buffer.Get(), 0, VK_INDEX_TYPE_UINT32);

          vkCmdDrawIndexed(m_drawCmdBuffers[i], m_indexCount, 1, 0, 0, 0);

          DrawUI(m_drawCmdBuffers.Get(i));
        }
        m_drawCmdBuffers.CmdEndRenderPass(i);
      }
      m_drawCmdBuffers.End(i);
    }
  }

  void Texturing::OnViewChanged()
  {
    UpdateUniformBuffers();
  }


  void Texturing::OnKeyEvent(const KeyEvent& event)
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
      default:
        break;
      }
    }

    VulkanWillemsDemoApp::OnKeyEvent(event);
  }


  void Texturing::Update(const DemoTime& demoTime)
  {
  }


  void Texturing::Draw(const DemoTime& demoTime)
  {
    if (!TryPrepareFrame())
    {
      return;
    }

    // Command buffer to be submitted to the queue
    m_submitInfo.commandBufferCount = 1;
    m_submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);

    // Submit to queue
    m_deviceQueue.Submit(1, &m_submitInfo, VK_NULL_HANDLE);

    SubmitFrame();
  }


  void Texturing::GenerateQuad()
  {
    // Setup vertices for a single uv-mapped quad made from two triangles
    std::vector<Vertex> vertices = {{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                                    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                                    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    // Setup indices
    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    m_indexCount = static_cast<uint32_t>(indices.size());

    // Create buffers
    // For the sake of simplicity we won't stage the vertex data to the gpu memory
    // Vertex buffer
    RAPIDVULKAN_CHECK(DoCreateBuffer(m_vertexBuffer, m_physicalDevice, m_device.Get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.size() * sizeof(Vertex),
                                     vertices.data()));

    // Index buffer
    RAPIDVULKAN_CHECK(DoCreateBuffer(m_indexBuffer, m_physicalDevice, m_device.Get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.size() * sizeof(uint32_t),
                                     indices.data()));
  }


  void Texturing::SetupVertexDescriptions()
  {
    // Binding description
    m_vertices.BindingDescriptions.resize(1);
    m_vertices.BindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.BindingDescriptions[0].stride = sizeof(Vertex);
    m_vertices.BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions
    // Describes memory layout and shader positions
    m_vertices.AttributeDescriptions.resize(3);

    // Location 0 : Position
    m_vertices.AttributeDescriptions[0].location = 0;
    m_vertices.AttributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[0].offset = offsetof(Vertex, pos);

    // Location 1 : Texture coordinates
    m_vertices.AttributeDescriptions[1].location = 1;
    m_vertices.AttributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    m_vertices.AttributeDescriptions[1].offset = offsetof(Vertex, uv);

    // Location 2 : Vertex normal
    m_vertices.AttributeDescriptions[2].location = 2;
    m_vertices.AttributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.AttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.AttributeDescriptions[2].offset = offsetof(Vertex, normal);


    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertices.BindingDescriptions.size());
    m_vertices.InputState.pVertexBindingDescriptions = m_vertices.BindingDescriptions.data();
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.AttributeDescriptions.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.AttributeDescriptions.data();
  }

  // Prepare and initialize uniform buffer containing shader uniforms
  void Texturing::PrepareUniformBuffers()
  {
    // Vertex shader uniform buffer block
    RAPIDVULKAN_CHECK(DoCreateBuffer(m_uniformBufferVS, m_physicalDevice, m_device.Get(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(m_uboVS), &m_uboVS));

    UpdateUniformBuffers();
  }


  void Texturing::UpdateUniformBuffers()
  {
    auto resolution = GetScreenResolution();

    // Vertex shader
    m_uboVS.Projection = glm::perspective(glm::radians(60.0f), static_cast<float>(resolution.X) / static_cast<float>(resolution.Y), 0.001f, 256.0f);
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, m_zoom));

    m_uboVS.Model = viewMatrix * glm::translate(glm::mat4(1.0f), m_cameraPos);
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboVS.Model = glm::rotate(m_uboVS.Model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    m_uboVS.ViewPos = glm::vec4(0.0f, 0.0f, -m_zoom, 0.0f);

    RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), m_uniformBufferVS.Memory.Get(), 0, VK_WHOLE_SIZE, 0, &m_uniformBufferVS.pMapped));
    std::memcpy(m_uniformBufferVS.pMapped, &m_uboVS, sizeof(m_uboVS));
    vkUnmapMemory(m_device.Get(), m_uniformBufferVS.Memory.Get());
  }


  void Texturing::LoadTexture(const std::string& fileName, const bool forceLinearTiling)
  {
    using namespace MemoryTypeUtil;
    Texture texture;
    GetContentManager()->Read(texture, fileName);

    m_texture.Width = texture.GetExtent().Width;
    m_texture.Height = texture.GetExtent().Height;
    m_texture.MipLevels = texture.GetLevels();

    const VkFormat format = ConvertUtil::Convert(texture.GetPixelFormat());

    // We prefer using staging to copy the texture data to a device local optimal image

    bool useStaging = true;

    // Only use linear tiling if forced
    if (forceLinearTiling)
    {
      // Don't use linear if format is not supported for (linear) shader sampling
      // Get device properties for the requested texture format
      VkFormatProperties formatProperties{};
      vkGetPhysicalDeviceFormatProperties(m_physicalDevice.Device, format, &formatProperties);
      useStaging = ((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == 0u);
    }

    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.allocationSize = 0;
    memAllocInfo.memoryTypeIndex = 0;

    const VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;

    if (useStaging)
    {
      // Copy data to an optimal tiled image
      // This loads the texture data into a host local buffer that is copied to the optimal tiled image on the device

      // Create a host-visible staging buffer that contains the raw image data
      // This buffer will be the data source for copying texture data to the optimal tiled image on the device

      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = texture.GetByteSize();
      // This buffer is used as a transfer source for the buffer copy
      bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Buffer stagingBuffer(m_device.Get(), bufferCreateInfo);


      // Get memory requirements for the staging buffer (alignment, memory type bits)
      VkMemoryRequirements memReqs = stagingBuffer.GetBufferMemoryRequirements();
      memAllocInfo.allocationSize = memReqs.size;
      // Get memory type index for a host visible buffer
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      Memory stagingMemory(m_device.Get(), memAllocInfo);
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), stagingBuffer.Get(), stagingMemory.Get(), 0));

      // Copy texture data into staging buffer
      uint8_t* data;
      RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), stagingMemory.Get(), 0, memReqs.size, 0, reinterpret_cast<void**>(&data)));
      {
        RawTexture rawTexture;
        Texture::ScopedDirectAccess directAccess(texture, rawTexture);

        std::memcpy(data, rawTexture.GetContent(), rawTexture.GetByteSize());
      }
      vkUnmapMemory(m_device.Get(), stagingMemory.Get());

      // Setup buffer copy regions for each mip level
      std::vector<VkBufferImageCopy> bufferCopyRegions;

      for (uint32_t i = 0; i < texture.GetLevels(); ++i)
      {
        VkBufferImageCopy bufferCopyRegion{};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = i;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent = Convert(texture.GetExtent(i));
        bufferCopyRegion.bufferOffset = texture.GetTextureBlob(i).Offset;

        bufferCopyRegions.push_back(bufferCopyRegion);
      }

      // Create optimal tiled target image on the device
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.pNext = nullptr;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = format;
      imageCreateInfo.mipLevels = texture.GetLevels();
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      // Set initial layout of the image to undefined
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageCreateInfo.extent = Convert(texture.GetExtent());
      imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

      m_texture.Image.Reset(m_device.Get(), imageCreateInfo);

      memReqs = m_texture.Image.GetImageMemoryRequirements();

      memAllocInfo.allocationSize = memReqs.size;
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      m_texture.DeviceMemory.Reset(m_device.Get(), memAllocInfo);
      RAPIDVULKAN_CHECK(vkBindImageMemory(m_device.Get(), m_texture.Image.Get(), m_texture.DeviceMemory.Get(), 0));

      CommandBuffer copyCmd(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

      VkCommandBufferBeginInfo copyCommandBufferBeginInfo{};
      copyCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      copyCmd.Begin(copyCommandBufferBeginInfo);
      {
        // Image memory barriers for the texture image

        // The sub resource range describes the regions of the image that will be transitioned using the memory barriers below
        VkImageSubresourceRange subresourceRange{};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        // We will transition on all mip levels
        subresourceRange.levelCount = m_texture.MipLevels;
        // The 2D texture only has one layer
        subresourceRange.layerCount = 1;

        // Transition the texture image layout to transfer target, so we can safely copy our buffer data to it.
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        imageMemoryBarrier.image = m_texture.Image.Get();
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        // Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
        // Source pipeline stage is host write/read exection (VK_PIPELINE_STAGE_HOST_BIT)
        // Destination pipeline stage is copy command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
        vkCmdPipelineBarrier(copyCmd.Get(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);

        // Copy mip levels from staging buffer
        vkCmdCopyBufferToImage(copyCmd.Get(), stagingBuffer.Get(), m_texture.Image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

        // Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
        // Source pipeline stage stage is copy command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
        // Destination pipeline stage fragment shader access (VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        vkCmdPipelineBarrier(copyCmd.Get(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);

        // Store current layout for later reuse
        m_texture.ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      }
      copyCmd.End();

      // Normally its recommended to reuse fence objects, but we only need this for one use.
      Fence fence(m_device.Get(), 0);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = copyCmd.GetPointer();

      m_deviceQueue.Submit(1, &submitInfo, fence.Get());

      fence.WaitForFence(DEFAULT_FENCE_TIMEOUT);

      // Clean up staging resources is done automatically due to the use of the RAII containers
    }
    else
    {
      // Copy data to a linear tiled image


      // Load mip map level 0 to linear tiling image
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.pNext = nullptr;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = format;
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
      imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
      imageCreateInfo.extent = {m_texture.Width, m_texture.Height, 1};

      m_texture.Image.Reset(m_device.Get(), imageCreateInfo);


      // Get memory requirements for this image like size and alignment
      auto memReqs = m_texture.Image.GetImageMemoryRequirements();
      // Set memory allocation size to required memory size
      memAllocInfo.allocationSize = memReqs.size;
      // Get memory type that can be mapped to host memory
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      // Allocate host memory
      m_texture.DeviceMemory.Reset(m_device.Get(), memAllocInfo);

      // Bind allocated image for use
      RAPIDVULKAN_CHECK(vkBindImageMemory(m_device.Get(), m_texture.Image.Get(), m_texture.DeviceMemory.Get(), 0));

      // Map image memory
      void* data;
      RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), m_texture.DeviceMemory.Get(), 0, memReqs.size, 0, &data));
      {
        RawTexture rawTexture;
        Texture::ScopedDirectAccess directAccess(texture, rawTexture);
        // Copy image data into memory
        const auto blob0 = rawTexture.GetBlob(0);
        assert(blob0.Size == memReqs.size);
        const uint8_t* const pContent = static_cast<const uint8_t*>(rawTexture.GetContent()) + blob0.Offset;
        std::memcpy(data, pContent, blob0.Size);
      }
      vkUnmapMemory(m_device.Get(), m_texture.DeviceMemory.Get());

      // Linear tiled images don't need to be staged and can be directly used as textures
      m_texture.ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      CommandBuffer copyCmd(m_device.Get(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

      VkCommandBufferBeginInfo copyCommandBufferBeginInfo{};
      copyCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      copyCmd.Begin(copyCommandBufferBeginInfo);
      {
        // Setup image memory barrier transfer image to shader read layout

        // The sub resource range describes the regions of the image we will be transition
        VkImageSubresourceRange subresourceRange{};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        // Only one mip level, most implementations won't support more for linear tiled images
        subresourceRange.levelCount = 1;
        // The 2D texture only has one layer
        subresourceRange.layerCount = 1;

        // Transition the texture image layout to shader read, so it can be sampled from
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        imageMemoryBarrier.image = m_texture.Image.Get();
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
        // Source pipeline stage is host write/read exection (VK_PIPELINE_STAGE_HOST_BIT)
        // Destination pipeline stage fragment shader access (VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        vkCmdPipelineBarrier(copyCmd.Get(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }
      copyCmd.End();

      // Normally its recommended to reuse fence objects, but we only need this for one use.
      Fence fence(m_device.Get(), 0);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = copyCmd.GetPointer();

      m_deviceQueue.Submit(1, &submitInfo, fence.Get());

      fence.WaitForFence(DEFAULT_FENCE_TIMEOUT);
    }

    // Create a texture sampler
    // In Vulkan textures are accessed by samplers
    // This separates all the sampling information from the texture data. This means you could have multiple sampler objects for the same texture with
    // different settings Note: Similar to the samplers available with OpenGL 3.3
    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.pNext = nullptr;
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.mipLodBias = 0.0f;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    // Set max level-of-detail to mip level count of the texture
    sampler.maxLod = (useStaging) ? static_cast<float>(m_texture.MipLevels) : 0.0f;
    // Enable anisotropic filtering
    // This feature is optional, so we must check if it's supported on the device

    if (m_vulkanDevice.GetFeatures().samplerAnisotropy != VK_FALSE)
    {
      auto maxAnisotropy = m_vulkanDevice.GetProperties().limits.maxSamplerAnisotropy;
      FSLLOG("Using sampler anisotropy: " << maxAnisotropy)
      // Use max. level of anisotropy for this example
      sampler.maxAnisotropy = maxAnisotropy;
      sampler.anisotropyEnable = VK_TRUE;
    }
    else
    {
      // The device does not support anisotropic filtering
      sampler.maxAnisotropy = 1.0;
      sampler.anisotropyEnable = VK_FALSE;
    }
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    m_texture.Sampler.Reset(m_device.Get(), sampler);

    // Create image view
    // Textures are not directly accessed by the shaders and
    // are abstracted by image views containing additional
    // information and sub resource ranges
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;

    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = format;
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    // The subresource range describes the set of mip levels (and array layers) that can be accessed through this image view
    // It's possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 1;
    // Linear tiling usually won't support mip maps
    // Only set mip map count if optimal tiling is used
    view.subresourceRange.levelCount = (useStaging) ? m_texture.MipLevels : 1;
    view.image = m_texture.Image.Get();
    m_texture.View.Reset(m_device.Get(), view);

    // Fill image descriptor image info that can be used during the descriptor set setup
    m_texture.Descriptor.imageLayout = m_texture.ImageLayout;
    m_texture.Descriptor.imageView = m_texture.View.Get();
    m_texture.Descriptor.sampler = m_texture.Sampler.Get();
  }


  void Texturing::SetupDescriptorSetLayout()
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


  void Texturing::PreparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
    depthStencilState.front = depthStencilState.back;    // FIX: is this a bug?? it seems like its intended to execute after the line below
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;    //! FIX: Is this wrong? we are not supplying any pointers
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;    //! FIX: Is this wrong? we are not supplying any pointers
    viewportState.pScissors = nullptr;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();

    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0] = LoadShader("shaders/texture.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("shaders/texture.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = m_pipelineLayout.Get();
    pipelineCreateInfo.renderPass = m_renderPass.Get();

    m_graphicsPipeline.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void Texturing::SetupDescriptorPool()
  {
    // Example uses one ubo and one image sampler
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.maxSets = 2;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void Texturing::SetupDescriptorSet()
  {
    // Allocate a new descriptor set from the global descriptor pool
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();
    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));

    std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
    // Binding 0 : Vertex shader uniform buffer
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = m_descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &m_uniformBufferVS.Descriptor;

    // Binding 1 : Fragment shader texture sampler
    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = m_descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[1].pImageInfo = &m_texture.Descriptor;

    vkUpdateDescriptorSets(m_device.Get(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
  }


  void Texturing::ChangeLodBias(const float delta)
  {
    m_uboVS.LodBias += delta;
    if (m_uboVS.LodBias < 0.0f)
    {
      m_uboVS.LodBias = 0.0f;
    }
    if (m_uboVS.LodBias > m_texture.MipLevels)
    {
      m_uboVS.LodBias = static_cast<float>(m_texture.MipLevels);
    }
    UpdateUniformBuffers();
    UpdateTextOverlay();
  }
}
