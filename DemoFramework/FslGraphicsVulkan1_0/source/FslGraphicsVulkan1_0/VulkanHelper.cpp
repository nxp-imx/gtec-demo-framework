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

#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    std::vector<VkLayerProperties> VulkanHelper::EnumerateInstanceLayerProperties()
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkEnumerateInstanceLayerProperties(&count, nullptr), "failed to acquire the count");

      std::vector<VkLayerProperties> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkEnumerateInstanceLayerProperties(&count, result.data()), "failed to enumerate layer properties");
      return result;
    }


    std::vector<VkExtensionProperties> VulkanHelper::EnumerateInstanceExtensionProperties(const char*const pszLayerName)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkEnumerateInstanceExtensionProperties(pszLayerName, &count, nullptr), "failed to acquire the count");

      std::vector<VkExtensionProperties> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkEnumerateInstanceExtensionProperties(pszLayerName, &count, result.data()), "failed to enumerate layer properties");
      return result;
    }


    std::vector<VkPhysicalDevice> VulkanHelper::EnumeratePhysicalDevices(const VkInstance instance)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkEnumeratePhysicalDevices(instance, &count, nullptr), "failed to acquire the count");

      std::vector<VkPhysicalDevice> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkEnumeratePhysicalDevices(instance, &count, result.data()), "failed to enumerate devices");
      return result;
    }


    std::vector<VkDisplayPropertiesKHR> VulkanHelper::GetPhysicalDeviceDisplayPropertiesKHR(const VkPhysicalDevice device)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &count, nullptr), "failed to acquire the count");

      std::vector<VkDisplayPropertiesKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &count, result.data()), "failed to get the DeviceDisplayPropertiesKHR");
      return result;
    }


    std::vector<VkQueueFamilyProperties> VulkanHelper::GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice device)
    {
      uint32_t count;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

      std::vector<VkQueueFamilyProperties> result(count);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &count, result.data());
      return result;
    }


    std::vector<VkDisplayModePropertiesKHR> VulkanHelper::GetDisplayModePropertiesKHR(const VkPhysicalDevice device, const VkDisplayKHR display)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetDisplayModePropertiesKHR(device, display, &count, nullptr), "failed to acquire the count");

      std::vector<VkDisplayModePropertiesKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetDisplayModePropertiesKHR(device, display, &count, result.data()), "failed to get the display mode properties");
      return result;
    }


    std::vector<VkPresentModeKHR> VulkanHelper::GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr), "failed to acquire the count");

      std::vector<VkPresentModeKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, result.data()), "failed to get the present modes");
      return result;
    }


    std::vector<VkSurfaceFormatKHR> VulkanHelper::GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr), "failed to acquire the device count");

      std::vector<VkSurfaceFormatKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, result.data()), "failed to get surface formats");

      return result;
    }


    std::vector<VkDisplayPlanePropertiesKHR> VulkanHelper::GetPhysicalDeviceDisplayPlanePropertiesKHR(const VkPhysicalDevice device)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(device, &count, nullptr), "failed to acquire the plane count");

      std::vector<VkDisplayPlanePropertiesKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(device, &count, result.data()), "failed to get plane properties");

      return result;
    }


    std::vector<VkDisplayKHR> VulkanHelper::GetDisplayPlaneSupportedDisplaysKHR(const VkPhysicalDevice device, const uint32_t planeIndex)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetDisplayPlaneSupportedDisplaysKHR(device, planeIndex, &count, nullptr), "failed to acquire the supported display count");

      std::vector<VkDisplayKHR> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetDisplayPlaneSupportedDisplaysKHR(device, planeIndex, &count, result.data()), "failed to get plane supported displays");
      return result;
    }


    std::vector<VkImage> VulkanHelper::GetSwapchainImagesKHR(const VkDevice device, const VkSwapchainKHR swapchain)
    {
      uint32_t count;
      FSLGRAPHICSVULKAN_CHECK2(vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr), "could not get swap chain images size");

      std::vector<VkImage> result(count);
      FSLGRAPHICSVULKAN_CHECK2(vkGetSwapchainImagesKHR(device, swapchain, &count, result.data()), "could not get swap chain image info");
      return result;
    }


    bool VulkanHelper::IsImageTilingAvailable(const VkPhysicalDevice physicalDevice, const VkImageTiling imageTiling, const VkFormat format, const VkImageType type,
      const VkImageCreateFlags flags, const VkExtent3D& extent, const uint32_t mipLevels,
      const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts, const VkDeviceSize resourceSize)
    {
      const VkFormatProperties formatProperties = GetPhysicalDeviceFormatProperties(physicalDevice, format);

      if (imageTiling == VK_IMAGE_TILING_OPTIMAL && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
        return false;

      if (imageTiling == VK_IMAGE_TILING_LINEAR && !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
        return false;

      VkImageFormatProperties imageFormatProperties;
      if (! TryGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, imageTiling, VK_IMAGE_USAGE_SAMPLED_BIT, flags, imageFormatProperties))
        return false;

      if (imageFormatProperties.maxResourceSize < resourceSize)
        return false;

      if (imageFormatProperties.maxExtent.width < extent.width || imageFormatProperties.maxExtent.height < extent.height || imageFormatProperties.maxExtent.depth < extent.depth)
        return false;

      if (imageFormatProperties.maxMipLevels < mipLevels)
        return false;

      if (imageFormatProperties.maxArrayLayers < arrayLayers)
        return false;

      if (!(imageFormatProperties.sampleCounts & sampleCounts))
        return false;

      return true;
    }


  }
}
