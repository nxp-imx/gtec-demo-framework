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

#include <VulkanExperimental/VulkanUtil.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <cstring>

namespace Fsl
{
  using namespace Vulkan;

  namespace VulkanUtil
  {
    namespace
    {
      const int ENGINE_MAJOR = 1;
      const int ENGINE_MINOR = 0;
      const int ENGINE_PATCH = 0;
    }


    bool IsInstanceExtensionsAvailable(const uint32_t extensionCount, const char* const* enabledExtensionNames)
    {
      if (extensionCount == 0)
        return true;

      if (!enabledExtensionNames)
        return false;

      const std::vector<VkExtensionProperties> extensionProperties = VulkanHelper::EnumerateInstanceExtensionProperties();
      for (uint32_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex)
      {
        for (uint32_t i = 0; i < extensionProperties.size(); ++i)
        {
          if (strcmp(enabledExtensionNames[extensionIndex], extensionProperties[i].extensionName) == 0)
            return true;
        }
      }
      return false;
    }


    Instance CreateInstance(const std::string& applicationName, const uint32_t applicationVersion,
                            const uint32_t apiVersion, const VkInstanceCreateFlags flags, const uint32_t enabledLayerCount,
                            const char* const* ppszEnabledLayerNames, const uint32_t enabledExtensionCount,
                            const char* const* ppszEnabledExtensionNames)
    {
      if (ppszEnabledExtensionNames == nullptr && enabledExtensionCount > 0)
        throw std::invalid_argument("enabledExtensionCount can not be non-zero when no enabled extensions are supplied");

      if (enabledExtensionCount > 0)
      {
        if (!IsInstanceExtensionsAvailable(enabledExtensionCount, ppszEnabledExtensionNames))
          throw NotSupportedException("Extension not available");
      }

      VkApplicationInfo applicationInfo{};
      applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      applicationInfo.pApplicationName = applicationName.c_str();
      applicationInfo.applicationVersion = applicationVersion;
      applicationInfo.pEngineName = "ExampleEngine";
      applicationInfo.engineVersion = VK_MAKE_VERSION(ENGINE_MAJOR, ENGINE_MINOR, ENGINE_PATCH);
      applicationInfo.apiVersion = apiVersion;

      VkInstanceCreateInfo instanceCreateInfo{};
      instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      instanceCreateInfo.flags = flags;
      instanceCreateInfo.pApplicationInfo = &applicationInfo;
      instanceCreateInfo.enabledLayerCount = enabledLayerCount;
      instanceCreateInfo.ppEnabledLayerNames = ppszEnabledLayerNames;
      instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
      instanceCreateInfo.ppEnabledExtensionNames = ppszEnabledExtensionNames;
      return Instance(instanceCreateInfo);
    }


    PhysicalDeviceEx GetPhysicalDevice(const VkInstance instance, const uint32_t index)
    {
      if (instance == VK_NULL_HANDLE)
        throw std::invalid_argument("Instance has to be valid");

      const std::vector<VkPhysicalDevice> physicalDevices = VulkanHelper::EnumeratePhysicalDevices(instance);
      if (index >= physicalDevices.size())
        throw std::invalid_argument("physical device index out of bounds");

      return PhysicalDeviceEx(instance, physicalDevices[index], index);
    }


    bool TryGetQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags, const uint32_t queueIndex, const std::vector<bool>* pFilter, uint32_t& rQueueFamilyIndex)
    {
      if (pFilter && pFilter->size() != allQueueFamilyProperties.size())
        throw std::invalid_argument("The filter size must match the allQueueFamilyProperties size");

      for (uint32_t queueFamilyIndexWalker = 0; queueFamilyIndexWalker < allQueueFamilyProperties.size(); ++queueFamilyIndexWalker)
      {
        if ((allQueueFamilyProperties[queueFamilyIndexWalker].queueFlags & queueFlags) && allQueueFamilyProperties[queueFamilyIndexWalker].queueCount > queueIndex)
        {
          if (!pFilter || (pFilter && (*pFilter)[queueFamilyIndexWalker]))
          {
            rQueueFamilyIndex = queueFamilyIndexWalker;
            return true;
          }
        }
      }
      return false;
    }


    std::vector<bool> GetPhysicalDeviceSurfaceSupport(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const std::size_t queueFamilyIndexCount)
    {
      std::vector<bool> supported;
      for (uint32_t currentQueueFamilyIndex = 0; currentQueueFamilyIndex < queueFamilyIndexCount; ++currentQueueFamilyIndex)
      {
        VkBool32 isSupported;
        FSLGRAPHICSVULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, surface, &isSupported));
        supported.push_back(isSupported != VK_FALSE);
      }
      return supported;
    }


    uint32_t GetFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags, const uint32_t queueIndex, const std::vector<bool>* pFilter)
    {
      uint32_t queueFamilyIndex;
      if (TryGetQueueFamilyIndex(allQueueFamilyProperties, queueFlags, queueIndex, pFilter, queueFamilyIndex))
        return queueFamilyIndex;
      throw NotSupportedException("Could not find matching physical device queue properties");
    }


    DeviceQueue GetDeviceQueue(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t queueIndex)
    {
      VkQueue queue;
      vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);
      return DeviceQueue(device, queueFamilyIndex, queueIndex, queue);
    }


    Vulkan::SwapchainKHREx CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
    {
      return CreateSwapchain(physicalDevice, device, flags, surface, minImageCount, imageArrayLayers, imageUsage, imageSharingMode, queueFamilyIndexCount, queueFamilyIndices, compositeAlpha, VK_PRESENT_MODE_FIFO_KHR, clipped, oldSwapchain);
    }

    Vulkan::SwapchainKHREx CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
    {
      if (physicalDevice == VK_NULL_HANDLE || device == VK_NULL_HANDLE)
        throw std::invalid_argument("physicalDevice and device can not be VK_NULL_HANDLE");

      VkSurfaceCapabilitiesKHR surfaceCapabilities;
      FSLGRAPHICSVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

      VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
      if (!(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
      {
        preTransform = surfaceCapabilities.currentTransform;
      }

      const auto surfacePresentModes = VulkanHelper::GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface);
      const auto surfaceFormats = VulkanHelper::GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);

      VkFormat imageFormat = surfaceFormats[0].format;
      VkColorSpaceKHR imageColorSpace = surfaceFormats[0].colorSpace;

      VkSwapchainCreateInfoKHR swapchainCreateInfo{};
      swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      swapchainCreateInfo.flags = flags;
      swapchainCreateInfo.surface = surface;
      swapchainCreateInfo.minImageCount = minImageCount;
      swapchainCreateInfo.imageFormat = imageFormat;
      swapchainCreateInfo.imageColorSpace = imageColorSpace;
      swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
      swapchainCreateInfo.imageArrayLayers = imageArrayLayers;
      swapchainCreateInfo.imageUsage = imageUsage;
      swapchainCreateInfo.imageSharingMode = imageSharingMode;
      swapchainCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
      swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
      swapchainCreateInfo.preTransform = preTransform;
      swapchainCreateInfo.compositeAlpha = compositeAlpha;
      swapchainCreateInfo.presentMode = presentMode;
      swapchainCreateInfo.clipped = clipped;
      swapchainCreateInfo.oldSwapchain = oldSwapchain;

      return Vulkan::SwapchainKHREx(device, swapchainCreateInfo);
    }
  }
}
