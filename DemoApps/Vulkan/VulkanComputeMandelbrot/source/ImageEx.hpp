#ifndef VULKAN_VULKANCOMPUTEMANDELBROT_IMAGEEX_HPP
#define VULKAN_VULKANCOMPUTEMANDELBROT_IMAGEEX_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/Image.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace Fsl
{
  // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
  class ImageEx
  {
    struct LayerRecord
    {
      VkImageLayout Layout = VK_IMAGE_LAYOUT_UNDEFINED;
      VkAccessFlags Access = 0;
    };

    RapidVulkan::Image m_image;
    std::vector<LayerRecord> m_imageLayers;
    VkImageCreateInfo m_createInfo;

  public:
    ImageEx(const ImageEx&) = delete;
    ImageEx& operator=(const ImageEx&) = delete;

    // move assignment operator
    ImageEx& operator=(ImageEx&& other) noexcept;
    // move constructor
    ImageEx(ImageEx&& other) noexcept;

    ImageEx();

    //! @brief Create a new ImageEx.
    ImageEx(const VkDevice device, const VkImageCreateInfo& createInfo, const VkAccessFlags accessMask);

    //! @brief Create a new ImageEx.
    ImageEx(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
            const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples, const VkImageTiling tiling,
            const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices,
            const VkImageLayout initialLayout, const VkAccessFlags accessMask);

    ~ImageEx();

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateImage
    void Reset(const VkDevice device, const VkImageCreateInfo& createInfo, const VkAccessFlags accessMask);

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
               const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples, const VkImageTiling tiling,
               const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount,
               const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout, const VkAccessFlags accessMask);

    //! @brief Get the device associated with this object
    VkDevice GetDevice() const
    {
      return m_image.GetDevice();
    }

    //! @brief Get the handle associated with this object
    VkImage Get() const
    {
      return m_image.Get();
    }


    //! @brief Check if this object is valid
    bool IsValid() const
    {
      return m_image.IsValid();
    }


    VkImageCreateFlags GetFlags() const
    {
      return m_createInfo.flags;
    }


    VkFormat GetFormat() const
    {
      return m_createInfo.format;
    }

    VkMemoryRequirements GetImageMemoryRequirements() const
    {
      return m_image.GetImageMemoryRequirements();
    }

    VkSubresourceLayout GetImageSubresourceLayout(const VkImageSubresource& imageSubresource) const
    {
      return m_image.GetImageSubresourceLayout(imageSubresource);
    }

    uint32_t GetMipLevels() const
    {
      return m_createInfo.mipLevels;
    }

    VkExtent3D GetExtent() const
    {
      return m_createInfo.extent;
    }

    VkAccessFlags GetAccessMask(const uint32_t mipLevel, const uint32_t arrayLayer) const;
    VkImageLayout GetImageLayout(const uint32_t mipLevel, const uint32_t arrayLayer) const;

    void CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout,
                            const VkImageSubresourceRange& subresourceRange);

    void CopyImage(const VkCommandBuffer cmdBuffer, ImageEx& rTargetImage, const VkImageCopy& imageCopy);
  };
}

#endif
