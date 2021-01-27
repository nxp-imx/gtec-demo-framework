#ifndef FSLUTIL_VULKAN1_0_UTIL_PHYSICALDEVICEUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_PHYSICALDEVICEUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <array>
#include <cassert>
#include <vector>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    namespace PhysicalDeviceUtil
    {
      bool IsDeviceExtensionsAvailable(const VkPhysicalDevice device, const uint32_t extensionCount, const char* const* enabledExtensionNames,
                                       const char* const pszLayerName = nullptr);

      std::vector<VkExtensionProperties> EnumerateDeviceExtensionProperties(const VkPhysicalDevice device, const char* const pszLayerName = nullptr);

      //! vkGetPhysicalDeviceFormatProperties
      inline VkFormatProperties GetPhysicalDeviceFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format)
      {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);
        return properties;
      }


      //! vkGetPhysicalDeviceImageFormatProperties
      inline bool TryGetPhysicalDeviceImageFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format, const VkImageType type,
                                                            const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags,
                                                            VkImageFormatProperties& rImageFormatProperties)
      {
        return (vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, &rImageFormatProperties) == VK_SUCCESS);
      }


      //! @brief Get the physical devices queue family properties
      //! vkGetPhysicalDeviceQueueFamilyProperties
      std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice device);


      //! @brief Check if image tiling is available
      bool IsImageTilingAvailable(const VkPhysicalDevice physicalDevice, const VkImageTiling imageTiling, const VkFormat format,
                                  const VkImageType type, const VkImageCreateFlags flags, const VkExtent3D& extent, const uint32_t mipLevels,
                                  const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts, const VkDeviceSize resourceSize);


      //! @brief Scan the candidate list and find a format that is supported.
      //! @param candidates the candidate list with the preferred format first and the least preferred format last.
      template <std::size_t TSize>
      inline VkFormat TryFindSupportedFormat(const VkPhysicalDevice physicalDevice, const std::array<VkFormat, TSize>& candidates,
                                             const VkImageTiling tiling, const VkFormatFeatureFlags features)
      {
        assert(physicalDevice != VK_NULL_HANDLE);
        for (VkFormat format : candidates)
        {
          VkFormatProperties props;
          vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

          if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
          {
            return format;
          }
          if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
          {
            return format;
          }
        }
        return VK_FORMAT_UNDEFINED;
      }

      //! @brief Scan the candidate list and find a format that is supported.
      //! @param candidates the candidate list with the preferred format first and the least preferred format last.
      template <std::size_t TSize>
      inline VkFormat FindSupportedFormat(const VkPhysicalDevice physicalDevice, const std::array<VkFormat, TSize>& candidates,
                                          const VkImageTiling tiling, const VkFormatFeatureFlags features)
      {
        auto foundFormat = TryFindSupportedFormat(physicalDevice, candidates, tiling, features);
        if (foundFormat != VK_FORMAT_UNDEFINED)
        {
          return foundFormat;
        }
        throw NotFoundException("No supported format found");
      }

      //! @brief Find a VK_IMAGE_TILING_OPTIMAL depth format
      inline VkFormat FindDepthFormat(const VkPhysicalDevice physicalDevice, const bool enableStencil)
      {
        assert(physicalDevice != VK_NULL_HANDLE);
        if (!enableStencil)
        {
          constexpr std::array<VkFormat, 2> depthFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM};
          return PhysicalDeviceUtil::FindSupportedFormat(physicalDevice, depthFormats, VK_IMAGE_TILING_OPTIMAL,
                                                         VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        constexpr std::array<VkFormat, 3> depthFormatsWithS = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,
                                                               VK_FORMAT_D16_UNORM_S8_UINT};
        return PhysicalDeviceUtil::FindSupportedFormat(physicalDevice, depthFormatsWithS, VK_IMAGE_TILING_OPTIMAL,
                                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
      }
    }
  }
}

#endif
