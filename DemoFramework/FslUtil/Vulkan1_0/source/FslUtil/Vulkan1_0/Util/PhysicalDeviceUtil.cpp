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

#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PropertyUtil.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/ReadOnlySpanUtil.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace PhysicalDeviceUtil
    {
      bool IsDeviceExtensionsAvailable(const VkPhysicalDevice device, const uint32_t extensionCount, const char* const* enabledExtensionNames,
                                       const char* const pszLayerName)
      {
        if (device == VK_NULL_HANDLE || extensionCount == 0 || enabledExtensionNames == nullptr)
        {
          return false;
        }

        const std::vector<VkExtensionProperties> extensionProperties = EnumerateDeviceExtensionProperties(device, pszLayerName);
        const auto extensionPropertiesSpan = ReadOnlySpanUtil::AsSpan(extensionProperties);
        for (uint32_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex)
        {
          if (!PropertyUtil::IsExtensionAvailable(extensionPropertiesSpan, enabledExtensionNames[extensionIndex]))
          {
            return false;
          }
        }
        return true;
      }

      std::vector<VkExtensionProperties> EnumerateDeviceExtensionProperties(const VkPhysicalDevice device, const char* const pszLayerName)
      {
        uint32_t count = 0;
        RAPIDVULKAN_CHECK2(vkEnumerateDeviceExtensionProperties(device, pszLayerName, &count, nullptr), "failed to acquire the count");

        std::vector<VkExtensionProperties> result(count);
        RAPIDVULKAN_CHECK2(vkEnumerateDeviceExtensionProperties(device, pszLayerName, &count, result.data()), "failed to enumerate layer properties");
        return result;
      }

      std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice device)
      {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        std::vector<VkQueueFamilyProperties> result(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, result.data());
        return result;
      }


      bool IsImageTilingAvailable(const VkPhysicalDevice physicalDevice, const VkImageTiling imageTiling, const VkFormat format,
                                  const VkImageType type, const VkImageCreateFlags flags, const VkExtent3D& extent, const uint32_t mipLevels,
                                  const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts, const VkDeviceSize resourceSize)
      {
        const VkFormatProperties formatProperties = GetPhysicalDeviceFormatProperties(physicalDevice, format);

        if (imageTiling == VK_IMAGE_TILING_OPTIMAL && ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == 0u))
        {
          return false;
        }

        if (imageTiling == VK_IMAGE_TILING_LINEAR && ((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == 0u))
        {
          return false;
        }

        VkImageFormatProperties imageFormatProperties;
        if (!TryGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, imageTiling, VK_IMAGE_USAGE_SAMPLED_BIT, flags,
                                                       imageFormatProperties))
        {
          return false;
        }

        if (imageFormatProperties.maxResourceSize < resourceSize)
        {
          return false;
        }

        if (imageFormatProperties.maxExtent.width < extent.width || imageFormatProperties.maxExtent.height < extent.height ||
            imageFormatProperties.maxExtent.depth < extent.depth)
        {
          return false;
        }

        if (imageFormatProperties.maxMipLevels < mipLevels)
        {
          return false;
        }

        if (imageFormatProperties.maxArrayLayers < arrayLayers)
        {
          return false;
        }

        if ((imageFormatProperties.sampleCounts & sampleCounts) == 0u)
        {
          return false;
        }

        return true;
      }
    }
  }
}
