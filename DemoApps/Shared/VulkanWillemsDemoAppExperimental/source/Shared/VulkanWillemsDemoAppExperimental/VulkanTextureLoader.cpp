/*
 * Texture loader for Vulkan
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanTextureLoader to make it easier
// to port samples. It is not a straight port, but it has instead been converted to
// follow the RAII principle used in this framework

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/Config.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTextureLoader.hpp>
#include <cstring>
#include <utility>

using namespace RapidVulkan;

namespace Fsl::Willems
{
  namespace
  {
    struct TexConfig
    {
      VkExtent3D Extent;
      uint32_t Levels{0};
      uint32_t Faces{0};
      uint32_t Layers{0};
      float MaxLod{0};

      TexConfig()
        : Extent()
      {
      }

      TexConfig(const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t faces, const uint32_t layers, const float maxLod)
        : Extent(extent)
        , Levels(mipLevels)
        , Faces(faces)
        , Layers(layers)
        , MaxLod(maxLod)
      {
      }
    };

    struct LocalTexture
    {
      RapidVulkan::Image Image;
      RapidVulkan::Memory Memory;
      VkImageLayout ImageLayout;
      TexConfig Config;

      LocalTexture& operator=(LocalTexture&& other) noexcept
      {
        Image = std::move(other.Image);
        Memory = std::move(other.Memory);
        ImageLayout = other.ImageLayout;
        Config = other.Config;

        other.ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        other.Config = TexConfig();
        return *this;
      }

      LocalTexture(LocalTexture&& other) noexcept
        : Image(std::move(other.Image))
        , Memory(std::move(other.Memory))
        , ImageLayout(other.ImageLayout)
        , Config(other.Config)
      {
        other.ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        other.Config = TexConfig();
      }

      LocalTexture(RapidVulkan::Image&& image, RapidVulkan::Memory&& memory, const VkImageLayout& imageLayout, const TexConfig& config)
        : Image(std::move(image))
        , Memory(std::move(memory))
        , ImageLayout(imageLayout)
        , Config(config)
      {
      }
    };


    TexConfig GetTextureConfig(const Texture& texture, const TextureType texType)
    {
      VkExtent3D texExtent = TypeConverter::UncheckedTo<VkExtent3D>(texture.GetExtent());
      uint32_t texMipLevels = texture.GetLevels();
      uint32_t texFaces = texture.GetFaces();
      uint32_t texLayers = texture.GetLayers();

      switch (texType)
      {
      case TextureType::Tex1D:
        texExtent.depth = 1;
        texFaces = 1;
        texLayers = 1;
        break;
      case TextureType::Tex2D:
        texExtent.depth = 1;
        texFaces = 1;
        texLayers = 1;
        break;
      case TextureType::Tex3D:
        texFaces = 1;
        texLayers = 1;
        break;
      case TextureType::TexCube:
        assert(texture.GetFaces() == 6);
        texExtent.depth = 1;
        texFaces = 6;
        texLayers = 1;
        break;
      case TextureType::Tex1DArray:
        texExtent.depth = 1;
        texFaces = 1;
        break;
      case TextureType::Tex2DArray:
        texExtent.depth = 1;
        texFaces = 1;
        break;
      case TextureType::TexCubeArray:
        assert(texture.GetFaces() == 6);
        texExtent.depth = 1;
        texFaces = 6;
        break;
      default:
        throw NotSupportedException("Unsupported texture type");
      }

      assert(texFaces <= texture.GetFaces());
      assert(texLayers <= texture.GetLayers());
      assert(texExtent.width == texture.GetExtent().Width.Value);
      assert(texExtent.height == texture.GetExtent().Height.Value);
      assert(texExtent.depth <= texture.GetExtent().Depth.Value);
      return {texExtent, texMipLevels, texFaces, texLayers, static_cast<float>(texMipLevels)};
    }


    LocalTexture LoadTextureUsingStaging(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkQueue queue, CommandBuffer& rCmdBuffer,
                                         const Texture& texture, const TextureType texType, const VkImageUsageFlags imageUsageFlags)
    {
      using namespace Vulkan::MemoryTypeUtil;
      // Create a host-visible staging buffer that contains the raw image data

      const TexConfig texConfig = GetTextureConfig(texture, texType != TextureType::Undefined ? texType : texture.GetTextureType());

      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.pNext = nullptr;
      bufferCreateInfo.size = texture.GetByteSize();
      // This buffer is used as a transfer source for the buffer copy
      bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      Buffer stagingBuffer(device, bufferCreateInfo);

      // Get memory requirements for the staging buffer (alignment, memory type bits)
      VkMemoryRequirements memReqs = stagingBuffer.GetBufferMemoryRequirements();

      VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

      VkMemoryAllocateInfo memAllocInfo{};
      memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAllocInfo.pNext = nullptr;
      memAllocInfo.allocationSize = memReqs.size;
      // Get memory type index for a host visible buffer
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(deviceMemoryProperties, memReqs.memoryTypeBits,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      Memory stagingMemory(device, memAllocInfo);

      RAPIDVULKAN_CHECK(vkBindBufferMemory(device, stagingBuffer.Get(), stagingMemory.Get(), 0));

      // Copy texture data into staging buffer
      void* pData = nullptr;
      stagingMemory.MapMemory(0, memReqs.size, 0, &pData);
      {
        Texture::ScopedDirectReadAccess directAccess(texture);
        std::memcpy(pData, directAccess.AsRawTexture().GetContent(), directAccess.AsRawTexture().GetByteSize());
      }
      stagingMemory.UnmapMemory();


      // Setup buffer copy regions for each mip level
      std::vector<VkBufferImageCopy> bufferCopyRegions(texConfig.Levels * texConfig.Faces * texConfig.Layers);
      std::size_t dstOffset = 0;
      for (uint32_t layerIndex = 0; layerIndex < texConfig.Layers; ++layerIndex)
      {
        for (uint32_t faceIndex = 0; faceIndex < texConfig.Faces; ++faceIndex)
        {
          for (uint32_t levelIndex = 0; levelIndex < texConfig.Levels; ++levelIndex)
          {
            const auto blobRecord = texture.GetTextureBlob(levelIndex, faceIndex, layerIndex);
            const auto blobExtent = texture.GetExtent(levelIndex);

            VkBufferImageCopy bufferCopyRegion{};
            bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.mipLevel = levelIndex;
            // FIX: handle proper layer calc when using both multiple faces and multiple layers (cube array)
            assert((texConfig.Layers == 1 && texConfig.Faces >= 1) || (texConfig.Layers >= 1 && texConfig.Faces == 1));
            bufferCopyRegion.imageSubresource.baseArrayLayer = (texConfig.Faces > 1 ? faceIndex : layerIndex);
            bufferCopyRegion.imageSubresource.layerCount = 1;
            bufferCopyRegion.imageExtent = TypeConverter::UncheckedTo<VkExtent3D>(blobExtent);
            bufferCopyRegion.bufferOffset = blobRecord.Offset;

            bufferCopyRegions[dstOffset] = bufferCopyRegion;
            ++dstOffset;
          }
        }
      }

      // Create optimal tiled target image
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.pNext = nullptr;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.format = Vulkan::VulkanConvert::ToVkFormat(texture.GetPixelFormat());
      imageCreateInfo.mipLevels = texConfig.Levels;
      imageCreateInfo.arrayLayers = (texConfig.Faces * texConfig.Layers);    // Cube faces count as array layers in Vulkan
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageCreateInfo.extent = texConfig.Extent;
      // Ensure that the TRANSFER_DST bit is set for staging
      imageCreateInfo.usage = imageUsageFlags | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

      if (texType == TextureType::TexCube || texType == TextureType::TexCubeArray)
      {
        imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
      }

      Image texImage(device, imageCreateInfo);

      memReqs = texImage.GetImageMemoryRequirements();
      memAllocInfo.allocationSize = memReqs.size;
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(deviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      Memory texMemory(device, memAllocInfo);

      RAPIDVULKAN_CHECK(vkBindImageMemory(device, texImage.Get(), texMemory.Get(), 0));

      const VkImageLayout texImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      // Use a separate command buffer for texture loading
      VkCommandBufferBeginInfo cmdBufInfo{};
      cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      cmdBufInfo.pNext = nullptr;
      rCmdBuffer.Begin(cmdBufInfo);
      {
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = texConfig.Levels;
        subresourceRange.layerCount = texConfig.Faces * texConfig.Layers;

        // Image barrier for optimal image (target)
        // Optimal image will be used as destination for the copy
        Vulkan::CommandBufferUtil::SetImageLayout(rCmdBuffer.Get(), texImage.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

        // Copy mip levels from staging buffer
        vkCmdCopyBufferToImage(rCmdBuffer.Get(), stagingBuffer.Get(), texImage.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               UncheckedNumericCast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

        // Change texture image layout to shader read after all mip levels have been copied
        Vulkan::CommandBufferUtil::SetImageLayout(rCmdBuffer.Get(), texImage.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                  texImageLayout, subresourceRange);
      }    // Submit command buffer containing copy and image layout commands
      rCmdBuffer.End();

      // Create a fence to make sure that the copies have finished before continuing
      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceCreateInfo.flags = Config::FLAGS_NONE;
      Fence copyFence(device, fenceCreateInfo);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.pNext = nullptr;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCmdBuffer.GetPointer();

      RAPIDVULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, copyFence.Get()));
      RAPIDVULKAN_CHECK(vkWaitForFences(device, 1, copyFence.GetPointer(), VK_TRUE, Config::DEFAULT_FENCE_TIMEOUT));
      return {std::move(texImage), std::move(texMemory), texImageLayout, texConfig};
    }


    LocalTexture LoadTextureLinear(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkQueue queue, CommandBuffer& rCmdBuffer,
                                   const Texture& texture, const TextureType texType, const VkImageUsageFlags imageUsageFlags)
    {
      using namespace Vulkan::MemoryTypeUtil;
      assert(texType == TextureType::Tex1D);
      assert(texType == TextureType::Tex2D);

      // Prefer using optimal tiling, as linear tiling
      // may support only a small set of features
      // depending on implementation (e.g. no mip maps, only one layer, etc.)

      TexConfig texConfig = GetTextureConfig(texture, texType);
      texConfig.Levels = 1;
      texConfig.Layers = 1;
      texConfig.MaxLod = 0.0f;
      const VkFormat format = Vulkan::VulkanConvert::ToVkFormat(texture.GetPixelFormat());

      // Get device properties for the requested texture format
      VkFormatProperties formatProperties;
      vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

      // Check if this support is supported for linear tiling
      assert(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.pNext = nullptr;
      imageCreateInfo.imageType = Vulkan::VulkanConvert::ToVkImageType(texType);
      imageCreateInfo.format = format;
      imageCreateInfo.extent = texConfig.Extent;
      imageCreateInfo.mipLevels = texConfig.Levels;
      imageCreateInfo.arrayLayers = texConfig.Layers;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
      imageCreateInfo.usage = imageUsageFlags;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

      // Load mip map level 0 to linear tiling image
      Image texImage(device, imageCreateInfo);
      // Get memory requirements for this image like size and alignment
      VkMemoryRequirements memReqs = texImage.GetImageMemoryRequirements();

      VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

      VkMemoryAllocateInfo memAllocInfo{};
      memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAllocInfo.pNext = nullptr;
      memAllocInfo.allocationSize = 0;
      memAllocInfo.memoryTypeIndex = 0;
      // Set memory allocation size to required memory size
      memAllocInfo.allocationSize = memReqs.size;
      // Get memory type that can be mapped to host memory
      memAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(deviceMemoryProperties, memReqs.memoryTypeBits,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      // Allocate host memory
      Memory texMemory(device, memAllocInfo);

      // Bind allocated image for use
      RAPIDVULKAN_CHECK(vkBindImageMemory(device, texImage.Get(), texMemory.Get(), 0));

      // Get sub resource layout
      // Mip map count, array layer, etc.
      VkImageSubresource subRes{};
      subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subRes.mipLevel = 0;

      VkSubresourceLayout subResLayout;

      // Get sub resources layout
      // Includes row pitch, size offsets, etc.
      texImage.GetImageSubresourceLayout(&subRes, &subResLayout);

      void* pData = nullptr;
      // Map image memory
      texMemory.MapMemory(0, memReqs.size, 0, &pData);
      {
        // Copy image data into memory
        Texture::ScopedDirectReadAccess directAccess(texture);
        ReadOnlyRawTexture rawTexture = directAccess.AsRawTexture();
        // Copy image data into memory
        const auto blob0 = rawTexture.GetBlob(0);
        assert(blob0.Size == memReqs.size);
        const uint8_t* const pContent = static_cast<const uint8_t*>(rawTexture.GetContent()) + blob0.Offset;
        std::memcpy(pData, pContent, blob0.Size);
      }
      texMemory.UnmapMemory();

      // Linear tiled images don't need to be staged
      // and can be directly used as textures
      const VkImageLayout texImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      // Use a separate command buffer for texture loading
      VkCommandBufferBeginInfo cmdBufInfo{};
      cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      cmdBufInfo.pNext = nullptr;
      rCmdBuffer.Begin(cmdBufInfo);
      {
        // Setup image memory barrier
        Vulkan::CommandBufferUtil::SetImageLayout(rCmdBuffer.Get(), texImage.Get(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                                  texImageLayout);
      }    // Submit command buffer containing copy and image layout commands
      rCmdBuffer.End();

      VkFence nullFence = VK_NULL_HANDLE;
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.pNext = nullptr;
      submitInfo.waitSemaphoreCount = 0;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCmdBuffer.GetPointer();

      RAPIDVULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, nullFence));
      RAPIDVULKAN_CHECK(vkQueueWaitIdle(queue));
      return {std::move(texImage), std::move(texMemory), texImageLayout, texConfig};
    }
  }


  VulkanTextureLoader::VulkanTextureLoader(const std::shared_ptr<IContentManager>& contentManager, const VkPhysicalDevice physicalDevice,
                                           const VkDevice device, const VkQueue queue, const VkCommandPool cmdPool,
                                           const VkPhysicalDeviceFeatures activeDeviceFeatures)
    : m_contentManager(contentManager)
    , m_physicalDevice(physicalDevice)
    , m_device(device)
    , m_queue(queue)
    //, m_cmdPool(cmdPool)
    , m_maxAnisotropy(1.0f)
  {
    if (!contentManager || physicalDevice == VK_NULL_HANDLE || device == VK_NULL_HANDLE || queue == VK_NULL_HANDLE || cmdPool == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("arguments can not be null");
    }

    // Check if we have anisotropic filtering enabled

    if (activeDeviceFeatures.samplerAnisotropy != VK_FALSE)
    {
      m_maxAnisotropy = 8.0f;
    }

    // Create command buffer for submitting image barriers and converting tilings
    VkCommandBufferAllocateInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufInfo.commandPool = cmdPool;
    cmdBufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufInfo.commandBufferCount = 1;

    m_cmdBuffer.Reset(device, cmdBufInfo);
  }


  VulkanTextureLoader::~VulkanTextureLoader() = default;


  // TODO: create a unified texture load method
  //       These methods has artificial limits from the original source of the code.
  //       It ought to possible ot generalize all the loading so we can load any format into any format (more or less)
  VulkanTexture VulkanTextureLoader::LoadTexture(const std::string& filename, const VkFormat format, const bool forceLinear,
                                                 const VkImageUsageFlags imageUsageFlags)
  {
    const auto pixelFormat = Vulkan::VulkanConvert::ToPixelFormat(format);
    Texture texture;
    m_contentManager->Read(texture, IO::Path(filename), pixelFormat);

    // Only use linear tiling if requested (and supported by the device)
    // Support for linear tiling is mostly limited, so prefer to use optimal tiling instead
    // On most implementations linear tiling will only support a very
    // limited amount of formats and features (mip maps, cubemaps, arrays, etc.)
    const bool useStaging = !forceLinear;
    auto localTexture = useStaging
                          ? LoadTextureUsingStaging(m_physicalDevice, m_device, m_queue, m_cmdBuffer, texture, TextureType::Tex2D, imageUsageFlags)
                          : LoadTextureLinear(m_physicalDevice, m_device, m_queue, m_cmdBuffer, texture, TextureType::Tex2D, imageUsageFlags);

    // Create sampler
    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.mipLodBias = 0.0f;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = localTexture.Config.MaxLod;
    // Enable anisotropic filtering
    sampler.maxAnisotropy = m_maxAnisotropy;
    sampler.anisotropyEnable = VK_TRUE;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    Sampler texSampler(m_device, sampler);

    // Create image view
    // Textures are not directly accessed by the shaders and
    // are abstracted by image views containing additional
    // information and sub resource ranges
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;
    view.image = VK_NULL_HANDLE;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = Vulkan::VulkanConvert::ToVkFormat(texture.GetPixelFormat());
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    // Linear tiling usually won't support mip maps
    // Only set mip map count if optimal tiling is used
    view.subresourceRange.levelCount = localTexture.Config.Levels;
    view.image = localTexture.Image.Get();

    ImageView texImageView(m_device, view);

    // Fill descriptor image info that can be used for setting up descriptor sets
    VkDescriptorImageInfo texDescriptor{};
    texDescriptor.sampler = texSampler.Get();
    texDescriptor.imageView = texImageView.Get();
    texDescriptor.imageLayout = localTexture.ImageLayout;

    // Transfer ownership to the texture object (move)
    return {std::move(texSampler),          std::move(localTexture.Image), localTexture.ImageLayout,
            std::move(localTexture.Memory), std::move(texImageView),       TypeConverter::UncheckedTo<PxExtent3D>(localTexture.Config.Extent),
            localTexture.Config.Levels,     localTexture.Config.Layers,    texDescriptor};
  }


  VulkanTexture VulkanTextureLoader::LoadCubemap(const std::string& filename, const VkFormat format, const VkImageUsageFlags imageUsageFlags)
  {
    const auto pixelFormat = Vulkan::VulkanConvert::ToPixelFormat(format);
    Texture texture;
    m_contentManager->Read(texture, IO::Path(filename), pixelFormat);

    if (texture.GetTextureType() != TextureType::TexCube)
    {
      throw NotSupportedException("The texture did not contain a cube map");
    }

    auto localTexture = LoadTextureUsingStaging(m_physicalDevice, m_device, m_queue, m_cmdBuffer, texture, TextureType::TexCube, imageUsageFlags);

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
    sampler.maxAnisotropy = m_maxAnisotropy;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = localTexture.Config.MaxLod;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    Sampler texSampler(m_device, sampler);

    // Create image view
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;
    view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    view.format = Vulkan::VulkanConvert::ToVkFormat(texture.GetPixelFormat());
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    view.subresourceRange.layerCount = localTexture.Config.Faces * localTexture.Config.Layers;
    view.subresourceRange.levelCount = localTexture.Config.Levels;
    view.image = localTexture.Image.Get();

    ImageView texImageView(m_device, view);

    // Fill descriptor image info that can be used for setting up descriptor sets
    VkDescriptorImageInfo texDescriptor{};
    texDescriptor.sampler = texSampler.Get();
    texDescriptor.imageView = texImageView.Get();
    texDescriptor.imageLayout = localTexture.ImageLayout;

    // Transfer ownership to the texture object (move)
    return {std::move(texSampler),          std::move(localTexture.Image), localTexture.ImageLayout,
            std::move(localTexture.Memory), std::move(texImageView),       TypeConverter::UncheckedTo<PxExtent3D>(localTexture.Config.Extent),
            localTexture.Config.Levels,     localTexture.Config.Layers,    texDescriptor};
  }


  VulkanTexture VulkanTextureLoader::LoadTextureArray(const std::string& filename, const VkFormat format, const VkImageUsageFlags imageUsageFlags)
  {
    const auto pixelFormat = Vulkan::VulkanConvert::ToPixelFormat(format);
    Texture texture;
    m_contentManager->Read(texture, IO::Path(filename), pixelFormat);

    if (texture.GetTextureType() != TextureType::Tex2DArray)
    {
      throw NotSupportedException("The texture did not contain a 2d array");
    }

    auto localTexture = LoadTextureUsingStaging(m_physicalDevice, m_device, m_queue, m_cmdBuffer, texture, TextureType::Undefined, imageUsageFlags);

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
    sampler.maxAnisotropy = m_maxAnisotropy;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = localTexture.Config.MaxLod;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    Sampler texSampler(m_device, sampler);

    // Create image view
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.pNext = nullptr;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    view.format = Vulkan::VulkanConvert::ToVkFormat(texture.GetPixelFormat());
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    view.subresourceRange.layerCount = localTexture.Config.Faces * localTexture.Config.Layers;
    view.subresourceRange.levelCount = localTexture.Config.Levels;
    view.image = localTexture.Image.Get();
    ImageView texImageView(m_device, view);

    // Fill descriptor image info that can be used for setting up descriptor sets
    VkDescriptorImageInfo texDescriptor{};
    texDescriptor.sampler = texSampler.Get();
    texDescriptor.imageView = texImageView.Get();
    texDescriptor.imageLayout = localTexture.ImageLayout;

    // Transfer ownership to the texture object (move)
    return {std::move(texSampler),          std::move(localTexture.Image), localTexture.ImageLayout,
            std::move(localTexture.Memory), std::move(texImageView),       TypeConverter::UncheckedTo<PxExtent3D>(localTexture.Config.Extent),
            localTexture.Config.Levels,     localTexture.Config.Layers,    texDescriptor};
  }
}
