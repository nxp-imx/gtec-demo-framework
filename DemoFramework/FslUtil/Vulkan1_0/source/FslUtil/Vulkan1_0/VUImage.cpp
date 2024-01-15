/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/VUImage.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl::Vulkan
{
  VUImage::VUImage(const VkDevice device, const VkImageCreateInfo& createInfo)
    : VUImage()
  {
    Reset(device, createInfo);
  }


  VUImage::VUImage(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format,
                   const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples,
                   const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount,
                   const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout)
    : VUImage()
  {
    Reset(device, flags, imageType, format, extent, mipLevels, arrayLayers, samples, tiling, usage, sharingMode, queueFamilyIndexCount,
          queueFamilyIndices, initialLayout);
  }


  void VUImage::Reset() noexcept
  {
    if (!m_image.IsValid())
    {
      return;
    }

    // Use destruction order
    m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_mipLevels = 0;
    m_extent = {};
    m_format = VK_FORMAT_UNDEFINED;
    m_flags = 0;
    m_image.Reset();
  }


  void VUImage::Reset(const VkDevice device, const VkImageCreateInfo& createInfo)
  {
    if (IsValid())
    {
      Reset();
    }

    try
    {
      m_image.Reset(device, createInfo);
    }
    catch (const std::exception&)
    {
      m_image.Reset();
      throw;
    }

    m_flags = createInfo.flags;
    m_format = createInfo.format;
    m_extent = createInfo.extent;
    m_mipLevels = createInfo.mipLevels;
    m_layout = createInfo.initialLayout;
  }


  void VUImage::Reset(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format,
                      const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples,
                      const VkImageTiling tiling, const VkImageUsageFlags usage, const VkSharingMode sharingMode,
                      const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout)
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

    Reset(device, createInfo);
  }
}
