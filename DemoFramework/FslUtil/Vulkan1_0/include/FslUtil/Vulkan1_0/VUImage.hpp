#ifndef FSLUTIL_VULKAN1_0_VUIMAGE_HPP
#define FSLUTIL_VULKAN1_0_VUIMAGE_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/Image.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl::Vulkan
{
  // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
  class VUImage
  {
    RapidVulkan::Image m_image;
    VkImageCreateFlags m_flags = 0;
    VkFormat m_format = VK_FORMAT_UNDEFINED;
    VkExtent3D m_extent{};
    uint32_t m_mipLevels = 0;
    VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

  public:
    VUImage(const VUImage&) = delete;
    VUImage& operator=(const VUImage&) = delete;


    // move assignment operator
    VUImage& operator=(VUImage&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_image = std::move(other.m_image);
        m_flags = other.m_flags;
        m_format = other.m_format;
        m_extent = other.m_extent;
        m_mipLevels = other.m_mipLevels;
        m_layout = other.m_layout;

        // Remove the data from other
        other.m_flags = 0;
        other.m_format = VK_FORMAT_UNDEFINED;
        other.m_extent = {};
        other.m_mipLevels = 0;
        other.m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
      }
      return *this;
    }

    // move constructor
    VUImage(VUImage&& other) noexcept
      : m_image(std::move(other.m_image))
      , m_flags(other.m_flags)
      , m_format(other.m_format)
      , m_extent(other.m_extent)
      , m_mipLevels(other.m_mipLevels)
      , m_layout(other.m_layout)
    {
      // Remove the data from other
      other.m_flags = 0;
      other.m_format = VK_FORMAT_UNDEFINED;
      other.m_extent = {};
      other.m_mipLevels = 0;
      other.m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    VUImage() = default;

    //! @brief Create a new VUImage.
    VUImage(const VkDevice device, const VkImageCreateInfo& createInfo);

    //! @brief Create a new VUImage.
    VUImage(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
            const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples, const VkImageTiling tiling,
            const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices,
            const VkImageLayout initialLayout);

    ~VUImage() noexcept
    {
      Reset();
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateImage
    void Reset(const VkDevice device, const VkImageCreateInfo& createInfo);

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkImageCreateFlags flags, const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
               const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlagBits samples, const VkImageTiling tiling,
               const VkImageUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount,
               const uint32_t* queueFamilyIndices, const VkImageLayout initialLayout);

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
      return m_flags;
    }


    VkFormat GetFormat() const
    {
      return m_format;
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
      return m_mipLevels;
    }

    VkExtent2D GetExtent2D() const
    {
      return VkExtent2D{m_extent.width, m_extent.height};
    }

    PxSize2D GetSize() const
    {
      return {UncheckedNumericCast<PxSize2D::value_type>(m_extent.width), UncheckedNumericCast<PxSize2D::value_type>(m_extent.height)};
    }


    VkExtent3D GetExtent() const
    {
      return m_extent;
    }

    //! @brief Beware that the image layout can easily be changed outside of this classes control,
    //         so its up to you to keep it up to date with SetImageLayout
    VkImageLayout GetImageLayout() const
    {
      return m_layout;
    }

    //! @brief Beware this does not actually do anything to the image, it just sets the stored layout variable
    void SetImageLayout(const VkImageLayout newLayout)
    {
      m_layout = newLayout;
    }
  };
}

#endif
