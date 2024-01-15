#ifndef FSLUTIL_VULKAN1_0_VUPHYSICALDEVICERECORD_HPP
#define FSLUTIL_VULKAN1_0_VUPHYSICALDEVICERECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <vulkan/vulkan.h>

namespace Fsl::Vulkan
{
  //! This is contains information about a vulkan PhysicalDevice.
  //! This is not really a RAII object as it doesn't manage lifetime, hence the postfix Record.
  //! While you do have direct access to the members of this record, it is best to utilize the Constructor or Reset method
  //! to change the physical device.
  struct VUPhysicalDeviceRecord
  {
    VkPhysicalDevice Device = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties Properties{};
    VkPhysicalDeviceFeatures Features{};
    VkPhysicalDeviceMemoryProperties MemoryProperties{};

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    VUPhysicalDeviceRecord() = default;

    explicit VUPhysicalDeviceRecord(const VkPhysicalDevice physicalDevice);

    void Reset() noexcept;
    void Reset(const VkPhysicalDevice physicalDevice);

    bool IsValid() const noexcept
    {
      return Device != VK_NULL_HANDLE;
    }

    //! @brief
    uint32_t GetMemoryTypeIndex(const uint32_t typeBits, const VkMemoryPropertyFlags properties) const;


    VkFormatProperties GetPhysicalDeviceFormatProperties(const VkFormat format) const
    {
      VkFormatProperties properties;
      vkGetPhysicalDeviceFormatProperties(Device, format, &properties);
      return properties;
    }

    void GetPhysicalDeviceFormatProperties(VkFormatProperties& rProperties, const VkFormat format) const
    {
      vkGetPhysicalDeviceFormatProperties(Device, format, &rProperties);
    }


    VkImageFormatProperties GetPhysicalDeviceImageFormatProperties(const VkFormat format, const VkImageType type, const VkImageTiling tiling,
                                                                   const VkImageUsageFlags usage, const VkImageCreateFlags flags) const
    {
      VkImageFormatProperties properties;
      RapidVulkan::CheckError(vkGetPhysicalDeviceImageFormatProperties(Device, format, type, tiling, usage, flags, &properties),
                              "vkGetPhysicalDeviceImageFormatProperties", __FILE__, __LINE__);
      return properties;
    }


    bool TryGetPhysicalDeviceImageFormatProperties(const VkFormat format, const VkImageType type, const VkImageTiling tiling,
                                                   const VkImageUsageFlags usage, const VkImageCreateFlags flags,
                                                   VkImageFormatProperties& rImageFormatProperties) const
    {
      return (vkGetPhysicalDeviceImageFormatProperties(Device, format, type, tiling, usage, flags, &rImageFormatProperties) == VK_SUCCESS);
    }


    bool IsImageTilingAvailable(const VkImageTiling imageTiling, const VkFormat format, const VkImageType type, const VkImageCreateFlags flags,
                                const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts,
                                const VkDeviceSize resourceSize)
    {
      return PhysicalDeviceUtil::IsImageTilingAvailable(Device, imageTiling, format, type, flags, extent, mipLevels, arrayLayers, sampleCounts,
                                                        resourceSize);
    }


    //! @brief Scan the candidate list and find a format that is supported.
    //! @param candidates the candidate list with the preferred format first and the least preferred format last.
    template <std::size_t TSize>
    inline VkFormat TryFindSupportedFormat(const std::array<VkFormat, TSize>& candidates, const VkImageTiling tiling,
                                           const VkFormatFeatureFlags features) const
    {
      return PhysicalDeviceUtil::TryFindSupportedFormat(Device, candidates, tiling, features);
    }

    //! @brief Scan the candidate list and find a format that is supported.
    //! @param candidates the candidate list with the preferred format first and the least preferred format last.
    template <std::size_t TSize>
    inline VkFormat FindSupportedFormat(const std::array<VkFormat, TSize>& candidates, const VkImageTiling tiling,
                                        const VkFormatFeatureFlags features) const
    {
      return PhysicalDeviceUtil::FindSupportedFormat(Device, candidates, tiling, features);
    }

    //! @brief Find a VK_IMAGE_TILING_OPTIMAL depth format
    VkFormat FindDepthFormat(const bool enableStencil) const
    {
      return PhysicalDeviceUtil::FindDepthFormat(Device, enableStencil);
    }
  };
}

#endif
