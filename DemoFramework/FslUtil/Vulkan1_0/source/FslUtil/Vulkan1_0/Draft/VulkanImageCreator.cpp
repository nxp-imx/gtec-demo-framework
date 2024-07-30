/****************************************************************************************************************************************************
 * Copyright 2017, 2022, 2024 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawCubeBitmap.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/Texture/ReadOnlyRawTexture.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreatorUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <cassert>
#include <limits>
#include <utility>

using namespace RapidVulkan;

namespace Fsl::Vulkan
{
  namespace
  {
    ImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat imageFormat, const TextureType textureType,
                              const TextureInfo& textureInfo)
    {
      VkImageSubresourceRange imageSubresourceRange{};
      imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageSubresourceRange.baseMipLevel = 0;
      imageSubresourceRange.levelCount = textureInfo.Levels;
      imageSubresourceRange.baseArrayLayer = 0;
      imageSubresourceRange.layerCount = textureInfo.Faces * textureInfo.Layers;

      VkImageViewCreateInfo imageViewCreateInfo{};
      imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewCreateInfo.flags = 0;
      imageViewCreateInfo.image = image;
      imageViewCreateInfo.viewType = VulkanConvert::ToVkImageViewType(textureType);
      imageViewCreateInfo.format = imageFormat;
      imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                        VK_COMPONENT_SWIZZLE_IDENTITY};
      imageViewCreateInfo.subresourceRange = imageSubresourceRange;

      return {device, imageViewCreateInfo};
    }

    inline TextureInfo GetTextureInfo(const ReadOnlyRawBitmap& src)
    {
      FSL_PARAM_NOT_USED(src);
      return {1u, 1u, 1u};
    }

    inline TextureInfo GetTextureInfo(const Bitmap& src)
    {
      FSL_PARAM_NOT_USED(src);
      return {1u, 1u, 1u};
    }

    template <typename T>
    inline TextureInfo GetTextureInfo(const T& src)
    {
      return src.GetTextureInfo();
    }

    template <typename T>
    inline VUImageMemoryView DoCreateImage(const VUPhysicalDeviceRecord& physicalDevice, RapidVulkan::CommandBuffer& rCommandBuffer,
                                           const VkQueue queue, const T& src, const TextureType textureType, const std::string& name,
                                           const VkImageUsageFlags imageUsageFlags)
    {
      const VkAccessFlags accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

      VUImage image;
      Memory memory;
      VulkanImageCreatorUtil::Create(image, memory, physicalDevice, rCommandBuffer.GetDevice(), queue, rCommandBuffer.Get(), src, accessMask,
                                     imageUsageFlags);

      auto imageView = CreateImageView(rCommandBuffer.GetDevice(), image.Get(), image.GetFormat(), textureType, GetTextureInfo(src));

      return VUImageMemoryView(std::move(image), std::move(memory), std::move(imageView), name);
    }
  }


  VulkanImageCreator::VulkanImageCreator(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex)
    : m_physicalDevice(device.GetPhysicalDevice())
    , m_commandPool(device.Get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex)
    , m_queue(queue)
  {
    // Create command buffer for submitting image barriers and converting tilings
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool.Get();
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    m_commandBuffer.Reset(device.Get(), commandBufferAllocateInfo);
  }


  VulkanImageCreator::~VulkanImageCreator() = default;


  VUImageMemoryView VulkanImageCreator::CreateImage(const Bitmap& src, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    return DoCreateImage(m_physicalDevice, m_commandBuffer, m_queue, src, TextureType::Tex2D, name, imageUsageFlags);
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const Texture& src, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    const VkAccessFlags accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    VUImage image;
    Memory memory;
    VulkanImageCreatorUtil::Create(image, memory, m_physicalDevice, m_commandBuffer.GetDevice(), m_queue, m_commandBuffer.Get(), src, accessMask,
                                   imageUsageFlags);

    auto imageView = CreateImageView(m_commandBuffer.GetDevice(), image.Get(), image.GetFormat(), src.GetTextureType(), src.GetTextureInfo());

    return {std::move(image), std::move(memory), std::move(imageView), name};
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const ReadOnlyRawBitmap& src, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    return DoCreateImage(m_physicalDevice, m_commandBuffer, m_queue, src, TextureType::Tex2D, name, imageUsageFlags);
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const RawCubeBitmap& src, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    return DoCreateImage(m_physicalDevice, m_commandBuffer, m_queue, src, TextureType::TexCube, name, imageUsageFlags);
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const ReadOnlyRawTexture& src, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    const VkAccessFlags accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    VUImage image;
    Memory memory;
    VulkanImageCreatorUtil::Create(image, memory, m_physicalDevice, m_commandBuffer.GetDevice(), m_queue, m_commandBuffer.Get(), src, accessMask,
                                   imageUsageFlags);

    auto imageView = CreateImageView(m_commandBuffer.GetDevice(), image.Get(), image.GetFormat(), src.GetTextureType(), src.GetTextureInfo());

    return {std::move(image), std::move(memory), std::move(imageView), name};
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY,
                                                    const Bitmap& srcBitmapNegY, const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ,
                                                    const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    const Bitmap::ScopedDirectReadAccess directAccessPosX(srcBitmapPosX);
    const Bitmap::ScopedDirectReadAccess directAccessNegX(srcBitmapNegX);
    const Bitmap::ScopedDirectReadAccess directAccessPosY(srcBitmapPosY);
    const Bitmap::ScopedDirectReadAccess directAccessNegY(srcBitmapNegY);
    const Bitmap::ScopedDirectReadAccess directAccessPosZ(srcBitmapPosZ);
    const Bitmap::ScopedDirectReadAccess directAccessNegZ(srcBitmapNegZ);
    RawCubeBitmap rawCubeBitmap(directAccessPosX.AsRawBitmap(), directAccessNegX.AsRawBitmap(), directAccessPosY.AsRawBitmap(),
                                directAccessNegY.AsRawBitmap(), directAccessPosZ.AsRawBitmap(), directAccessNegZ.AsRawBitmap());
    return CreateImage(rawCubeBitmap, name, imageUsageFlags);
  }


  VUImageMemoryView VulkanImageCreator::CreateImage(const ReadOnlyRawBitmap& srcBitmapPosX, const ReadOnlyRawBitmap& srcBitmapNegX,
                                                    const ReadOnlyRawBitmap& srcBitmapPosY, const ReadOnlyRawBitmap& srcBitmapNegY,
                                                    const ReadOnlyRawBitmap& srcBitmapPosZ, const ReadOnlyRawBitmap& srcBitmapNegZ,
                                                    const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    RawCubeBitmap rawCubeBitmap(srcBitmapPosX, srcBitmapNegX, srcBitmapPosY, srcBitmapNegY, srcBitmapPosZ, srcBitmapNegZ);
    return CreateImage(rawCubeBitmap, name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const Bitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(src, std::move(sampler), name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const Texture& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(src, std::move(sampler), name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const Bitmap& src, Sampler&& sampler, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(src, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }


  VUTexture VulkanImageCreator::CreateTexture(const Texture& src, Sampler&& sampler, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(src, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }


  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawBitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(src, std::move(sampler), name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const RawCubeBitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(src, std::move(sampler), name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawTexture& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(src, std::move(sampler), name, imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawBitmap& src, RapidVulkan::Sampler&& sampler, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(src, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }


  VUTexture VulkanImageCreator::CreateTexture(const RawCubeBitmap& src, RapidVulkan::Sampler&& sampler, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(src, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }


  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawTexture& src, RapidVulkan::Sampler&& sampler, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(src, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }


  VUTexture VulkanImageCreator::CreateTexture(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY,
                                              const Bitmap& srcBitmapNegY, const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ,
                                              const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(srcBitmapPosX, srcBitmapNegX, srcBitmapPosY, srcBitmapNegY, srcBitmapPosZ, srcBitmapNegZ, std::move(sampler), name,
                         imageUsageFlags);
  }

  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawBitmap& srcBitmapPosX, const ReadOnlyRawBitmap& srcBitmapNegX,
                                              const ReadOnlyRawBitmap& srcBitmapPosY, const ReadOnlyRawBitmap& srcBitmapNegY,
                                              const ReadOnlyRawBitmap& srcBitmapPosZ, const ReadOnlyRawBitmap& srcBitmapNegZ,
                                              const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name,
                                              const VkImageUsageFlags imageUsageFlags)
  {
    Sampler sampler(m_commandBuffer.GetDevice(), samplerCreateInfo);
    return CreateTexture(srcBitmapPosX, srcBitmapNegX, srcBitmapPosY, srcBitmapNegY, srcBitmapPosZ, srcBitmapNegZ, std::move(sampler), name,
                         imageUsageFlags);
  }


  VUTexture VulkanImageCreator::CreateTexture(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY,
                                              const Bitmap& srcBitmapNegY, const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ,
                                              RapidVulkan::Sampler&& sampler, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(srcBitmapPosX, srcBitmapNegX, srcBitmapPosY, srcBitmapNegY, srcBitmapPosZ, srcBitmapNegZ, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }

  VUTexture VulkanImageCreator::CreateTexture(const ReadOnlyRawBitmap& srcBitmapPosX, const ReadOnlyRawBitmap& srcBitmapNegX,
                                              const ReadOnlyRawBitmap& srcBitmapPosY, const ReadOnlyRawBitmap& srcBitmapNegY,
                                              const ReadOnlyRawBitmap& srcBitmapPosZ, const ReadOnlyRawBitmap& srcBitmapNegZ,
                                              RapidVulkan::Sampler&& sampler, const std::string& name, const VkImageUsageFlags imageUsageFlags)
  {
    auto image = CreateImage(srcBitmapPosX, srcBitmapNegX, srcBitmapPosY, srcBitmapNegY, srcBitmapPosZ, srcBitmapNegZ, name, imageUsageFlags);
    return {std::move(image), std::move(sampler)};
  }
}
