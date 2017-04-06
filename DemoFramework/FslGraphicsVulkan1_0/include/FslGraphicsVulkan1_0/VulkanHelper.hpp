#ifndef FSLGRAPHICSVULKAN1_0_VULKANHELPER_HPP
#define FSLGRAPHICSVULKAN1_0_VULKANHELPER_HPP
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
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    class VulkanHelper
    {
    public:
      //! @brief Enumerate the instance layer properties
      static std::vector<VkLayerProperties> EnumerateInstanceLayerProperties();

      //! @brief Enumerate
      static std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties(const char*const pszLayerName = nullptr);

      //! @brief Enumerate the physical devices
      static std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(const VkInstance instance);

      //! @brief Get the physical devices display properties
      static std::vector<VkDisplayPropertiesKHR> GetPhysicalDeviceDisplayPropertiesKHR(const VkPhysicalDevice device);

      //! @brief Get the physical devices queue family properties
      static std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice device);

      //! @brief Get the display mode properties.
      static std::vector<VkDisplayModePropertiesKHR> GetDisplayModePropertiesKHR(const VkPhysicalDevice device, const VkDisplayKHR display);

      //! @brief Get the physical devices surface present modes
      static std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface);

      //! @brief Get the physical devices surface formats
      static std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface);

      //! @brief Get the physical devices display plane properties
      static std::vector<VkDisplayPlanePropertiesKHR> GetPhysicalDeviceDisplayPlanePropertiesKHR(const VkPhysicalDevice device);

      //! @brief Get the supported displays for the plane
      static std::vector<VkDisplayKHR> GetDisplayPlaneSupportedDisplaysKHR(const VkPhysicalDevice device, const uint32_t planeIndex);

      //! @brief Get the swap chain images
      static std::vector<VkImage> GetSwapchainImagesKHR(const VkDevice device, const VkSwapchainKHR swapchain);


      static VkFormatProperties GetPhysicalDeviceFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format)
      {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);
        return properties;
      }

      static bool TryGetPhysicalDeviceImageFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format, const VkImageType type, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImageFormatProperties& rImageFormatProperties)
      {
        return (vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, &rImageFormatProperties) == VK_SUCCESS);
      }

      //! @brief Check if image tiling is available
      static bool IsImageTilingAvailable(const VkPhysicalDevice physicalDevice, const VkImageTiling imageTiling, const VkFormat format, const VkImageType type,
        const VkImageCreateFlags flags, const VkExtent3D& extent, const uint32_t mipLevels,
        const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts, const VkDeviceSize resourceSize);

    };

  }
}

#endif
