/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "ImageEx.hpp"
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  // move assignment operator
  ImageEx& ImageEx::operator=(ImageEx&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_image = std::move(other.m_image);
      m_imageLayers = std::move(other.m_imageLayers);
      m_createInfo = other.m_createInfo;

      // Remove the data from other
      other.m_createInfo = VkImageCreateInfo{};
    }
    return *this;
  }


  // Transfer ownership from other to this
  ImageEx::ImageEx(ImageEx&& other) noexcept
    : m_image(std::move(other.m_image))
    , m_imageLayers(std::move(other.m_imageLayers))
    , m_createInfo(other.m_createInfo)
  {
    // Remove the data from other
    other.m_createInfo = VkImageCreateInfo{};
  }


  // For now we implement the code here, if this turns out to be a performance problem we can move the code
  // to the header file to ensure its inlined.
  ImageEx::ImageEx()
    : m_createInfo{}
  {
  }


  ImageEx::ImageEx(const VkDevice device, const VkImageCreateInfo& createInfo, const VkAccessFlags accessMask)
    : ImageEx()
  {
    Reset(device, createInfo, accessMask);
  }


  ImageEx::ImageEx(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format,
                   const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples,
                   const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount,
                   const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout, const VkAccessFlags accessMask)
    : ImageEx()
  {
    Reset(device, flags, imageType, format, extent, mipLevels, arrayLayers, samples, tiling, usage, sharingMode, queueFamilyIndexCount,
          queueFamilyIndices, initialLayout, accessMask);
  }


  ImageEx::~ImageEx()
  {
    Reset();
  }


  void ImageEx::Reset() noexcept
  {
    if (!m_image.IsValid())
    {
      return;
    }

    m_image.Reset();
    m_imageLayers.clear();
  }


  void ImageEx::Reset(const VkDevice device, const VkImageCreateInfo& createInfo, const VkAccessFlags /*accessMask*/)
  {
    if (IsValid())
    {
      Reset();
    }

    try
    {
      m_image.Reset(device, createInfo);
      m_imageLayers.resize(createInfo.mipLevels * createInfo.arrayLayers);
    }
    catch (const std::exception&)
    {
      m_image.Reset();
      m_imageLayers.resize(0);
      throw;
    }

    m_createInfo = createInfo;

    for (uint32_t arrayLayer = 0; arrayLayer < createInfo.arrayLayers; ++arrayLayer)
    {
      const uint32_t mipOffset = arrayLayer * createInfo.mipLevels;
      for (uint32_t i = 0; i < createInfo.mipLevels; ++i)
      {
        m_imageLayers[mipOffset + i].Layout = createInfo.initialLayout;
      }
    }
  }


  void ImageEx::Reset(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format,
                      const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples,
                      const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode sharingMode,
                      const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout,
                      const VkAccessFlags accessMask)
  {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.imageType = imageType;
    createInfo.format = format;
    createInfo.extent = extent;
    createInfo.mipLevels = mipLevels;
    createInfo.arrayLayers = arrayLayers;
    createInfo.samples = samples;
    createInfo.tiling = tiling;
    createInfo.usage = usage;
    createInfo.sharingMode = sharingMode;
    createInfo.queueFamilyIndexCount = queueFamilyIndexCount;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
    createInfo.initialLayout = initialLayout;

    Reset(device, createInfo, accessMask);
  }


  VkAccessFlags ImageEx::GetAccessMask(const uint32_t mipLevel, const uint32_t arrayLayer) const
  {
    assert(((arrayLayer * GetMipLevels()) + mipLevel) < m_imageLayers.size());
    return m_imageLayers[arrayLayer * GetMipLevels() + mipLevel].Access;
  }


  VkImageLayout ImageEx::GetImageLayout(const uint32_t mipLevel, const uint32_t arrayLayer) const
  {
    // assert(mipLevel < GetMipLevels());
    // assert(arrayLayer < GetArrayLevels());
    assert(((arrayLayer * GetMipLevels()) + mipLevel) < m_imageLayers.size());
    return m_imageLayers[(arrayLayer * GetMipLevels()) + mipLevel].Layout;
  }


  // Inspired by VKTS::Image::CmdPipelineBarrier by Norbert Nopper
  void ImageEx::CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout,
                                   const VkImageSubresourceRange& subresourceRange)
  {
    if (newLayout == VK_IMAGE_LAYOUT_UNDEFINED || newLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
      throw std::invalid_argument("New layout not allowed");
    }

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = m_image.Get();
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // It is allowed, that each mip level can have different layouts.
    const uint32_t mipLevels = GetMipLevels();
    for (uint32_t arrayLayer = subresourceRange.baseArrayLayer; arrayLayer < subresourceRange.baseArrayLayer + subresourceRange.layerCount;
         ++arrayLayer)
    {
      const uint32_t offset = arrayLayer * mipLevels;
      for (uint32_t mipLevel = subresourceRange.baseMipLevel; mipLevel < subresourceRange.baseMipLevel + subresourceRange.levelCount; ++mipLevel)
      {
        if (m_imageLayers[offset + mipLevel].Access == dstAccessMask && m_imageLayers[offset + mipLevel].Layout == newLayout)
        {
          continue;
        }

        imageMemoryBarrier.srcAccessMask = m_imageLayers[offset + mipLevel].Access;
        imageMemoryBarrier.oldLayout = m_imageLayers[offset + mipLevel].Layout;
        imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevel;
        imageMemoryBarrier.subresourceRange.levelCount = 1;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = arrayLayer;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);

        m_imageLayers[offset + mipLevel].Access = dstAccessMask;
        m_imageLayers[offset + mipLevel].Layout = newLayout;
      }
    }
  }


  // Inspired by VKTS::Image::CmdPipelineBarrier by Norbert Nopper
  void ImageEx::CopyImage(const VkCommandBuffer cmdBuffer, ImageEx& rTargetImage, const VkImageCopy& imageCopy)
  {
    if (!rTargetImage.IsValid())
    {
      throw std::invalid_argument("rTargetImage must be valid");
    }

    const VkImageLayout sourceImageLayout = GetImageLayout(imageCopy.srcSubresource.mipLevel, imageCopy.srcSubresource.baseArrayLayer);
    const VkAccessFlags sourceAccessMask = GetAccessMask(imageCopy.srcSubresource.mipLevel, imageCopy.srcSubresource.baseArrayLayer);
    const VkImageLayout targetImageLayout = rTargetImage.GetImageLayout(imageCopy.dstSubresource.mipLevel, imageCopy.dstSubresource.baseArrayLayer);
    const VkAccessFlags targetAccessMask = rTargetImage.GetAccessMask(imageCopy.dstSubresource.mipLevel, imageCopy.dstSubresource.baseArrayLayer);

    // Prepare source image for copy.
    VkImageSubresourceRange srcImageSubresourceRange = {imageCopy.srcSubresource.aspectMask, imageCopy.srcSubresource.mipLevel, 1,
                                                        imageCopy.srcSubresource.baseArrayLayer, imageCopy.srcSubresource.layerCount};

    CmdPipelineBarrier(cmdBuffer, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcImageSubresourceRange);

    // Prepare target image for copy.
    VkImageSubresourceRange dstImageSubresourceRange = {imageCopy.dstSubresource.aspectMask, imageCopy.dstSubresource.mipLevel, 1,
                                                        imageCopy.dstSubresource.baseArrayLayer, imageCopy.dstSubresource.layerCount};

    rTargetImage.CmdPipelineBarrier(cmdBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstImageSubresourceRange);

    // Copy image by command.
    vkCmdCopyImage(cmdBuffer, m_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rTargetImage.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                   &imageCopy);

    // Revert back.
    rTargetImage.CmdPipelineBarrier(cmdBuffer, targetAccessMask, targetImageLayout, dstImageSubresourceRange);

    // Revert back.
    CmdPipelineBarrier(cmdBuffer, sourceAccessMask, sourceImageLayout, srcImageSubresourceRange);
  }
}
